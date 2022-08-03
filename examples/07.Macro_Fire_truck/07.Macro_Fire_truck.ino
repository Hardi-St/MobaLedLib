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


 Macros for Fire truck                                                                     by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~
 This example demonstrates the usage of macros which are used to combine the lights
 of a Fire truck to one command.
 The fire truck is equipped with 8 LEDs:
   2x Front light
   2x Blue light
   2x Rear turn indicator
   2x Rear light
 These lights use different functions which are grouped to one macro command which
 could be used easily in the configuration.

 In addition the example shows the commands Blink2(), BlueLight1() and Const().

 The "#define" command in c/c++ could be used to define macros. This macros could be
 constant definitions like "#define LED_DO_PIN 6" or pseudo-functions.
 (See: https://doc.bccnsoft.com/docs/cppreference_en/preprocessor/all.html)

 In this example several commands are grouped to one command using the pseudo-function.
 The "#define" statement is valid for one line. This could be expanded to several lines
 by adding a back slash to the end of the line.

 Command description:
 ~~~~~~~~~~~~~~~~~~~~
 At first the commands have to be described.

 Blink2():
 ~~~~~~~~~
 The Blink2() command is used to change the brightness of LED periodically. It uses two values for
 the duration and for the brightness of the LED (There is also a simplified "Blinker()" command which
 uses only one parameter for the period.). The "Blink2()" command is used to show the front light of
 the fire truck which is flashing a bright and dark white LED. In addition it's used for the turn
 indicator on the back of the vehicle.

          LED:                                      First LED number in the stripe
          |    Color:                               Color/Channel of the LED. (C1, C2, C3, C12, C23, C_ALL, C_RED, C_GREEN, C_BLUE)
          |    |   InCh:                            Input channel.
          |    |   |     Pause:                     Duration while Val0 is shown in milliseconds. Add "Sec" to use seconds instead.
          |    |   |     |      Act:                Duration while Val1 is shown in milliseconds. Add "Sec" to use seconds instead.
          |    |   |     |      |    Val0:          Value shown in the "Pause" in the range from 0 to 255.
          |    |   |     |      |    |     Val1:    Value shown in the active phase in the range from 0 to 255.
          |    |   |     |      |    |     |
   Blink2(LED, Cx, InCh, Pause, Act, Val0, Val1)

 BlueLight1() / BlueLight2():
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 The "BlueLight" command is used to show the flashing blue light of the fire truck. There are two
 versions of the command which have a small timing difference to generate asynchronous flashing lights.

              LED:                                  First LED number in the stripe
              |    Color:                           Color/Channel of the LED. (C1, C2, C3, C12, C23, C_ALL, C_RED, C_GREEN, C_BLUE)
              |    |   InCh:                        Input channel.
              |    |   |
   BlueLight1(LED, Cx, InCh)

 Const():
 ~~~~~~~~
 The "Const()" command is used in the example before. It simply writes Val1 to the LED if the input
 is active. Otherwise Val0 is send to the LED.
         LED:                                      First LED number in the stripe
         |    Color:                               Color/Channel of the LED. (C1, C2, C3, C12, C23, C_ALL, C_RED, C_GREEN, C_BLUE)
         |    |   InCh:                            Input channel.
         |    |   |     Val0:                      Value shown if the input is not active in the range from 0 to 255.
         |    |   |     |     Val1:                Value shown if the input is active in the range from 0 to 255.
         |    |   |     |     |
   Const(LED, Cx, InCh, Val0, Val1)


 Macros:
 ~~~~~~~
 There are two macros defined in this example. The first "Fire_truck" is used with WS2811 modules which drive
 the LEDs in the fire truck.
 The second macro "Fire_truckRGB" is added to demonstate the function if you don't have a fire truck with
 WS2811 modules. It uses five LEDs of a RGB LED stripe to show the function. By default this macro is used
 in the configuration.
 The macros simply list the necessary commands in a cind of table. Important is the back slash at the end
 of the line if an other line follows.
 A pseudo-function macro is started wit "#define" followed by a name which ends with "(". A space between
 the macro name an the bracket is not allowed. After the bracket a list of parameter follows. In the
 following lines this parameters could be used. The examples show that it's possible to use this parameters
 in calculations as well. This calculations are done in the preprocessor => they are performed at compile
 time and don't use CPU time and FLASH.
 => Macros are a great feature to simplify the configuration!

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
 and 2  WS2811 modules connected to the LED's of a fire truck. For tests a LED stripe could also be used.
 The DIN pin of the first WS2811 module is connected to pin D6 (LED_DO_PIN). The DOUT pin of
 the first module is connected to the DIN pin of the next WS2811 module...
 One toggle switch connected to D7 is used to turn on / off the lights of the fire truck.
 The other pin of the switch is connected to ground.


 Fire truck schematic:                                              From Arduino
 ~~~~~~~~~~~~~~~~~~~~~                                                   |
                                                      ------------       |
  rear turn indicator (Yellow): .--(LED)---(LED)--C3--+  ------  +--+5V  |
  rear light (Red):             +--(LED)---(LED)--C1--+ |WS2811| +--DIN--'
                                '---------------------+ | 12V  | +--DOut-.
                                                  C2--+  ------  +--GND  |
                                                      ------------       |
                                                                         |
                                                      ------------       |
  Blue light (Blue):            .------(LED)------C3--+  ------  +--+5V  |
                                +------(LED)------C1--+ |WS2811| +--DIN--'
                                +---------------------+ | 12V  | +--DOut
  Front light (White)           '--(LED)---(LED)--C2--+  ------  +--GND
                                                      ------------

  12V WS2811 modules are used because white LEDs require more then 2.5V => They can't be connected in series.

  Since there is one unused channel the cirquit above it could be improved by connecting the front lights to
  individual channels. In this case 5V WS2811 modules could also be used. (But our fire truck is already build up).
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


// **** Macro definition: Fire truck with WS2811 module connected to separate LED's ****
#define Fire_truck(LED,    InCh)                                                                                       \
  Blink2    (LED+1, C2,    InCh, 500 ms, 500 ms, 50,  255) /* Vorderlicht Blinken Dunkel/Hell                       */ \
  BlueLight1(LED+1, C1,    InCh)                           /* Blaulicht rechts                                      */ \
  BlueLight2(LED+1, C3,    InCh)                           /* Blaulicht Links (Andere Periode damit nicht synchron) */ \
  Blink2    (LED,   C3,    InCh, 500 ms, 500 ms,  0,  255) /* Blinker hinten                                        */ \
  Const     (LED,   C1,    InCh,                  0,   25) /* Ruecklicht                                            */


// **** Macro definition: Fire truck for tests with RGB LED stipe if no vehicle is available ****
#define Fire_truckRGB(LED, InCh)                                                                                       \
  Blink2    (LED,   C_ALL, InCh, 500 ms, 500 ms, 15,  128) /* Vorderlicht Blinken Dunkel 15/ Hell 128 (Weis)        */ \
  BlueLight1(LED+1, C3,    InCh)                           /* Blaulicht rechts                                      */ \
  BlueLight2(LED+2, C3,    InCh)                           /* Blaulicht Links (Andere Periode damit nicht synchron) */ \
  Blink2    (LED+3, C12,   InCh, 500 ms, 500 ms,  0,  128) /* Blinker hinten (Gelb)                                 */ \
  Const     (LED+4, C1,    InCh,                  0,   25) /* Ruecklicht     (Rot)                                  */


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {//          LED:      First LED number in the stripe
   //           |  InCh: Input channel. The input is read in below using the digitalRead() function.
   //           |  |
//Fire_truck   (0, 0)
  Fire_truckRGB(0, 0)
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


*/
