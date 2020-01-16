/* Achtung: Ich habe drei verschieden ArduinoISP Programmier-Adapter gebaut:
 - High V. Prog               => ArduinoISP_Hardi.ino
 - Prog+CAN Debug             => ArduinoISP_CAN2ASCII.ino
 - Tiny_UniProg               => Tiny_UniProg.ini

 Sie verwenden unterschiedliche Pins !

 ToDo: Doku, Schaltplaene, ...

 Angepasste Version an das Layout des ATTinyFuseReset2
 Damit kann mit einer Hardware programmiert und Fuses gesetzt werden.



 Programmierung:
 - Werkzeuge/Board:       "Arduino/Genuino Uno"
 - Werkzeuge/Programmer:  "ArduinoISP"
 - CTRL+U

 LEDs:
 - Gruen:  Upd_LED_Heartbeat (Blinkt wenn aktiv)
 - Gelb:   Prog. Mode
 - Orange: Check Programm
 - Rot:    Error

 Danach kann ein ATTiny programmiert werden. Dazu muss Board und Programmer
 wieder umgestellt werden:
 - Werkzeuge/Board:       "ATTiny"
 - Wekkzeuge/Prozessor:   "ATTiny85"
 - Werkzeuge/Programmer:  "Arduino as ISP"


 -----------------------------------------
 Based on the ArduinoISP form Randall Bohn
 -----------------------------------------


 Revision History:
 ~~~~~~~~~~~~~~~~~
 16.04.19:  - Added serial feedback from the ATTiny over Pin 11
              ATTiny85 test program: C:\Dat\Maerklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ISP_mit_Serial_print\ATTiny85_Test\ATTiny85_Test.ino
 19.05.19:  - Starting Relais output
 04.06.19:  - Changed the programming pins to be able to use the Hardware SPI which is faster
              Adapted also the test hardware
 20.06.19:  - Release the reset pin at the end of the upload (Changed by mistake 30.06.19)
 11.07.19:  - Started implementing the Charliplexing LEDs
 13.07.19:  - Print the programming duration to the console. This feature uses the modul new Persist_Msg.h
              to be able to show messages after the restart of the program.
            - Reducing the Flash time by updating the programming LED without delay (10.5 -> 8 sec)
 15.07.19:  - Using the old state of the other fuses when changing the reset fuse.
              The Reset fuse is not written it it already has the requested state.
            - Improved the Persist_Msg module (Calulate CRC over additional data)
            - Reset mode LEDs are working.
              White: Reset pin = reset
              Blue:  Reset pin = IO pin
              The state is stored in the persistent RAM.
 16.07.19:  - Fuses could be set/checked without resetting the program at the end
            - Check the Reset fuse at power on
 22.07.19:  - Fuses are printed as text
 23.07.19:  - Tiny Reset button implemented23.07.19:
 24.07.19:  - Print version and compile Date
 07.01.20:  - Detect if the Debug hardware is available and disable the software serial port if not
              because otherwise the Charliplexing module generates to much interrupts which cause the
              Reset pin LEDs to flicker

 Todo:
 ~~~~~
 - Funktioniert die Taster Erkennung mit 1K und 10K ? See: "// Wrong R2"

*/

#include "Arduino.h"

#include "ATTinyFuseReset2.h"

#include "LEDs.h"


#undef SERIAL
// Serial print for the ATTiny over one signal from ATTiny to the Uno                                         // 16.04.19:
// Copied and adapted form
//   https://www.arduinoforum.de/arduino-Thread-Serial-Monitor-als-Debughilfe-f%C3%BCr-den-ATtiny85-und-andere-ATMEL-Mikrocontroller
// Orignal code:
//   C:\Dat\Maerklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ISP_mit_Serial_print
// This program/HW uses different pins to connect the ATTiny and the Uno
//           Original   This Prog/High V. Prog hardware
//  ATTiny   3          7
//  Uno      4          11

#define ATTINY_FEEDBACK
#ifdef ATTINY_FEEDBACK
  #define SOFTSERIAL_BAUD  9600

  #define SOFT_RX_PIN 13
  #define SERIAL_PIN_STR "Serial feedback 9600 Baud from ATTiny (PIN 13) => Set S.Debug2 jumper =3 on TiniUniProg"
  #include <SoftwareSerial.h>                             // Tiny85 Feedback
  SoftwareSerial softSerial(SOFT_RX_PIN, 99); // RX, TX   // Tiny85 Feedback, 99=Dummy weil TX nicht benoetigt wird
  unsigned long softserialtime;                           // Tiny85 Feedback
  #define toSoftSerial 1000UL                             // Tiny85 Feedback
  boolean softSerialAktiv = false;                        // Tiny85 Feedback
#else
  #define SERIAL_PIN_STR
#endif

#include "Printf.h"

/*
 Configure SPI clock (in Hz).
 E.g. for an attiny @128 kHz: the datasheet states that both the high
 and low spi clock pulse must be > 2 cpu cycles, so take 3 cycles i.e.
 divide target f_cpu by 6:
     #define SPI_CLOCK            (128000/6) // = 21.333 kHz

 A clock slow enough for an attiny85 @ 1MHz, is a reasonable default: 1000000/6

 Laut einem #if im original Programm kan der HW SPI nur verwendet werden
 wenn SPI_CLOCK > (F_CPU / 128) = 125 KHz


*/
#define SPI_CLOCK       (1000000/6)  // 1 or 8 MHz  Normal line
//#define SPI_CLOCK     ( 128000/6)  // 128 kHz     Enable this line if the Target is configured to 128 kHz

#if SPI_CLOCK > (F_CPU / 128) // 125 KHz
  #define USE_HARDWARE_SPI // hardware SPI can only be used if the SPI_CLOCK is > 125 KHz
#endif
/*
 Tests with ATTiny85_Servo.ino @ 8MHz cpu clock                                                               13.07.19:
 1000000/6 =  166666: 7979 ms
 4000000/6 =  666666: 77059 ms
 8000000/6 = 1333333: 6914 ms    *
 8000000/4 = 2000000: 6824 ms
 8000000/3 = 2666666: 6827 ms
 8000000/2 = 4000000: Fehler

 If the ATTiny is configured @ 1MHz it dosn't work with a SPI clock of 8000000/6 ;-(
 => use 1000000/6

 ToDo:
 Use the SPI Clock 8000000/6 if a CPU clock of 8MHz is detected

 Aus original Programm: "hardware SPI is probably too fast anyway."
*/



#include "Global_Defines.h"


// Configure the serial port to use.
//
// Prefer the USB virtual serial port (aka. native USB port), if the Arduino has one:
//   - it does not autoreset (except for the magic baud rate of 1200).
//   - it is more reliable because of USB handshaking.
//
// Leonardo and similar have an USB virtual serial port: 'Serial'.
// Due and Zero have an USB virtual serial port: 'SerialUSB'.
//
// On the Due and Zero, 'Serial' can be used too, provided you disable autoreset.
// To use 'Serial': #define SERIAL Serial

#ifdef SERIAL_PORT_USBVIRTUAL
  #define SERIAL SERIAL_PORT_USBVIRTUAL
#else
  #define SERIAL Serial
#endif


#define HWVER 2
#define SWMAJ 1
#define SWMIN 18

// STK Definitions
#define STK_OK      0x10
#define STK_FAILED  0x11
#define STK_UNKNOWN 0x12
#define STK_INSYNC  0x14
#define STK_NOSYNC  0x15
#define CRC_EOP     0x20 //ok it is a space...

#ifdef USE_HARDWARE_SPI // Hardware SPI is used
  #include "SPI.h"
#else
  #define SPI_MODE0 0x00

  class SPISettings {
  public:
    // clock is in Hz
    SPISettings(uint32_t clock, uint8_t bitOrder, uint8_t dataMode) : clock(clock){
      (void) bitOrder;
      (void) dataMode;
    };

  private:
    uint32_t clock;

  friend class BitBangedSPI;
  };

  class BitBangedSPI {
  public:
    void begin() {
      digitalWrite(PIN_SCK, LOW);
      digitalWrite(PIN_MOSI, LOW);
      pinMode(PIN_SCK, OUTPUT);
      pinMode(PIN_MOSI, OUTPUT);
      pinMode(PIN_MISO, INPUT);
    }

    void beginTransaction(SPISettings settings) {
      pulseWidth = (500000 + settings.clock - 1) / settings.clock;
      if (pulseWidth == 0)
        pulseWidth = 1;
    }

    void end()
    {
    }

    uint8_t transfer (uint8_t b) {
      for (unsigned int i = 0; i < 8; ++i) {
        digitalWrite(PIN_MOSI, (b & 0x80) ? HIGH : LOW);
        digitalWrite(PIN_SCK, HIGH);
        delayMicroseconds(pulseWidth);
        b = (b << 1) | digitalRead(PIN_MISO);
        digitalWrite(PIN_SCK, LOW); // slow pulse
        delayMicroseconds(pulseWidth);
      }
      return b;
    }

  private:
    unsigned long pulseWidth; // in microseconds
  };

  static BitBangedSPI SPI;

#endif // USE_HARDWARE_SPI


int error = 0;
int pmode = 0;
// address for reading and writing, set by 'U' command
unsigned int here;
uint8_t buff[256]; // global block storage

typedef struct param {
  uint8_t  devicecode;
  uint8_t  revision;
  uint8_t  progtype;
  uint8_t  parmode;
  uint8_t  polling;
  uint8_t  selftimed;
  uint8_t  lockbytes;
  uint8_t  fusebytes;
  uint8_t  flashpoll;
  uint16_t eeprompoll;
  uint16_t pagesize;
  uint16_t eepromsize;
  uint32_t flashsize;
} parameter_T;

typedef struct
  {
  uint8_t Error:1;
  uint8_t ResetNorm:1;
  uint8_t ResetAsIO:1;
  } Pers_Flags_T;

typedef struct
  {
  parameter_T  param;
  Pers_Flags_T Flags;
  } Pers_Dat_T;

Pers_Dat_T __attribute__((section(".noinit"))) Pers; // Data stored in the persistent memory which is not initialiced at program start

//#define USE_DELAYED_CHECK_FUSES
#ifdef USE_DELAYED_CHECK_FUSES
  uint8_t Delayed_Check_Fuses = 0;
#endif

uint8_t DebugHardwareAvailable;

#include "Persist_Msg.h"


//-------------------------------
void Set_Reset_Flag(uint8_t Flag)
//-------------------------------
// 0: Clear both flags
// 1: Reset pin acts as reset pin
// 2: reset pin as IO pin
{
  Pers.Flags.ResetNorm = 0;
  Pers.Flags.ResetAsIO = 0;
  switch (Flag)
    {
    case 1: Pers.Flags.ResetNorm = 1; break;
    case 2: Pers.Flags.ResetAsIO = 1; break;
    }
}

//-------------------------------
void Set_Error_Flag(uint8_t Flag)
//-------------------------------
{
  error = Pers.Flags.Error = Flag;
}

//----------------------
void Save_Persist_Data()
//----------------------
{
  Save_Persist_Checksum(&Pers, sizeof(Pers));
}

//----------
void setup()
//----------
{
  SERIAL.begin(BAUDRATE);
  Serial.println(F("ArduinoISP Tiny_UniProg by Hardi   V.1.0\t" __DATE__ " " __TIME__));

  pinMode(RELAIS_RST_PIN, INPUT_PULLUP);                                                                      // 07.01.20:
  DebugHardwareAvailable = digitalRead(RELAIS_RST_PIN) == 0; // If the Relais are available the Pulldown resistor for the FETs pull the pin to gnd

  #ifdef ATTINY_FEEDBACK
    if (DebugHardwareAvailable)                                                                               // 07.01.20:
         Serial.println(F(SERIAL_PIN_STR)); // Don't use printf because the string is to long
    else Serial.println(F("No debug hardware detected"));
  #endif

  #ifndef USE_HARDWARE_SPI
     printf("Software SPI\n");
  #endif


  if (Check_Persist_Checksum(&Pers, sizeof(Pers)))
       {
       error = Pers.Flags.Error;
       //Debug_Print_Parameters(); // Debug
       Serial.println(Get_Persist_Msg());
       }
  else {
       memset(&Pers, 0, sizeof(Pers));
       }
  Pers.Flags.Error = 0;


  Store_Persistent_Msg(""); // Clear the persistent message
  Save_Persist_Data();

  Setup_LEDs_with_Power_on_Ani(HV_Interrupt_Proc);

  pinMode(PIN_POWER,      OUTPUT);  digitalWrite(PIN_POWER, HIGH);
  pinMode(RELAIS_PWR_PIN, OUTPUT);  digitalWrite(RELAIS_PWR_PIN, LOW);
  pinMode(RELAIS_PRG_PIN, OUTPUT);  digitalWrite(RELAIS_PRG_PIN, LOW);
  pinMode(RELAIS_RST_PIN, OUTPUT);  digitalWrite(RELAIS_RST_PIN, LOW);

  pinMode(HV_RESET_BUTTON_PIN, INPUT_PULLUP);

  // *** Important for ATTinyFuseReset2.cpp ***
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

  #ifdef USE_HARDWARE_SPI /// The HV programming can't be used with slow target clock rates
  // Check the reset fuse of the target at power on and activate the blue or white LED
  if (Pers.Flags.ResetNorm == 0 && Pers.Flags.ResetAsIO == 0)
     {
     #ifdef USE_DELAYED_CHECK_FUSES
        Delayed_Check_Fuses = 1;
     #else
        // If the Uno is plugged to the PC the reset is pulled after one second and released 4 seconds later
        // The following delay prevents starting the Write_or_Check_Reset_Fuse() in the first second.
        for (uint8_t i = 0; i < 5; i++)
           { Set_LED_Read(1); delay(200); Set_LED_Read(0); delay(200); }

        Write_or_Check_Reset_Fuse(-1, 0); // Check the fuse
        if (error)
           Write_or_Check_Reset_Fuse(-1, 0); // second try in case of an error
     #endif // USE_DELAYED_CHECK_FUSES
     }
  #endif // USE_HARDWARE_SPI
}



#define beget16(addr) (*addr * 256 + *(addr+1) )


static bool rst_active_high;

//---------------------------
void reset_target(bool reset)
//---------------------------
{
  digitalWrite(PIN_RESET, ((reset && rst_active_high) || (!reset && !rst_active_high)) ? HIGH : LOW);
}

/*
   ---,  5V
      |                            1023 * Rx
     ,-,                         ------------- = Mess
     | | R_PullUp                R_PullUp + Rx
     | | 20-50K
     '-'
      |
      *------- U_Mess
      |
     ,-,
     | |
     | | 47K, 10K, 2.2K
     '-'
      |
     ---

*/
//-------------------------------
uint8_t Read_Buttons_and_Jumper()
//-------------------------------
{
  int val = analogRead(HV_RESET_BUTTON_PIN);
#if 1 // Wrong R2
  if (val < 20)  return 3;
  if (val < 40)  return 1;  // Wrong R2 Resistor 1K instead of 10K
#else
  if (val < 30)  return 3;
#endif
  if (val < 150) return 2;
  if (val < 400) return 1;
  if (val < 800) return 9;

  return 0;
}


//-------------
void loop(void)
//-------------
{
  Set_LED_Error(error);
  Set_LED_Norm_ResetF(Pers.Flags.ResetNorm);
  Set_LED_Reset_as_IO(Pers.Flags.ResetAsIO);


  #ifdef ATTINY_FEEDBACK                                                                                      // 16.04.19:
    if (DebugHardwareAvailable && (millis() > (softserialtime + toSoftSerial)) && !softSerialAktiv)           // 07.01.20:  Added: DebugHardwareAvailable
       {
       softSerial.begin(SOFTSERIAL_BAUD);
       softSerial.flush();
       softSerialAktiv = true;
       }

    Upd_LED_Heartbeat();

    if (Serial.available())
       {
       softserialtime = millis();
       softSerial.end();
       softSerialAktiv = false;
       avrisp();
       }

    if (softSerial.available() && !pmode && softSerialAktiv)
       Serial.write(softSerial.read());
  #else
    // light the heartbeat LED
    Upd_LED_Heartbeat();
    if (SERIAL.available())
       avrisp();
  #endif

  #ifdef USE_DELAYED_CHECK_FUSES
  if (Delayed_Check_Fuses && millis() > 8000)
       {
       Delayed_Check_Fuses = 0;
       Check_HV_Reset_Button(1);
       }
  #endif // USE_DELAYED_CHECK_FUSES

  uint8_t Buttons = Read_Buttons_and_Jumper();

  Check_HV_Reset_Button(Buttons == 3);

  // Reset Tiny button
  static uint8_t OldResetTiny_Btn = 0;
  uint8_t ResetTiny_Btn = (Buttons == 2);
  if (ResetTiny_Btn != OldResetTiny_Btn)
     {
     digitalWrite(RELAIS_PWR_PIN, ResetTiny_Btn);
     digitalWrite(PIN_POWER,     !ResetTiny_Btn);
     OldResetTiny_Btn = ResetTiny_Btn;
     }
}

//-------------
uint8_t getch()
//-------------
{
  while (!SERIAL.available());
  return SERIAL.read();
}

//--------------
void fill(int n)
//--------------
{
  for (int x = 0; x < n; x++) {
    buff[x] = getch();
  }
}


//-----------------------------------------------------------------
uint8_t spi_transaction(uint8_t a, uint8_t b, uint8_t c, uint8_t d)
//-----------------------------------------------------------------
{
  SPI.transfer(a);
  SPI.transfer(b);
  SPI.transfer(c);
  return SPI.transfer(d);
}

//----------------
void empty_reply()
//----------------
{
  if (CRC_EOP == getch())
       {
       SERIAL.print((char)STK_INSYNC);
       SERIAL.print((char)STK_OK);
       }
  else {
       error++;
       SERIAL.print((char)STK_NOSYNC);
       }
}

//--------------------
void breply(uint8_t b)
//--------------------
{
  if (CRC_EOP == getch())
       {
       SERIAL.print((char)STK_INSYNC);
       SERIAL.print((char)b);
       SERIAL.print((char)STK_OK);
       }
  else {
       error++;
       SERIAL.print((char)STK_NOSYNC);
       }
}

//-------------------------
void get_version(uint8_t c)
//-------------------------
{
  switch (c)
    {
    case 0x80:  breply(HWVER); break;
    case 0x81:  breply(SWMAJ); break;
    case 0x82:  breply(SWMIN); break;
    case 0x93:  breply('S');   break; // serial programmer
    default:    breply(0);
    }
}

//-------------------
void set_parameters()
//-------------------
{
  // call this after reading paramter packet into buff[]
  Pers.param.devicecode = buff[0];
  Pers.param.revision   = buff[1];
  Pers.param.progtype   = buff[2];
  Pers.param.parmode    = buff[3];
  Pers.param.polling    = buff[4];
  Pers.param.selftimed  = buff[5];
  Pers.param.lockbytes  = buff[6];
  Pers.param.fusebytes  = buff[7];
  Pers.param.flashpoll  = buff[8];
  // ignore buff[9] (= buff[8])
  // following are 16 bits (big endian)
  Pers.param.eeprompoll = beget16(&buff[10]);
  Pers.param.pagesize   = beget16(&buff[12]);
  Pers.param.eepromsize = beget16(&buff[14]);

  // 32 bits flashsize (big endian)
  Pers.param.flashsize = buff[16] * 0x01000000
                  + buff[17] * 0x00010000
                  + buff[18] * 0x00000100
                  + buff[19];

  // avr devices have active low reset, at89sx are active high
  #ifdef RESET_INV                                               // Hardi
    rst_active_high = (Pers.param.devicecode <  0xe0);
  #else
    rst_active_high = (Pers.param.devicecode >= 0xe0);
  #endif
  // 17.07.19:  Ein High aktiver reset geht doch gar nicht mit dem Transistor der im TinyUniProg verwendet wird
  //            => Der at89sx kann nicht gehen
}

//---------------------------
void Debug_Print_Parameters()
//---------------------------
{
  printf("devicecode: %i\n", Pers.param.devicecode);
  printf("revision  : %i\n", Pers.param.revision  );
  printf("progtype  : %i\n", Pers.param.progtype  );
  printf("parmode   : %i\n", Pers.param.parmode   );
  printf("polling   : %i\n", Pers.param.polling   );
  printf("selftimed : %i\n", Pers.param.selftimed );
  printf("lockbytes : %i\n", Pers.param.lockbytes );
  printf("fusebytes : %i\n", Pers.param.fusebytes );
  printf("flashpoll : %i\n", Pers.param.flashpoll );
  printf("eeprompoll: %i\n", Pers.param.eeprompoll);
  printf("pagesize  : %i\n", Pers.param.pagesize  );
  printf("eepromsize: %i\n", Pers.param.eepromsize);
  printf("flashsize : %i\n", Pers.param.flashsize );
}

//------------------
void Enable_Relais()
//------------------
{
  uint8_t Wait = digitalRead(RELAIS_PWR_PIN) == 0 || digitalRead(RELAIS_PRG_PIN) == 0 || digitalRead(RELAIS_RST_PIN) == 0;
  digitalWrite(RELAIS_PWR_PIN, HIGH);                                                                         // 31.05.19:  27.06.19:  Old LOW
  digitalWrite(RELAIS_PRG_PIN, HIGH);                                                                         // 31.05.19:  27.06.19:
  digitalWrite(RELAIS_RST_PIN, HIGH);                                                                         // 31.05.19:  27.06.19:
  if (Wait) delay(300);                                                                                                 //   "
}

//----------------
void start_pmode()
//----------------
{
  Enable_Relais();

  // Reset target before driving PIN_SCK or PIN_MOSI

  // SPI.begin() will configure SS as output,
  // so SPI master mode is selected.
  // We have defined PIN_RESET as pin 10,
  // which for many arduino's is not the SS pin.
  // So we have to configure PIN_RESET as output here,
  // (reset_target() first sets the correct level)

  reset_target(true);
  pinMode(PIN_RESET, OUTPUT);
  SPI.begin();
  SPI.beginTransaction(SPISettings(SPI_CLOCK, MSBFIRST, SPI_MODE0));

  // See avr datasheets, chapter "SERIAL_PRG Programming Algorithm":

  // Pulse PIN_RESET after PIN_SCK is low:
  digitalWrite(PIN_SCK, LOW);
  delay(20); // discharge PIN_SCK, value arbitrally chosen
  reset_target(false);
  // Pulse must be minimum 2 target CPU clock cycles
  // so 100 usec is ok for CPU speeds above 20KHz
  delayMicroseconds(100);
  reset_target(true);

  // Send the enable programming command:
  delay(50); // datasheet: must be > 20 msec
  spi_transaction(0xAC, 0x53, 0x00, 0x00);
  pmode = 1;
  Set_LED_Prog(1);
}

//--------------
void end_pmode()
//--------------
{
  SPI.end();
  // We're about to take the target out of reset
  // so configure SPI pins as input
  pinMode(PIN_MOSI, INPUT);
  pinMode(PIN_SCK,  INPUT);
  reset_target(false);
  #ifndef RESET_INV                                                                                           // 20.06.19:  Only without inverting reset transistor
    pinMode(PIN_RESET, INPUT);
  #endif
  digitalWrite(RELAIS_PWR_PIN, LOW);                                                                         // 31.05.19:  27.06.19:  Old HIGH
  digitalWrite(RELAIS_PRG_PIN, LOW);                                                                         // 31.05.19:  27.06.19:
  digitalWrite(RELAIS_RST_PIN, LOW);                                                                         // 31.05.19:  27.06.19:
  pmode = 0;
  Set_LED_Prog(0);
  Set_LED_Read(0);
}

//--------------
void universal()
//--------------
{
  uint8_t ch;

  fill(4);
  ch = spi_transaction(buff[0], buff[1], buff[2], buff[3]);
  breply(ch);
}

//-------------------------------------------------------
void flash(uint8_t hilo, unsigned int addr, uint8_t data)
//-------------------------------------------------------
{
  spi_transaction(0x40 + 8 * hilo,
                  addr >> 8 & 0xFF,
                  addr & 0xFF,
                  data);
}

//----------------------------
void commit(unsigned int addr)
//----------------------------
{
  Set_LED_Prog(Get_LED_Prog() == 0); // Toggle the programing LED
  spi_transaction(0x4C, (addr >> 8) & 0xFF, addr & 0xFF, 0);
}

//-------------------------
unsigned int current_page()
//-------------------------
{
  if (Pers.param.pagesize == 32)  return here & 0xFFFFFFF0;
  if (Pers.param.pagesize == 64)  return here & 0xFFFFFFE0;
  if (Pers.param.pagesize == 128) return here & 0xFFFFFFC0;
  if (Pers.param.pagesize == 256) return here & 0xFFFFFF80;
  return here;
}

//--------------------------
void write_flash(int length)
//--------------------------
{
  fill(length);
  if (CRC_EOP == getch())
       {
       SERIAL.print((char) STK_INSYNC);
       SERIAL.print((char) write_flash_pages(length));
       }
  else {
       error++;
       SERIAL.print((char) STK_NOSYNC);
       }
}

//-----------------------------------
uint8_t write_flash_pages(int length)
//-----------------------------------
{
  int x = 0;
  unsigned int page = current_page();
  while (x < length) {
    if (page != current_page()) {
      commit(page);
      page = current_page();
    }
    flash(LOW, here, buff[x++]);
    flash(HIGH, here, buff[x++]);
    here++;
  }

  commit(page);
  return STK_OK;
}

#define EECHUNK (32)
//---------------------------------------
uint8_t write_eeprom(unsigned int length)
//---------------------------------------
{
  // here is a word address, get the byte address
  unsigned int start = here * 2;
  unsigned int remaining = length;
  if (length > Pers.param.eepromsize)
    {
    error++;
    return STK_FAILED;
    }
  while (remaining > EECHUNK)
    {
    write_eeprom_chunk(start, EECHUNK);
    start += EECHUNK;
    remaining -= EECHUNK;
    }
  write_eeprom_chunk(start, remaining);
  return STK_OK;
}

// write (length) bytes, (start) is a byte address
//-----------------------------------------------------------------
uint8_t write_eeprom_chunk(unsigned int start, unsigned int length)
//-----------------------------------------------------------------
{
  // this writes byte-by-byte,
  // page writing may be faster (4 bytes at a time)
  fill(length);
  Set_LED_Prog(Get_LED_Prog() == 0); // Toggle the programing LED
  for (unsigned int x = 0; x < length; x++) {
    unsigned int addr = start + x;
    spi_transaction(0xC0, (addr >> 8) & 0xFF, addr & 0xFF, buff[x]);
    delay(30);
  }
  return STK_OK;
}

//-----------------
void program_page()
//-----------------
{
  char result = (char) STK_FAILED;
  unsigned int length = 256 * getch();
  length += getch();
  char memtype = getch();
  // flash memory @here, (length) bytes
  if (memtype == 'F') {
    write_flash(length);
    return;
  }
  if (memtype == 'E') {
    result = (char)write_eeprom(length);
    if (CRC_EOP == getch()) {
      SERIAL.print((char) STK_INSYNC);
      SERIAL.print(result);
    } else {
      error++;
      SERIAL.print((char) STK_NOSYNC);
    }
    return;
  }
  SERIAL.print((char)STK_FAILED);
  return;
}

//-------------------------------------------------
uint8_t flash_read(uint8_t hilo, unsigned int addr)
//-------------------------------------------------
{
  return spi_transaction(0x20 + hilo * 8,
                         (addr >> 8) & 0xFF,
                         addr & 0xFF,
                         0);
}

//------------------------------
char flash_read_page(int length)
//------------------------------
{
  for (int x = 0; x < length; x += 2) {
    uint8_t low = flash_read(LOW, here);
    SERIAL.print((char) low);
    uint8_t high = flash_read(HIGH, here);
    SERIAL.print((char) high);
    here++;
  }
  return STK_OK;
}

//-------------------------------
char eeprom_read_page(int length)
//-------------------------------
{
  // here again we have a word address
  int start = here * 2;
  for (int x = 0; x < length; x++) {
    int addr = start + x;
    uint8_t ee = spi_transaction(0xA0, (addr >> 8) & 0xFF, addr & 0xFF, 0xFF);
    SERIAL.print((char) ee);
  }
  return STK_OK;
}

//--------------
void read_page()
//--------------
{
  char result = (char)STK_FAILED;
  int length = 256 * getch();
  length += getch();
  char memtype = getch();
  if (CRC_EOP != getch()) {
    error++;
    SERIAL.print((char) STK_NOSYNC);
    return;
  }
  SERIAL.print((char) STK_INSYNC);
  if (memtype == 'F') result = flash_read_page(length);
  if (memtype == 'E') result = eeprom_read_page(length);
  SERIAL.print(result);
  Set_LED_Read(Get_LED_Read() == 0); // Toggle the programing LED
}

//-------------------
void read_signature()
//-------------------
{
  if (CRC_EOP != getch()) {
    error++;
    SERIAL.print((char) STK_NOSYNC);
    return;
  }
  SERIAL.print((char) STK_INSYNC);
  uint8_t high = spi_transaction(0x30, 0x00, 0x00, 0x00);
  SERIAL.print((char) high);
  uint8_t middle = spi_transaction(0x30, 0x00, 0x01, 0x00);
  SERIAL.print((char) middle);
  uint8_t low = spi_transaction(0x30, 0x00, 0x02, 0x00);
  SERIAL.print((char) low);
  SERIAL.print((char) STK_OK);
}
//////////////////////////////////////////
//////////////////////////////////////////


////////////////////////////////////
////////////////////////////////////

/*
// Write Bootloader:
 '0' '0' 'A' 'A' 'B' 'E' 'P' 'u' 'V' 'A' 'A' 'B' 'E' 'P' 'U' 'd' 'U' 't' 'Q' Pro
                          |   |   |   |   |   |   |   |   |   |   |   |
                          |   |   |   |   |   |   |   |   |   |   |   read_page();
                          |   |   |   |   |   |   |   |   |   |   set address (word)
                          |   |   |   |   |   |   |   |   |   program_page
                          |   |   |   |   |   |   |   |   set address (word)
                          |   |   |   |   |   |   |   Start programming mode
                          |   |   |   |   |   |   extended parameters - ignore for now
                          |   |   |   |   /   set_parameters
                          |   |   |   get_version
                          |   |   universal function 4 SPI Bytes
                          |   read_signature
                          Start programming mode
*/

//-----------
void avrisp()
//-----------
{
  static uint32_t Start;
  static uint8_t  Last_ch = 0;
  uint8_t ch = getch();
  Enable_Relais();
  //Add_Persistent_Msg("'%c' ", ch, ch); // Debug
  switch (ch)
    {
    case '0':  // signon
               error = 0;
               empty_reply();
               break;
    case '1':  if (getch() == CRC_EOP)
                    {
                    SERIAL.print((char) STK_INSYNC);
                    SERIAL.print("AVR ISP");
                    SERIAL.print((char) STK_OK);
                    }
               else {
                    error++;
                    SERIAL.print((char) STK_NOSYNC);
                    }
               break;
    case 'A':  get_version(getch()); break;
    case 'B':  fill(20);
               set_parameters();
               empty_reply();
               break;
    case 'E':  // extended parameters - ignore for now
               fill(5);
               empty_reply();
               break;
    case 'P':
               if (!pmode)
                 {
                 Start = millis();
                 //delay(5000);  // Test

                 /* Das geht noch nicht so
                 if (Get_LED_Reset_as_IO())
                    { // Activate the normal Reset function
                    Add_Persistent_Msg("Start Write_or_Check_Reset_Fuse");
                    Write_or_Check_Reset_Fuse(1, 1);
                    Add_Persistent_Msg("Res %i\n", Get_LED_Reset_as_IO());
                    Save_Persist_Data();
                    }
                 */

                 start_pmode();
                 }
               empty_reply();
               break;
    case 'U':  // set address (word)
               here = getch();
               here += 256 * getch();
               empty_reply();
               break;
    case 0x60: //STK_PROG_FLASH  "'"
               getch(); // low addr
               getch(); // high addr
               empty_reply();
               break;
    case 'a': //STK_PROG_DATA
               getch(); // data
               empty_reply();
               break;
    case 'd': //STK_PROG_PAGE
               program_page();
               break;
    case 't':  //STK_READ_PAGE
               read_page();
               break;

    case 'V':  //0x56
               universal();
               break;
    case 'Q':  //0x51
               if (error>0)
                    Add_Persistent_Msg("Error occurred (Last cmd: 0x%02X '%c'\n", Last_ch, Last_ch);
               else {
                    switch (Last_ch)
                      {
                      case 0x75: // 'u'
                                 error = -1; // Store negative to the persistent variable because it's not printed as general error above
                                 Add_Persistent_Msg("Error: Wrong signatur detected\n");
                                 break;
                      }
                    }
               end_pmode();
               empty_reply();
               // The serial monitor is deactivated if a file is downloaded. At the end the program
               // is restarted if the serial monitor was active.
               // => Messages can't be shown during flashing and not at the end.
               //    We use a special trick to store messages to a persistent RAM region and show them
               //    when the program is restarted.
               // The messages could also be read if the serial monitor is opened after the flash programming
               Add_Persistent_Msg("Prog. duration %i ms%s\n", millis() - Start); // This message is printed after the restart
               Pers.Flags.Error = error;
               Save_Persist_Data();
               break;
    case 'u':  //STK_READ_SIGN
               read_signature();
               break;

    // expecting a command, not CRC_EOP
    // this is how we can get back in sync
    case CRC_EOP: // ' '
               error++;
               SERIAL.print((char) STK_NOSYNC);
               break;

    default:   // anything else we will return STK_UNKNOWN
               error++;
               if (CRC_EOP == getch())
                    SERIAL.print((char)STK_UNKNOWN);
               else SERIAL.print((char)STK_NOSYNC);
    }
  Last_ch = ch;
}

