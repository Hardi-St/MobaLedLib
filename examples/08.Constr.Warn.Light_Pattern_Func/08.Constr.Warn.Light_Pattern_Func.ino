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


 Construction Warning Light Pattern Function                                               by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes the pattern function using a Construction Warning Light.

 The pattern function is a very powerful tool which can be used to create arbitrary sequences.

 On a model railway there are several sequences which could be implemented with the Pattern
 function. This could be traffic lights, construction lightning, hazard lights,
 fairground running lights, push button actions or many others.

 The Pattern function describes the actions with a table. This table defines when a certain output
 should be enabled. The table is defined with the Excel file "Pattern_Configurator.xlsm".

 For this example the following table is used:
   LED 0   x   x   x   x   x   x   x   x   x   x   x   x   x   x   x   x   x   x   .   .   x   x
   LED 1           x   x   x   x   x   x   x   x   x   x   x   x   x   x   x   x                   x   x
   LED 2                   x   x   x   x   x   x   x   x   x   x   x   x   x   x                           x   x
   LED 3                           x   x   x   x   x   x   x   x   x   x   x   x                                   x   x
   LED 4                                   x   x   x   x   x   x   x   x   x   x                                           x   x
   LED 5                                           x   x   x   x   x   x   x   x                                                   x   x
   LED 6                                                   x   x   x   x   x   x                                                          x   x
   LED 7                                                           x   x   x   x                                                                  x   x
   LED 8                                                                   x   x                                                                          x   x

 At this point we won't discribe the details. Just try it and have fun !

 In the "09.TrafficLight_Pattern_Func" the details of the pattern function are explained.

 Test with RGB LED Stripe:
 ~~~~~~~~~~~~~~~~~~~~~~~~~
 To be able to test the example without special construction warning lights there is one
 line in the configuration below which could be used with an RGB stripe.
 To use the example with single LEDs enable the second line.

 Other examples:
 ~~~~~~~~~~~~~~~
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
 and 3 WS2811 modules. For tests a LED stripe could also be used.

                            from Arduino pin D6
                                  |
                ------------      |
   .-(LED2)--C3-+  ------  +-+5V  |
   o-(LED0)--C1-+ |WS2811| +-DIN--'
   o------------+ |  5V  | +-DOut-.
   '-(LED1)--C2-+  ------  +-GND  |
                ------------      |
                                  |
                ------------      |
   .-(LED5)--C3-+  ------  +-+5V  |
   o-(LED3)--C1-+ |WS2811| +-DIN--'
   o------------+ |  5V  | +-DOut-.
   '-(LED4)--C2-+  ------  +-GND  |
                ------------      |
                                  |
                ------------      |
   .-(LED8)--C3-+  ------  +-+5V  |
   o-(LED6)--C1-+ |WS2811| +-DIN--'
   o------------+ |  5V  | +-DOut-.
   '-(LED7)--C2-+  ------  +-GND  |
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


// *** Macro definitions ***
#define RGB_ConstrWarnLight(LED,InNr)   APatternT2(LED,0,InNr,26,0,255,0,0,100 ms,200 ms,3,0,0,12,0,0,176,1,0,192,6,0,0,219,0,0,108,3,0,176,109,0,192,182,1,0,219,54,0,108,219,0,176,109,27,192,182,109,0,219,182,13,108,219,54,176,109,219,198,182,109,27,219,182,109,111,219,182,13,0,0,0,0,0,0,3,0,0,12,0,0,128,1,0,0,6,0,0,192,0,0,0,3,0,0,96,0,0,128,1,0,0,48,0,0,192,0,0,0,24,0,0,96,0,0,0,12,0,0,48,0,0,0,6,0,0,24,0,0,0,3,0,0,12,0,0,0,0,0,0)
#define ConstrWarnLight(    LED,InNr)   APatternT2(LED,0,InNr,9, 0,255,0,0,100 ms,200 ms,1,2,12,24,112,224,192,131,7,31,62,252,248,241,231,207,191,127,255,255,3,0,16,32,128,0,1,4,8,32,64,0,1,2,8,16,64,128,0,2,4,16,32,0,0)


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {//                LED:       First LED number in the stripe
   //                 |   InCh: Input channel (Special Input which is allways on)..
   //                 |   |
  RGB_ConstrWarnLight(0,  SI_1)  // Test with an RGB stripe
//ConstrWarnLight(    0,  SI_1)  // Use this with separate LEDs controlled by WS2811 modules
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
           | [ ]v.ref                 MISO/12[A] |   .
           | [ ]RST                   MOSI/11[A]~|   .
           | [ ]3V3    +---+               10[ ]~|   .
           | [ ]5v     | A |                9[ ]~|   .
           | [ ]GND   -| R |-               8[B] |   B0
           | [ ]GND   -| D |-                    |
           | [ ]Vin   -| U |-               7[A] |   D7
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

