/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de

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


 Logic                                                                                     by Hardi   13.12.18
 ~~~~~

 This example demonstrates the logic() function of the MobaLedLib to turn on the St. Andrew's Cross
 (Andreaskreuz) when a train is detected.

 Let's assume we have a railway crossing and a railway switch like shown below:

               / /         | :   :
              / / 1        X : | :
  ___________/ /_____________:   :___________________________
  ____________/______________: | :___________________________
           Switch 0          :   : X
                             : | : |

  |-----A----|--------B--------|--------C--------|-----D----|


 The St. Andrew's Cross (X) should be activated if a train is detected in one of the sections A-D.
 It shold not be activated in section A if the switch is pointing to 1 because in this case the
 train will not go to the railway crossing.

 The logic will be:   StAndrew =  (NOT Switch AND A) OR B OR C OR D.

 The logical links must be expressed as Disjunctive Normal Form (DNF). In this representation,
 groups of "AND" are combined with "OR". The brackets are not displayed in the DNF (implicit
 bracketing). In the macro it looks like this:

   Logic(StAndrew, NOT Switch AND A OR B OR C OR D)

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and one WS2811 modules. For tests a LED stripe could also be used.
 The DIN pin of the first WS2811 module is connected to pin D6 (LED_DO_PIN).
 One toggle switches and four push buttons are connected to D7-D11 to simumate the railway switch
 and the sections. The other pin of the switches are connected to ground.
 The switches are active (1) if they are connected to ground.
*/

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#define NUM_LEDS      32 // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#else 
#error this example does not support this plattform
#endif
#define RW_SWITCH_PIN 7  // Pin D7  is connected to the railway switch
#define SWITCH_A_PIN  8  // Pin D8  is connected to the switch A
#define SWITCH_B_PIN  9  // Pin D9  is connected to the switch B
#define SWITCH_C_PIN  10 // Pin D10 is connected to the switch C
#define SWITCH_D_PIN  11 // Pin D11 is connected to the switch B

// Define the InpChannels
#define Switch    0
#define A         1
#define B         2
#define C         3
#define D         4
#define E         5
#define StAndrew  200   // Internal variable

//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  Logic(StAndrew, NOT Switch AND A OR B OR C OR D)

  //          LED:          First LED number in the stripe
  //           |  Cx:       First used channel of the WS2811 module (C1, C2, C3 or C_RED, C_GREEN, C_BLUE)
  //           |  |   InCh: Input channel. The input is read in below using the digitalRead() function.
  //           |  |   |
  AndreaskrRGB(0,     StAndrew)    // St. Andrew's Cross demonstartion with a LED stripe
//Andreaskreuz(0, C1, StAndrew)    // St. Andrew's Cross for two channels of a WS2811 module (If used with RGB LEDs the Red and Green LED of one RGB LED is alternating flashing)

  // Debug LEDs which show the inputs
  Const(3, C_RED,  Switch,  0, 10)
  Const(3, C_GREEN,Switch, 10,  0)
  Const(4, C_BLUE, A,       0, 10)
  Const(5, C_BLUE, B,       0, 10)
  Const(6, C_BLUE, C,       0, 10)
  Const(7, C_BLUE, D,       0, 10)

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

  pinMode(RW_SWITCH_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pins
  pinMode(SWITCH_A_PIN,  INPUT_PULLUP);
  pinMode(SWITCH_B_PIN,  INPUT_PULLUP);
  pinMode(SWITCH_C_PIN,  INPUT_PULLUP);
  pinMode(SWITCH_D_PIN,  INPUT_PULLUP);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib.Set_Input(Switch, !digitalRead(RW_SWITCH_PIN));
  MobaLedLib.Set_Input(A,      !digitalRead(SWITCH_A_PIN));
  MobaLedLib.Set_Input(B,      !digitalRead(SWITCH_B_PIN));
  MobaLedLib.Set_Input(C,      !digitalRead(SWITCH_C_PIN));
  MobaLedLib.Set_Input(D,      !digitalRead(SWITCH_D_PIN));

  MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)

  LED_Heartbeat.Update(); // Update the heartbeat LED. This must be called periodically in the loop() function.
}

/*
 Arduino Nano:          +-----+
           +------------| USB |------------+
           |            +-----+            |
        B5 | [ ]D13/SCK        MISO/D12[ ] |   B4
           | [ ]3.3V           MOSI/D11[ ]~|   B3   -> push button D --- GND
           | [ ]V.ref     ___    SS/D10[ ]~|   B2   -> push button C --- GND
        C0 | [ ]A0       / N \       D9[ ]~|   B1   -> push button B --- GND
        C1 | [ ]A1      /  A  \      D8[ ] |   B0   -> push button A --- GND
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   -> toggle switch --- GND
        C3 | [ ]A3       \_0_/       D6[ ]~|   D6   -> WS2811 Modules pin DIN
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
           | [ ]v.ref                 MISO/12[A] |   .   -> push button D --- GND
           | [ ]RST                   MOSI/11[A]~|   .   -> push button C --- GND
           | [ ]3V3    +---+               10[ ]~|   .   -> push button B --- GND
           | [ ]5v     | A |                9[ ]~|   .   -> push button A --- GND
           | [ ]GND   -| R |-               8[B] |   B0  -> toggle switch --- GND
           | [ ]GND   -| D |-                    |
           | [ ]Vin   -| U |-               7[A] |   D7  -> toggle switch --- GND
           |          -| I |-               6[A]~|   .   -> WS2811 Modules pin DIN
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

