#include <TimerOne.h>
/*
 AVR High-voltage Serial Fuse Reprogrammer with 12 Volt Charge Pump
 Adapted from code and design by Paul Willoughby 03/20/2010
   http://www.rickety.us/2010/03/arduino-avr-high-voltage-serial-programmer/

 Fuse Calc:
   http://www.engbedded.com/fusecalc/

 Siehe C:\Dat\Maerklin\Datenblaetter\154219-da-01-en-ATTINY45_20PU_DIL8.pdf
 Seite 158

 AtTiny84: C:\Dat\Maerklin\Datenblaetter\ATTiny24_44_84.pdf
 Seite 167

 Die Fuses koennen mit der Arduino IDE gesetzt werden.
 Die Fuses werden bei den ATTinys mit dem Menupunkt Bootloader brennen geschrieben.
 Achtung: Der Serielle Monitor darf nicht aktiv sein. Er wird nicht wie beim Flashen
          automatisch geschlossen.
 Der Reset Pin kann aber nicht ueber die IDE geaendert werden

 Mit diesem Programm kann man die Fuses setzen. Zunaechst habe ich es dazu verwendet
 den Reset Pin eines ATTiny85 zum Ausgang umzukonfigurieren. Danach laesst sich der
 ATTiny aber nicht mehr Programmieren ;-(
 Mit dem HVReset kann man das aber wieder rueckgaengig machen.

 Das Programm unterstuetzt jetzt auch den AtTiny84 (14 Pin Chip). Dazu habe ich einen
 Adapterstecker gemacht (Dip8 => Dip14).

 Das Programm verwendet den "Seriellen Monitor" der Arduino IDE als Ein- und Ausgabe.
 Die Baudrate muss auf 19200 stehen.

 Befehle:
   'r' to use the reset pin of as output
   'p' to power up the ATTiny for testing
   '8' to set the clock to 8 MHz
   'R' to use the reset pin of as output @ 8 MHz
       All other keys reset the fuses

 ToDo:
 - LEDs beim Fuses Loeschen aktivieren
 - Nur dann den HV Programmer aktivieren wenn der Reset Pin nicht aktiviert ist.
 - Andere Fuses nicht veraendern
 - Aufraeumen


 - Nach dem Programmieren der Fuses automatisch die
   Versorgungsspannung einschalten und den REset Pin als Input konfigurieren
   Problem: Momentan veraendert das die Interruptroutine

                   ATMEL ATTINY45 / ARDUINO

     Programmer                     +-\/-+                            Programmer
     HVRes         Ain0 (D 5) PB5  1|    |8  Vcc
     UNO 12   CLKI Ain3 (D 3) PB3  2|    |7  PB2 (D 2) Ain1 SCK       UNO 13
                   Ain2 (D 4) PB4  3|    |6  PB1 (D 1) pwm1 MISO      UNO 12
                              GND  4|    |5  PB0 (D 0) pwm0 MOSI      UNO 11
                                    +----+



                   ATMEL ATTINY84 / ARDUINO

                                             +-\/-+
                                       VCC  1|    |14  GND
     UNO 12              CLKI  (D 10)  PB0  2|    |13  AREF (D  0)           GND !!
                               (D  9)  PB1  3|    |12  PA1  (D  1)           GND !!
     HVRes                      Reset  PB3  4|    |11  PA2  (D  2)           GND !!
                    PWM  INT0  (D  8)  PB2  5|    |10  PA3  (D  3)
                    PWM        (D  7)  PA7  6|    |9   PA4  (D  4) SCK       UNO 13
      UNO 11        PWM  MOSI  (D  6)  PA6  7|    |8   PA5  (D  5) MISO PWM  UNO 12
                                             +----+

  Adapter to program the AtTiny84 uin the AtTiny85 programmer:
     Dip8  Dip14
      1      4     HVReset
      2      2     CLKI
      3      10    unused
      4    11-14   GND
      5      7     MOSI
      6      8     MISO
      7      9     SCK
      8      1     Vcc



*/
// Todo: Use one H-File which contains the defines for ino and this file

#include "Global_Defines.h"
#include "LEDs.h"
#include "Fuses.h"


#define  HFUSE  0x747C
#define  LFUSE  0x646C
#define  EFUSE  0x666E


#define REF 425   // 12 volt reference  Old: 404   28.06.19:  470K instead of 510K   Old 407
                  //

// Messungen mit Ossi mit Platine 2 (Ohne Relais) 29.06.19:
// Interrupt period 100us, R7=100K, R8=470K
// 300 = 7.8V   Kein Reset
// 350 = 9.6V   HV Prog o.k.
// 400 = 11V    HV Prog o.k.
// 425 = 11.5V  HV Prog o.k.   => Use this. According to the spec 11.5V-12.5V are required   (Calculated value: 425 => 11.84V)
// 450 = 12V    HV Prog o.k.
// 480 = 12.2V  Timeout
//       Charge pump is not able to generate a higher voltage with a interrupt period of 100us

//                    Attention R8 is NOT 510K! It is replaced with 470K
//  ---,  U_HV
//     |
//    ,-,                            U_Mess * (R7+R8)                         5V * MessDig
//    | | R8                  U_HV = ---------------- =  5.7 * U_Mess = 5.7 * ------------ = 0.0279
//    | | 470K 1%                           R7                                  1023
//    '-'
//     |
//     *------- U_Mess
//     |
//    ,-,
//    | | R7
//    | | 100K 1%
//    '-'
//     |
//    ---


// Variables used by Charge pump
volatile char phase = 0;
volatile char onOff = 0;
volatile char pwrOn = 0;

uint8_t shiftOut_Timeout = 0;

// Forward definitions
unsigned int readSignature();
void Read_and_Print_Fuses(Chip_Data_T &Chip_Data);
void writeFuse(unsigned int fuse, byte val);

void Set_Reset_Flag(uint8_t Flag); // Defined in Tiny_UniProg.ino
void Set_Error_Flag(uint8_t Flag); //   "
void Save_Persist_Data();          //   "
uint8_t Read_Buttons_and_Jumper(); //   "


//----------------------
void HV_Interrupt_Proc()
//----------------------
// Interrupt function
{
  if (onOff)
       {
       DDRD = PIN_MSKD_REL | PIN_MSKD_P1 | PIN_MSKD_P2 | PIN_MSKD_PWR | PIN_MSKD_GND;
       int volts = analogRead(PIN_HV_AD_12V); // ~ 100 microseconds
       if (volts < REF)
           {
           if (phase)
                 PORTD = PIN_MSKD_REL | PIN_MSKD_P1 | PIN_MSKD_PWR;
            else PORTD = PIN_MSKD_REL | PIN_MSKD_P2 | PIN_MSKD_PWR;
            phase ^= 1;
            }
       else pwrOn = 1;
       }
  else {
       if (pwrOn)
          {
          pwrOn = 0;
          DDRD  = PIN_MSKD_REL | PIN_MSKD_GND;
          PORTD = PIN_MSKD_REL | PIN_MSKD_GND;
          }
       }
}


//--------------------
void FuseReset_setup()
//--------------------
{
  pinMode(PIN_POWER , OUTPUT);     // Power supply for the ATTimy
  pinMode(PIN_HV_SDI, OUTPUT);     // Target Data Input
  pinMode(PIN_HV_SII, OUTPUT);     // Target Instruction Input
  pinMode(PIN_HV_SCI, OUTPUT);     // Target Clock Input
  pinMode(PIN_HV_SDO, OUTPUT);     // Target Data Output, Configured as input when in programming mode

  /*
  The following setups have been moved to the Tiny_UniProg.ino setup() function
  analogReference(DEFAULT);
  // Faster ADC conversion: http://www.optiloading.be/willem/Arduino/speeding.pdf
  // Normaly the ADC conversion takes 100us. With the following change it takes 13us.
  // With the original ADC conversion the interrupt period can't be below 100 us
  // A faster interrupt period is needed to get a higher output voltage.
  #define  cbi(sfr, bit)  (_SFR_BYTE(sfr) &= ~_BV(bit)) // macro to clear bit in special function register
  #define  sbi(sfr, bit)  (_SFR_BYTE(sfr) |=  _BV(bit)) // macro to set   bit in special function register
  sbi (ADCSRA, ADPS2);
  cbi (ADCSRA, ADPS1);
  cbi (ADCSRA, ADPS0);
  // Setup timer interrupt for  charge pump
  Timer1.initialize(100);  // [us] Original 500                                                               // 28.06.19:  Old 250
  Timer1.attachInterrupt(HV_Interrupt_Proc);
  */

  digitalWrite(RELAIS_PWR_PIN, 1);                                                                            // 27.06.19:  ??? 0 oder 1 ?
  digitalWrite(RELAIS_PRG_PIN, 1);                                                                            // 27.06.19:  ??? 0 oder 1 ?
  digitalWrite(RELAIS_RST_PIN, 1);                                                                            // 27.06.19:  ??? 0 oder 1 ?
  //delay(500);
}

//-------------------------------------------------
void Print_Signatur_and_ChipName(uint16_t Signatur)
//-------------------------------------------------
{
  printf("Signature is: %04X (", Signatur);
  uint16_t TinyNr = 0;
  switch (Signatur)
    {
    //case ATTINY13: ??
    case ATTINY24: TinyNr = 24; break;
    case ATTINY44: TinyNr = 44; break;
    case ATTINY84: TinyNr = 84; break;
    case ATTINY25: TinyNr = 25; break;
    case ATTINY45: TinyNr = 45; break;
    case ATTINY85: TinyNr = 85; break;
    }
  if (TinyNr)
       printf("ATTiny%i", TinyNr);
  else printf("Unknown chip");
  printf(")\n");
}

//-------------------------------------------------
void Print_12V_Value(uint16_t Val, uint8_t WithRaw)
//-------------------------------------------------
{
  Serial.print(0.0279*Val+0.05, 1);
  if (WithRaw)
       {
       Serial.print(F("V ("));
       Serial.print(Val);
       Serial.println(F(")"));
       }
  else Serial.println(F("V"));
}


//--------------------------
void HV_Reset(uint8_t Quiet)
//--------------------------
// See: 20.7.1 Enter High-voltage Serial Programming Mode on page 156
// in atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
{
  digitalWrite(PIN_POWER, LOW);    // Vcc Off
  delay(100),
  pinMode(PIN_HV_SDO, OUTPUT);     // Set SDO to output
  digitalWrite(PIN_HV_SDI, LOW);
  digitalWrite(PIN_HV_SII, LOW);
  digitalWrite(PIN_HV_SDO, LOW);
  PORTD |= PIN_MSKD_GND;           // Discharge the capacitors                                                // 14.05.20:  Juergen
  onOff = 0;                       // 12v Off
  delay(100);                                                                                                 //  "
  PORTD &= ~PIN_MSKD_GND;                                                                                     // 07.08.20:  Added "~"
  digitalWrite(PIN_POWER, HIGH);   // Vcc On
  delayMicroseconds(20);
  onOff = 1;                       // 12v On

  uint32_t TimeOut = millis() + 900;
  while (pwrOn == 0 && millis() < TimeOut)
    Upd_LED_Heartbeat(); // Wait until the 12V reached or timeout

  if (!Quiet)
     {
     Serial.print(F("Reset Voltage: ")); Print_12V_Value(analogRead(PIN_HV_AD_12V), 0);
     if (millis() >= TimeOut)
        Serial.println(F("Timeout generating reset voltage!\n"
                         "Hope that the actual voltage works also."));
     }
  delayMicroseconds(10);
  pinMode(PIN_HV_SDO, INPUT);      // Set SDO to input
  delayMicroseconds(300);
}

//-----------------------------------------------------------
void Write_or_Check_Reset_Fuse(int8_t Mode, uint8_t ProgMode)
//-----------------------------------------------------------
//  1 = Normal reset function
//  0 = Reset pin as IO pin
// -1 = Check the fuse
//  2 = Default fuses                                                                                         // 07.02.20:
// The persistent reset flags
//   Pers.Flags.ResetNorm
//   Pers.Flags.ResetAsIO
// which controls the blue and white LED are set.
//
// If ProgMode is enabled no serial messages are generated
// and end_pmode() is not called
{
  FuseReset_setup();
  HV_Reset(ProgMode);
  Set_LED_Error(0);                                                                                           // 07.02.20:
  Set_LED_Reset_as_IO(0);                                                                                     //   "
  Set_LED_Norm_ResetF(0);                                                                                     //   "
  Set_Error_Flag(0);
  Chip_Data_T Chip_Data;
  Chip_Data.Signatur = readSignature();
  if (!ProgMode)
     {
     Print_Signatur_and_ChipName(Chip_Data.Signatur);
     if (Mode == -1)
          Serial.print(F("Act Fuses:"));                                                                     // 07.02.20:
     else Serial.print(F("Old Fuses:"));
     Read_and_Print_Fuses(Chip_Data);
     }
  uint8_t Rstdisbl_Mask = 0;
  switch (Chip_Data.Signatur)
    {
    //case ATTINY13: ??
    case ATTINY24:
    case ATTINY44:
    case ATTINY84:
    case ATTINY25:
    case ATTINY45:
    case ATTINY85: Rstdisbl_Mask = B10000000; break;
    default: if (!ProgMode) printf("Unsupported chip\n");
             Set_Error_Flag(1);
    }

  if (Rstdisbl_Mask) // Known chip ?
       {
       uint8_t hfuse;
       switch (Mode)
          {
          case  1: hfuse = Chip_Data.hfuse |  Rstdisbl_Mask; break; // Set   RSTDISBL fuse
          case  0: hfuse = Chip_Data.hfuse & ~Rstdisbl_Mask; break; // Clear RSTDISBL fuse
          case  2: hfuse = 0xDF;                             break; // Default for ATTiny?4 and ATTiny?5      // 07.02.20:
          default: hfuse = Chip_Data.hfuse;                         // don't change the fuse just read it
          }

       if (hfuse != Chip_Data.hfuse || Mode == 2)
            {
            writeFuse(HFUSE, hfuse);
            if (!ProgMode) { printf("New Fuses:"); Read_and_Print_Fuses(Chip_Data);}
            if (hfuse != Chip_Data.hfuse) Set_Error_Flag(1);
            }
       else if (Mode != -1 && !ProgMode) printf("Fuse already o.k.\n");

       if (Mode == 2) writeFuse(LFUSE, 0x62); // Default for ATTiny?4 and ATTiny?5                            // 07.02.20:

       // Set the LED flags
       if (Chip_Data.hfuse & Rstdisbl_Mask)
            Set_Reset_Flag(1);  // 1: Reset pin acts as reset pin
       else Set_Reset_Flag(2);  // 2: reset pin as IO pin
       }
  else Set_Reset_Flag(0);       // 0: Clear both flags

  digitalWrite(PIN_HV_SCI, LOW);
  digitalWrite(PIN_POWER,  LOW);    // Vcc Off
  onOff = 0;                        // 12v Off
  delay(50);                                                                                                  // 07.02.20:
  Save_Persist_Data();
  end_pmode();
  digitalWrite(PIN_POWER, HIGH);    // Vcc on to be able to program it again                                  // 07.02.20: Without this programming was not possible if the button was pressed. is also turned on in setup()
  digitalWrite(PIN_RESET, LOW);     // Turn off the reset transistor                                          //    "
}


//----------------------------------------
void Check_HV_Reset_Button(uint8_t Button)
//----------------------------------------
{
static uint8_t   Old_Button = 17;
static uint32_t  Start_Press = 0;
  if (Button == 1 && Old_Button == 0)
       { // Button pressed
       Set_LED_Read(1);
       Start_Press = millis();
       Serial.println(F("\nReset Pin:\n"
                        " Short press:    Check the actual reset fuse\n"
                        " > 0.5 secs:     Activate normal reset function\n"
                        " Hold 2 seconds: Program reset pin as output\n"
                        " Hold 4 seconds: Program default fuses\n"));
       delay(50); // for debouncing
       }
  else {
       uint32_t PressTime = millis() - Start_Press;
       if (PressTime > 500 && PressTime < 550) Set_LED_Read(0);
       // Check if the button has been releleased
       bool LongPressed = Start_Press && (PressTime > 2000);
       if ((Button == 0 && Old_Button == 1) || (Button == 1 && LongPressed))
          { // Button released or pressed longer than 2 seconds
          if (!LongPressed)
               {
               if (PressTime > 500)
                    {
                    Serial.println(F("*** Activating normal function of the RESET Pin ***\n"));
                    Write_or_Check_Reset_Fuse(1, 0);
                    }
               else {
                    Serial.println(F("*** Check reset fuse ***\n"));
                    Write_or_Check_Reset_Fuse(-1, 0);
                    delay(500-PressTime); // delay to see the Read LED
                    }
               }
          else {
               Serial.println(F("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n"
                                "!!! Programming reset as output !!!\n"
                                "!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!\n\n"));
               //onOff = 1; // Das wird doch in HV_Reset() gemacht
               uint32_t t, Start;
               t = Start = millis();
               while(Read_Buttons_and_Jumper() == 3)
                 {
                 Upd_LED_Heartbeat();// Wait until the button is released
                 if (millis() - t > 250) // Flash To indicate that the button could be released
                    {
                    Set_LED_Reset_as_IO(!Get_LED_Reset_as_IO());
                    if (millis() - Start > 4000) Set_LED_Norm_ResetF(!Get_LED_Reset_as_IO());                 // 07.02.20:
                    t = millis();
                    }
                 }
               onOff = 0;
               if (millis() - Start > 4000)                                                                   // 07.02.20:
                    {
                    Serial.println(F("*** Writing default fuses ***\n"));
                    Write_or_Check_Reset_Fuse(2, 0); // Write default configuration in case some thing was wrong
                    }
               else Write_or_Check_Reset_Fuse(0, 0);
               }
          Button = 17; // is stored below to Old_Button.
          Set_LED_Read(0);
          }
       }
  Old_Button = Button;
}

// 19.8 Programming Time for Flash when Using SPM
//
/* 22.07.19:
 Das lesen der Daten funktioniert nicht wenn die Fuses des Tinys so gesetzt sind,
 dass dieser mit 128 KHz laeuft ;-(
 Timing laut "21.9 High-voltage Serial Programming Characteristics"
 Die hier angegebenen Zeiten sind im Bereich von 50ns
 Bei 16 MHz dauert eine Instruktion des UNO 62.5ns
 => Eigentlich muss nicht gewartet werden
 Trotzdem wird immer eine 1 gelesen

 Der Tiny kann momentan nur dann wieder aktiviert werden indem die Folgende Zeile im
 Tiny_UniProg.ino Programm aktiviert wird:
   #define SPI_CLOCK     ( 128000/6)  // 128 kHz
 und der Bootloade mit 1 oder 8 MHz neu geschrieben wird.
*/
#define TEST_DELAY // delayMicroseconds(1);

//---------------------------------
byte shiftOut(byte val1, byte val2)
//---------------------------------
{
  int inBits = 0;
  uint32_t Timeout = millis() + 300;
  if (shiftOut_Timeout) return 0;
  while (!digitalRead(PIN_HV_SDO) && millis() < Timeout)
    ; //Wait until SDO goes high
  if (millis()>=Timeout)
     {
     printf("Timeout in shiftOut\n");
     shiftOut_Timeout = 1;
     return 0;
     }

  unsigned int dout = (unsigned int) val1 << 2;
  unsigned int iout = (unsigned int) val2 << 2;
  for (int ii = 10; ii >= 0; ii--)
      {
      TEST_DELAY;                                                                                             // 22.07.19:
      digitalWrite(PIN_HV_SDI, !!(dout & (1 << ii)));
      digitalWrite(PIN_HV_SII, !!(iout & (1 << ii)));
      TEST_DELAY;                                                                                             // 22.07.19:
      inBits <<= 1;
      inBits |= digitalRead(PIN_HV_SDO);                   // Hier wird immer 1 gelesen ;-(
      TEST_DELAY;                                                                                             // 22.07.19:
      digitalWrite(PIN_HV_SCI, HIGH);
      TEST_DELAY;                                                                                             // 22.07.19:
      digitalWrite(PIN_HV_SCI, LOW);
      }
  TEST_DELAY;                                                                                                 // 22.07.19:
  return inBits >> 2;
}

//-----------------------------------------
void writeFuse(unsigned int fuse, byte val)
//-----------------------------------------
{
  shiftOut_Timeout = 0;
  shiftOut(0x40, 0x4C);
  shiftOut( val, 0x2C);
  shiftOut(0x00, (byte) (fuse >> 8));
  shiftOut(0x00, (byte) fuse);
}

//--------------------------------------
void Read_Fuses(Chip_Data_T &Chip_Data)
//--------------------------------------
// LFuse: E2, HFuse: DF, EFuse: FF

{
  shiftOut_Timeout = 0;
  shiftOut(0x04, 0x4C);  // LFuse
  shiftOut(0x00, 0x68);
  Chip_Data.lfuse = shiftOut(0x00, 0x6C);

  shiftOut(0x04, 0x4C);  // HFuse
  shiftOut(0x00, 0x7A);
  Chip_Data.hfuse = shiftOut(0x00, 0x7E);

  shiftOut(0x04, 0x4C);  // EFuse
  shiftOut(0x00, 0x6A);
  Chip_Data.efuse= shiftOut(0x00, 0x6E);
}

//-----------------------------------------------
void Read_and_Print_Fuses(Chip_Data_T &Chip_Data)
//-----------------------------------------------
{
  Read_Fuses(Chip_Data);
  Print_Fuses(Chip_Data);
}


//---------------------------
unsigned int readSignature ()
//---------------------------
{
  unsigned int sig = 0;
  byte val;
  shiftOut_Timeout = 0;
  for (int ii = 1; ii < 3; ii++)
      {
      shiftOut(0x08, 0x4C);
      shiftOut(  ii, 0x0C);
      shiftOut(0x00, 0x68);
      val = shiftOut(0x00, 0x6C);
      sig = (sig << 8) + val;
      }
  return sig;
}

