/*
 Charlieplexing LED driver program for the MobaLedLib
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Arduino Configuration:                            Comments
 ~~~~~~~~~~~~~~~~~~~~~~
 Board:              "ATtiny25/45/85"              See: "ATTiny Board package must be installed:" below
 Clock:              "16 MHz (PLL)"                Importand => See: "Set ATTiny fuses:" below
 Chip:               "ATTiny85"
 B.O.D.Level:        "B.O.D. ENabled (2.7V)        Precent EEPROM Corroption if power is turned off
 Save EEPROM:        "EEPROM retained"             Keep EEPROM if the program is flashed
 Timer 1 Clock:      "CPU"
 LTO (1.6.11+ only): "Enabled"                     Link Time Optimization
 millis()/micros():  "Enabled"
 -------------------------------
 Programmer:         "Arduino as ISP"              See: "Programmer" below

 For debugging the software could also be used with an Arduino Uno/Nano.

 ATTENTION: The ATTiny must be "Fused" to 16 MHz


 ATTiny Board package must be installed:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 By default there is board package for the ATTiny available in the boards menue.
 It has to be done once afer the Arduino IDE is installed.
 Therefore th following line has to be added to the Boards manager URL:
   http://drazzy.com/package_drazzy.com_index.json
 Menu: "Datei/Voreinstellungen" => "Zusaetzliche Boardverwalter-URLs:"

 After this step the boards package can be installed:
 Menu: "Werkzeuge/Board/Boardsverwalter"
 Enter "ATTiny" in the seach line
 and install "ATTinyCore by Spencer Konde"      (Ver. 1.3.3 is used for this project (Also tested with 1.3.2))
   https://github.com/SpenceKonde/ATTinyCore
   This core can be installed using the boards manager. The boards manager URL is:
   http://drazzy.com/package_drazzy.com_index.json
 There are other ATTiny packages available which have not been tested.


 Programmer:
 ~~~~~~~~~~~
 The ATTiny has no USB port to be programmed. Therefore an additional programmer is needed.
 This could be an Arduino Uno or Nano and a breadboard or a special hardware.
 There is a special programmer module for the MobaLedLib available which could be downloaded
 from here:
   https://github.com/Hardi-St/MobaLedLib_Docu/blob/master/Platinen/Tiny_UniProg.zip
 or ordered from:
   LedLib@yahoo.com (See: https://www.stummiforum.de/viewtopic.php?f=7&t=165060&sd=a&start=499)
 The module has to be programmed once with this software:
   ...\examples\90.Tools\02.Tiny_UniProg

 Here is a "German" Video which describes the programming with an Arduino Uno without a
 special hardware: (Attention here a different ATTiny board package from "David A Mellis" is used)
    https://www.youtube.com/watch?v=esJAPokgq1I
 There are a lot of other descriptions in the internet how to program an ATTiny with an Arduino UNO


 Set ATTiny fuses:
 ~~~~~~~~~~~~~~~~~
 The fuses of the ATTiny have to be set correctly. This has to be done once per ATTiny.
 To set the fuses the Arduino Configuration has to be set like described above and the
 bootloader has to be burned:
  Menu: "Werkzeuge/Board/Bootloader brennen"
 If the Fuses are not set the CPU clock is set to 1 MHz or 8 Mhz and the B.O.D is disabled ;-(
 To set the fuses a "Programmer" see above has to be used. Addention: The programming fails
 if the serial monitor of the Arduino IDE is opened.



 Revision History:
 ~~~~~~~~~~~~~~~~~
 10.12.19:  - Started
 13.12.19:  - First version is running
              - 300 Hz LED update rate
              - Read PWM from WS2811 and set Goto Number
 14.12.19:  - Splitting the PWM signal in up to 8 parts to generate a higher update rate
              without incresing the interrupt frequency
 23.12.19:  - Configuration is received via LED PWM
 25.12.19:  - Goto Nr start with LED PWM changed to 0 (Old 45)
 26.12.19:  - Analog input A2 could be used to set the GOTO numbers



 ToDo:
 ~~~~~

  EEConfig size 18:
  18, 0, 10, 0, 128, 255, 255, 2, 0, 128, 0, 0, 232, 3, 1, 0, 9, 0,
  Calc.CRC: 6FEE
  Exp. CRC: D676
 Analog Ueberblenden
  EEConfig size 18:
  18, 0, 40, 0, 128, 255, 255, 2, 0, 128, 0, 0, 232, 3, 1, 0, 9, 0,
  Calc.CRC: D676
  Exp. CRC: D676



 - Excel Interface:
   - Zusaetzliche Daten
     - COM Port
     - RGB LED Nummer des Charliplexing Moduls => Auswahl mit Blinkender LED
     - LED Assignement
     - Analog Input
   - Dialog:

     - Test der Goto Modes


   - Achtung Warnung wenn erste RGB LED nicht 0 ist in Excel einbauen

 - Untersuchen ob der PeriodBuffer fuer eine verzoegerung der PWM Messung sorgt. Evtl.
   Muessen darum 5 Messungen abgewartet werden bis das PWM Signal Stabil ist.

 - RAM der Pattern Funktion ist momentan eine Konstannte


 Fehler
 ~~~~~~
 - Hat das EEPROM Gedaechtniss verlust? Tritt Evtl. beim ausschalten auf.
   Aktuelle Werte bei der Ampel 23.12.19:
   29, 0, 13, 0, 0, 255, 255, 6, 0, 255, 0, 16, 208, 7, 232, 3, 16, 39, 184, 11, 6, 0, 201, 194, 40, 73, 22, 70, 0,
   Calc.CRC: 6238
   Exp. CRC: 6238
   Fuses
   C:\Users\Hardi\AppData\Local\Arduino15\packages\arduino\hardware\avr\1.8.1\boards.txt
   uno.bootloader.low_fuses=0xFF
   uno.bootloader.high_fuses=0xDE
   uno.bootloader.extended_fuses=0xFD   => BOD = 2.7V

 - Fehler nach den Test des Farbkastens
     EEConfig size 110:
     110, 0, 41, 0, 4, 255, 255, 12, 0, 255, 0, 0, 100, 0, 200, 0, 84, 0, 3, 0, 0, 3, 0, 0, 12, 0, 0, 12, 0, 0, 48, 0, 0, 48, 0, 0, 192, 0, 0, 192, 0, 0, 0, 3, 0, 0, 3, 0, 0, 12, 0, 0, 12, 0, 0, 48, 0, 0, 48, 0, 0, 192, 0, 0, 192, 0, 0, 0, 3, 0, 0, 3, 0, 0, 12, 0, 0, 12, 0, 0, 48, 0, 0, 48, 0, 0, 192, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 27, 46, 0, 0, 0, 0, 0,
     Calc.CRC: 0D0D
     Exp. CRC: 5139
   Dabei war der LED Stecker gar nicht an der Hauptplatine angeschlossen?!?
   Das sind die normalen Lauflichtdaten nur die Laenge stimmt nicht:
     EEConfig size 103:
     103, 0, 41, 0, 4, 255, 255, 12, 0, 255, 0, 0, 100, 0, 200, 0, 84, 0, 3, 0, 0, 3, 0, 0, 12, 0, 0, 12, 0, 0, 48, 0, 0, 48, 0, 0, 192, 0, 0, 192, 0, 0, 0, 3, 0, 0, 3, 0, 0, 12, 0, 0, 12, 0, 0, 48, 0, 0, 48, 0, 0, 192, 0, 0, 192, 0, 0, 0, 3, 0, 0, 3, 0, 0, 12, 0, 0, 12, 0, 0, 48, 0, 0, 48, 0, 0, 192, 0, 0, 192, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
     Calc.CRC: 2E1B
     Exp. CRC: 2E1B
   Es wurden noch 7 Bytes angehaengt: 27, 46, 0, 0, 0, 0, 0,
   => Evtl. wurde nur das erste Byte veraendert

 - Gedaechtnisschwund am UNO:    26.12.19:
     EEConfig size 48:
     48, 0, 140, 155, 88, 97, 167, 36, 3, 40, 0, 32, 254, 255, 11, 0, 128, 0, 8, 44, 1, 31, 0, 0, 96, 0, 0, 4, 5, 0, 26, 0, 64, 104, 0, 0, 128, 64, 1, 0, 0, 0, 127, 128, 63, 128, 63, 192,
     Calc.CRC: D948
     Exp. CRC: 813F
   Soll:
     EEConfig size 48:
     48, 0, 40, 0, 32, 254, 255, 11, 0, 128, 0, 8, 44, 1, 31, 0, 0, 96, 0, 0, 4, 5, 0, 26, 0, 64, 104, 0, 0, 128, 64, 1, 0, 0, 0, 127, 128, 63, 128, 63, 192, 63, 129, 128, 63, 129, 130, 0,
     Calc.CRC: 1179
     Exp. CRC: 1179
   Oder war das der Sollzustand ? (Viessmann + V Assignement)
     EEConfig size 55:
     55, 0, 140, 134, 115, 1, 42, 180, 89, 40, 0, 32, 254, 255, 11, 0, 128, 0, 8, 44, 1, 31, 0, 0, 96, 0, 0, 4, 5, 0, 26, 0, 64, 104, 0, 0, 128, 64, 1, 0, 0, 0, 127, 128, 63, 128, 63, 192, 63, 129, 128, 63, 129, 130, 0,
     Calc.CRC: 7154
     Exp. CRC: 7154


 Problem: Messung der WS2811 PWM Signale
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Die Messung der PWM Signale ist sehr stark verrauscht. Das liegt daran, dass die Interrupts
 w„hrend der Messung aktiv sind. Bei dem Servo Modul konnten die Interrupts w„hrend der Messung
 abgeschaltet werden weil die Servo Signale nur alle 20ms kommen mssen. In den Pausen wurde gemessen.
 Bei der Ansteuerung der Charliplexing LEDs geht das nicht. Die LEDs werden mit einer Periode von
 18.5 us aktualisiert (54 kHz).
 Bei 16 MHz sind zwischen zwei Interrupts nur 296 Prozessor Zyklen!

 Zur Messung des PWM Signals wurden beim Servo die Prozessor Zyklen gez„hlt. Wenn dieser Vorgang jetzt
 alle 296 Zyplen fr eine bestimmte Zeit unterbrochen wird ist das Messergebnis ungenau.
 Zu Messung eines PWM Signals wird die Zeit w„hrend das Signal aktiv ist und die inaktive Zeit gemessen.
 Daraus berechnet sich das PWM Signal.
 Wenn die Dauer der Unterbrechung konstannt w„re, dann wrden beide Messungen gleichm„áig verlangsamt
 werden. Das ermittelte PWM Signal w„re trotzdem richtig.
 Da die durch den Interrupt "verbrauchte" Zeit sich aber nicht gleichm„áig auf die Dauer der Messung
 verteilt sondern nur w„hrend der Interrupt aktiv ist auftritt wird das Ergebnis Sprnge aufweisen.
 Das WS2811 PWM Signal hat eine Frequenz von 420 Hz = 2.38 ms. W„hrend einer Periode treten also 128
 Interrupts auf. Jetzt kann ein Interrupt genau in dem Moment auftreten wenn die Flanke des Signals
 kommt. Die Messung erkennt das aber erst nachdem der Interrupt bearbeitet ist. Die Messung ist krzer.

                    Ungnstiger Fall                                      Gnstiger Fall
 WS2811             ___________ . . . __________                         ___________ . . . __________
 PWM          _____|                            |______          _______|                            |______

 Interrupt          ____                   ____                     ____                         ____
 Ausl”sung    _____|    |___           ___|    |_____           ___|    |___                 ___|    |_____

 Erkennung              ^                      ^                        ^                            ^
 der Flanke

 Bei der zweiten Flanke kann der gleiche Effekt auftreten.
 => Die Messung hat einen Fehler von der Dauer einer Interruptverarbeitung.
 Fr die Messung des PWM Signals wird aber noch eine weitere Flanke ben”tigt. Die Aktive Zeit und die
 inaktive Zeit k”nnen um die Dauer der Interruptverarbeitung falsch sein ;-(

 ==> Die Dauer der Interruptverarbeitung muss m”glichst kurz sein!
     Evtl. k”nnte man Teile der Interrupt funktion auáerhalb berechnen

 Dummerweise ist die Interruptzeit zus„tzlich unterschiedlich lang. Das h„ngt von den internen Zust„nden ab
 - Meiátens wird nur geprft ob die LED abgeschaltet werden soll.
   - in 1/15 der F„lle muss die LED ausgeschaltet werden
 - Alle 15 Interrupts muss auf die n„chste LED umgeschaltet werden
   - wenn die LED leuchten soll kostet das zus„tzliche Zeit


 Achtung die Messung das WS2811 PWM Signals ist manchmal ganz unterschiedlich
 wenn die c++ Routinen in PWM_In.cpp verwendet werden. Abhaengig von irgend
 welchen Aenderungen optimiert der Complier unterschiedlich.
 => Immer die ASM Routinen verwenden !!!

 ===> Waehrend der Datenuebertragung werden die Interrupts abgeschaltet. Dadurch ist die Messung sehr genau.


 Minimal Resistor value for the LED:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   39 Ohm
 => 39 mA per pin (Max allowed 40mA)
 => 3.3 mA multiplexed current per LED
 For tests I use 180 Ohm



 Print Memory Usage:
 ~~~~~~~~~~~~~~~~~~~
 - Nach dieser Anleitung geht es: https://arduino.stackexchange.com/questions/31190/detailed-analyse-of-memory-usage
     cd C:\Program Files (x86)\Arduino\hardware\tools\avr\bin
     RAM:
       avr-nm.exe -Crtd --size-sort "C:\Users\Hardi\AppData\Local\Temp\arduino_build_516136\ATTiny85_Servo.ino.elf" | C:\Users\Hardi\AppData\Local\atom\app-1.28.0\resources\app.asar.unpacked\node_modules\dugite\git\usr\bin\grep.exe -i ' [dbv] ' | sort
     FLASH:
       avr-nm.exe -Crtd --size-sort "C:\Users\Hardi\AppData\Local\Temp\arduino_build_776467\Charliplex_Test.ino.elf" | C:\Users\Hardi\AppData\Local\atom\app-1.28.0\resources\app.asar.unpacked\node_modules\dugite\git\usr\bin\grep.exe -i ' [tw] ' | sort
       avr-nm.exe -Crtd --size-sort "C:\Users\qzmhgp\AppData\Local\Temp\arduino_build_178847\Charliplex_Test.ino.elf" | T:\Tools\cygwin\bin\grep.exe -i ' [tw] ' | sort


 Pin Assignement:                                                             R10 is replaced by a Blue LED
 ~~~~~~~~~~~~~~~~                                                             It's located next to the 150 Ohm
                                                                              resistor on the PCB. Plus-pin
               Prog.       ATTiny 85       Prog.                              of the LED points to to the
                           +-\/-+                                             border of the PCB.
               Reset PB5  1|    |8  VCC
 Mux LED 4  (Tx)     PB3  2|    |7  PB2    SCK      Mux LED 3 + 180 Ohm       Pin 2 is used also as debug TXT
 PWM Inp Green   AD2 PB4  3|    |6  PB1    MISO     Mux LED 2    "            It's connected to an TTL to USB
                     GND  4|    |5  PB0    MOSI     Mux LED 1    "            converter (115200 Baud)
                           +----+
                                                                              Pin3 (AD2) could be connected to
 For debugging an Arduino Uno                                                 resistance-codeed switches to enable
 could also be used:                    +-----+                               the images in stand alone mode.
           +----[PWR]-------------------| USB |--+
           |                            +-----+  |
           |           GND/RST2  [ ] [ ]         |
           |         MOSI2/SCK2  [ ] [ ]  SCL[ ] |   C5
           |            5V/MISO2 [ ] [ ]  SDA[ ] |   C4
           |                             AREF[ ] |
           |                              GND[ ] |
           | [ ]N/C                    SCK/13[A] |   B5
           | [ ]v.ref                 MISO/12[A] |   .
           | [ ]RST                   MOSI/11[A]~|   .    Mux LED 4 + 180 Ohm
           | [ ]3V3    +---+               10[ ]~|   .    Mux LED 3    "
           | [ ]5v     | A |                9[ ]~|   .    Mux LED 2    "
           | [ ]GND   -| R |-               8[B] |   B0   Mux LED 1    "
           | [ ]GND   -| D |-                    |
           | [ ]Vin   -| U |-               7[A] |   D7
           |          -| I |-               6[A]~|   .
           | [ ]A0    -| N |-               5[C]~|   .
           | [ ]A1    -| O |-               4[A] |   .    LED PWM in from WS2811 (Green) + 1K Pull-Up
 Ana Inp.  | [ ]A2     +---+           INT1/3[A]~|   .
           | [ ]A3                     INT0/2[ ] |   .
      SDA  | [ ]A4      RST SCK MISO     TX>1[ ] |   .
      SCL  | [ ]A5      [ ] [ ] [ ]      RX<0[ ] |   D0
           |            [ ] [ ] [ ]              |
           |  UNO_R3    GND MOSI 5V  ____________/
            \_______________________/


 Ueberlegung:
 ~~~~~~~~~~~
 Fuer komplexe Ampelanlagen braucht mehr als 12 LEDs (Bereits eine Kreuzung mit 4 normalen
 Ampeln belegt 12 LEDs).
 Mit 3x4 Pins koennen 36 LEDs angesteuert werden.
 Dazu koennte man eine Platine mit einem ATtiny861 machen (16 IO Pins).
 Einfacher ist es aber wenn man einen Nano (Uno) verwendet.
 Dazu muessten aber das LED PWM Signal und der Analoge Eingang verschoben werden.
 LED PWM: D3, Ana Inp. A4
 Dann haette man zusaetzlich 8 Pins: D4-D7 und A0-A3

 Achtung:
 ~~~~~~~~
 Der ATTiny84 kann nicht mit dem "Special Adaper fuer Servo Platine" (Siehe Tiny_UniProg Schaltplan)
 zum Debuggen verwendet werden weil beim ATTiny84 der Port PB3 der Reset Eingang ist.
 => PB3 kann nicht als LED Anschluss benutzt werden
 Zum Debuggen wird der 8-polige Adapter verwendet. Die LED Platine muss ueber den Wannenstecker mit 5V
 versorgt werden. Die Debug Jumper auf der Tiny_Uniprog Platine verbinden beide Pin1&2.

 Tests:
 ~~~~~~
 - Die Zeiten des Lauflichts stimmen erstaunlich gut.
   Ich haette erwartet, das es deutlich langsamer laeuft. => Warum? Im normalen Mode sind die Interrupts aktiv.
   Soll 6 Sek, ist 6.3 Sek. (Handgestoppt)

*/

#include "Public_Defines.h"
#include "PWM_In.h"
#include "CharlieLEDs12.h"
#include "AnalogPattern.h"
#include "ReceiveConfigViaPWM.h"
#include "Cpx_Special_Functions.h"

#if defined __AVR_ATtiny85__
  #ifdef DEBUG_TINY
     #include "ATtinySerialOut.h"
     #include <avr/pgmspace.h> // needed for PSTR()
  #endif
#else // Uno or Nano is used for debugging
  #define BAUDRATE    19200  // Configure the baud rate  (Disable the line if no serial input/output is used)
#endif


#define NUM_LEDS      4
CRGB leds[NUM_LEDS];       // Define the array of leds

uint32_t PeriodBuffer = 0; // Used to filter the measured PWM signal

int8_t  Config_Load_Error = CFG_LOAD_OK;

#define LA(L1, L2) (L1<<4 | L2)
#define LED_ASSIGNEMENT(L1, L2, L3, L4, L5, L6, L7, L8, L9, L10, L11, L12)   CPX_LED_ASSIGNEMENT, LA(L1, L2), LA(L3, L4), LA(L5, L6), LA(L7,L8), LA(L9, L10), LA(L11, L12),

                                // Ref.Nr           0   1   2   3   4   5   6   7   8   9  10  11
#define VIESSMANN_LED_ASSIGNEMENT  LED_ASSIGNEMENT( 8,  6,  7,  3,  0,  1,  2, 10, 11,  4,  5,  9) // Viessmann Multiplex
#define UNIPROG_LED_ASSIGNEMANT    LED_ASSIGNEMENT( 0,  1,  8,  9, 11, 10,  3,  2,  4,  5,  7,  6) // LED Numbers see Tiny_UniProg_Sch2.pdf
#define REVERSE_LED_ASSIGNEMENT    LED_ASSIGNEMENT(11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0) // Reverse Test Board

#define SET_ANALOG_LIMMITS(L1, L2, L3, L4, L5, L6, L7, L8, L9, L10)   CPX_ANALOG_LIMMITS, _W2B(L1), _W2B(L2), _W2B(L3), _W2B(L4), _W2B(L5), _W2B(L6), _W2B(L7), _W2B(L8), _W2B(L9), _W2B(L10),

// The configuration could be read out of the EEPROM or the FLASH.
// Normally it's stored in the EEPROM to be able to change it without recompiling the programm
// If the compiler switch USE_EEPROM_CFG is not defined in "Public_Defines.h" the configuration is read
// form the FLASH.
// This program supports ony one pattern function in the configuration unlike the "normal" MobaLedLib because
// of the limmited FLASH of the ATTiny85.


#ifndef USE_EEPROM_CFG
//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
     // REVERSE_LED_ASSIGNEMENT // Change the LED Assignement

     // CPX_ANALOG_INPUT,       // Use the analog input to set the Goto Number

     // SET_ANALOG_LIMMITS(761, 256, 144, 86, 50, 34, 22, 14,  7, 2)  // Set limmits for the Analog Goto Numbers



     // ATTENTION: Only one Pattern line could be activated

     // Ampel:
     //APatternT8(0,28,SI_1,6,0,255,0,PF_NO_SWITCH_OFF,200 ms,1 Sec,200 ms,1 Sec,200 ms,3 Sec,200 ms,1 Sec,255,0,0,255,0,0,255,0,0,255,0,0,255,255,0,255,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,255,255,0,0,0,255,0,255,0,0,0,255,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,0,0,255,255,0,255,0,0,255,255,0,255,0,0,0,0,255,255,0,0,0,0,255,255,0,0,0,255,0,255,0,0,0,255,0)     // RGB_AmpelX_Fade

     // Baustellenlauflicht 12 LEDs
     APatternT2(0,4,SI_1,12,0,255,0,0,100 ms,200 ms,3,0,0,3,0,0,12,0,0,12,0,0,48,0,0,48,0,0,192,0,0,192,0,0,0,3,0,0,3,0,0,12,0,0,12,0,0,48,0,0,48,0,0,192,0,0,192,0,0,0,3,0,0,3,0,0,12,0,0,12,0,0,48,0,0,48,0,0,192,0,0,192,0,0,0,0,0,0,0,0,0,0,0,0)     // ConstrWarnLight

     // KS_Hauptsignal_Zs3_Zs1
     //XPatternT11(0,128,SI_LocalVar,4,0,255,0,0,200 ms,500 ms,200 ms,500 ms,200 ms,500 ms,200 ms,500 ms,200 ms,500 ms,200 ms,32,64,80,160,42,2  ,0,63,128,63,128,63,128,64,0,0,1)     // KS_Hauptsignal_Zs3_Zs1

     EndCfg // End of the configuration
  };
//*******************************************************************
#endif


MobaLedLib_Create(leds); // Define the MobaLedLib instance


#ifndef DEBUG_TINY   // Serial debuging is not available for the ATTiny

// Test mit der seriellen Konsole:
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// 0..10:   Eine Zahl zwischen 0 und 10 setzt den entsprechenden "Goto Mode"
//          Ueberschreibt das letzte LED PWM Kommando bis dieses sich aendert.
//          => PWM ind Serieller Monitor koennen den "Goto Mode" setzen.
// L0..L11: Aktiviert die entsprechende LED
// -:       Verringert Helligkeit der LED
// +:       Erhoeht Helligkeit der LED
// B<Nr>:   Setzt die Helligkeit
//

//----------------------
void Proc_Serial_Input()
//----------------------
{
  if (Serial.available() == 0) return ;
  static char Line[11] = "";
  char c = Serial.read();
  uint8_t LED_Test = 0;
  int Nr = 0, LastCmd = 0;
  uint8_t Brightness = MAX_BRIGHT;
  if (strlen(Line) < sizeof(Line) - 1)
     {
     do {
        if (c == '\r' || c == '\n')
             {
             switch (*Line)
               {
               case 'l' :
               case 'L' : LED_Test = 1;
                          Nr = atoi(Line+1);
                          LastCmd = 0;
                          break;
               case '+' : Brightness++; LastCmd = 1;  break;
               case '-' : Brightness--; LastCmd = -1; break;
               case '\0': if (LastCmd)
                               {
                               Brightness += LastCmd;
                               }
                          else {
                               Nr = Nr + 1;
                               if (Nr >= 12) Nr = 0;
                               }
                          break;
               case 'b' :
               case 'B' : Brightness = atoi(Line+1);
                          LastCmd = Brightness>=MAX_BRIGHT ? -1 : 1;
                          break;
               default  : LED_Test = 0;
                          Nr = atoi(Line);
               }
             if (Brightness > MAX_BRIGHT) Brightness = MAX_BRIGHT;
             if (Brightness < 1)          Brightness = 1;

             if (LED_Test)
                  {
                  Dprintf("LED_Test %i (%i)\n", Nr,Brightness);
                  for (uint8_t i = 0; i < 12; i++)
                    Set_LED(i, i == Nr ? Brightness : 0);
                  }
             else {
                  Dprintf("Set_ControlVar(%i)\n", Nr);
                  MobaLedLib.Set_ControlVar(Nr);
                  }
             *Line = '\0';
             }
        else {
             char *p = &Line[strlen(Line)];
             *(p++) = c;
             *p = '\0';
             }
        if (LED_Test)
           {
           if (Serial.available())
                c = Serial.read();
           else c = 0;
           }
        } while (LED_Test);
     }
}
#endif


//-------------------------------------
uint8_t PWM01_to_LED_PWM(uint16_t pwm)
//-------------------------------------
// Convert the measured high active pwm signal from the WS2811 chip with a resolotion of 0.1%
// to the LED commad value in the range of 0 to 255 which was send to the WS2811 chip.
// Based on LED_PWM Kennlinie.xlsx
{
//#if defined __AVR_ATtiny85__
//  if (pwm >= 985) return 10;  // pwm values above can't destinguished reliable => Always return 10.
//  if (pwm >= 966) return OptMap(pwm, 966, 985, 20,  10);
//  if (pwm >= 163) return OptMap(pwm, 163, 966, 215, 20);
//  if (pwm >= 36)  return OptMap(pwm, 36,  163, 255, 215);
//  if (pwm >= 5)   return 255;
//  else            return 0;
//  ToDo: Kann man hier nicht die gleich Tabelle wie beim Uno/Nano verwenden ?
//#else // Uno/Nano

  if (pwm >= 996) return 5;  // pwm values above can't destinguished reliable => Always return 5.             // 25.12.19:
  if (pwm >= 970) return OptMap(pwm, 970, 996, 20,  5);  // First 3 points
  if (pwm >= 53)  return OptMap(pwm, 53,  970, 255, 20);
  if (pwm >= 1)   return 255;
  else            return 0;
//#endif

/* Alt 25.12.19:
  if (pwm >= 985) return 10;  // pwm values above can't destinguished reliable => Always return 10.
  if (pwm >= 155) return OptMap(pwm, 155, 985, 220,  10);
//if (pwm >= 158) return OptMap(pwm, 158, 155, 230, 220);
  if (pwm >= 55)  return OptMap(pwm, 55,  158, 255, 230);
  if (pwm >= 5)   return 255;
  else            return 0;
*/
}

//-----------------------------
int8_t Setup_and_Check_Config()
//-----------------------------
{
  memset(leds, 0, sizeof(leds));
  Setup_LEDs(); // Initialize the LED assignment

  Enable_Analog_Input(0); // Disable reading analog Goto numbers. It may be enabled in Proc_CPX_InitMacros()

  int8_t res;
  #ifdef USE_EEPROM_CFG
    if ((res = Check_EEPROM_CRC()) != CFG_LOAD_OK) // If _PRINT_DEBUG_MESSAGES is defined the EEPROM is printed at startup
       return res;
  #else
    Debug_Print_Flash_Config(Config, sizeof(Config));
  #endif
  return MobaLedLib.Proc_CPX_InitMacros();
}


//----------
void setup()
//----------
{
  //wdt_disable();
  #ifdef BAUDRATE // Arduino Uno or Nano
    Serial.begin(BAUDRATE);
    Serial.println(F("Charliplexing Test\t" __DATE__ " " __TIME__));
  #endif

  #ifdef DEBUG_TINY // ATTiny with enabled serial debug (Attention: Uses Pin PB3 for debuging => Not all LEDs could be used & some LEDs are shown wrong)
    initTXPin();
    writeString(F("CharlieplexTiny " __DATE__ "\n"));
  #endif

//  for (uint8_t i = 0; i < 5; i++)
//    {
//    Dprintf("Hallo %i\n", i);
//    delay(1000);
//    }

  //Power_on_Ani(); // Test all LEDs at power on

  Config_Load_Error = Setup_and_Check_Config();

  if (Config_Load_Error == CFG_LOAD_OK)
     {
     MobaLedLib.Update();   // Must be called once before the inputs are read. (Attention: srandom() must be called before to get unpredictable random numbers)
     }

  //MobaLedLib.Set_ControlVar(3); // Debug
}
/*
//----------------------------
int8_t Get_Group(uint16_t val)
//----------------------------
//  < 36: -1
// 36-55:  0
// 56-75:  1
// ...
// 2
// 236 - 255: 10
{
  if (val < 36) return -1;
  return (val-36)/20;
}

//-----------------------------------
void Set_Goto_if_Stable(uint16_t val)
//-----------------------------------
{
  static int8_t  LastGroup = -1;
  static uint8_t EqualCnt = 0;
  int8_t Grp = Get_Group(val);
  if (Grp == LastGroup)
       {
       if (EqualCnt < 255) EqualCnt++;
       if (EqualCnt == 20)
          {
          if (Grp >= 0) MobaLedLib.Set_ControlVar(Grp);
          Dprintf("Goto %i PWM:%i\n", Grp, val); // Debug
          }
       }
  else {
       EqualCnt = 0;
       LastGroup = Grp;
       }
}
*/

//----------------------------
int8_t Get_Group(uint16_t val)
//----------------------------
//     <  26:  0
//  26 -  45:  1
//  46 -  65:  2
//  66 -  85:  3
// ...
// 186 - 205:  9
// 206 - 225   10
// 226 - 245   PMode     (11)
// 246 - 265   Unused    (12)
//
{
  if (val < 26) return 0;
  return (val-6)/20;
}

//-----------------------------------------------------
void Set_Goto_and_Check_Prog_Mode_if_Stable(int8_t Grp)
//-----------------------------------------------------
{
  static int8_t  LastGroup = -1;
  static uint8_t EqualCnt = 0;
  if (Grp == LastGroup)
       {
       if (EqualCnt < 255) EqualCnt++;
       if (EqualCnt == 20)
          {
          Dprintf("Goto %i\n", Grp); // Debug
          if (Grp >= 0 && Grp < 11) MobaLedLib.Set_ControlVar(Grp);
          else if (Grp == 11)
                  {
                  if (Check_ProgramMode())
                     {
                     Config_Load_Error = Setup_and_Check_Config(); // If _PRINT_DEBUG_MESSAGES is defined the EEPROM is printed
                     Dprintf("Restart MLL\n");
                     MobaLedLib.Restart();

                     }
                  }
          }
       }
  else {
       EqualCnt = 0;
       LastGroup = Grp;
       }
}



#include <EwmaT.h>
EwmaT <long> adcFilter(20, 100);   // 20 fast, 5 slow


// Analoger Eingang fuer Autarken Betrieb (Ohne LED PWM)
// ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Der Analoge Eingang A2 (Pin 3 des ATTiny) kann auch zum einlesen
// der Goto Nummer verwendet werden. Damit kann dann z.B. eine Ampel
// von Tag auf Nacht Betrieb umgeschaltet werden.
// Auf der Servo_LED Platine wird dann kein WS2811 verwendet.
// Der Pin2 und Pin5 des WS2811 werden verbunden. Ueber Widerstaende
// und Taster an Pin4 des Wannensteckers koennen die Goto Nummern
// ausgewaehlt werden.
//
// Analog und PWM EIngaenge koennen bedingt parallel benutzen koennte. Wenn das PWM Signal 0 ist,
// wird der analog Wert eingelesen. Wenn der Eingang mit Masse verbunden wird, dann wird das
// LED PWM Signal uebersteuert.
// Die Messung ist nicht so genau. Mit einem 1K Pull up koennen nur 5 Zustaende mit den folgenden Widerstaenden
// erkannt werden. Es scheint auch eine Rolle zu spielen on der WS2811 eingesteckt ist oder nicht.

// Analog Taster             WS2811+
// PulUp:   Int 22K  1K+Int  1K+Int   Goto   Limmit >= x
// 0 Ohm    1010     993     991        0    768
// 2.2K      952     319     301        1    272
// 4.7K      895     179     166        2    157
// 8.2K      826     112     101        3     96
// 15K       720      65     56         4     58
// 22K       644      45     39         5     40
// 33K       548      31     25         6     28
// 47K       461      23     17         7     20
// 68K       375      15     9          8     12
// 150K      225       7     3          9      5
// Open       35       0     0         10      0




//--------------------------------------
uint8_t Calc_Analog_GotoNr(uint16_t AIn)
//--------------------------------------
{
  uint8_t i;
  for (i = 0; i < ANALOG_KEYS; i++)
    {
    if (AIn >= AnalogLimmits[i]) return i;
    }
  return i;
}

//-------------------------
uint8_t Calc_Analog_Group()
//-------------------------
{
  pinMode(A2, INPUT_PULLUP);
  uint16_t AIn = 1023 - analogRead(A2);
  uint8_t AnaGroup = Calc_Analog_GotoNr(AIn);

  static uint32_t NextPrint = 0;               // Debug
  if (millis() > NextPrint)
     {
     NextPrint = millis() + 1000;
     Dprintf("Analog=%i %i\n", AIn, AnaGroup);
     }

  static uint32_t AnakeyPressed = 0;
  static uint8_t  LastAnaGroup  = 0;
  if (AnaGroup < ANALOG_KEYS)
       {
       AnakeyPressed = millis();
       return (LastAnaGroup = AnaGroup);
       }
  else { // Store the last pressed key
       if (millis() - AnakeyPressed > 2000) // Problem after 49 days? Yes: if someone holds the key for that long
            return AnaGroup;
       else return LastAnaGroup;
       }
}


//-------------
void loop(void)
//-------------
{
  uint16_t pwm01 = PWM_in(PWM_PIN, HIGH, 5800, PeriodBuffer); // timeout set to 5.8 ms to have a reserve because CPU clock is not very accurate
  long FilteredPwm = adcFilter.filter(pwm01);
  uint8_t LED_PWM  = PWM01_to_LED_PWM(FilteredPwm);
  int8_t Group     = Get_Group(LED_PWM);

  if (LED_PWM == 0 && ReadAnalog)
     {
     Group = Calc_Analog_Group();
     }

  #if 1 // 1 = Normal processing. Set to 0 for debugging the PWM signales
    Set_Goto_and_Check_Prog_Mode_if_Stable(Group);

    // Control the LEDs by the macro
    if (Config_Load_Error == CFG_LOAD_OK)
         {
         MobaLedLib.Update();    // Update the LEDs in the configuration
         for (uint8_t i = 0; i < LAST_LED; i++)
             Set_LED(i, leds[i/3][i%3]/(256/BRIGHT_LEVELS));
         }
    else { // Configuration Error => Fast flash
         static uint32_t Next_Flash = 0;
         if (millis() > Next_Flash)
            {
            uint8_t Val = Get_LED(0) ? 0 : MAX_BRIGHT;
            if ((Config_Load_Error == CFG_LOAD_ERR_INITMACRO) && Val > 0) // Error in Proc_CPX_InitMacros()
                 Next_Flash += 50;                                        // => Short time on
            else Next_Flash += 500;
            for (uint8_t i = 0; i < LAST_LED; i++)
                Set_LED(i, Val);
            }
         }

  #else // Debug output

    static uint32_t NextPrint = 0;
    if (millis() > NextPrint)
       {
       Serial.print(pwm01);        Serial.print(" ");  // Raw PWM
       Serial.print(FilteredPwm);  Serial.print(" ");  // Filtered PWM
       Serial.print(LED_PWM);      Serial.print(" ");  // LED PWM
     //Serial.print(Group);        Serial.print(" ");  // Group
     //Serial.print(" 0 1000 ");                       // For constant scale in the serial plotter
       Serial.println("");                             // New line
       NextPrint = millis() + 1000;
       }
    for (uint8_t i = 0; i <= 10; i++) // Show the Group with the LEDs
        {
        Set_LED(i, i == Group ? 255:0);
        }
    Set_LED(11, Group==-1 ? 255:0);
  #endif

  #ifdef BAUDRATE
    Proc_Serial_Input(); // Enter GotoMode, LED brightness with the serial monitor for debugging
  #endif

  #ifdef SHOW_HEARTBEAT  // Debug
    Upd_LED_Heartbeat(); // Uses 146 Bytes Flash
  #endif
}
