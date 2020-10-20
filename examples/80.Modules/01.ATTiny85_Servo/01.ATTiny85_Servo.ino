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
 16.04.19:  - Started
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
 Es gibt zwei verschiedene WS2811 Typen. Die älteren haben eine LED Frequenz von 400Hz,
 die neueren 2 kHz! Das fürhrt zu einer 5 mal ungenaueren PWM Messung ;-(

 Das wirkt sich beim Endbereich des Servos aus. Durch eine ungenaue Messung kann es passieren,
 dass ein PWM Signal von 220 als 221-224 interpretiert wird. Dieser Breich ist nicht benutzt.
 => Das Servo macht nichts.
 Im Excel programm wurde bisher 220 als maximal Wert vorgegeben. Durch die Fehlmessung wurde
 das Servo manchmal verzögert angesteuert ;-(
 => Im Excel darf nur noch maximal 210 gesendet werden

 Zur Erhöhung der Messgenauigkeit habe ich zusätzlich die CPU Frequenz vom 8 MHz auf 16 MHz erhöht.

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
 Allerdings hat dadurch eine Änderung der LED PWM um einen Wert u.U. keinen Einfluss mehr.
*/

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

#define DEBUG_PIN             7

#define MIN_SERVO         ( 500*8)  // 1/8 [us]   Die Servos haben ganz unterschiedliche PWM Bereiche
#define MAX_SERVO         (2500*8)  //   "        - SG90 hat den groessten Bereich von 600-2700us
#define DEF_MIN_SERVO     ( 500*8)  //   "        Default values used if the EEPROM values are wrong
#define DEF_MAX_SERVO     (2500*8)  //   "

#define MIN_MOVE              3
#define MAX_MOVE            300

#define MIN_MOVE_BUTT         1
#define MAX_MOVE_BUTT       300

#define MAX_LED_PWM4SERVO   220     // MAX_SERVO is reached at this LED PWM value



#define DEFAULT_MOVE_INC     80     // Defines the servo moving speed in 1/8 us per 20ms  (160 = 1 sec whole range)

#define TURNOFF_DIF          30     // If the difference between filtered servo impuls time and
                                    // raw impuls time is below this value the servo impulses are disabled
#define Damping_Fact          4     // For the filtered servo impuls time (127 = Fast, 1 = Slow)


#define MIN_ACT_DEST_EQUAL   25     // Number of 20ms counts to disable the servo if Act_pwmTime and Dst_pwmTime are equal

#define CNG_POS_SLOW          2     // slow change of the position in ReadMinMaxButton() per 20 ms
#define CNG_POS_FAST        200     // fast   "              "                "

#define CNG_SPD_SLOW          1     // slow change of the speed in ReadSpeed()
#define CNG_SPD_FAST         20     // fast   "             "         "


#define READ_PWM             // Read the LED PWM signals

#define ENAB_SERVO           // Enable the servo output

//#define USE_EEPROM_CRC     // Use a CRC Checksum in the EEPROM (Uses 610 bytes FLASH and 64 byte RAM)

#ifdef __AVR_ATtiny84__      // 14-pin CPU used for debugging
  #define S_DEBUG            // Enable serial print for debugging
#endif

//#define FILTER_TO_USE_LED_PWM_1 // Enable this to filter the LED PWM signals to be able to read PWM value 1
                                // If this switch is disabled sometimes timeouts are read because the PWM
                                // signal is very short.


// EEProm Adressen
#define EE_MinVal_0         2*0    // 2 Byte = Minimal position for Servo 0
#define EE_MaxVal_0         2*1    // 2 Byte = Maximal position for Servo 0
#define EE_Move_Inc_0       2*2    // 2 Byte = Move increment for servo 0
#define EE_Act_pwmTime_0    2*3    // 2 Byte = Actual position of servo 0
#define EE_Flags_0          2*4    // 2 Byte = Flags of servo 0
#define EE_MinVal_1         2*5    // 2 Byte = Minimal position for Servo 1
#define EE_MaxVal_1         2*6    // 2 Byte = Maximal position for Servo 1
#define EE_Move_Inc_1       2*7    // 2 Byte = Move increment for servo 1
#define EE_Act_pwmTime_1    2*8    // 2 Byte = Actual position of servo 1
#define EE_Flags_1          2*9    // 2 Byte = Flags of servo 1
#define EE_MinVal_2         2*10   // 2 Byte = Minimal position for Servo 2
#define EE_MaxVal_2         2*11   // 2 Byte = Maximal position for Servo 2
#define EE_Move_Inc_2       2*12   // 2 Byte = Move increment for servo 2
#define EE_Act_pwmTime_2    2*13   // 2 Byte = Actual position of servo 2
#define EE_Flags_2          2*14   // 2 Byte = Flags of servo 2
#define EE_CRC              2*15   // 4 Byte
                                   // ======
                                   // 34 Sum
// Bits used for the flags
#define EF_INVERS           1      // Invers direction

enum ModeStates_E : uint8_t       // States for the Mode variable
       {
       M_Init,
       M_ServoSel,
       M_MinMax,
       M_MinMaxButtons,
       M_Speed,
       M_SpeedButtons,                                                                                        // 21.06.19:
       M_TerServo
       };

// Constants for MMBut_Mode
#define MMB_ReadMin    1
#define MMB_WaitEndMin 2
#define MMB_ReadMax    3
#define MMB_WaitEndMax 4
#define MMB_End        5

#define NO_SERVO_SEL 3


typedef struct
    {
    int16_t       MinVal;              // Minimal value for the servo (detected in the state M_MinMax)
    int16_t       MaxVal;              // Maximal value for the servo (detected in the state M_MinMax)
    int16_t       Move_Inc;            // Defines the servo moving speed in 1/8 us per 20ms
    int16_t       Act_pwmTime;         // Actual output PWM time in 1/8 us
    int16_t       Dst_pwmTime;         // Target output PWM time in 1/8 us
    uint16_t      PWM01;               // Unfiltered high active LED PWM puls with a resolution of 0.1% (Attention the LED PWM is LOW active)
    uint8_t       DisableServo;        // Is set to 1 if the servo is disabled because the Dst_pwmTime and Act_pwmTime are similar to the lop pass filtered signal
  #ifdef FILTER_TO_USE_LED_PWM_1
    uint8_t       TimeOutCnt;          // Counts the number of input PWM timeouts to detect when the input PWM is turned off
  #endif
    uint8_t       LED_pwm;             // Minimal LED PWM signal
    uint8_t       max_pwm;             // Maximal LED PWM signal. min_pwm and max_pwm have a maximal distance of 1
    uint8_t       Act_Dest_Equal_Cnt;  // Used to turn of the servo if Act_pwmTime and Dst_pwmTime are equal for x * 20ms
    uint8_t       Save_Pos_Cnt;        // Used to save the servo position
    uint8_t       Butt_Change;         // Flag which is set if the Min/Max values have been changed in the button mode.
    uint8_t       Invers;              // Flag which is set if the turn direction of the servo is inverted
    } Channel_Data_t;

Channel_Data_t           Channel_Data[MAX_CHANNEL];
uint8_t                  Channel = 0;
Channel_Data_t          *cp = &Channel_Data[0];
volatile int8_t          IntProcessed;

int8_t                   Mode     = 0;            // Mode for special functions
ModeStates_E             MState   = M_Init;       // Modes for the configuration state machine
uint8_t                  MChannel = 3;            // Channel which has activated the state machine
uint8_t                  SelServo = NO_SERVO_SEL; // Number of the selected servo
Channel_Data_t          *Sel_p;
int16_t                  RdMinVal;                // MinVal read im ReadMinMax()
int16_t                  RdMaxVal;                // MaxVal read im ReadMinMax()
uint8_t                  MMBut_Mode;              // Button Mode in ReadMinMaxButton()

uint32_t                 PeriodBuffer = 0;

#ifdef S_DEBUG
   #include "SoftwareSerial_H.h"
   SoftwareSerial softSerial(99, SERIAL_DEBUG_PIN); // RX, TX   // bei ATTiny 85: PB2 = Pin 7  /  bei ATTiny 84: PA4 = Pin 9

   #define Print(x)   softSerial.print(x)
   #define Println(x) softSerial.println(x)
   void Print_(int x) { softSerial.print(x); softSerial.print(F(" ")); }

   uint16_t DebugTOCtr = 0;
#else
   #define Print(x)
   #define Println(x)
   #define Print_(x)
#endif

#include "PWM_In.h"

#if 1 // Saves 434 bytes
  #define OptMap(x, in_min, in_max, out_min, out_max) (((long)((x) - (in_min)) * ((out_max) - (out_min))) / ((in_max) - (in_min)) + (out_min))
#else
  #define OptMap map // Standard function defined in "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\WMath.cpp"
#endif


#include <EEPROM.h> // Achtung: Beim Flashen des ATTiny wird auch das EEPRom geloescht wenn die Fuse "Preserve EEPROM" nicht gesetzt ist

//-------------------------------------
uint8_t PWM01_to_LED_PWM(uint16_t pwm)
//-------------------------------------
// Convert the measured high active pwm signal from the WS2811 chip with a resolotion of 0.1%
// to the LED commad value in the range of 0 to 255 which was send to the WS2811 chip.
// Based on C:\Dat\Maerklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ATTiny85_Servo\PWM_to_pulseIn.xlsx
{
  if (pwm >= 989) return 10;  // pwm values above 98.94% = 10 can't destinguished reliable => Always return 10.
  if (pwm >= 970) return OptMap(pwm, 970, 989, 20,  10);
  if (pwm >= 52)  return OptMap(pwm, 52,  970, 255, 20);
  else            return 0;
}

#define CALC_T(pwm) (MIN_SERVO+(pwm-1)*(uint32_t)(MAX_SERVO-MIN_SERVO)/(MAX_LED_PWM4SERVO-1))


//---------------------------------------------
inline uint32_t InPuls_to_PWMTime(uint16_t InP)  // Debug
//---------------------------------------------
// LED PWM 1 - 220 => Servo PWM time 500 - 2800
{
  if (InP < 1 || InP > 1792)  return 0;
  if (InP < 18) return OptMap(InP,  1,   18,  CALC_T(1), CALC_T(  9));
  if (InP < 66) return OptMap(InP, 18,   66,  CALC_T(9), CALC_T( 20));
                return OptMap(InP, 66, 1792,  CALC_T(20),CALC_T(220));
}

//--------------------------------------------
inline int16_t LED_PWM_to_PWMTime(uint8_t pwm)
//--------------------------------------------
// LED PWM 1 - 220 => Servo PWM time 500 - 2800
{
  return OptMap(pwm, 1,220, MIN_SERVO, MAX_SERVO);
}

//------------------------------------------------
inline int16_t LED_PWM_to_PWMTime_Inv(uint8_t pwm)
//------------------------------------------------
// LED PWM 1 - 220 => Servo PWM time 2800 - 500
// Inverted turning direction
{
  return OptMap(pwm, 1,220, MAX_SERVO, MIN_SERVO);
}

//-------------------------------------------------
int16_t Servo_PWMTime_to_LED_PWM(int16_t ServoTime) // Debug
//-------------------------------------------------
{
  #define CORR_SERVO_ROUND ((MAX_SERVO-MIN_SERVO)/(220-1))
  return OptMap(ServoTime+CORR_SERVO_ROUND, MIN_SERVO, MAX_SERVO, 1, 220);
}

//----------------------------------------------
inline int16_t LED_PWM_to_Speed(uint8_t LED_pwm)
//----------------------------------------------
{
  return OptMap(LED_pwm, 2, 220, MIN_MOVE, MAX_MOVE);
}

//--------------------------------------------
inline int16_t Speed_to_LED_PWM(int16_t Speed) // Debug
//--------------------------------------------
{
  #define CORR_SPEED_ROUND ((MAX_MOVE-MIN_MOVE)/(220-2))
  return OptMap(Speed+CORR_SPEED_ROUND, MIN_MOVE, MAX_MOVE, 2, 220);
}

//----------------------------------
void Proc_TerServo(uint8_t LED_pwm)
//----------------------------------
// Control all servos with one LED PWM channel.
// The PWM values are coded ternary.
// Servo values:
//  0 = Disable Servo signal
//  1 = Minimal Position  (L)   (Left and right may be swapped depending on the servo)
//  2 = Maximal Position  (R)
//
// PWM Values in the range form 35 to 165 are used:
//
// PWM   Servo A Servo B Servo C                   (L,R could be swapped depending of the servo type)
// ~~~   ~~~~~~~ ~~~~~~~ ~~~~~~~
// 35    off     off     off
// 40    L       off     off
// 45    R       off     off
// 50    off     L       off
// 55    L       L       off
// 60    R       L       off
// 65    off     R       off
// 70    L       R       off
// 75    R       R       off
// 80    off     off     L
// 85    L       off     L
// 90    R       off     L
// 95    off     L       L
// 100   L       L       L
// 105   R       L       L
// 110   off     R       L
// 115   L       R       L
// 120   R       R       L
// 125   off     off     R
// 130   L       off     R
// 135   R       off     R
// 140   off     L       R
// 145   L       L       R
// 150   R       L       R
// 155   off     R       R
// 160   L       R       R
// 165   R       R       R
{
  //#define DEBUG_TERSERVO
  uint32_t do_print = 0;
  #if defined S_DEBUG && defined DEBUG_TERSERVO  // Debug
    static uint32_t LastT = 0;
    if (millis() - LastT > 2000)
       {
       LastT = millis();
       do_print = 1;
       }
  #endif

  if (LED_pwm < 33 || LED_pwm > 167) LED_pwm = 35; // All Servos are unchanged

  uint8_t v = (LED_pwm-35+2)/5; // -35 = Offset, +2 in case the signal is two points smaller then expected (Valid range 35 => 33 - 37)
  Channel_Data_t *lcp = &Channel_Data[0];
  for (uint8_t i = 0; i < 3; i++,lcp++)
    {
    uint8_t Servo = v % 3;
    uint8_t Ctl_LED_pwm;
    switch (Servo)
        {
        case 0:  Ctl_LED_pwm = 0;   break; // Disable the servo
        case 1:  Ctl_LED_pwm = 1;   break; // MinVal
        default: Ctl_LED_pwm = 220;        // MaxVal
        }
    Control_Servo(Ctl_LED_pwm, i, 1);
    v /= 3;

    if (do_print) Print_(Servo); // Debug (Doesn't use memory if DEBUG_TERSERVO is not defined)
    }
  if (do_print) Println(F(""));  // Debug                        "
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
      if (cp->Dst_pwmTime != 0 && !cp->DisableServo)
         {
         if (cp->Move_Inc == 0 || cp->Butt_Change)                                                            // 10.07.19:  Added: Butt_Change
              cp->Act_pwmTime = cp->Dst_pwmTime; // Full speed
         else { // Slow movement
              if (cp->Dst_pwmTime > cp->Act_pwmTime)
                   {
                   cp->Act_pwmTime += cp->Move_Inc;
                   if (cp->Act_pwmTime > cp->Dst_pwmTime) cp->Act_pwmTime = cp->Dst_pwmTime;
                   }
              else {
                   if (cp->Dst_pwmTime < cp->Act_pwmTime)
                      {
                      cp->Act_pwmTime -= cp->Move_Inc;
                      if (cp->Act_pwmTime < cp->Dst_pwmTime) cp->Act_pwmTime = cp->Dst_pwmTime;
                      }
                   }
              }
         Impuls_1_8_microseconds(cp->Act_pwmTime, Outp_PBx[Channel]);
         RemTime -= cp->Act_pwmTime;                                                                          // 30.05.19:  Old: RemTime =- cp->Act_pwmTime;
         }
      delayPartsOfMicroseconds(RemTime>>2);  // RemTime/4
      }
  IntProcessed = 1;
}



//-------------
void WipeTest()  // Debug  (Only active if ENAB_SERVO && !READ_PWM
//-------------
// First Wipe test without LED PWM
{
  static uint32_t t, Last = 0, Period = 500;
  static int8_t Direction = 2;

  if ((uint32_t)((t = micros()) - Last) > Period)
     {
     if (Direction == 2) // Is only called once to initialize the Dst_pwmTime
        {
        Direction = 1;
        Channel_Data[0].Dst_pwmTime =
        Channel_Data[1].Dst_pwmTime =
        Channel_Data[2].Dst_pwmTime = 1500*8;
        }
     Last = t;
     if (Direction > 0)
          {
          noInterrupts();
          cp->Dst_pwmTime++; // 1/8 microseconds
          interrupts();
          if (cp->Dst_pwmTime >= 2000*8) Direction = -1;  // Max 2800
          }
     else {
          noInterrupts();
          cp->Dst_pwmTime--;
          interrupts();
          if (cp->Dst_pwmTime <= 1000*8)  Direction =  1;  // Min 500
          }
     Channel_Data[1].Dst_pwmTime =
     Channel_Data[2].Dst_pwmTime = cp->Dst_pwmTime;
     }
}


//---------------------------------------
void Debug_Print_Input_PWM(uint16_t PWM01) // Debug (Only avtive if ENAB_SERVO is not defined)
//---------------------------------------
{
  #ifdef S_DEBUG
     static uint32_t Avg_Puls = 0;
     static float    avg_f = 0;
     //if (PWM01 != 0)
        {
        static uint32_t LastT = 0;
        if (millis() - LastT > 500)
           {
           LastT = millis();
           uint32_t k2 = 128 - Damping_Fact;
           uint32_t pTime = InPuls_to_PWMTime(PWM01);
           static uint32_t Filter_Val;
           Filter_Val  = ((k2 * Filter_Val + 63) >> 7)  +  Damping_Fact * pTime;  // +63 for correct rounding


           Avg_Puls = ((Avg_Puls * 900) / 1000) + PWM01 * 30;
           uint8_t FilterConst = 95;
           avg_f = (float)(avg_f * FilterConst + PWM01 * (100-FilterConst))/ 100.0;

           Print_(PWM01);
           Print_(avg_f);

           Print_(PeriodBuffer);

//           Print_(PWM01_to_LED_PWM(cp->PWM01));
//           Print_(Filter_Val>>7);
//           Print_(PWM01_to_LED_PWM(PWM01));

//           Print_(pTime );
//           Print_(Filter_Val>>7);
//           Print_(abs((Filter_Val>>7)-pTime));

           // Test ohne Filter
           //Print_(cp->min_pwm);
           Print_(cp->LED_pwm);
           Print_(DebugTOCtr);

           Println();
           }
        }
  #endif
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
  #ifdef S_DEBUG
    softSerial.begin(9600);                                                                                   // Only for 8 MHz internal clock ??
    for (uint8_t i = 0; i < 50; i++) delayMicroseconds(10000); // Wait until the ArduinoISP is ready to receive characters  // 05.05.19:  Don't use delay(500) to save 146 bytes Flash
    Print(F("Tiny Servo"));
    #ifdef __AVR_ATtiny84__
      Println(84);
    #elif __AVR_ATtiny85__
      Println(85);
    #else
      Println(F("?"));
    #endif
  #endif

//  #ifdef __AVR_ATtiny84__  // Debug CPU
//    pinMode(DEBUG_PIN, OUTPUT);    // Debug pin
//  #endif

  #ifdef READ_PWM
    for (uint8_t ch = 0; ch < MAX_CHANNEL; ch++)
        PORT_IN |= (1<<Inp_Prtx [ch]);                                                                           // 05.05.19:  Save FLASH (Old: pinMode(Inp_Prtx [ch], INPUT_PULLUP);)
  #endif

  for (uint8_t ch = 0; ch < MAX_CHANNEL; ch++)
      {
      uint8_t Mask = (1<<Outp_PBx[ch]);
      DDRB  |= Mask;  // pinMode(Outp_PBx[ch], OUTPUT);
      PORTB |= Mask;  // digitalWrite(Outp_PBx[ch], 1); // Disable the Servo
      }

  #ifdef ENAB_SERVO
    memset(Channel_Data, 0, sizeof(Channel_Data));
    Setup_EEPROM();
    Activate_Timer_Interrupt();
  #endif // ENAB_SERVO
}


//----------------------------------------
void EE_write_Char(int Adr, const char *s)
//----------------------------------------
{
  const char *p = s;
  while (1)
    {
    EEPROM.update(Adr++, *p);
    if (*p == '\0') return;
    p++;
    }
}

//-------------------------------------
char *EE_read_Char(int Adr, char *Buf)
//-------------------------------------
{
  char *p = Buf;
  while (1)
    {
    *p = EEPROM[Adr++];
    if (*(p++) == '\0') break;
    }
  return Buf;
}
//-------------
void WriteCRC()
//-------------
{
  #ifdef USE_EEPROM_CRC
    EEPROM.put(EE_CRC, eeprom_crc(EE_CRC)); // This function uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  #endif
}

//----------------------------------
unsigned long eeprom_crc(int length)
//----------------------------------
{
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  unsigned long crc = ~0L;

  for (int index = 0 ; index < length  ; ++index)
    {
    crc = crc_table[(crc ^  EEPROM[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (EEPROM[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
    }
  return crc;
}

//---------------------
void EE_Write_Default()
//---------------------
{
  Println(F("EE_Def"));
  uint8_t Addr = 0;
  uint8_t b = 0xFF;
  for (Addr = EE_MinVal_0; Addr < EE_CRC; Addr++)
      EEPROM.put(Addr, b);
  WriteCRC();
}

//------------------------------------------------------------------
void Get_and_Check_EEPROM(uint8_t &Addr, int16_t &Dest, uint8_t Typ)
//------------------------------------------------------------------
// Read and check one EEPROM variable. If it's outside the allowed
// range a default value is used.
// There are 3 different types:
//  0: Servo min value
//  1: Servo max value
//  2: servo Speed
{
  EEPROM.get(Addr, Dest);
  Addr+=2;
  int16_t MinVal = MIN_SERVO, MaxVal = MAX_SERVO, DefVal;
  switch (Typ)
    {
    case 0: DefVal = DEF_MIN_SERVO; break;
    case 1: DefVal = DEF_MAX_SERVO; break;
            break;
    default:DefVal = DEFAULT_MOVE_INC;
            MinVal = MIN_MOVE;
            MaxVal = MAX_MOVE;
            break;
    }

//  if (Typ == 2) // Debug
//       Print_(Speed_to_LED_PWM(Dest));
//  else Print_(Servo_PWMTime_to_LED_PWM(Dest));

  if (Dest < MinVal) Dest = DefVal;
  if (Dest > MaxVal) Dest = DefVal;
  //Print(F("?"));Print_(Dest);   // Debug
}

//-----------------
void Setup_EEPROM()
//-----------------
{
#ifdef USE_EEPROM_CRC
  unsigned long crc;
  EEPROM.get(EE_CRC, crc);
  if (eeprom_crc(EE_CRC) != crc) EE_Write_Default(); // Write default values to the EEPROM
#endif

  uint8_t Addr = 0;
  for (uint8_t S_Nr = 0; S_Nr < MAX_CHANNEL; S_Nr++)
    {
    Channel_Data_t *p = &Channel_Data[S_Nr];
    Get_and_Check_EEPROM(Addr, p->MinVal,      0);
    Get_and_Check_EEPROM(Addr, p->MaxVal,      1);
    Get_and_Check_EEPROM(Addr, p->Move_Inc,    2);
    Get_and_Check_EEPROM(Addr, p->Act_pwmTime, 0);
    uint16_t Flags;
    EEPROM.get(Addr, Flags);
    p->Invers = Flags & EF_INVERS;
    Addr+=2;
    //Println(F("")); // Debug (Must be enabled if Print() is used in Get_and_Check_EEPROM)
    }
}

//---------------
void SaveMinMax()
//---------------
{
  if (abs(RdMaxVal - RdMinVal) < 3*84) return; // Difference must be greater than 3 LED PWM steps

  if (RdMaxVal < RdMinVal)
       {
       int16_t Tmp = RdMinVal;
       RdMinVal = RdMaxVal;
       RdMaxVal = Tmp;
       Sel_p->Invers = 1;
       }
  else Sel_p->Invers = 0;

  uint8_t Addr = EE_MinVal_0 + (EE_MinVal_1 - EE_MinVal_0) * SelServo;
  Sel_p->MinVal = RdMinVal;
  EEPROM.put(Addr, RdMinVal);
  Addr += EE_MaxVal_0 - EE_MinVal_0;
  Sel_p->MaxVal = RdMaxVal;
  EEPROM.put(Addr, RdMaxVal);

  // Save the invers flag
  Addr += EE_Flags_0 - EE_MaxVal_0;
  uint16_t Flags;
  EEPROM.get(Addr, Flags);
  if (Sel_p->Invers)
       Flags |=  EF_INVERS;
  else Flags &= ~EF_INVERS;
  EEPROM.put(Addr, Flags);
  WriteCRC();
  //Println(F("SaveMinMax"));
  //Print_(Sel_p->MinVal); Println(Sel_p->MaxVal);
  //Pintln(Flags);
}

//--------------
void SaveSpeed()
//--------------
{
  uint8_t Addr = EE_Move_Inc_0 + (EE_Move_Inc_1 - EE_Move_Inc_0) * SelServo;
  int16_t Val = Sel_p->Move_Inc;
  EEPROM.put(Addr, Val);
  WriteCRC();
  //Println(F("SaveSpeed"));
  //Print_(Addr); Print_(Val); EEPROM.get(Addr, Val); Println(Val);
}

//-------------------------------
void Change_Mode_Server(int8_t m)
//-------------------------------
// Is called once if the Mode was changed
{
  //Print(F("Mode:")); Println(m);
  switch (m)
    {
    case 1:                                //                 (LED PWM 225)
    case 2:                                //                 (LED PWM 230)
    case 3: MState   = M_ServoSel;         //                 (LED PWM 235)
            MChannel = Channel;
            SelServo = m-1;
            Sel_p = &Channel_Data[SelServo];
            //Print(F("SelServo")); Print(SelServo);
            //Print(F(" MCh"));Println(MChannel);
            break;
    case 4: if (SelServo != NO_SERVO_SEL)  // State M_MinMax  (LED PWM 240)
               {
               MState = M_MinMax;
               RdMinVal = 0x7FFF;
               RdMaxVal = 0;
               Sel_p->Butt_Change = 0;                                                                        // 22.06.19:
               //Println(F("M_MinMax"));
               }
            break;
    case 5: if (SelServo != NO_SERVO_SEL)  // State M_Speed   (LED PWM 245)
                 {
                 if (MState == M_MinMax || MState == M_MinMaxButtons)
                    {
                    SaveMinMax();
                    Sel_p->Dst_pwmTime = Sel_p->Act_pwmTime; // To start the wipe (Only activated if prior the MinMax was defined)
                    Sel_p->DisableServo = 0;                 // To be able to change the speed in the normal opreation mode
                    }
                 //Print(F("UpdMin ")); Println(Sel_p->MinVal);
                 //Print(F("UpdMax ")); Println(Sel_p->MaxVal);
                 MState = M_Speed;
                 }
            break;

    case 6: //                                                (LED PWM 250)
            if (MState == M_MinMax && SelServo != NO_SERVO_SEL)
                 {
                 MState = M_MinMaxButtons;
                 RdMinVal = Sel_p->MinVal;                                                                    // 22.06.19:
                 RdMaxVal = Sel_p->MaxVal;
                 MMBut_Mode = MMB_ReadMin;
                 Sel_p->Dst_pwmTime = Sel_p->Act_pwmTime;
                 }
            if (MState == M_Speed && SelServo != NO_SERVO_SEL)                                                // 20.06.19:
                 {
                 MState = M_SpeedButtons;
                 }
            else if (MState == M_Init)
                 {
                 //Println(F("TerServo"));         // Debug: State M_TerServo (LED PWM 250)
                 MState = M_TerServo;
                 MChannel = Channel;   // Prevent that other channels disable the mode again because then the mode is toggled permanently
                 }
            break;
    case 7: switch (MState)  // Save  (LED PWM 254) not 255 because 255 is not always detected
              {
              case M_MinMax:
              case M_MinMaxButtons: SaveMinMax();    break;
              case M_Speed:
              case M_SpeedButtons:  SaveSpeed();     break;
              default:              ; // Prevent compiler warning
              }
            // no break, continut with the next block
    case 0:
            if (MState != M_Init)
               {
               MState = M_Init;
               //Println(F("=>M_Init")); // Debug
               SelServo = NO_SERVO_SEL;
               MChannel = 3;
               }
            break;
    }
}


//-------------------------------------------------------------------------------------------------------------------
int8_t Proc_Up_Down_Button(uint8_t LED_pwm, int16_t &Val, int16_t Min, int16_t Max, uint8_t MinStep, uint8_t MaxStep)
//-------------------------------------------------------------------------------------------------------------------
// Change Val with "buttons" (LED_pwm) in the range of 1..100..200
// 0       = Noting    return  0
// 1-95    = Dec Val   return -1
// 100     = Noting    return  0
// 105-200 = Inc Val   return  1
// 205     = Button 1  return  2
// 210-220 = Noting    return  3
// 225-255 = Noting    return  4
{
  int8_t res;
  if      (LED_pwm <    1  ) return 0;
  else if (LED_pwm <=  95+2) { res = -1; Val -= OptMap(LED_pwm,   1, 95, MaxStep, MinStep); }
  else if (LED_pwm <= 100+2) return 0;
  else if (LED_pwm <= 200+2) { res =  1; Val += OptMap(LED_pwm, 105,200, MinStep, MaxStep); }
  else if (LED_pwm <= 205+2) return 2;
  else if (LED_pwm <= 220+2) return 3;
  else                       return 4;

  Val = constrain(Val, Min, Max);
  return  res;
}

//---------------------
void ReadMinMaxButton()
//---------------------
// Move the servo with some "Buttons" and store the MinMax values
{
  Channel_Data_t *ap = &Channel_Data[SelServo];
  int8_t res = Proc_Up_Down_Button(cp->LED_pwm, ap->Dst_pwmTime, MIN_SERVO, MAX_SERVO, CNG_POS_SLOW, CNG_POS_FAST);
               // 0/100=Noting, ret 0; 1-95=Dec Val, ret -1; 105-200=Inc Val, ret 1, 205=Button1, ret 2,
               // 210-220=Noting, ret 3; 225-255=Noting, ret 4
  ap->DisableServo = !(res == 1 || res == -1); // 0, 100, > 200

  if (res == 1 || res == -1) ap->Butt_Change = 1;

  if (res == 2) // 205
       { // Button 1 pressed
       switch (MMBut_Mode)
          {
          case MMB_ReadMin: if (ap->Butt_Change) RdMinVal = ap->Dst_pwmTime;   MMBut_Mode++; break;
          case MMB_ReadMax: if (ap->Butt_Change) RdMaxVal = ap->Dst_pwmTime;   MMBut_Mode++; break;
          }
       ap->Butt_Change = 0;
       }
  else { // Button released or other PWM signal
       if      (MMBut_Mode == MMB_WaitEndMin)  MMBut_Mode++;
       else if (MMBut_Mode >= MMB_ReadMax && res == 4) // res == 4: PWM 225 - 255
               {
               SaveMinMax();
               //Println("End"); // Debug
               }
        }
}

//---------------
void ReadMinMax()
//---------------
// Two different ways to set the min/max positions
// - A "button" mode where the position is inc/dec with a LED PWM number in the range of 1-95: Dec, 105-200: Inc
//   The Min / Max values are stored if the PWM value 205 is received
// - A absolute mode where the Min and Max values are set with LED PWM values in the range of 1 to 220
{
  if (MState == M_MinMaxButtons)
       ReadMinMaxButton();
  else {
       Control_Servo(cp->LED_pwm, SelServo, 0); // The selected servo is controlled

       if (Sel_p->Dst_pwmTime != 0)
          {
          if (Sel_p->Dst_pwmTime < RdMinVal) RdMinVal = Sel_p->Dst_pwmTime;
          if (Sel_p->Dst_pwmTime > RdMaxVal) RdMaxVal = Sel_p->Dst_pwmTime;
          }
       }
}

//--------------
void ReadSpeed()
//--------------
{
  // Wipe
  static uint32_t LastWipeT = 0;
  uint32_t t = millis();
  if (Sel_p->Act_pwmTime == Sel_p->Dst_pwmTime)
       {
       if (t-LastWipeT > 500) // 500 ms pause at the end
          {
          if      (Sel_p->Dst_pwmTime < Sel_p->MaxVal) Sel_p->Dst_pwmTime = Sel_p->MaxVal;
          else if (Sel_p->Dst_pwmTime > Sel_p->MinVal) Sel_p->Dst_pwmTime = Sel_p->MinVal;
          if (Sel_p->Move_Inc == 0) LastWipeT = t; // At maximal speed the LastWipeT is not set below
                                                   // => The waiting time is reduced by the moving duration
          }                                        //    No pause if the servo is slow and the travel distance high
       }
  else LastWipeT = t; // Is not called if Sel_p->Move_Inc == 0


 // Change the speed
 if (MState == M_SpeedButtons)                                                                                // 20.06.19:
      {
      static uint32_t Last_Cng = 0;
      if (t - Last_Cng > 200) // Don't change the speed to fast
         {
         Last_Cng = t;
         Proc_Up_Down_Button(cp->LED_pwm, Sel_p->Move_Inc, MIN_MOVE_BUTT, MAX_MOVE_BUTT, CNG_SPD_SLOW, CNG_SPD_FAST);
         // 0/100=Noting, ret 0; 1-95=Dec Val, ret -1; 105-200=Inc Val, ret 1, 205=Button1, ret 2,
         // 210-220=Noting, ret 3; 225-255=Noting, ret 4
         //Println(Sel_p->Move_Inc); // Debug
         }
      }
 else {
      if (cp->LED_pwm > 0)
         {
         if (cp->LED_pwm < 218)
              Sel_p->Move_Inc = LED_PWM_to_Speed(cp->LED_pwm);                                                // 05.05.19:  Old: map(cp->LED_pwm, 2, 220, MIN_MOVE, MAX_MOVE);
         else if (cp->LED_pwm < 223) Sel_p->Move_Inc = 0;      // Maximal Speed
         }
      }
}

// Es darf nur ein LED PWM Signal aktiv sein waehrend die Daten Konfiguriert werden


//-----------------------------
int8_t pwm_to_Mode(uint8_t pwm)
//-----------------------------
//       0 = 0
//   1-222 =-1
// 223-227 = 1 \              Servo 0
// 228-232 = 2  Select servo  Servo 1
// 233-237 = 3 /              Servo 2
// 238-242 = 4  Set Min_Max
// 243-247 = 5  Set Speed
// 248-252 = 6  Res
// 253-255 = 7  Ok
//
{
  if (pwm == 0)   return 0;
  if (pwm <  223) return -1;
  return OptMap(pwm, 223, 253, 1, 7);
}

//---------------------
void Proc_Mode_Server()
//---------------------
{

  if (MState == M_Init || MChannel == Channel)
     {
     int8_t m = pwm_to_Mode(cp->LED_pwm); // 0=0, 1-222=-1, 223-227=1, 228-232=2, 233-237=3, 238-242=4, 243-247=5, 248-252=6, 253-255=7

//     static uint32_t LastT = 0;     // Periodic print variables for Debug
//     if (millis() - LastT > 2000)
//        {
//        //Print_(pwm_to_Mode(cp->LED_pwm)); Print_(cp->LED_pwm); Println(cp->TimeOutCnt);
//        //Print_(m); Println(Mode);
//        //Print_(Channel_Data[0].Act_pwmTime);
//        //uint8_t pwm = Servo_PWMTime_to_LED_PWM(Channel_Data[0].Act_pwmTime);
//        //Print_(pwm);
//        //Println(LED_PWM_to_PWMTime(pwm));
//        //Print_(Servo_PWMTime_to_LED_PWM(Channel_Data[1].Act_pwmTime));
//        //Print_(Channel_Data[0].Act_pwmTime); Println(Channel_Data[1].Act_pwmTime);
//        uint8_t pwm = Speed_to_LED_PWM(Channel_Data[1].Move_Inc);
//        Print_(Channel_Data[1].Move_Inc); Print_(pwm); Println(LED_PWM_to_Speed(pwm));
//        LastT = millis();
//        }

     if (m != -1 && m != Mode)
          {
          //if (m != 0) digitalWrite(DEBUG_PIN, 1);  // Debug
          //Print(F("Mode")); Print(m); Print(F(" pwm")); Print(cp->LED_pwm); Print(F(" Ch")); Println(Channel); // Debug
          Mode = m;
          Change_Mode_Server(Mode);
          // digitalWrite(DEBUG_PIN,0); Debug
          }
     else {
          switch (MState)
            {
            case M_ServoSel:      Control_Servo(cp->LED_pwm, SelServo, 1); break;// The selected servo is controlled
            case M_MinMax:
            case M_MinMaxButtons: ReadMinMax();                            break;
            case M_Speed:
            case M_SpeedButtons:  ReadSpeed();                             break;
            case M_TerServo:      Proc_TerServo(cp->LED_pwm);              break;
            default: break;
            }
          }
     }
}

//-----------------------------------
void Save_ServoPos(uint8_t ChannelNr)
//-----------------------------------
{
  uint8_t Addr = EE_Act_pwmTime_0 + (EE_Act_pwmTime_1 - EE_Act_pwmTime_0) * ChannelNr;
  int16_t Val = Channel_Data[ChannelNr].Act_pwmTime;
  EEPROM.put(Addr, Val); // This function uses EEPROM.update() to perform the write, so does not rewrites the value if it didn't change.
  WriteCRC();
  //Print(F("Save ")); Println(ChannelNr);
}

//----------------------------------------------------------------------
void Control_Servo(uint8_t LED_pwm, uint8_t ChannelNr, bool LimmitRange)
//----------------------------------------------------------------------
// Control the servo[ChannelNr]
{
  Channel_Data_t *ap = &Channel_Data[ChannelNr];
  if (LED_pwm == 0)
       {
       PORTB |= (1 << Outp_PBx[ChannelNr]); // Disable the Servo                                              // 05.05.19:  Old: digitalWrite(Outp_PBx[ChannelNr], 1);
       ap->DisableServo = 1; // Solves problem if the wipe mode is left (Test: G0 B230 B245 B240 => Wipe, B0 => Wrong movement, Puls 4ms)
       }
  else {
       if (LED_pwm <= 220)
          {
          if (ap->Invers)
               ap->Dst_pwmTime = LED_PWM_to_PWMTime_Inv(LED_pwm); // inline function
          else ap->Dst_pwmTime = LED_PWM_to_PWMTime(LED_pwm);     // inline function

          if (LimmitRange) ap->Dst_pwmTime = constrain(ap->Dst_pwmTime, ap->MinVal, ap->MaxVal);

          // Timeout and save servo position
          if (ap->Dst_pwmTime == ap->Act_pwmTime)
               {
               if (ap->Act_Dest_Equal_Cnt  < 255) ap->Act_Dest_Equal_Cnt++;
               //if (ap->Act_Dest_Equal_Cnt == 250) Save_ServoPos(ChannelNr);                                 // 21.06.19:  Old pos
               if (ap->Act_Dest_Equal_Cnt == MIN_ACT_DEST_EQUAL) ap->DisableServo = 1;
               }
          else {
               ap->Act_Dest_Equal_Cnt = 0;
               ap->DisableServo       = 0;
               }
          }
       }

  // Save the Servo position                                                                                  // 21.06.19:
  if (ap->Dst_pwmTime == ap->Act_pwmTime)
       {
       if (ap->Save_Pos_Cnt < 255) ap->Save_Pos_Cnt++;
       if (ap->Save_Pos_Cnt == 250) Save_ServoPos(ChannelNr);
       }
  else ap->Save_Pos_Cnt = 0;

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



//---------
void loop()
//---------
{
  #ifdef READ_PWM
    noInterrupts();
    cp->PWM01 = PWM_in(Inp_Prtx[Channel], HIGH, 5800, PeriodBuffer); // timeout set to 5.8 ms to have a reserve because CPU clock is not very accurate
    // Calculate min_pwm and LED_pwm                                                                          // 30.04.19:
    uint8_t pwm = PWM01_to_LED_PWM(cp->PWM01);
    if (cp->PWM01 == 0)
         {
         #ifdef FILTER_TO_USE_LED_PWM_1
           // Check if the LED signal is 0. Unfortunately sometimes the PWM01_to_LED_PWM() function gets a time out
           // if short low active peaks are received (<3us)
           // To filter this failures the Timeout is set after several LED signals = 0 are detected
           if (cp->TimeOutCnt >= 10) // Attention: The maximal value for TimeOutCnt is 31-1 because only a 5 bit number is used (30 = 0.6 sec)
                {
                if (cp->TimeOutCnt == 10)
                   {
                   cp->TimeOutCnt++;
                   cp->Dst_pwmTime = 0;
                   cp->max_pwm = cp->LED_pwm = 0;
                   }
                }
           else cp->TimeOutCnt++;
           if (cp->TimeOutCnt > DebugTOCtr) DebugTOCtr = cp->TimeOutCnt;
         #else // FILTER_TO_USE_LED_PWM_1
           cp->max_pwm = cp->LED_pwm = 0;
         #endif
         }
    else { // min_pwm and max_pwm are used to filter jidder of the LED PWM signal. If two PWM signals are read which differ by 1 the smaller signal ist used.
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



    #ifdef ENAB_SERVO
      //Print(F("."));
      if (MState == M_Init) Control_Servo(cp->LED_pwm, Channel, 1);

      // LED PWM signals 225 - 255 are used for special modes
      Proc_Mode_Server();

      IntProcessed = 0;
      interrupts(); // Enable the interrupts again

      static uint32_t RemCount = 0; // Debug
      while (!IntProcessed)  // Wait until the interrupt is started to update the servos
            {
            delayMicroseconds(5); // Without this delay the while loop doesn't end for some reason
            RemCount++; // Debug
            }

      // Debug
//      static uint8_t UpdDispCnt = 0;
//      if (++UpdDispCnt >= 100)  // 20ms * 100 = 2 seconds
//         {
//         UpdDispCnt = 0;
//         Print(F("Remaining:"));  // time in us 3 * 2.5ms + 5.8 = 6.7   Gemessen 7.2 ms
//         Println((RemCount*5)/100);
//         RemCount = 0;
//         }

      NextChannel();

    #else // ENAB_SERVO is not defined
       interrupts();
       Debug_Print_Input_PWM(cp->PWM01);
    #endif
  #endif // READ_PWM


  #if defined ENAB_SERVO && !defined READ_PWM // *** First Wipe test without LED PWM ***
      WipeTest();
  #endif
}
