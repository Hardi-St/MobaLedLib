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


 Railway Light Signal Pattern Function                                                     by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes the pattern function using a Railway Light Signal control. In addition the
 Counter() and the New_Local_Var() function are explained.
 The example uses two push buttons to change the shown signal.

 The pattern function is a very powerful tool which can be used to create arbitrary sequences.

 On a model railway there are several sequences which could be implemented with the pattern
 function. This could be traffic lights, construction lightning, hazard lights,
 fairground running lights, push button actions or many others.

 The pattern function describe the actions with a table. This table defines when a certain output
 should be enabled.

 This example shows how a Railway Light Signal could be implemented. The entry signal should show
 the following three signal aspects:

   Hp0         Hp1         Hp2       @ = active, O = not active
  ______      ______      ______
 /      \    /      \    /      \
 |    O |    |    @ |    |    @ |        Green
 |      |    |      |    |      |
 |      |    |      |    |      |
 |      |    |      |    |      |
 | @  O |    | O  O |    | O  @ |    Red Yellow
 '------'    '------'    '------'
    ||          ||          ||

 The lights fade slowly from one aspect to the other which looks quite nice.

 In contrast to the last example, the signal should be controlled manually. At the traffic light,
 the different pictures were displayed one after the other. Here buttons determine the display.
 In this example, one switch is used to switch to the next state and the other to the previous one.

 Therefor the Counter() function is used. This function has one mode where the counter is stored to
 a local variable. This is activated with the flag CF_ONLY_LOCALVAR. But prior the local variable
 has to be created. That's done with the New_Local_Var() in the macro below.
 The other flags of the Counter() function are:
   - CF_ROTATE:      Counter starts with the beginning if the end is reached
   - CF_UP_DOWN:     The enable input is use as a second input. If it's triggered the counter is decremented

   #define Entry_Signal_UpDown_RGB(LED, InChU, InChD)                                                \
                         New_Local_Var()                                                             \
                         Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_UP_DOWN, InChU, InChD, 0 Sec, 3)  \
                         _Entry_Signal3_RGB(LED)

 The last command in the macro is the _Entry_Signal3_RGB() macro which is generated with the Excel program
 Pattern_Configurator.xlsm. Here the switch "Goto Mode" is set to 1. In this mode an additional line
 is shown. In the "Goto Tabelle" following one letter commands could be used:
    S   : Start
    P   : Position for goto
    G nr: Goto position. The followed number define the position which is executed next
    E   : Goto end

 The excel table looks like:

    Goto Mode:     1
    Dauer          500 ms

    Goto Tabelle:  E    SE   SE

    Spalten Nr->   1    2    3
    Rot  1
    Gruen 1              x    x
    Blau 1
    Rot  2                   x
    Gruen 2                   x
    Blau 2
    Rot  3         x
    Gruen 3
    Blau 3
                   HP0  HP1  HP 2

 In the table we have tree columns where an 'x' is used to mark an active LED. The first column shows
 the HP0 signal. Therefore the lower Red LED is marked with an 'x'. The second column show HP1 and the
 third HP2 with the corresponding LEDs.

 Important for this example is the row "Goto Tabelle". Here we find an 'E' in the tree columns.
 This is used to end the pattern sequence after the corresponding column is shown. Without this 'x'
 the following columns would be show like in the traffic light example.
 The columns 2 and 3 are marked in addition with a 'S'. This defines the start point which is used if the
 Counter() function is changed. The first column don't has to be marked with an 'S' because it's activated
 automatically if the Counter() function returns 0.
 => If the counter returns 0 HP0 is shown, if it returns 1 HP1 and if 2 is returned HP2 is shown.
 It's important, that the states are only activated once if the counter changes.
 The result of the excel table is represented by this macro line:

    #define _Entry_Signal3_RGB(LED)   XPatternT1(LED,160,SI_LocalVar,9,0,128,0,0,500 ms,64,4,104,0  ,63,191,191)

 It's used in the "Entry_Signal_UpDown_RGB()" macro to show the signal.

 The example could easily be expanded to show other light signals by modifying the excel table.
 One example is the departure signal:

   Hp0         Hp1         Hp2        Hp0+SH1    @ = active, O = not active
  ______      ______      ______      ______
 /      \    /      \    /      \    /      \
 | O    |    | @    |    | @    |    | O    |    Green
 | @  @ |    | O  O |    | O  O |    | @  O |    Red    Red
 |    . |    |    . |    |    . |    |    * |    White           * = active, . = not active
 | .    |    | .    |    | .    |    | *    |           White
 | O    |    | O    |    | @    |    | O    |    Yellow
 '------'    '------'    '------'    '------'
    ||          ||          ||          ||


 Test with RGB LED Stripe:
 ~~~~~~~~~~~~~~~~~~~~~~~~~
 To be able to test the example without a special light signal a two macros are generated below.
 The first could be used for demonstration with an RGB LED stripe (Entry_Signal_UpDown_RGB).

 The second macro (Entry_Signal_UpDown) is designed for the usage with a real light signal.
 It is controlled by a WS2811 module.
 Attention: "Viessmann multiplex" signals could not be used. I'm planing to implement a
            multiplex driver with a ATTiny which costs about 2 Euro. But therefore I need a
            signal from Viessmann to test it. Maybe there is someone out there who donates one...

 By default the RGB example is enabled in the configuration (Change the comments to use the example
 with a real signal).

 There are two further macros in the configuration which define an departure signal with four aspects.

 Other examples:
 ~~~~~~~~~~~~~~~
 In the following example "CAN_Bus_MS2_RailwaySignal" the signal is controlled over the CAN
 bus from a Maerklin MS2.
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and one WS2811 module. For tests a LED stripe could also be used.
 The DIN pin of the WS2811 module is connected to pin D6 (LED_DO_PIN).
 Two push buttons are connected to D7 an D8. The other pin of the switches is connected to ground.
 The switches are active if the pin is connected to ground.
 Attention in the other examples an inverse logic was used. Here this logic is better because
 one switch could easily be simulated with a wire.
 Only one switch should be connected to ground at a time.


 Entry signal wit 3 LEDs:                                    Departure signal with 5 LEDs:
                            from Arduino pin D6                                         from Arduino pin D6
      LEDs                          |                             LEDs                          |
                  ------------      |                                         ------------      |
   .-(Yellow)--C3-+  ------  +-+5V  |                          .-(Yellow)--C3-+  ------  +-+5V  |
   o--(Red )---C1-+ |WS2811| +-DIN--'                          o--(Red1)---C1-+ |WS2811| +-DIN--'
   o--------------+ |  5V  | +-DOut-.                          o--------------+ |  5V  | +-DOut-.
   '-(Green)---C2-+  ------  +-GND  |                          o-(Green)---C2-+  ------  +-GND  |
                  ------------      |                          |              ------------      |
                            (to the next WS281x)               |                                |
                                                               |              ------------      |
                                                               |         --C3-+  ------  +-+5V  |
                                                               o--(Red2)---C1-+ |WS2811| +-DIN--'
                                                               |              + |  5V  | +-DOut-.
                                                               '-(White)---C2-+  ------  +-GND  |
                                                                              ------------      |
                                                                                        (to the next WS281x)
*/

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#define NUM_LEDS     32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#elif defined(ESP32)
#define LED_DO_PIN   27  // Pin 27 is connected to the LED stripe
#ifndef LED_BUILTIN 
#define LED_BUILTIN  2   // Built in LED
#endif
#else 
#error this example does not support this plattform
#endif
#define SWITCH1_PIN  7   // Pin D7 is connected to switch 1
#define SWITCH2_PIN  8   // Pin D8 is connected to switch 2


// *** Macros ***
// Entry signal with 3 aspects
#define _Entry_Signal3_RGB(LED)   XPatternT1(LED,160,SI_LocalVar,9,0,128,0,0,500 ms,64,4,104,0  ,63,191,191)
#define _Entry_Signal3(LED)       XPatternT1(LED,224,SI_LocalVar,3,0,128,0,0,500 ms,145,1  ,63,191,191)

#define Entry_Signal_UpDown_RGB(LED, InChU, InChD)                                                         \
                         New_Local_Var()                                                             \
                         Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_UP_DOWN, InChU, InChD, 0 Sec, 3)  \
                         _Entry_Signal3_RGB(LED)

#define Entry_Signal_UpDown(LED, InChU, InChD)                                                             \
                         New_Local_Var()                                                             \
                         Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_UP_DOWN, InChU, InChD, 0 Sec, 3)  \
                         _Entry_Signal3(LED)


// Departure signal with 4 aspects
#define _Dep_Signal4_RGB(LED)     XPatternT1(LED,12,SI_LocalVar,18,0,128,0,0,500 ms,0,240,0,15,0,0,0,0,0,240,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,240,15,0,240,0,0,16,17,17,1,0  ,63,191,191,191)
#define _Dep_Signal4(LED)         XPatternT1(LED,12,SI_LocalVar,5,0,128,0,0,500 ms,15,240,0,15,0,240,15,240,0,16  ,63,191,191,191)

#define Depart_Signal_UpDown_RGB(LED, InChU, InChD)                                                  \
                         New_Local_Var()                                                             \
                         Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_UP_DOWN, InChU, InChD, 0 Sec, 4)  \
                         _Dep_Signal4_RGB(LED)

#define Depart_Signal_UpDown(LED, InChU, InChD)                                                      \
                         New_Local_Var()                                                             \
                         Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_UP_DOWN, InChU, InChD, 0 Sec, 4)  \
                         _Dep_Signal4(LED)




//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {//                     LED:          First LED number in the stripe
   //                      |  InChU:    Input channel which activates the next aspects..
   //                      |  |  InChD: Input channel which activates the previous aspects.
   //                      |  |  |
  Entry_Signal_UpDown_RGB( 0, 1, 2)
//Entry_Signal_UpDown(     0, 1, 2)     // This could be used wit a real Railway signal
//Depart_Signal_UpDown_RGB(0, 1, 2)
//Depart_Signal_UpDown(    0, 1, 2)
  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];     // Define the array of leds

MobaLedLib_Create(leds); // Define the MobaLedLib instance

LED_Heartbeat_C LED_Heartbeat(LED_BUILTIN); // Use the build in LED as heartbeat

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  pinMode(SWITCH1_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pins
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib.Set_Input(1, !digitalRead(SWITCH1_PIN));
  MobaLedLib.Set_Input(2, !digitalRead(SWITCH2_PIN));

  MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)

  LED_Heartbeat.Update(); // Update the heartbeat LED. This must be called periodically in the loop() function.
}

/*
 Arduino Nano:          +-----+
           +------------| USB |------------+
           |            +-----+            |
        B5 | [ ]D13/SCK        MISO/D12[ ] |   B4
           | [ ]3.3V           MOSI/D11[ ]~|   B3
           | [ ]V.ref     ___    SS/D10[ ]~|   B2
        C0 | [ ]A0       / N \       D9[ ]~|   B1
        C1 | [ ]A1      /  A  \      D8[ ] |   B0   -> push button 2  --o-- GND
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   -> push button 1  --'
        C3 | [ ]A3       \_0_/       D6[ ]~|   D6   -> WS281x LED stripe pin DIN
        C4 | [ ]A4/SDA               D5[ ]~|   D5
        C5 | [ ]A5/SCL               D4[ ] |   D4
           | [ ]A6              INT1/D3[ ]~|   D3
           | [ ]A7              INT0/D2[ ] |   D2
           | [ ]5V                  GND[ ] |
        C6 | [ ]RST                 RST[ ] |   C6
           | [ ]GND   5V MOSI GND   TX1[ ] |   D0
           | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
           |          [ ] [ ] [ ]          |
           |          MISO SCK RST         |
           | NANO-V3                       |
           +-------------------------------+

 Arduino Uno:                           +-----+
           +----[PWR]-------------------| USB |--+
           |                            +-----+  |
           |           GND/RST2  [ ] [ ]         |
           |         MOSI2/SCK2  [ ] [ ]  SCL[ ] |   C5
           |            5V/MISO2 [ ] [ ]  SDA[ ] |   C4
           |                             AREF[ ] |
           |                              GND[ ] |
           | [ ]N/C                    SCK/13[A] |   B5
           | [ ]v.ref                 MISO/12[A] |   .
           | [ ]RST                   MOSI/11[A]~|   .
           | [ ]3V3    +---+               10[ ]~|   .
           | [ ]5v     | A |                9[ ]~|   .
           | [ ]GND   -| R |-               8[B] |   B0  -> push button 2  --o-- GND
           | [ ]GND   -| D |-                    |                           |
           | [ ]Vin   -| U |-               7[A] |   D7  -> push button 1  --'
           |          -| I |-               6[A]~|   .   -> WS281x LED stripe pin DIN
           | [ ]A0    -| N |-               5[C]~|   .
           | [ ]A1    -| O |-               4[A] |   .
           | [ ]A2     +---+           INT1/3[A]~|   .
           | [ ]A3                     INT0/2[ ] |   .
      SDA  | [ ]A4      RST SCK MISO     TX>1[ ] |   .
      SCL  | [ ]A5      [ ] [ ] [ ]      RX<0[ ] |   D0
           |            [ ] [ ] [ ]              |
           |  UNO_R3    GND MOSI 5V  ____________/
            \_______________________/
*/

