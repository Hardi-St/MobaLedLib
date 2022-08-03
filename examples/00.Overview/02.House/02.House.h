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


 Animated house with 7 rooms which are illuminated randomly                                by Hardi   02.10.18
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with one animated house.
 The house has 7 different rooms which are illuminated randomly to simulate a house
 where people live. There are different light types used:
 There are rooms with
 - dark light
 - neon light
 - colored light
 - running TV
 - chimney
 - ...

 Attention: It takes some time (up to 2.5 minutes) to see changes. The people in the houses
            don't run from room to room and turnig the lights on and off.
            Change the #defines HOUSE_MIN_T and HOUSE_MAX_T below to modify the update rates.

 In this example the house is always active when the power is turned on. At the beginning
 one room is illuminated. After a random time the light in other rooms is turned on or off.
 The numbers in the "House()" line below define how many rooms are "used". The first number
 (On_Min = 2) controls the minimal number of illuminated rooms. If the number of illuminated
 rooms is below this value additional roomes are turned on. In this example there should
 be at least two active rooms after a while.
 The second number (On_Max = 5) defines how many LEDs are turned on maximal. The average
 number of active lights will be some where in the middle: (On_Min + On_Max) / 2 = 3.5.

 The number of rooms could be changed by adding or removing "ROOM_.." constants to the
 "House()" line. Several houses could be controlled by adding "House()" lines.

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 The "03.Switched_Houses" example demonstrates how several houses can be turned on and
 off with switches.

 Video:
 ~~~~~~
 This video demonstrates the example:
   https://vimeo.com/308722422

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and a WS2812 LED stripe.
 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).

 All examples could also be used with the other LED stripe types which are supported
 by the FastLED library (See FastLED Blink example how to adapt the "FastLED.addLeds"
 line.).
*/

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#define HOUSE_MIN_T  50  // Minimal time [s] to the next event (1..255)
#define HOUSE_MAX_T 150  // Maximal random time [s]              "

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

//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {//   LED:                   First LED number in the stripe
   //    |    InCh:            Input channel. Here the special input 1 is used which is always on
   //    |    |    On_Min:     Minimal number of active rooms. At least two rooms are illuminated.
   //    |    |    |   On_Max: Number of maximal active lights.
   //    |    |    |   |       rooms: List of room types (see documentation for possible types).
   //    |    |    |   |       |
  House(0,   SI_1, 2,  5,      ROOM_DARK, ROOM_BRIGHT, ROOM_WARM_W, ROOM_TV0, NEON_LIGHT, ROOM_D_RED, ROOM_COL2) // House with 7 rooms
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
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
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
        C1 | [ ]A1      /  A  \      D8[ ] |   B0
        C2 | [ ]A2      \  N  /      D7[ ] |   D7
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
           | [ ]GND   -| R |-               8[B] |   B0
           | [ ]GND   -| D |-                    |
           | [ ]Vin   -| U |-               7[A] |   D7
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










