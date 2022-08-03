/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------------------------------------


 DCC rail signal decoder and transmitter to LED Arduino                                    by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes how to read DCC signals (Digital Command Control) from the rail signal
 of a model railway and transmit the accessory and the signalling commands to this Arduino.
 To use this example the program "23_A.DCC_Rail_Decoder_Transmitter.ino" must be running on an
 other Arduino which is connected to this Arduino by two signal wires (and GND).

 See the documentation in the program "23_A.DCC_Rail_Decoder_Transmitter.ino" for more details.


 ATTENTION: Since library version 0.9.0 there is a very comfortable Excel sheet which could           08.12.19
            be used to generate the Arduino code. In the versions before 0.9.3 this Excel
            program was located in the "examples" directory: examples/23_B.LEDs_AutoProg.
            Now the excel sheet is moved to the "extras" directory.
            To start the excel program press the Win+r key together and copy the following line
            into the "Run" dialog and press enter:
               %USERPROFILE%\Documents\Arduino\libraries\MobaLedLib\extras\Prog_Generator_MobaLedLib.xlsm

            When the program is started the first time it's copied to a separate directory to
            be able to save changes without affecting the library. A link to this location is
            generated automatically on the desktop.



 This program is the part for the LED-Arduino.
 To change the behavior of the LEDs only this program has to be modified. The program on the DCC-Arduino
 don't has to be changed.

 The program uses the build in LED of the Arduino as a heartbeat. It's normal if the LED is flashing
 slowly (1 Hz). Don't mix up this with LED on the DCC-Arduino which is used to show buffer overrun error
 with a fast flashing signal.

 The DCC signals are used to control two light signals, two houses an a welding light.
 The following DCC addresses are used:

 DCC           Output
 ~~~~~~~~      ~~~~~~~~~~~~~~~~
 1 Red:        Entry signal Hp0
 1 Green:      Entry signal Hp1
 2 Red:        Entry signal Hp2
 2 Green:      not used

 3 Red:        Departure signal Hp0
 3 Green:      Departure signal Hp1
 4 Red:        Departure signal Hp2
 4 Green:      Departure signal Hp0+Sh1

 11 Red/Green: House with 3 rooms starting with LED 17. When it's turned on (Green) one random room is activated directly, the others are following later
 12 Red/Green: Office building with 3 rooms with neon lights starting with LED 20.
 13 Red/Green: Welding simulation

 Entry signal:                                        Departure signal:
   Hp0         Hp1         Hp2                          Hp0         Hp1         Hp2        Hp0+SH1    @ = active, O = not active
  ______      ______      ______                       ______      ______      ______      ______
 /      \    /      \    /      \                     /      \    /      \    /      \    /      \
 |    0 |    |    @ |    |    @ |       Green         | O    |    | @    |    | @    |    | O    |    Green
 |      |    |      |    |      |                     | @  @ |    | O  O |    | O  O |    | @  O |    Red    Red
 |      |    |      |    |      |                     |    . |    |    . |    |    . |    |    * |    White           * = active, . = not active
 |      |    |      |    |      |                     | .    |    | .    |    | .    |    | *    |           White
 | @  0 |    | 0  0 |    | 0  @ |  Red  Yellow        | O    |    | O    |    | @    |    | O    |    Yellow
 '------'    '------'    '------'                     '------'    '------'    '------'    '------'
    ||          ||          ||                           ||          ||          ||          ||

 The lights fade slowly from one aspect to the other which looks quite nice.

 Details:
 ~~~~~~~~
 The program reads in the DCC accessory signals and maps them to the input variables of the MobaLedLib.
 The constants DCC_FIRST_LOC_ID, DCC_LAST_LOC_ID, DCC_FIRST_TOGGLE_ID and DCC_INPSTRUCT_START below
 define which DCC addresses are mapped to which variables in the InpStructArray[].
 There are two methods to use the DCC signals.
  1. Momentary commands
     One DCC address controls two input channels of the MobaLedLib.
     If the "Red" button is pressed the first InpCh is activated until the button is released.
     If the "Green" button is pressed the second input channel is activated.
     This method is used to control the light signals.
  2. Toggle commands
     This method controls one InpCh of the library. The input is turned on if the "Green"
     button is pressed and its turned of again if the "Red" button is pressed.
     This method is used to control the houses and the welding function below.

 The constant DCC_FIRST_TOGGLE_ID define the first DCC ID which is used for the toggle commands.
 All DCC IDs before are used for Momentary commands.

 In this example the DCC ID 1 - 10 are Momentary commands. And the ID 11 to 30 controls toggle commands.

 The last constant DCC_INPSTRUCT_START determines the first used input channel in the library. Here 0
 is used in this example.
 We get the following mapping:

  DCC  Button      InpCh           Debug LED
  ~~~~ ~~~~~~~~~   ~~~~~           ~~~~~~~~~
  1    Red         0               24 Red
  1    Green       1               25 Green
  2    Red         2               26 Red
  2    Green       3               27 Green
  :    :            :              :
  10   Red         18              42 Red
  10   Green       19              43 Green

  11   Red/Green   20 off/on       44 Blue
  12   Red/Green   21 off/on       45 Blue
  :    :                           :
  30   Red/Green   39 off/on       63 Blue

 This example may be even more complicated than the FlipFlop_Counter example. For this reason,
 all input channels are shown here with LEDs for a better understanding. For this the LED numbers
 42 to 63 are used.
 The LEDs of the momentary inputs are red or Green, the toggle LEDs Blue.

 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php

 Hardware:
 ~~~~~~~~~
 See "23_A.DCC_Interface.ino"

*/

#define SEND_DISABLE_PIN      A1
#define ERROR_LED_PIN         13

#define LED_HEARTBEAT_PIN     13

#define NUM_LEDS              64  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN            6   // Pin D6 is connected to the LED stripe
#else 
#error this example does not support this plattform
#endif

// Define which accessoires CAN messages should be used.
#define DCC_FIRST_LOC_ID      1                      // First local ID which should be copied to the InpStructArray[] of the MobaLedLib
#define DCC_FIRST_TOGGLE_ID   (10+DCC_FIRST_LOC_ID)  // DCC adresses greater equal this number are used to toggle an entry in the InpStructArray[]
                                                     // The DCC adresses smaller than this number are treated as momentarry events.
#define DCC_LAST_LOC_ID       (29+DCC_FIRST_LOC_ID)  // Last local ID which should be copied to the InpStructArray[] of the MobaLedLib
#define DCC_INPSTRUCT_START   0   // Start number in the InpStructArray[]

#define SERIAL_BAUD      115200   // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"

// *** Macros ***
// Entry signal with 3 aspects
#define Entry_Signal3_RGB(LED)   XPatternT1(LED,160,SI_LocalVar,9,0,128,0,0,500 ms,64,4,104,0  ,63,191,191)
#define Entry_Signal3(LED)       XPatternT1(LED,224,SI_LocalVar,3,0,128,0,0,500 ms,145,1  ,63,191,191)

// Departure signal with 4 aspects
#define Dep_Signal4_RGB(LED)     XPatternT1(LED,12,SI_LocalVar,18,0,128,0,0,500 ms,0,240,0,15,0,0,0,0,0,240,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,240,15,0,240,0,0,16,17,17,1,0  ,63,191,191,191)
#define Dep_Signal4(LED)         XPatternT1(LED,12,SI_LocalVar,5,0,128,0,0,500 ms,15,240,0,15,0,240,15,240,0,16  ,63,191,191,191)



//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  // Attention if the signals are not used disable also the MobaLedLib.Set_Input(..) lines in the setup() funktion
  InCh_to_TmpVar(0, 3)    // Use 3 input channels starting with InpCh 0 to set a temporary variable which is used to controll the entry signal
  Entry_Signal3_RGB(4)    // Start LED 4: Demonstration with a LED stripe
//Entry_Signal3(    0)    // Start LED channel 0: Real signal connected to a WS2811 module

  InCh_to_TmpVar(4, 7)    // Use 7 input channels starting with InpCh 4 to set a temporary variable which is used to control the departure signal
  Dep_Signal4_RGB(11)     // Start LED 11: Demonstration with a LED stripe
//Dep_Signal4(    1)      // Start LED channel 1: Real departure signal connected to two  WS2811 modules

  //   LED:                             First LED number in the stripe
  //    |            InCh:              Input channel. The inputs are read in below using the digitalRead() function.
  //    |            |   On_Min:        Minimal number of active rooms. At least two rooms are illuminated.
  //    |            |   |   On_Max:    Number of maximal active lights.
  //    |            |   |   |          rooms: List of room types (see documentation for possible types).
  //    |            |   |   |          |
  House(17,          20, 1,  2,         ROOM_DARK,  ROOM_WARM_W,   ROOM_TV0)   // House with 3 rooms
  House(20,          21, 3,  3,         NEON_LIGHTL,NEON_LIGHTL,  NEON_LIGHTL) // Office building 3 rooms
  Welding(23,        22)

#if 1
  // Debug:
  // ~~~~~~
  //   LED:                             First LED number in the stripe
  //    |   Color:                      Color/Channel of the LED
  //    |   |        InCh:              Input channel. The inputs are read in below using the digitalRead() function.
  //    |   |        |   Val0:          Value if the input is turned off
  //    |   |        |   |   Val1:      Value if the input is turned on
  //    |   |        |   |   |
  Const(24, C_RED,   0,  0, 25)   // DCC 1   \                                                                *
  Const(25, C_GREEN, 1,  0, 25)   //          |                                                               *
  Const(26, C_RED,   2,  0, 25)   // DCC 2    |                                                               *
  Const(27, C_GREEN, 3,  0, 25)   //          |                                                               *
  Const(28, C_RED,   4,  0, 25)   // DCC 3    |                                                               *
  Const(29, C_GREEN, 5,  0, 25)   //          |                                                               *
  Const(30, C_RED,   6,  0, 25)   // DCC 4    |                                                               *
  Const(31, C_GREEN, 7,  0, 25)   //          |                                                               *
  Const(32, C_RED,   8,  0, 25)   // DCC 5    \                                                               * Don't remove this comment and the leading asterix
  Const(33, C_GREEN, 9,  0, 25)   //            Momentary commands                                            * because then the \ is the last character in the
  Const(34, C_RED,  10,  0, 25)   // DCC 6    /                                                               * line which is the sign to continue this line in the
  Const(35, C_GREEN,11,  0, 25)   //          |                                                               * following line => The following line is commented ;-(
  Const(36, C_RED,  12,  0, 25)   // DCC 7    |                                                               * => LED 33 doesn't work !!!
  Const(37, C_GREEN,13,  0, 25)   //          |                                                               * It took 2 h to find this stupid '\' ;-( ;-( ;-( ;-(
  Const(38, C_RED,  14,  0, 25)   // DCC 8    |                                                               *
  Const(39, C_GREEN,15,  0, 25)   //          |                                                               *
  Const(40, C_RED,  16,  0, 25)   // DCC 9    |                                                               *
  Const(41, C_GREEN,17,  0, 25)   //          |                                                               *
  Const(42, C_RED,  18,  0, 25)   // DCC 10   |                                                               *
  Const(43, C_GREEN,19,  0, 25)   //         /                                                                *
  Const(44, C_BLUE, 20,  0, 25)   // DCC 11  \                                                                #
  Const(45, C_BLUE, 21,  0, 25)   // DCC 12   |                                                               #
  Const(46, C_BLUE, 22,  0, 25)   // DCC 13   |                                                               #
  Const(47, C_BLUE, 23,  0, 25)   // DCC 14   |                                                               #
  Const(48, C_BLUE, 24,  0, 25)   // DCC 15   |                                                               #
  Const(49, C_BLUE, 25,  0, 25)   // DCC 16   |                                                               #
  Const(50, C_BLUE, 26,  0, 25)   // DCC 17   |                                                               #
  Const(51, C_BLUE, 27,  0, 25)   // DCC 18   |                                                               #
  Const(52, C_BLUE, 28,  0, 25)   // DCC 19   \                                                               #
  Const(53, C_BLUE, 29,  0, 25)   // DCC 20     Toggle commands                                               #
  Const(54, C_BLUE, 30,  0, 25)   // DCC 21   /                                                               #
  Const(55, C_BLUE, 31,  0, 25)   // DCC 22   |                                                               #
  Const(56, C_BLUE, 32,  0, 25)   // DCC 23   |                                                               #
  Const(57, C_BLUE, 33,  0, 25)   // DCC 24   |                                                               #
  Const(58, C_BLUE, 34,  0, 25)   // DCC 25   |                                                               #
  Const(59, C_BLUE, 35,  0, 25)   // DCC 26   |                                                               #
  Const(60, C_BLUE, 36,  0, 25)   // DCC 27   |                                                               #
  Const(61, C_BLUE, 36,  0, 25)   // DCC 28   |                                                               #
  Const(62, C_BLUE, 36,  0, 25)   // DCC 29   |                                                               #
  Const(63, C_BLUE, 36,  0, 25)   // DCC 30  /                                                                #
#endif
  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];       // Define the array of leds

MobaLedLib_Create(leds);   // Define the MobaLedLib instance
LED_Heartbeat_C LED_HeartBeat(LED_HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.

char Buffer[12] = "";      // Store the received messages from the DCC-Arduino
/*
 Problem:
 Bei der Lenz Zentrale LZV100 von Rolf wird keine Message beim
 loslassen des Tasters geschickt ;-(
 Das fuehrt dazu, dass die mit den Tasten verknuepften Ausgaenge nicht mehr aus gehen.
 Als Abhilfe wird die letzte Taste und der Zeitpunkt zu dem sie empfangen wurde
 gespeichert. Nach 400ms wird automatisch das Taste losgelassen Ereignis generiert.
 Wenn eine andere Taste empfangen wird wird die alte Taste ebenfalls "losgelassen".
*/
#define GEN_BUTTON_RELEASE
#ifdef GEN_BUTTON_RELEASE
  uint8_t  LastChannel;
  uint32_t LastTime = 0;
#endif

//----------------
void Proc_Buffer()
//----------------
{
  uint16_t Addr, Direction, OutputPower, State;
  //Serial.println(Buffer); // Debug
  switch (*Buffer)
    {
    case '@': if (sscanf(Buffer+1, "%i %x %x", &Addr, &Direction, &OutputPower) == 3)
                 {
                 //Addr--; // Internally the address starts with 0
                 if (Addr >= DCC_FIRST_LOC_ID)
                    {
                    if (Addr < DCC_FIRST_TOGGLE_ID)
                         {
                         uint8_t Channel = ((Addr - DCC_FIRST_LOC_ID)*2) + DCC_INPSTRUCT_START + Direction;
                         //char s[30];                                      // Debug
                         //sprintf(s, "InpNr %i %i", Channel, OutputPower); // Debug  23.05.19:  Added: OutputPower
                         //Serial.println(s);                               // Debug
                         MobaLedLib.Set_Input(Channel, OutputPower);

                         #ifdef GEN_BUTTON_RELEASE                                                            // 23.05.19:
                           if (OutputPower)
                              {
                              if (LastTime && LastChannel != Channel) MobaLedLib.Set_Input(LastChannel, 0); // Send release
                              LastTime = millis();
                              LastChannel = Channel;
                              }
                         #endif
                         }
                    else if (Addr <= DCC_LAST_LOC_ID) // Addr >= DCC_FIRST_TOGGLE_ID
                            {                                        // ~~~~~~~~~~~ 18 ~~~~~~~~~~~~~~~~~~~~~~~~~~
                            uint8_t dst = Addr - DCC_FIRST_TOGGLE_ID + (DCC_FIRST_TOGGLE_ID - DCC_FIRST_LOC_ID)*2 + DCC_INPSTRUCT_START;
                            //char s[30];                                                                       // Debug
                            //sprintf(s, "Addr %i Dir %i Pow %i dst %i", Addr, Direction, OutputPower, dst);    // Debug
                            //Serial.println(s);                                                                // Debug
                            if (OutputPower > 0)
                               MobaLedLib.Set_Input(dst, Direction > 0);
                            }
                            // Addr  InNr
                            //  0    0, 1
                            //  1    2, 3
                            //  9    18,19
                            // 10    20
                            // 11    21

                    }
                 return;
                 }
              break;
    case '$': if (sscanf(Buffer+1, "%i %x", &Addr, &State) == 2)
                 {
                 // Not implemented yet
                 return;
                 }
              break;
    }
  Serial.println(" Parse Error");
}

//-----------------------
void Proc_Received_Char()
//-----------------------
// Receive DCC Accessory Turnout messages:
//  "@ 319 00 01\n":  Addr, Direction, OutputPower
//
// and DCC Signal Output messages:
//  "$ 123 00\n":     Addr, State
{
  #ifdef GEN_BUTTON_RELEASE                                                                                   // 23.05.19:
    if (LastTime && millis()-LastTime > 400) // Use 1100 if no repeat is wanted
       {
       MobaLedLib.Set_Input(LastChannel, 0); // Send release
       LastTime = 0;
       Serial.print(F("Timeout "));
       Serial.println(millis());
       }
  #endif

  while (Serial.available() > 0)
     {
     char c = Serial.read();
     //Serial.print(c); // Debug
     switch (c)
        {
        case '@':
        case '$': if (*Buffer) Serial.println(F(" Unterm. Error"));
                  *Buffer = c; Buffer[1] = '\0'; break;
        default:  // Other characters
                  uint8_t ExpLen;
                  switch(*Buffer)
                     {
                     case '@': ExpLen = 11; break;
                     case '$': ExpLen =  8; break;
                     default : ExpLen =  0;
                               Serial.print(c); // Enable this line to show status messages from the DCC-Arduino
                     }
                  if (ExpLen)
                     {
                     uint8_t len = strlen(Buffer);
                     if (len < ExpLen)
                          {
                          Buffer[len++] = c;
                          Buffer[len]   = '\0';
                          }
                     else {
                          if (c != '\n')
                               Serial.println(F(" Length Error"));
                          else Proc_Buffer();
                          *Buffer = '\0';
                          }
                     }
        }
     }
}

//-----------
void setup(){
//-----------
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  Serial.begin(SERIAL_BAUD); // Communication with the DCC-Arduino must be fast

  Serial.println(F("DCC_Rail_Decoder_Transmitter Example B"));

  pinMode(SEND_DISABLE_PIN, OUTPUT);
  digitalWrite(SEND_DISABLE_PIN, 0);

  MobaLedLib.Set_Input(0, 1); // Set the default value for the entry light signal at power on
  MobaLedLib.Set_Input(4, 1); // Set the default value for the departure light signal at power on
}

//-----------
void loop(){
//-----------
  LED_HeartBeat.Update();

  MobaLedLib.Update();                // Update the LEDs in the configuration

  Proc_Received_Char();               // Process characters received from the DCC-Arduino

  digitalWrite(SEND_DISABLE_PIN, 1);  // Stop the sending of the DCC-Arduino because the RS232 interrupts are not processed during the FastLED.show() command
  FastLED.show();                     // Show the LEDs (send the leds[] array to the LED stripe)
  digitalWrite(SEND_DISABLE_PIN, 0);  // Allow the sanding of the DCC commands again
}

