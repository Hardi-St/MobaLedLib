/*
 Based on the following project:
 https://arduino-projekte.webnode.at/meine-projekte/servosteuerung/servotest-attiny/


High V.                  ATTiny 85       Prog.
Prog.                    +-\/-+
       LED 2 Reset PB5  1|    |8  VCC
       LED 1 PWM   PB3  2|    |7  PB2    SCK      Servo 2   SoftwareSerial
LED    LED 0 PWM   PB4  3|    |6  PB1    MISO     Servo 1
                   GND  4|    |5  PB0    MOSI     Servo 0
                         +----+


Zu debugging zwecken kann auch der ATTiny 84 verwendet werden.

                         ATTiny 84
                         +-\/-+                     Pin numbers D0..D10 if pin mapping "Counterclockwise" is selected in the IDE
                   vcc  1|    |14 GND
 D0     Servo 0    PB0  2|    |13 PA0    LED 1 PWM  D10
 D1     Servo 1    PB1  3|    |12 PA1    LED 0 PWM  D9
        Reset      PB3  4|    |11 PA2    LED 2 PWM  D8
 D2     Servo 2    PB2  5|    |10 PA3               D7
 D3                PA7  6|    |9  PA4    SCK        D6    SoftwareSerial
 D4     MOSI       PA6  7|    |8  PA5    MISO       D5
                         +----+



 Revision History:
 ~~~~~~~~~~~~~~~~~
 23.04.19:  - Servo signal is turned off if input is not changed
 24.04.19:  - Using filtered pulsIn signal to detect the modes (Problem LowPass)
 25.04.19:  - Using the unfiltered pulsIn signal
 01.05.19:  - Tested the NeoSWSerial.h library, but it uses more memory than the softSerial Lib
            - Optimized the SoftwareSerial library to save memory if RX is not used
 03.05.19:  - Started the EEPROM function
 04.05.19:  - Saving the Last position to the EEPROM
            - Limiting the range is working
 05.05.19:  - Solved problem when the wipe mode was left
            - Speed change with wipe mode without changing the limits
            - Saving 294 bytes FLASH by replacing digitalWrite and pinMode
              (Attention this must be used in all modules, use avr-nm.ex to check this )
 06.05.19:  - TerServo mode is working. It's activated with LED PWM 250.
 06.05.19:  - If the MinMax values are not changed in mode 240 they are not saved.
              => The Wipe mode could be reached with B230 B240 B245
                 Prior this sequence was used B230 B245 B240
              To change the speed B230 B245 is used. In this case the wipe mode is not active
 07.05.19:  - Changed the variable type for Puls_t form uint32_t to uint16_t
 09.05.19:  - Saved 434 bytes (5%) by replacing the map() function with a macro
 12.05.19:  - Using the own PWM_In() function instead of the (improved) pulseIn() function
              => Now the signals are much more stable and robust against temperature, voltage
                 and chip variances. Also variations on the WS2811 signals are no problem.
              In the LED PWM range form 20 to 250 there are nearly no errors. (10 hours work!)
 13.05.19:  - Changed the process logic.
              - Now only one LED PWM per loop is read in because it's not possible to read in all
                3 signals and generate the servo signals in 20 ms.
                To read the LED PWM signal could take 2 times of the PWM frequency = 5 ms. 5.8 ms
                are used to have a reserve because CPU clock is not very accurate.
              - The servo signal generation is also changed. Now the interrupt is generated every 20 ms
                and all servo signals are generated at once. Prior the interrupt was generated every 7.7 ms.
                Now we have a larger block to read the LEDs.
 30.05.19:  - Found error which causes problems if 3 servos are used
                Worng line:     RemTime =- cp->Act_pwmTime;
                Corrected line: RemTime -= cp->Act_pwmTime;
 21.06.19:  - Changing the servo speed by "buttons" added
            - Servo position is also saved if the LED PWM is 0 or > 220
 22.06.19:  - Min or Max pos could be changed separately
 10.07.19:  - Corrected PWM input pins (R <-> G)
            - Using full speed if Butt_Change is active to move to the destination position immediately
 28.07.19:  - If min > max the input is inverted.
 12.10.19:  - The compiler switch USE_PWM_IN_ASM was disabled for some reasons.
              This caused problems with the GCC version 7.2.0. The servo position
              was not stable.
              If the compiler switch is enabled it works fine.
              ATTiny Board package: ATTinyCore by Spencer Konde Version 1.3.2
              Prior the version 1.1.2 was used.
 04.08.20:  - Using 16 MHz CPU Frequenz instead of 8 MHz to read the LED PWM signal of new WS2811 chips
              because they use a PWM signal of 2 kHz instead of 400Hz.
              Attention: The maximal PWM signal to control a servo should be 210 and not 220 because
              otherwise the servo movement may be delayed because 221-224 stop the servo.
 08.03.21:  - Started, base on 01.ATTiny85_Servo, most of the original code removed, new serial MP3 added
 10.03.21:  - Changed to support 3 MP3-TF-16p modules on all 3 servo outputs


 ToDo:
 ~~~~~
 - Pruefen nach welcher Zeit die Servoposition gespeichert wird. Die Proc Control_Servo wird
   an verschiedenen Stellen aufgerufen ;-(


 Arduino settings:
 ~~~~~~~~~~~~~~~~~
 Board:       ATTiny25/45/85      for tests the ATTiny 84 could also be used
 Clock:       16 Mhz (8 MHz)                                                                                  // 04.08.20:  Added 16 MHz
 Chip:        ATtiny85
 B.O.D:       2.7V
 Save EEPROM: "EEPROM retained"
 Pin Mapping: "Counterclockwise"
 LTO:         Disabled
 millis():    Enabled


 Print Memory Usage:
 ~~~~~~~~~~~~~~~~~~~
 - Nach dieser Anleitung geht es: https://arduino.stackexchange.com/questions/31190/detailed-analyse-of-memory-usage
     cd C:\Program Files (x86)\Arduino\hardware\tools\avr\bin
     RAM:
       avr-nm.exe -Crtd --size-sort "C:\Users\Hardi\AppData\Local\Temp\arduino_build_516136\ATTiny85_Servo.ino.elf" | C:\Users\Hardi\AppData\Local\atom\app-1.28.0\resources\app.asar.unpacked\node_modules\dugite\git\usr\bin\grep.exe -i ' [dbv] ' | sort
     FLASH:
       avr-nm.exe -Crtd --size-sort "C:\Users\Hardi\AppData\Local\Temp\arduino_build_516136\ATTiny85_Servo.ino.elf" | C:\Users\Hardi\AppData\Local\atom\app-1.28.0\resources\app.asar.unpacked\node_modules\dugite\git\usr\bin\grep.exe -i ' [tw] ' | sort


 400Hz / 2kHz                                                                                                 // 04.08.20:
 ~~~~~~~~~~~~
 Es gibt zwei verschiedene WS2811 Typen. Die �lteren haben eine LED Frequenz von 400Hz,
 die neueren 2 kHz! Das f�rhrt zu einer 5 mal ungenaueren PWM Messung ;-(

 Das wirkt sich beim Endbereich des Servos aus. Durch eine ungenaue Messung kann es passieren,
 dass ein PWM Signal von 220 als 221-224 interpretiert wird. Dieser Breich ist nicht benutzt.
 => Das Servo macht nichts.
 Im Excel programm wurde bisher 220 als maximal Wert vorgegeben. Durch die Fehlmessung wurde
 das Servo manchmal verz�gert angesteuert ;-(
 => Im Excel darf nur noch maximal 210 gesendet werden

 Zur Erh�hung der Messgenauigkeit habe ich zus�tzlich die CPU Frequenz vom 8 MHz auf 16 MHz erh�ht.

 Dummerweise ist das LED PWM Signal ungenau:
    @400Hz:
    Min   0.9655ms  40.23%
    Max   0.9704ms  40.43%
    Delta 0.0049ms   0.20%

    @2kHz:
    Min   203.6us   39.92%
    Max   205.9us   40.37%
    Delta   2.3us    0.45%

 Das erzeugt einen Jidder auf dem Servo Signal wenn eine zwischenposition angefahren werden soll.
 => Das Servo brummt/zuckt bei einem neuen WS2811

 Als Abhilfe wurde das "delta" in "loop()" welches die PWM Signale "filtert" verdoppelt.
 Damit ist auch bei einem 2kHz WS2811 das Signal fast ganz stabil.
 Allerdings hat dadurch eine �nderung der LED PWM um einen Wert u.U. keinen Einfluss mehr.
*/

#include <EEPROM.h>

#define MAX_CHANNEL 3

#if defined __AVR_ATtiny85__ // Normal 8-pin CPU
    #define PORT_IN PORTB
    #include "Inp_Prtx.h"                             // defines Inp_Prtx[MAX_CHANNEL]
    const uint8_t Outp_PBx[MAX_CHANNEL] = {0, 1, 2 }; // PBx number n
    #define SERIAL_DEBUG_PIN 2                        // PB2 = Pin 7 (D2)
#else
  #ifdef __AVR_ATtiny84__    // 14-pin CPU used for debugging
    #define PORT_IN PORTA
    const uint8_t Inp_Prtx[MAX_CHANNEL] = {9,10, 8 }; // PAx number n
    const uint8_t Outp_PBx[MAX_CHANNEL] = {0, 1, 2 }; // PBx number n
    #define SERIAL_DEBUG_PIN 6                        // PA4 = Pin 9 (D6)  SCK
  //#define SERIAL_DEBUG_PIN 5                        // PA5 = Pin 8 (D5)  MISO
  //#define SERIAL_DEBUG_PIN 4                        // PA6 = Pin 7 (D4)  MOSI
  #else
    #warning "Wrong CPU Modul selected! Use ATTINY85 or ATTINY84 (For debugging)"
  #endif
#endif

// EEProm Adressen
#define EE_MODULE_TYPE1         0    // Type of Module at PIN 1
#define EE_MODULE_TYPE2         1    // Type of Module at PIN 2
#define EE_MODULE_TYPE3         2    // Type of Module at PIN 3


#define DEBUG_PIN             7

// #define S_DEBUG              // Enable serial print for debugging
// #define MP3_DEBUG            // Enable debugging on MP3 code
// #define PWM_DEBUG             // Enable debugging of PWM measurement

//#define FILTER_TO_USE_LED_PWM_1 // Enable this to filter the LED PWM signals to be able to read PWM value 1
                                // If this switch is disabled sometimes timeouts are read because the PWM
                                // signal is very short.

#define MAX_SERVO         (2500*8)  //   "        - SG90 hat den groessten Bereich von 600-2700us

typedef struct
    {
    int16_t       Act_pwmTime;         // Actual output PWM time in 1/8 us
    int16_t       Dst_pwmTime;         // Target output PWM time in 1/8 us
    uint16_t      PWM01;               // Unfiltered high active LED PWM puls with a resolution of 0.1% (Attention the LED PWM is LOW active)
  #ifdef FILTER_TO_USE_LED_PWM_1
    uint8_t       TimeOutCnt;          // Counts the number of input PWM timeouts to detect when the input PWM is turned off
  #endif
    uint8_t       LED_pwm;             // Minimal LED PWM signal
    uint8_t       max_pwm;             // Maximal LED PWM signal. min_pwm and max_pwm have a maximal distance of 1
    uint8_t       Act_Dest_Equal_Cnt;  // Used to turn of the servo if Act_pwmTime and Dst_pwmTime are equal for x * 20ms
    uint8_t       Butt_Change;         // Flag which is set if the Min/Max values have been changed in the button mode.
    } Channel_Data_t;

Channel_Data_t           Channel_Data[MAX_CHANNEL];
uint8_t                  Channel = 0;
Channel_Data_t          *cp = &Channel_Data[0];
volatile int8_t          IntProcessed;
uint32_t                 PeriodBuffer = 0;

#define SERVO1_PIN  0
#define SERVO2_PIN  1
#define SERVO3_PIN  2

#include "SoftwareSerial_H.h"
// SoftwareSerial softSerial(99, SERIAL_DEBUG_PIN); // RX, TX   // bei ATTiny 85: PB2 = Pin 7  /  bei ATTiny 84: PA4 = Pin 9
SoftwareSerial mySerial[3] = {
   SoftwareSerial(99, SERVO1_PIN),
   SoftwareSerial(99, SERVO2_PIN),
   SoftwareSerial(99, SERVO3_PIN),
};

uint8_t activeModule = 0; // Default: Module connected to SERVO1

#if defined(S_DEBUG) || defined(MP3_DEBUG) || defined(PWM_DEBUG)
   #define Print(x)   mySerial[2].print(x)
   #define Println(x) mySerial[2].println(x)
   void Print_(int x) { mySerial[2].print(x); mySerial[2].print(F(" ")); }

   uint16_t DebugTOCtr = 0;
#else
   #define Print(x)
   #define Println(x)
   #define Print_(x)
#endif

#include "DFPlayerMini.h"

DFPlayerMini MP3[MAX_CHANNEL] = {
  DFPlayerMini(),
  DFPlayerMini(),
  DFPlayerMini()
};

#include "PWM_In.h"

#if 1 // Saves 434 bytes
  #define OptMap(x, in_min, in_max, out_min, out_max) (((long)((x) - (in_min)) * ((out_max) - (out_min))) / ((in_max) - (in_min)) + (out_min))
#else
  #define OptMap map // Standard function defined in "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\WMath.cpp"
#endif


//-------------------------------------
uint8_t PWM01_to_LED_PWM(uint16_t pwm)
//-------------------------------------
// Convert the measured high active pwm signal from the WS2811 chip with a resolotion of 0.1%
// to the LED commad value in the range of 0 to 255 which was send to the WS2811 chip.
// Based on C:\Dat\Maerklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ATTiny85_Servo\PWM_to_pulseIn.xlsx
{
  // if (pwm >= 989) return 10;  // pwm values above 98.94% = 10 can't destinguished reliable => Always return 10.
  if (pwm >= 970) return OptMap(pwm, 970, 989, 20,  10);
  if (pwm >= 52)  return OptMap(pwm, 52,  970, 255, 20);
  else            return 0;
}


//------------------------------------------
void delayPartsOfMicroseconds(uint16_t Time)
//------------------------------------------
// Delay for the given number of half microseconds.  At 8 MHz clock.
// At 16 MHz the delay is 1/4 microseconds
//
// Modifierd delayMicroseconds form
// C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\wiring.c
// Maximal time 32 ms / 16 ms @ 16MHz
{
  // for a 1 and 2 microsecond delay, simply return.  the overhead
  // of the function call takes 14 (16) cycles, which is 2us
    if (Time <= 4) return; //  = 3 cycles, (4 when true)                                                       // 15.04.19:  Old 3

  // the following loop takes 1/2 of a microsecond (4 cycles)
  // per iteration, so execute it twice for each microsecond of
  // delay requested.
    //us <<= 1; //x2 us, = 2 cycles                                                                           // 15.04.19:

  // account for the time taken in the preceeding commands.
  // we just burned 17 (19) cycles above, remove 4, (4*4=16)
  // us is at least 6 so we can substract 4
    Time -= 4; // = 2 cycles
  // busy wait
  __asm__ __volatile__ (
    "1: sbiw %0,1" "\n\t" // 2 cycles
        "brne 1b" : "=w" (Time) : "0" (Time) // 2 cycles
  );
}

//--------------------------------------------------------------
void Impuls_1_8_microseconds(uint16_t pwmTime, uint8_t PortB_Nr)
//--------------------------------------------------------------
#if F_CPU == 8000000L // for the 8 MHz internal clock
#elif F_CPU == 16000000L // for the 16 MHz internal clock
{
  Impuls_1_16_microseconds(pwmTime*2, PortB_Nr); // At 16 MHz we multyply the time be 2
}
//---------------------------------------------------------------
void Impuls_1_16_microseconds(uint16_t pwmTime, uint8_t PortB_Nr)
//---------------------------------------------------------------
#else
    #error Wrong CPU frequency
#endif
// Unterteilung in 1/8us:
// Der HS-311 macht nicht mehr spuerbare Schritte
// Der CS3 "Tickt" unregelmaessig im 0.8 sec Takt
// Der SG90 mini Servo Brummt und ruckt alle 3 sec ein Stueck weiter
// CS3 und SG90 haben anscheinend eine Hysterese die verhindert,
// das sich der Servo bei kleinen "Stoerungen" bewegt.
// Das setzen des Ports ist in jedem case fall enthalten
// weil sich sonst zeiten nicht genau sind. Das ist wichtig wenn
// sich pwmTime in Einerschritten veraendert.
{
  uint16_t pwmTime_d4 = pwmTime>>2;
  uint8_t Or_Mask  =  (1 << PortB_Nr);
  uint8_t And_Mask = ~Or_Mask;
  switch (pwmTime%4)
    {
    case 3: PORTB |= Or_Mask;
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            delayPartsOfMicroseconds(pwmTime_d4);
            PORTB &= And_Mask;
            break;
    case 2: PORTB |= Or_Mask;
            __asm__("nop\n\t");
            __asm__("nop\n\t");
            delayPartsOfMicroseconds(pwmTime_d4);
            PORTB &= And_Mask;
            break;
    case 1: PORTB |= Or_Mask;
            __asm__("nop\n\t");
            delayPartsOfMicroseconds(pwmTime_d4);
            PORTB &= And_Mask;
            break;
    case 0: PORTB |= Or_Mask;
            delayPartsOfMicroseconds(pwmTime_d4);
            PORTB &= And_Mask;
            break;
    }
}

//----------------
void NextChannel()
//----------------
{
  Channel++;
  if (Channel >= MAX_CHANNEL) Channel = 0;
  cp = &Channel_Data[Channel];
}

#ifdef __AVR_ATtiny85__  // Normal 8-pin CPU
  //--------------------
  ISR(TIMER1_COMPA_vect) //Interrupt-Serviceroutine
  //--------------------
#else // Debug CPU
  //--------------------
  ISR(TIM1_COMPA_vect)
  //--------------------
#endif
{ // Tnterrupt function which is called every 20ms
  Channel_Data_t *cp = &Channel_Data[0];
  for (uint8_t Channel = 0; Channel < sizeof(Inp_Prtx) ; Channel++, cp++)                                     // 13.05.19:
      {
      uint16_t RemTime = MAX_SERVO;
      delayPartsOfMicroseconds(RemTime>>2);  // RemTime/4
      }
  IntProcessed = 1;
}

//-----------------------------
void Activate_Timer_Interrupt()
//-----------------------------
{
  // Setzen der Register fuer 20 ms Timerinterrupt = 50Hz  (https://arduino-projekte.webnode.at/meine-projekte/servosteuerung/servotest-attiny/)
  // Vergleichswert = OCR1C = (8.000.000 / (1024 * 50)) - 1 = 155.25
  //
  // For 16 MHz a prescaler of 2048 is used instead of 1024
  //
  cli();      // Loesche globales Interruptflag
  #ifdef __AVR_ATtiny85__ // Normal 8-pin CPU
    TCNT1 = 0;  // Loesche Timer Counter 1
    TCCR1 = 0;  // Loesche Timer Counter Controll Register
    OCR1C = 155; // Setze Output Compare Register C
    #if F_CPU == 8000000L      // 8 MHz
      TCCR1 |= (1 << CS10) | (1 << CS11) | (1 << CS13); // Setze CS10, CS11 und CS13 - Clock Select Bit 10,11,13 (Prescaler 1024)
    #elif F_CPU == 16000000L   // 16 MHz
      TCCR1 |= (1 << CS12) | (1 << CS13);               // Setze CS10, CS11 und CS13 - Clock Select Bit 12,13    (Prescaler 2028)        // 04.08.20:
    #else
      #error Wrong CPU frequency
    #endif
    TCCR1 |= (1 << CTC1); // CTC-Mode ein (Clear Timer and Compare)
    //Timer/Counter Interrupt Mask Register
    TIMSK |= (1 << OCIE1A); //Output Compare A Match Interrupt Enable
  #else // Debug CPU
    TCCR1A = 0;
    TCCR1B = 0;
    TCCR1C = 0;
    TCNT1  = 0;
    OCR1A = 155;
    #if F_CPU == 8000000L
      TCCR1B = (1 << WGM12) | (1 << CS12) | (1 << CS10);    // CTC mode and 1024 prescaler
    #else
      #error Wrong CPU frequency
    #endif
    TIMSK1 |= (1 << OCIE1A);  // enable timer compare interrupt
  #endif
  sei(); //Setze globales Interruptflag
}


//----------
void setup()
//----------
{
  mySerial[0].begin(9600);                                                                                   // Only for 8 MHz internal clock ??
  mySerial[1].begin(9600);                                                                                   // Only for 8 MHz internal clock ??
  mySerial[2].begin(9600);                                                                                   // Only for 8 MHz internal clock ??
  #if defined(S_DEBUG) || defined(MP3_DEBUG) || defined(PWM_DEBUG)
    for (uint8_t i = 0; i < 50; i++) delayMicroseconds(10000); // Wait until the ArduinoISP is ready to receive characters  // 05.05.19:  Don't use delay(500) to save 146 bytes Flash
    Print(F("Snd"));
    #ifdef __AVR_ATtiny84__
      Println(84);
    #elif __AVR_ATtiny85__
      Println(85);
    #else
      Println(F("?"));
    #endif 
  #endif
  
  uint8_t moduleType = EEPROM.read(EE_MODULE_TYPE1);
  if (moduleType>1) moduleType = MODULE_JQ6500;
  MP3[0].begin(mySerial[0]);
  MP3[0].setModuleType(moduleType);

  moduleType = EEPROM.read(EE_MODULE_TYPE2);
  if (moduleType>1) moduleType = MODULE_MP3_TF_16P;
  MP3[1].begin(mySerial[1]);
  MP3[1].setModuleType(moduleType);

  moduleType = EEPROM.read(EE_MODULE_TYPE3);
  if (moduleType>1) moduleType = MODULE_JQ6500;
  MP3[2].begin(mySerial[2]);
  MP3[2].setModuleType(moduleType);

  for (uint8_t ch = 0; ch < MAX_CHANNEL; ch++)
    PORT_IN |= (1<<Inp_Prtx [ch]);                                                                           // 05.05.19:  Save FLASH (Old: pinMode(Inp_Prtx [ch], INPUT_PULLUP);)

  for (uint8_t ch = 0; ch < MAX_CHANNEL; ch++) {
    uint8_t Mask = (1<<Outp_PBx[ch]);
    DDRB  |= Mask;  // pinMode(Outp_PBx[ch], OUTPUT);
    PORTB |= Mask;  // digitalWrite(Outp_PBx[ch], 1); // Disable the Servo
  }

  memset(Channel_Data, 0, sizeof(Channel_Data));
  Activate_Timer_Interrupt();
}


// LED PWM B127
// Ossi:               1068     +-0
// Org:                 990     986  (989   991)  994    (+-2)    = 16 CPU Takte
//
// Ohne Korrektur der skalierung
// pulseInSimpl:       1084
// pulseInSimpl10_c:   1578
// pulseInSimpl10_asm: 1579    1576  (1578 1580)  1582   (+-2)    =>10 CPU Takte


// Die Taktfrequenz des ATTinys ist sehr ungenau +/- 10%
// Seite 164: atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
// und stark Temperaturabhaengig ;-(

// Abhaengigkeit des WS2811 von der Temperatur B240:
//
//                Raumtemp     Kaeltespray   Foen
//  Impuls Breite     2111 us    1948 us    2201 us
//  PWM Low          88.86 %    88.86 %    88.86 %
//  PWM High         11.14 %    11.14 %    11.14 %

// ===>> PWM Signal messen !


extern "C" unsigned int pulseInSimpl10_asm(volatile uint8_t *port, uint8_t bit, uint8_t stateMask, unsigned int maxloops); // located in pulseIn10.S

//----------------------------------------------------------------------
unsigned int pulseIn10(uint8_t pin, uint8_t state, unsigned int timeout)
//----------------------------------------------------------------------
// Improved pulseIn() function with a resolution of 10us @8mHz instead
// of 16us of the original function.
// Attention: The maximal puls length is 65ms !
{
  // cache the port and bit of the pin in order to speed up the
  // pulse width measuring loop and achieve finer resolution.  calling
  // digitalRead() instead yields much coarser resolution.
  uint8_t bit = digitalPinToBitMask(pin);
  uint8_t port = digitalPinToPort(pin);
  uint8_t stateMask = (state ? bit : 0);

  // convert the timeout from microseconds to a number of times through
  // the initial loop; it takes approximately 16 clock cycles per iteration
    unsigned long maxloops = microsecondsToClockCycles(timeout)/10;

    unsigned long width = pulseInSimpl10_asm(portInputRegister(port), bit, stateMask, maxloops);

  // prevent clockCyclesToMicroseconds to return bogus values if countPulseASM timed out
  if (width)
        return clockCyclesToMicroseconds(width *10 + 16);                                                     // 08.05.19:
      //return clockCyclesToMicroseconds(width * 16 + 16);                                                    // 08.05.19:  Org. Line
  else
    return 0;
}

void SendToMp3(uint8_t cmd, uint8_t param1, uint8_t param2) {
  #ifdef MP3_DEBUG
    Print("cmd: ");Println(cmd);
  #endif
  switch (cmd) {
		case DFPLAYER_NEXT:
		case DFPLAYER_PREV:
		case DFPLAYER_VOLUME_INCREASE:
		case DFPLAYER_VOLUME_DECREASE:
		case DFPLAYER_STANDBY:
		case DFPLAYER_RESET:
		case DFPLAYER_PLAYBACK:
		case DFPLAYER_PAUSE:
		case DFPLAYER_STOP_ADVERTISEMENT:
		case DFPLAYER_STOP:
		case DFPLAYER_NORMAL:
		case DFPLAYER_SET_RANDOM:
      MP3[activeModule].sendCmd(cmd);
      break;
		// 8-bit parameter
		case DFPLAYER_SET_VOLUME:
		case DFPLAYER_EQ:
		case DFPLAYER_SOURCE:
		case DFPLAYER_REPEAT:
		case DFPLAYER_SET_DAC:
		case DFPLAYER_SET_REPEAT_CURRENT:
      MP3[activeModule].sendCmd(cmd, param1);
			break;
		// 16 bit parameter
		case DFPLAYER_PLAY_TRACK:	// Play track (param1<<5+param2)+1, 0-1023 => 0001-1024.mp3/wav
		case DFPLAYER_SINGLE_REPEAT:	// Repeat (param1<<5+param2)+1, 0-1023 => 0001-1024.mp3/wav
		case DFPLAYER_PLAY_MP3:	// Play mp3/track (param1<<5+param2)+1, 0-1023 => 0001-1024.mp3/wav
		case DFPLAYER_ADVERTISEMENT: // Interrupt playback with ADVERT/track (param1<<5+param2)+1, 0-1023=>0001-1024.mp3/wav
		case DFPLAYER_FOLDER_REPEAT:	// Repeat files from folder (param1<<5+param2)+1, 0-98 => 01-99
      #ifdef MP3_DEBUGE
        Print("Track: ");Println(uint16_t((param1<<5)+param2+1)));
      #endif
			MP3[activeModule].sendCmd(cmd, uint16_t((param1<<5)+param2+1));
			break;
		// two 8 bit parameters
		case DFPLAYER_PLAY_FOLDER:			// folder, file
			MP3[activeModule].sendCmd(cmd, param1+1, (uint16_t) (param2+1));  // Folder 0-31 => "01"-"32", track 00-31 => 01-32.mp3/wav
			break;
		case DFPLAYER_ADJUST_VOLUME:		// enable 0|1, gain 0-31
			MP3[activeModule].sendCmd(cmd, param1, param2);
			break;
		// 4+12 bit parameter
		case DFPLAYER_PLAY_FOLDER_TRACK:
			MP3[activeModule].sendCmd(0x14, (((uint16_t)(param1+1)) << 12) | (param2));
			break;

  }
}

int oldCmd = 0;

//---------
void loop()
//---------
{
  noInterrupts();
  cp->PWM01 = PWM_in(Inp_Prtx[Channel], HIGH, 5800, PeriodBuffer); // timeout set to 5.8 ms to have a reserve because CPU clock is not very accurate
  // Calculate min_pwm and LED_pwm                                                                          // 30.04.19:
  uint8_t pwm = PWM01_to_LED_PWM(cp->PWM01);
  if (cp->PWM01 == 0) {
       #ifdef FILTER_TO_USE_LED_PWM_1
         // Check if the LED signal is 0. Unfortunately sometimes the PWM01_to_LED_PWM() function gets a time out
         // if short low active peaks are received (<3us)
         // To filter this failures the Timeout is set after several LED signals = 0 are detected
         if (cp->TimeOutCnt >= 10) { // Attention: The maximal value for TimeOutCnt is 31-1 because only a 5 bit number is used (30 = 0.6 sec)
           if (cp->TimeOutCnt == 10) {
             cp->TimeOutCnt++;
             cp->Dst_pwmTime = 0;
             cp->max_pwm = cp->LED_pwm = 0;
           }
         } else cp->TimeOutCnt++;
         if (cp->TimeOutCnt > DebugTOCtr) DebugTOCtr = cp->TimeOutCnt;
       #else // FILTER_TO_USE_LED_PWM_1
         cp->max_pwm = cp->LED_pwm = 0;
       #endif
   } else { // min_pwm and max_pwm are used to filter jidder of the LED PWM signal. If two PWM signals are read which differ by 1 the smaller signal ist used.
     if (pwm > cp->max_pwm) cp->max_pwm = pwm;
     if (pwm < cp->LED_pwm) cp->LED_pwm = pwm;
     uint8_t delta;
     if (pwm < 20)
       delta = 4; // In the low range we use a delta of 2                                              // 04.08.20:  Old: 2
      else delta = 2;                                                                                      //            Old: 1
     if (cp->max_pwm - cp->LED_pwm > delta) cp->LED_pwm = cp->max_pwm = pwm;
     #ifdef FILTER_TO_USE_LED_PWM_1
       cp->TimeOutCnt = 0;
     #endif
   }

   IntProcessed = 0;
   
   interrupts(); // Enable the interrupts again

   NextChannel();

   interrupts();

   if (Channel==0) {		// 3 valid values should exists
     // Process PWM signals and send to MP3 Module
     uint8_t nr = 0;
		 uint8_t cmd = PWM01_to_LED_PWM(Channel_Data[0].PWM01);
		 uint8_t param1 = PWM01_to_LED_PWM(Channel_Data[1].PWM01);
		 uint8_t param2 = PWM01_to_LED_PWM(Channel_Data[2].PWM01);
		 if (cmd>63) {
		    cmd = (cmd-64)/6;
		 } else {
        cmd = 0;
		 }
		 if (param1>63) {
		   param1 = (param1-64)/6;
		 } else {
       param1=0;
		 }
		 if (param2>63) {
		   param2 = (param2-64)/6;
		 } else {
       param2 = 0;
		 }
     if (cmd!=oldCmd) {
         #ifdef PWM_DEBUG
            Print("PWM: ");
            Print(Channel_Data[0].PWM01);Print("/");
            Print(Channel_Data[1].PWM01);Print("/");
            Print(Channel_Data[2].PWM01);Print("  ");
            Print(PWM01_to_LED_PWM(Channel_Data[0].PWM01));Print("/");
            Print(PWM01_to_LED_PWM(Channel_Data[1].PWM01));Print("/");
            Print(PWM01_to_LED_PWM(Channel_Data[2].PWM01));Print("  ");
            Print("Cmd: ");Print(cmd);Print(" P1: ");Print(param1);Print(" P2: ");Println(param2);
         #endif
         #ifdef S_DEBUG
            Print(PWM01_to_LED_PWM(Channel_Data[0].PWM01));Print("/");
            Print(PWM01_to_LED_PWM(Channel_Data[1].PWM01));Print("/");
            Print(PWM01_to_LED_PWM(Channel_Data[2].PWM01));Print("  ");
            Print("Cmd: ");Print(cmd);Print(" P1: ");Print(param1);Print(" P2: ");Println(param2);
         #endif
       // We have something to do
       switch (cmd) {
        case DFPLAYER_SET_ACTIVE_MODULE:
          if ( (param1>0) && (param1<=MAX_CHANNEL) ) {
            #if defined(S_DEBUG) || defined(MP3_DEBUG)
              Print("Change to mod ");Println((param1-1));
            #endif
            activeModule = param1-1;
          }
          break;
        case DFPLAYER_SET_MODULE_TYPE:
          if ( (param1>0) && (param2>0) && (param1<=MAX_CHANNEL) && (param2<=2) ) {
            #if defined(S_DEBUG) || defined(MP3_DEBUG)
              Println("Module type on p");Print((param1-1)); Print(" to ");Println((param2-1));
            #endif
            MP3[param1-1].setModuleType(param2-1);
            EEPROM.write(param1-1, param2-1);
          }
          break;
        case DFPLAYER_PLAY_TRACK_ON:
          nr = param1>>3;
          param1 = param1 & 0x7;
          if (nr>0) {
            activeModule = nr-1;
            #if defined(S_DEBUG) || defined(MP3_DEBUG)
              Print("Change to mod ");Println((activeModule));
              Print("Track ");Print(" p1: ");Print(param1);Print(" p2: ");Println(param2);
            #endif
          }
          SendToMp3(DFPLAYER_PLAY_TRACK, param1, param2);
          break;
        case DFPLAYER_PLAY_MP3_ON:
          nr = param1>>3;
          param1 = param1 & 0x7;
          if (nr>0) {
            activeModule = nr-1;
            #if defined(S_DEBUG) || defined(MP3_DEBUG)
              Print("Change to mod ");Println((activeModule));
              Print("MP3 ");Print(" p1: ");Print(param1);Print(" p2: ");Println(param2);
            #endif
            SendToMp3(DFPLAYER_PLAY_MP3, param1, param2);
          }
          break;
        case DFPLAYER_PLAY_TRACK_REPEAT_ON:
          nr = param1>>3;
          param1 = param1 & 0x7;
          if (nr>0) {
            activeModule = nr-1;
          }
          SendToMp3(DFPLAYER_PLAY_TRACK, param1, param2);
          SendToMp3(DFPLAYER_SET_REPEAT_CURRENT, 1, 0);
          break;        
        case 0:
          // Do nothing - preparation for next command
          break;
        default:
          #ifdef S_DEBUG
            Print("Execute cmd ");Println(cmd);
          #endif
						SendToMp3(cmd, param1, param2);
			 }
			 oldCmd = cmd;
		 }
	 }

}
