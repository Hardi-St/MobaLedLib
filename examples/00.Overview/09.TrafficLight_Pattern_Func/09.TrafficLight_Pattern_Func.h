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


 Traffic Light and Pattern Function                                                        by Hardi   12.10.18:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes the pattern function using a traffic light control.

 The pattern function is a very powerful tool which can be used to create arbitrary sequences.

 On a model railway there are several sequences which could be implemented with the Pattern
 function. This could be traffic lights, construction lightning, hazard lights,
 fairground running lights, push button actions or many others.

 The Pattern function describes the actions with a table. This table defines when a certain output
 should be enabled.
 We use the traffic light at a crossing to explain this. The following table shows
 the different states:

   Description    States
   ~~~~~~~~~~~    ~~~~~~~~~~~~~~~
                  1 2 3 4 5 6 7 8
   Red    1       x x     x x x x
   Yellow 1         x   x
   Green  1           x

   Red    2       x x x x x x   x
   Yellow 2                 x
   Green  2                   x

 1: At the beginning in state 1 both traffic lights show Red.
 2: Then the Yellow light on the first traffic light is turned on.
 3: The vehicles on lane one can drive (Green is shown).
 4: The Yellow light indicate that the vehicles have to stop soon.
 5: ...

 The times for the different states are defined in a second table:

   State:  1      2      3      4      5      6      7      8
   Time:   2 Sec  1 Sec  10 Sec 3 Sec  2 Sec  1 Sec  10 Sec 3 Sec

 These two tables define the parameters of the Pattern function. In addition the first LED
 number, the start channel within the first LED and some other parameters are necessary.
 Especially because the first table is bit coded it's to complicated to enter all this
 parameters manually. This is done with the Excel file "Pattern_Configurator.xlsm".
 Here the values are entered and a result the Pattern function is generated.
 See sheet "AmpelX". The Yellow and Green lines are swapped in the Excel sheet because
 the second LED of a RGB LED is Green. Unfortunately the third LED of such a RGB LED is
 Blue => The example can't be visualized with correct colors using a RGB stripe.
 There are three possible result lines:
    1. PatternT4(0,0,SI_1,6,0,255,0,PF_NO_SWITCH_OFF,2 Sec,1 Sec,10 Sec,3 Sec,73,163,48,73,26,133)
    2. #define RGB_AmpelX(LED,InNr)               PatternT4(LED,0,InNr,6,0,255,0,PF_NO_SWITCH_OFF,2 Sec,1 Sec,10 Sec,3 Sec,73,163,48,73,26,133)
    3. #define RGB_AmpelX_StCh(LED,StCh,InNr)     PatternT4(LED,StCh+0,InNr,6,0,255,0,PF_NO_SWITCH_OFF,2 Sec,1 Sec,10 Sec,3 Sec,73,163,48,73,26,133)

 In this example the second line is used. This line defines a the macro "RGB_AmpelX"
 which could be used easily in the configuration. The macro expects the two parameters
 LED and InNr.
   LED:  First used RGB LED
   InNr: Input Number which enables the function

 Other lines generated by th Excel sheet:
 The first line could be used directly in the configuration array, and the third line
 has got the additional parameter StCh: Start channel.

 Optimization of the time table:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 The values in the time table are repeated after state 5. The memory usage of the function
 could be optimized if this values are not repeated in the configuration.
 You may have noticed that the duplicate times in the excel sheet are grayed out.
 This is activated by using a formula instead of a number in excel. The time for state 5
 is entered as "=E21". That's the reason why there are only 4 times in the macro definition.

 Night Mode:
 ~~~~~~~~~~~
 At night the traffic light is not active. In this case the Yellow lights should flash.
 The traffic light can be turned on and off by a toggle switch in this example.
 The switch is connected to ground because then the internal pull up resistor of the
 cpu could be used (No external resistor needed).
 => The traffic lights are turned on if the switch is open. This is also useful because
 the example could be used without switches.

 The flashing Yellow light is done by a second table.
 (See sheet "AmpelX_Off")

 In this sheet the Mode "PF_INVERT_INP" is used to invert the input signal.
 => This pattern is used if the input is turned off.
 The second Mode switch "PF_NO_SWITCH_OFF" defines that the outputs are not switched off
 if the function is disabled. This is important if two or more lines control the same LEDs.
 If it's not used the LEDs flicker if the input is changed.
 The following macro is generated by the Excel sheet:
   #define RGB_AmpelX(LED,InNr)    PatternT1(LED,128,InNr,6,0,255,0,PF_INVERT_INP | PF_NO_SWITCH_OFF,500 ms,36,0)
 Both macros are combined to the new RGB_AmpelX_IO (see below).

 Test with RGB LED Stripe:
 ~~~~~~~~~~~~~~~~~~~~~~~~~
 To be able to test the example without a special traffic light a second macro is generated
 in the same way which could be used with a standard RGB LED stripe. By default this example
 is enabled in the configuration (Change the comments to test the first example.).

 Smooth fade over:
 ~~~~~~~~~~~~~~~~~
 The pattern function could be used in a second mode where the lights are switched on and
 off slowly to simulate light bulbs. This is a really nice feature.
 To use the fading the switch "Analoges Ueberblenden" in the excel sheet has to be set to 1.
 If the switch is activated the intensity of the LED is slowly increased or decreased in the
 given time of one state. Therefor additional short states have to be added to the example.
 The corresponding tables could be found in the sheet "RGB_AmpelX_Fade" and "RGB_AmpelX_Fade_Off".

 Stop point for Faller model cars:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 The traffic light example could be expanded to stop the model cars by adding additional lines in the
 excel sheet which activate the stop magnet for the cars in the corresponding lane.

 The parameter Mode could be used with following modes:
   PM_NORMAL               Normal mode
   PM_SEQUENZ_W_RESTART    Rising edge-triggered unique sequence. A new edge starts with state 0.
   PM_SEQUENZ_W_ABORT      Rising edge-triggered unique sequence. Abort the sequence if new edge
                           is detected during run time.
   PM_SEQUENZ_NO_RESTART   Rising edge-triggered unique sequence. No restart if new edge is detected
   PM_PINGPONG             Change the direction at the end: 118 bytes
   PM_HSV                  Use HSV values instead of RGB for the channels
 and flags:
   PF_NO_SWITCH_OFF        Don't switch of the LEDs if the input is turned off. Useful if several
                           effects use the same LEDs alternated by the input switch.
   PF_EASEINOUT            Easing function (Uebergangsfunktion) is used because changes near 0 and
                           255 are noticed different than in the middle.
   PF_SLOW                 Slow timer (divided by 16) to be able to use longer durations.
   PF_INVERT_INP           Invert the input switch => Effect is active if the input is 0.

 Other examples:
 ~~~~~~~~~~~~~~~
 The Pattern function is really powerful. It could be used for a lot of different effects.
 It's internally used for the functions:
   Const, Blinker, BlinkerHD, Blink2, Button, BlueLight, Leuchtfeuer, Andreaskreuz, ...
 Check the other examples which end with "_Pattern_Func".

 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and 2/4 WS2811 modules. For tests a LED stripe could also be used.
 The DIN pin of the first WS2811 module is connected to pin D6 (LED_DO_PIN).
 One toggle switch connected to D7 is used to turn on / off the traffic light.
 The other pin of the switch is connected to ground.

 For a crossing of two streets, like shown below, four traffic lights are needed. Two of them show
 the same pattern (TL1 = TL4 and TL2 = TL3). To drive the LEDs of this equal traffic light pairs
 the following circuit could be used. Here two WS2811 modules use the same DIN signal. Therefore
 both show the same pattern. The DOUT signal of the right modules is not used.
 This method simplifies the configuration and improves the wiring of the LEDs because without
 this trick two LEDs have to be connected in series or the configuration has to be doubled.

  Crossing:
       T|     |                                       from Arduino pin D6
       L|     |                 TL1                         |                             TL4
       1|     | TL2                       ------------      |           ------------
  ------       ------       .--(YEL)---C3-+  ------  +-+5V  |      GND--+  ------  +-C3---(GRE)--.
                            o--(RED)---C1-+ |WS2811| +-DIN--o--.   DOUT-+ |WS2811| +-------------o
                            o-------------+ |  5V  | +-DOut-.  '---DIN--+ |  5V  | +-C1---(RED)--o
  ------       ------       '--(GRE)---C2-+  ------  +-GND  |      +5V--+  ------  +-C3---(YEL)--'
     TL3|     |T                          ------------      |           ------------
        |     |L                                            |
        |     |4                TL2                         |                             TL3
                                          ------------      |           ------------
                            .--(YEL)---C3-+  ------  +-+5V  |      GND--+  ------  +-C3---(GRE)--.
                            o--(RED)---C1-+ |WS2811| +-DIN--o--.   DOUT-+ |WS2811| +-------------o
                            o-------------+ |  5V  | +-DOut-.  '---DIN--+ |  5V  | +-C1---(RED)--o
                            '--(GRE)---C2-+  ------  +-GND  |      +5V--+  ------  +-C3---(YEL)--'
                                          ------------      |           ------------
                                                            |
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


// *** Macro definitions ***
#define AmpelX_IO(LED, InNr)   /* Traffic light for tests with a model using WS2811 modules to drive the LEDs */                      \
           PatternT4(LED,0,  InNr,6,0,255,0,PF_NO_SWITCH_OFF,2 Sec,1 Sec,10 Sec,3 Sec,73,163,48,73,26,133)                            \
           PatternT1(LED,128,InNr,6,0,255,0,PF_INVERT_INP | PF_NO_SWITCH_OFF,500 ms,36,0)


#define RGB_AmpelX_IO(LED, InNr)   /* Traffic light for tests with RGB LEDs */                                                        \
           PatternT4(LED,0,  InNr,18,0,255,0,PF_NO_SWITCH_OFF,2 Sec,1 Sec,10 Sec,3 Sec,1,2,100,8,0,40,0,134,0,1,2,4,200,16,0,80,0,12) \
           PatternT1(LED,128,InNr,18,0,255,0,PF_INVERT_INP | PF_NO_SWITCH_OFF,500 ms,24,48,0,0,0)

#define RGB_AmpelX_Fade_IO(LED, InNr)   /* Fading Traffic light for tests with RGB LEDs */                                                                                                                        \
           APatternT8(LED,0,InNr,18,0,255,0,PF_NO_SWITCH_OFF,200 ms,2 Sec,200 ms,1 Sec,200 ms,10 Sec,200 ms,3 Sec,1,2,4,8,144,33,64,134,0,128,2,0,10,128,33,0,134,0,1,2,4,8,16,32,67,128,12,1,0,5,0,20,0,67,0,12) \
           APatternT4(LED,0,InNr,18,0,255,0,PF_INVERT_INP | PF_NO_SWITCH_OFF,100 ms,300 ms,100 ms,500 ms,24,48,96,192,0,0,0,0,0)


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {//                LED:       First LED number in the stripe
   //                 |   InCh: Input channel. The input is read in below using the digitalRead() function.
   //                 |   |
// AmpelX_IO(         0,  0)  // Traffic light at at real model. The LEDs are driven by WS2811 modules
   RGB_AmpelX_IO(     0,  0)  // Demonstartion with a RGB LED stripe
// RGB_AmpelX_Fade_IO(0,  0)  // Fading demonstartion with a RGB LED stripe
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

