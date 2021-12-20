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


 Gaslights                                                                                 by Hardi   02.10.18
 ~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib to control gas street lamps.

 Gas lamps were previously lit by lamp lighters. Later they have been controlled by
 timers or a light sensors. In any case the lamps are not switched on at the same time.
 In addition it takes a while until the gas lights reach the full brightness.
 Due to wind and gas pressure drops they flicker some times.
 (See. http://www.gaswerk-augsburg.de/fernzuendung.html)
 This behavior is simulated with this example.

 The gas lights in the model are equipped with LEDs or light bulbs. They could be powered with
 WS2811 modules which have 3 outputs. Each of them could drive 20 mA.
 LED lamps could be powered with a single channel => 2 WS2811 modules are needed.

 Because light bulbs draw more current additional FETs are required. For each channel following
 parts are needed: FET BS250, Resistor 4K7, Diode 1N4148 (See "schematic" below or documentation).

 Alternative, if the light bulbs draw a current below 40 mA, tree channels of a 12V WS2811 module
 are connected together to increase the current. In this case 6 WS2811 modules are use for the
 example.
 Bulbs with a higher consumption should use the FET method because there are losses in the current
 detection even if 3 outputs are connected in parallel which reduces the brightness.

 In the configuration below there are to lines. The first one is used for LEDs, the second one for
 lamps with light bulbs. Comment / un-comment the corresponding line.

 The example uses one switch to turn on / off the gas lights. The switch is connected to ground
 because then the internal pull up resistor of the cpu could be used (No external resistor needed).
 => The gas lights are turned on if the switch is open. This is also useful because the example
 could be used without switches (Gas lights are always on).

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and 2/6  WS2811 modules concted to the gas lights. For tests a LED stripe could also be used.
 The DIN pin of the first WS2811 module is connected to pin D6 (LED_DO_PIN). The DOUT pin of
 the first module is connected to the DIN pin of the next WS2811 module...
 One toggle switch connected to D7 is used to turn on / off the gas lights. The other pin of the
 switch is connected to ground.
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
#define SWITCH_PIN   7   // Pin D7 is connected to the switch
#elif defined(ESP32)
#define LED_DO_PIN   27  // Pin 27 is connected to the LED stripe
#define SWITCH_PIN   12  // Pin 12 is connected to the switch
#ifndef LED_BUILTIN 
#define LED_BUILTIN  2   // Built in LED
#endif
#else 
#error this example does not support this plattform
#endif


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {//      LED:        First LED number in the stripe
   //       |    InCh: Input channel. The input is read in below using the digitalRead() function.
   //       |    |     Gas lights: List of gas light types (see documentation for possible types).
   //       |    |     |
//GasLights(0,   0,    GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3D, GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3) // LED lamps (..D = Dark (simulate broken light))
  GasLights(0,   0,    GAS_LIGHT,  GAS_LIGHT,  GAS_LIGHTD,  GAS_LIGHT,  GAS_LIGHT,  GAS_LIGHT)  // RGB stripe or light bulbs with parrallel used ouputs
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

  pinMode(SWITCH_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib.Set_Input(0, digitalRead(SWITCH_PIN));

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
           | [ ]v.ref                 MISO/12[A] |   .
           | [ ]RST                   MOSI/11[A]~|   .
           | [ ]3V3    +---+               10[ ]~|   .
           | [ ]5v     | A |                9[ ]~|   .
           | [ ]GND   -| R |-               8[B] |   B0
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


  Pins of a WS2811 module:

      .+--+--+--+.
      |G  +  R  B|
      |          |
      |          |
      |  WS2811  |
      |          |
      |          |
      | + DI GND |    DOUT is on the bottom side
      '-+--+--+--'



  Configuration with LEDs (5V or 12V WS2811 modules):
           .________.       .________.
           |  |  |  |       |  |  |  |
         LED LED LED|     LED LED LED|
           |  |  |  |       |  |  |  |
          .+--+--+--+.     .+--+--+--+.
 +5V -----|  WS2811  |-----|  WS2811  |
     -->--|DIN   DOUT|-->--|DIN   DOUT|
 GND -----|    0     |-----|    1     |
          '----------'     '----------'


  Configuration with light bulbs and aditional FETs (Only one FET driver shown)
  (5V or 12V WS2811 modules could be used):

 +12V-o--.
      |  |   .-----(bulb)---- GND
      |  |   |
      |  S   D
      4  -----
      K  BS250    .  .  .
      7  '---'
      |    |
      '----o
           |
          ---
           V  Diode
          --- 1N4148
           |
           |  |  |          |  |  |
          .+--+--+--+.     .+--+--+--+.
 +5V -----|  WS2811  |-----|  WS2811  |
     -->--|DIN   DOUT|-->--|DIN   DOUT|
 GND -----|    0     |-----|    1     |
          '----------'     '----------'



  Configuration with light bulbs (12V WS2811 modules with bridged output resistores):
               ._____.          ._____.          ._____.          ._____.          ._____.          ._____.
               |     |          |     |          |     |          |     |          |     |          |     |
              bulb   |         bulb   |         bulb   |         bulb   |         bulb   |         bulb   |
               |     |          |     |          |     |          |     |          |     |          |     |
            .--o--.  |       .--o--.  |       .--o--.  |       .--o--.  |       .--o--.  |       .--o--.  |
            |  |  |  |       |  |  |  |       |  |  |  |       |  |  |  |       |  |  |  |       |  |  |  |
           .+--+--+--+.     .+--+--+--+.     .+--+--+--+.     .+--+--+--+.     .+--+--+--+.     .+--+--+--+.
 +12V -----|  WS2811  |-----|  WS2811  |-----|  WS2811  |-----|  WS2811  |-----|  WS2811  |-----|  WS2811  |
      -->--|DIN   DOUT|-->--|DIN   DOUT|-->--|DIN   DOUT|-->--|DIN   DOUT|-->--|DIN   DOUT|-->--|DIN   DOUT|
 GND  -----|    0     |-----|    1     |-----|    2     |-----|    3     |-----|    4     |-----|    5     |
           '----------'     '----------'     '----------'     '----------'     '----------'     '----------'

 */
