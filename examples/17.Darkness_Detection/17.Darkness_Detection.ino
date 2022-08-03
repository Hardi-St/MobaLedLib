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


 Darkness Detection                                                                        by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~

 This example shows the usage of the MobaLedLib to control different lights with a darkness sensor.
 An LDR (Light Dependent Resistor) is used in this example to detect the darkness.

 If the ambient light in your model railway room is slowly dimmed down the lights on the layout
 could be controlled automatically by the LDR.

 The lights in the houses are activated automatically when it's getting dark.
 In addition, the street lights go on at the beginning of the evening.
 There is also a traffic light which is influenced by the darkness. Late in the night the traffic
 light is switched from normal operation to flashing yellow light.
 The lighting in the store is reduced at night when there are no more customers on the move.


 In the configuration below the Schedule() function is used to activate the lights at a random
 darkness value.

   Schedule(INCH0, INCH2, SI_1, 60,  110) // Random activated houses

 The Schedule() function calculates a random threshold in the range from 60 to 110 for the
 input channels INCH0 - INCH2. If the darkness value is greater then the individual threshold
 the corresponding channel is activated.
 On the other hand if the "sun" rises the channels are deactivated again with a new threshold.

 To understand this it's recommended to open the serial monitor in the Arduino IDE (Use a baud rate of 9600).
 It will show a line like this:

   \ Inp: 70 damped: 30 LDR

 The '\' sign in the front indicates that the program has detected that it's getting darker (Sunset).
 The '70' shows the actual value of the LDR. The second number shows the filtered value. The filtering
 is necessary that short fluctuations in brightness (shadows) do not trigger faulty switching operations.
 The 'LDR' at the end indicates that the algorithm has detected a connected LDR. It will show 'SW' in case
 a switch is detected.

 In the default configuration the example uses 6 RGB LEDs to represent the inputs of the houses and other
 lights so that the way of working is better understood.
 Four blue LEDs show the input of the houses and the shop. A white LED represents the street lights and
 a red LED indicate the input for the traffic light.

 When it gets darker, the first three blue LEDs are randomly activated. Then comes the fourth LED
 representing the store. When it is completely dark, the red LED indicates that the traffic light
 has been deactivated.

 If the following line is commented out, then instead of individual LEDs, the actual functions for
 controlling the houses, street lamps and the traffic light are used.
   // #define SINGLE_LEDS_EXAMPLE

 But then it becomes quite unpredictable as in real life. Added to this is the random activation of
 the individual rooms.

 Use a switch
 ~~~~~~~~~~~~
 Instead of a LDR or in addition to the LDR a switch could be used. With a 3 way switch an automatic
 mode, a night mode and a day mode is available:

                  3 pos. switch ...........
    .----[LDR]-----o            :         :     Automatic  (Switch connected to the LDR)
    |              -----o------>: Arduino :     Night      (   "   in middle position)
    o--------------o            :         :     Day        (   "    connected to ground)
    |                           ...........
   GND

 If the LDR is omitted, you can switch between day and night with a single switch.

 Without a switch, the night starts with the application of the supply voltage and the
 lights are activated one after the other.

 Attention: It takes some time (up to 2.5 minutes) to see changes. The people in the houses
            don't run from room to room and turning the lights on and off.
            Change the #defines HOUSE_MIN_T and HOUSE_MAX_T below to modify the update rates.


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
 and a WS2812 LED stripe.
 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).
 To detect the darkness an LDR and / or a switch is needed. If none is connected
 the "night" starts when the arduino is powered up.

 All examples could also be used with the other LED stripe types which are supported
 by the FastLED library (See FastLED Blink example how to adapt the "FastLED.addLeds"
 line.).
*/

#define SINGLE_LEDS_EXAMPLE  // If this line is enabled single LEDs are used to demonstrate the Schedule() function. Otherwise "real" houses, lamps, ... are used

#define FASTLED_INTERNAL  // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"      // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                          // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                          //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                          //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include <AnalogScanner.h>// Interrupt driven analog reading library. The library has to be installed manually from https://github.com/merose/AnalogScanner

#include "MobaLedLib.h"   // Use the Moba Led Library

#define SWITCH_DAMPING_FACT   1  // 1 = Slow, 100 Fast (Normal 1)
#include "Read_LDR.h"     // Darkness sensor


#define SERIAL_BAUD 9600  // Attention: The serial monitor in the Arduino IDE must use the same baudrate

#define NUM_LEDS     32   // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#else 
#error this example does not support this plattform
#endif
#define LDR_PIN      A7   // Use A7 if the MobaLedLib "LEDs Main Module" is used

AnalogScanner scanner;    // Creates an instance of the analog pin scanner.



#define INCH0        0    // Define names for the input channels to be able to change them easily.
#define INCH1        1    // In this small example this is not necessary, but it's useful in a
#define INCH2        2    // large configuration.
#define INCH3        3
#define INCH4        4
#define INCH5        5

#define HOUSE_A      0    // Define names for the LED numbers of the houses.
//      HOUSE_A      1    // In a real setup the names could be: "RailwayStation", "Town_Hall", "Pub", ...
//      HOUSE_A      2    // Each room get's an own name.
//      HOUSE_A      3    // Only the first LED numbers are used in the configuration,
//      HOUSE_A      4    // But it's a good practice to list the other rooms to because
//      HOUSE_A      5    // then the corrosponding numbers are increasing without gaps.
//      HOUSE_A      6    // This is usefull if a aditional house is inserted.
#define HOUSE_B      7    // In this case th sequence could easyly be checked / updated and
//      HOUSE_B      8    // the aditional lines could be used for documentation
//      HOUSE_B      9
//      HOUSE_B      10
#define HOUSE_C      11
#define HOUSE_C_SHOP 12  // There is a shop in the house which is controlled separately
//      HOUSE_C      13
//      HOUSE_C      14
//      HOUSE_C      15
#define GASLIGHTS_A  16  // 6 gaslights
//      GASLIGHTS_A  17
//      GASLIGHTS_A  18
//      GASLIGHTS_A  19
//      GASLIGHTS_A  20
//      GASLIGHTS_A  21
#define TRAFFIC_L_A  22
//      TRAFFIC_L_A  23
//      TRAFFIC_L_A  24
//      TRAFFIC_L_A  25
//      TRAFFIC_L_A  26
//      TRAFFIC_L_A  27

// *** Macro definitions ***
#define RGB_AmpelX_Fade_IO(LED, InNr)   /* Fading Traffic light for tests with RGB LEDs */                                                                                                                        \
           APatternT8(LED,0,InNr,18,0,255,0,PF_NO_SWITCH_OFF|PF_INVERT_INP,200 ms,2 Sec,200 ms,1 Sec,200 ms,10 Sec,200 ms,3 Sec,1,2,4,8,144,33,64,134,0,128,2,0,10,128,33,0,134,0,1,2,4,8,16,32,67,128,12,1,0,5,0,20,0,67,0,12) \
           APatternT4(LED,0,InNr,18,0,255,0,PF_NO_SWITCH_OFF,              100 ms,300 ms,100 ms,500 ms,24,48,96,192,0,0,0,0,0)


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
//*******************************************************************
MobaLedLib_Configuration()
  {
  Schedule(INCH0, INCH2, SI_1, 60,  110) // Random activated houses
  Schedule(INCH3, INCH3, SI_1, 150, 150) // Light in the shop
  Schedule(INCH4, INCH4, SI_1, 50,   50) // Gas lights are turned on early in the evening
  Schedule(INCH5, INCH5, SI_1, 200, 200) // Late in the night the traffic light is disabled (Yellow flashing)

#ifdef SINGLE_LEDS_EXAMPLE
  Const(0, C_BLUE, INCH0, 0, 50)
  Const(1, C_BLUE, INCH1, 0, 50)
  Const(2, C_BLUE, INCH2, 0, 50)
  Const(3, C_BLUE, INCH3, 0, 50)
  Const(4, C_ALL,  INCH4, 0, 50)
  Const(5, C_RED,  INCH5, 0, 50)
#else
  //    LED:                                 First LED number in the stripe
  //     |            InCh:                  Input channel. The inputs are read in below using the digitalRead() function.
  //     |            |      On_Min:         Minimal number of active rooms. At least two rooms are illuminated.
  //     |            |      |   On_Max:     Number of maximal active lights.
  //     |            |      |   |           rooms: List of room types (see documentation for possible types).
  //     |            |      |   |           |
  House(HOUSE_A,      INCH0, 2,  5,          ROOM_DARK, ROOM_BRIGHT, ROOM_WARM_W, ROOM_TV0,  NEON_LIGHT, ROOM_D_RED, ROOM_COL2) // House with 7 rooms
  House(HOUSE_B,      INCH1, 3,  3,          NEON_LIGHT,NEON_LIGHTL, NEON_LIGHTL, NEON_LIGHTD)                                  // House with 4 rooms (Office building with neon lights)
  House(HOUSE_C,      INCH2, 2,  5,          ROOM_TV0,  SKIP_ROOM,   ROOM_WARM_W, ROOM_COL1, ROOM_CHIMNEY)                      // House with 5 rooms, but the second room is allways off.

  //   LED:                                  First LED number in the stripe
  //    |             Color:                 Color/Channel of the LED
  //    |             |      InCh:           Input channel. The inputs are read in below using the digitalRead() function.
  //    |             |      |      Val0:    Value if the input is turned off
  //    |             |      |      |   Val1:Value if the input is turned on
  //    |             |      |      |    |
  Const(HOUSE_C_SHOP, C_ALL, INCH3, 255, 10) // The light in the shop is not turned off, it's just dimmed down in the evening if nobody is out on the street

  GasLights(GASLIGHTS_A,     INCH4,          GAS_LIGHT,  GAS_LIGHT,  GAS_LIGHTD,  GAS_LIGHT,  GAS_LIGHT,  GAS_LIGHT)  // RGB stripe or light bulbs with parrallel used ouputs

  RGB_AmpelX_Fade_IO(TRAFFIC_L_A,  INCH5)    // Traffic lights RGB LED stripe
#endif

  EndCfg // End of the configuration
  };


CRGB leds[NUM_LEDS];     // Define the array of leds

MobaLedLib_Create(leds); // Define the MobaLedLib instance

LED_Heartbeat_C LED_Heartbeat(LED_BUILTIN); // Use the build in LED as heartbeat

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  int scanOrder[] = {LDR_PIN};
  const int SCAN_COUNT = sizeof(scanOrder) / sizeof(scanOrder[0]);

  Init_DarknessSensor(LDR_PIN, 50, SCAN_COUNT); // Attention: The analogRead() function can't be used together with the darkness sensor !

  scanner.setScanOrder(SCAN_COUNT, scanOrder);
  scanner.setCallback(LDR_PIN, Darkness_Detection_Callback);
  scanner.beginScanning();

  Serial.begin(SERIAL_BAUD); // Debug
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously

  MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)

  LED_Heartbeat.Update(); // Update the heartbeat LED. This must be called periodically in the loop() function.

  // Debug
  static uint32_t Next_LDR_Update = 1000;
  if (millis() > Next_LDR_Update)
     {
     Next_LDR_Update = millis() + 1000;
     char ds = '-';
     switch (DayState)
       {
       case Unknown: ds ='-';  break;
       case SunRise: ds ='/';  break;
       case SunSet:  ds ='\\'; break;
       }
     char Buf[30];
     sprintf(Buf, "%c Inp:%3i damped:%3i %s", ds, Get_Act_Darkness(), Darkness, AD_Flags.SwitchMode?"SW":"LDR");
     Serial.println(Buf);
     }
}

/*
 Arduino Nano:          +-----+
           +------------| USB |------------+
           |            +-----+            |
           | [ ]D13/SCK        MISO/D12[ ] |   B4
           | [ ]3.3V           MOSI/D11[ ]~|   B3
           | [ ]V.ref     ___    SS/D10[ ]~|   B2
           | [ ]A0       / N \       D9[ ]~|   B1
    .------| [ ]A1      /  A  \      D8[ ] |   B0
    |      | [ ]A2      \  N  /      D7[ ] |   D7
    o      | [ ]A3       \_0_/       D6[ ]~|   D6   -> WS281x LED stripe pin DIN
A   |   D  | [ ]A4/SDA               D5[ ]~|   D5
    |      | [ ]A5/SCL               D4[ ] |   D4
.-o | o-.  | [ ]A6              INT1/D3[ ]~|   D3
|       |  | [ ]A7              INT0/D2[ ] |   D2
|   N   |  | [ ]5V                  GND[ ] |
|       |  | [ ]RST                 RST[ ] |   C6
'-[LDR]-o--| [ ]GND   5V MOSI GND   TX1[ ] |   D0
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
    .------| [ ]A1    -| O |-               4[A] |   .
    |      | [ ]A2     +---+           INT1/3[A]~|   .
    o      | [ ]A3                     INT0/2[ ] |   .
A   |   D  | [ ]A4      RST SCK MISO     TX>1[ ] |   .
    |      | [ ]A5      [ ] [ ] [ ]      RX<0[ ] |   D0
.-o | o-.  |            [ ] [ ] [ ]              |
|       |  |  UNO_R3    GND MOSI 5V  ____________/
|   N   |   \_______________________/
|       |
'-[LDR]-o
        |
       ---
       GND
*/
