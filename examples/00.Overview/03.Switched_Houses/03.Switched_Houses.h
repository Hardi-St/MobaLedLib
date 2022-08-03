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


 Switched houses                                                                           by Hardi   02.10.18
 ~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with three animated houses
 which could be turned on and off with external switches.
 The houses have different rooms which are illuminated randomly to simulate a house
 where people live. There are different light types used:
 There are rooms with
 - dark light
 - neon light
 - colored light
 - running TV
 - chimney
 - user defined color
 - ...

 Attention: It takes some time (up to 2.5 minutes) to see changes. The people in the houses
            don't run from room to room and turnig the lights on and off.
            Change the #defines HOUSE_MIN_T and HOUSE_MAX_T below to modify the update rates.

 When one house is switched on one room is activates immediately. The other rooms are
 turned on / off randomly after a while.
 The switches are connected to ground because then the internal pull up resistor of the
 cpu could be used (No external resistor needed). => The house is turned on if the
 switch is open. This is also useful because the example could be used without switches
 (Houses are always on).
 In the following examples switches are replaced by automatic functions (darkness detection)

 The numbers in the "House()" lines below define how many rooms are "used". The first number
 (On_Min) controls the minimal number of illuminated rooms. If the number of illuminated
 rooms is below this value additional rooms are turned on.
 The second number (On_Max) defines how many LEDs are turned on maximal. The average
 number of active lights will be some where in the middle: (On_Min + On_Max) / 2

 If On_min = On_Max the lights are turned on randomly until the given number is reached.
 The lights in the random activated rooms stay on until the input is turned off.
 Then active lights are turned of step by step.
 This could be used in an office building where lights are on during the office times.
 The are not turned off until the employees go home.

 The example contains in addition the Set_ColTab() command. It is used to define the color
 of the LEDs. Any kind of color could be used in the houses with this command.
 THe colors could be redefined before each House() line. For this example a room with a
 dark blue light and one with a dark green light is used (This is colors are used for
 demonstration only.).

 The second room in HOUSE_C is skipped. This is useful for lights which should be
 controlled in a different way. If there is a shop in the house for instance the lights
 are always on to present the goods in the best light. Late in the night, when nobody is
 walking in the streets they are dimmed down. The are not tuned on and off randomly.

 The number of rooms could be changed by adding or removing "ROOM_.." constants to the
 "House()" line. The number of houses could by increased by duplicating the corresponding lines.

 The example also shows the usage of the "#define" command. This can give names for the input
 and LED numbers. That facilitates later changes.

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and a WS2812 LED stripe.
 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).
 Four toggle switches are used to turn on / off the houses. They are connected to D7-D10 with
 the common pin connected to ground.

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
#define SWITCH0_PIN  7   // Pin D7 is connected to switch 0
#define SWITCH1_PIN  8   // Pin D8 is connected to switch 1
#define SWITCH2_PIN  9   // Pin D9 is connected to switch 2
#define SWITCH3_PIN  10  // Pin D10 is connected to switch 3

#define INCH_HOUSE_A 0   // Define names for the input channels to be able to change them easily.
#define INCH_HOUSE_B 1   // In this small example this is not necessary, but it's useful in a
#define INCH_HOUSE_C 2   // large configuration.
#define INCH_SHOP_C  3

#define HOUSE_A      0   // Define names for the LED numbers of the houses.
//      HOUSE_A      1   // In a real setup the names could be: "RailwayStation", "Town_Hall", "Pub", ...
//      HOUSE_A      2   // Each room get's an own name.
//      HOUSE_A      3   // Only the first LED numbers are used in the configuration,
//      HOUSE_A      4   // But it's a good practice to list the other rooms to because
//      HOUSE_A      5   // then the corrosponding numbers are increasing without gaps.
//      HOUSE_A      6   // This is usefull if a aditional house is inserted.
#define HOUSE_B      7   // In this case th sequence could easyly be checked / updated and
//      HOUSE_B      8   // the aditional lines could be used for documentation
//      HOUSE_B      9
//      HOUSE_B      10
#define HOUSE_C      11
#define HOUSE_C_SHOP 12  // There is a shop in the house which is controlled separately
//      HOUSE_C      13
//      HOUSE_C      14
//      HOUSE_C      15




//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  //         Red  Green Blue
  Set_ColTab(  1,   0,   0,    // 0  ROOM_COL0          Dark red for demonstration
               0,   1,   0,    // 1  ROOM_COL1           "   green  "
               0,   0,   1,    // 2  ROOM_COL2           "   blue   "
             100,   0,   0,    // 3  ROOM_COL345        red for demonstration      randomly color 3, 4 or 5 is used
               0, 100,   0,    // 4  ROOM_COL345        green  "
               0,   0, 100,    // 5  ROOM_COL345        blue   "
              50,  50,  50,    // 6  Gas light          For individually addressed gas light LEDs, the individual brightness value is used (GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3)
             255, 255, 255,    // 7  Gas light          If 3 channels are used together, the first value determines the brightness that is important so that all outputs are charged equally (GAS_LIGHT and GAS_LIGHT)
              20,  20,  27,    // 8  Neon light
              70,  70,  80,    // 9  Neon light
             245, 245, 255,    // 10 Neon light
              50,  50,  20,    // 11 TV0 and chimney color A randomly color A or B is used
              70,  70,  30,    // 12 TV0 and chimney color B
              50,  50,   8,    // 13 TV1 and chimney color A
              50,  50,   8,    // 14 TV2 and chimney color B
             255, 255, 255,    // 15 Single LED Room:      Fuer einzeln adressierte LEDs wird der individuelle Helligkeitswert verwendet (SINGLE_LED1,  SINGLE_LED2,  SINGLE_LED3)  // 06.09.19:
              50,  50,  50)    // 16 Single dark LED Room: Fuer einzeln adressierte LEDs wird der individuelle Helligkeitswert verwendet (SINGLE_LED1D, SINGLE_LED2D, SINGLE_LED3D)

  //    LED:                                       First LED number in the stripe
  //     |            InCh:                        Input channel. The inputs are read in below using the digitalRead() function.
  //     |            |             On_Min:        Minimal number of active rooms. At least two rooms are illuminated.
  //     |            |             |   On_Max:    Number of maximal active lights.
  //     |            |             |   |          rooms: List of room types (see documentation for possible types).
  //     |            |             |   |          |
  House(HOUSE_A,      INCH_HOUSE_A, 2,  5,         ROOM_DARK, ROOM_BRIGHT, ROOM_WARM_W, ROOM_TV0, ROOM_COL345, ROOM_D_RED, ROOM_COL2) // House with 7 rooms
  House(HOUSE_B,      INCH_HOUSE_B, 3,  3,         NEON_LIGHT,NEON_LIGHTL, NEON_LIGHTL, NEON_LIGHT)                                   // House with 4 rooms (Office building with neon lights)
  House(HOUSE_C,      INCH_HOUSE_C, 2,  5,         ROOM_TV0,  SKIP_ROOM,   ROOM_WARM_W, ROOM_COL1, ROOM_CHIMNEY)                      // House with 5 rooms, but the second room is allways off.

  //   LED:                                        First LED number in the stripe
  //    |             Color:                       Color/Channel of the LED
  //    |             |      InCh:                 Input channel. The inputs are read in below using the digitalRead() function.
  //    |             |      |            Val0:    Value if the input is turned off
  //    |             |      |            |   Val1:Value if the input is turned on
  //    |             |      |            |   |
  Const(HOUSE_C_SHOP, C_ALL, INCH_SHOP_C, 10, 255) // The light in the shop is not turned off, it's just dimmed down in the evening if nobody is out on the street
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

  pinMode(SWITCH0_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pins
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
  pinMode(SWITCH3_PIN, INPUT_PULLUP);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
  MobaLedLib.Set_Input(INCH_HOUSE_A, digitalRead(SWITCH0_PIN));
  MobaLedLib.Set_Input(INCH_HOUSE_B, digitalRead(SWITCH1_PIN));
  MobaLedLib.Set_Input(INCH_HOUSE_C, digitalRead(SWITCH2_PIN));
  MobaLedLib.Set_Input(INCH_SHOP_C,  digitalRead(SWITCH3_PIN));

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
        C0 | [ ]A0       / N \       D9[ ]~|   B1   -> toggle switch 2  --.
        C1 | [ ]A1      /  A  \      D8[ ] |   B0   -> toggle switch 1  --o-- GND
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   -> toggle switch 0  --'
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
           | [ ]5v     | A |                9[ ]~|   .   -> toggle switch 2  --.
           | [ ]GND   -| R |-               8[B] |   B0  -> toggle switch 1  --o-- GND
           | [ ]GND   -| D |-                    |                             |
           | [ ]Vin   -| U |-               7[A] |   D7  -> toggle switch 0  --'
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

