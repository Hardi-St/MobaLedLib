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


 Burning House                                                                             by Hardi   27.01.21
 ~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib and a complex "Push Button" action.
 A big attraction in the "Miniatur Wunderland" are the "Druck Knopf Aktionen". Around the model railway
 there are buttons which could be pressed by the visitors which start a special action on the layout.
 Here is one example how such actions could be implemented using the MobaLedLib.
 (See also: 25.Analog_Push_Button)
 When the button is pressed LEDs simulate a fire in a house. With the second press to the button smoke
 appears. And finally when the button is pressed the third time the fire truck gets active with flashing
 lights and sound. Any further press to the button generates a new random sound.

 The implementation of this action is quite simple:

    #define Burning_House(LED0, B_LED, B_LED_Cx, InNr, TmpNr, Timeout)                                                          \
            PushButton_w_LED_0_3(B_LED, B_LED_Cx, InNr, TmpNr, 0, Timeout)                                                      \
            Logic      (TmpNr+4,    TmpNr+3 AND InNr)                        // Sound is active when TmpNr+3 and the input = 1  \
            Logic      (TmpNr+2,    TmpNr+2 OR TmpNr+3)                      // TmpNr+2 is also active if TmpNr+3 is active     \
            Logic      (TmpNr+1,    TmpNr+1 OR TmpNr+2)                      // TmpNr+1 is also active if TmpNr+2 is active     \
            Fire       (LED0+1,     TmpNr+1, 5, 50)                                                                             \
            Const      (LED0+7, C1, TmpNr+2, 0,255)                          // Rauch                                           \
            Const      (LED0+7, C3, TmpNr+2, 0,63)                           // Status LED Rauc                                 \
            Fire_truck (LED0+8,     TmpNr+3)                                                                                    \
            Sound_PlayRandom(LED0+0,TmpNr+4, 5)                              // Sound zufaellig                                  \
            Sound_Seq8(      LED0+0,TmpNr+0)                                 // Sound aus (Stille.mp3)

 The PushButton_w_LED_0_3 line uses a macro which uses one input to trigger a counter and drives
 the LED in the button. The LED shows the active counter by the number of flashes. The state of the counter
 is stored in four temporary variables (TmpNr, TmpNr+1, TmpNr+2, TmpNr+3). TmpNr+1 is activated if the button
 is pressed the first time. TmpNr+2 is active when the button is pressed two times and so on. At the begin,
 if the action is not enabled, TmpNr is active.It's also active if the button is hold for 1.5 seconds or
 automatically after a long time (Timeout).

 The following logic line generates the signal for the sound generation. The sound module is also triggered
 over the same signal as the other LEDs. It uses the very inexpensive MP3 player MP3-TF-16P (1Eu from China)
 and a WS2811 chip. In the video (https://vimeo.com/311006857) the module is shown in the bottom.
 The schematic and PCB for this module are stored in the "extras" directory of the library.
 The logic function to trigger the sound is simply an "AND" combination of the counter state 3 and the push
 button. => it's triggered if the counter is 3 and the button is pressed.

 The next two logic lines are "OR" combinations of the counter results:
   Logic      (TmpNr+2,    TmpNr+2 OR TmpNr+3)
   Logic      (TmpNr+1,    TmpNr+1 OR TmpNr+2)
 TmpNr+2 is activating the smoke. It is activated if the button is pressed the second time. But it should
 also be active if the button is pressed a third time. Therefore the logic function activates TmpNr+2
 if TmpNr+2 OR TmpNr+3 is active.
 The third logic line implements the same functionallity for TmpNr+1 which activates the LEDs to simulate
 the fire. The (reverse) sequence of the lines is used to activate the LEDs also in counter state 3.

 The "Fire" macro generates drives the LED randomly in a way to simulate a fire. It is activated by the
 temporary variable TmpNr+1. The fire effect is generated by 5 RGB LEDs.

 To enable the smoke generator the next line is used. Here the "Const" function is used which is activated
 by TmpNr+2. To drive the power for the smoke generator an additional FET is used which is controlled by
 an WS2811 module. The S3PO module (schematic in the "extras" directory of the library) provides such
 power outputs. In the video the transistor is located in the house together with the smoke generator.
 The electronic is shown in the last picture of post #58 here:
   https://www.stummiforum.de/viewtopic.php?p=1924210#p1924210
 Unfortunately the transistor is hidden below the WS2811 module ;-(
 The picture shows an additional Red LED which is used to indicate the smoke activation. It is enabled
 with the second "Const" line.

 The Fire_truck macro is activated by TmpNr+3. It is described in example "07.Macro_Fire_truck.ino".

 At the end we have two "Sound" macros. The first one is used to play one of 5 random sounds if the
 button is pressed in state 3 (TmpNr+4).
 The second sound line plays an empty sound when the action is disabled by holding the button for
 1.5 seconds. That's useful because the sound is quite loud and some sounds are quite long. They
 should stop when the action is disabled.


 Button:
 ~~~~~~~
 To read in the button a special circuit is used. The corresponding PCB could be found in the "extras"
 directory of the library: "PushButtonAction_4017.zip". In the video it's shown on the right side.
 This module could read in up to 10 buttons. It could be cascaded to read in more buttons with
 additional PCBs. The modules are connected by 8 wires.

 But this additional hardware is not necessary to read in one button. The button could also be
 read in directly by the arduino.
 This method is enabled by default. To use the PushButtonAction_4017 module the compiler switch
 USE_KEYS_4017 below must be activated.


 Video:
 ~~~~~~
 This video demonstrates the example:
   https://vimeo.com/311006857


 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration lines and
 eventual the macros and adapt the first LED to avoid overlapping (First parameter in the configuration line).


 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php

 Hardware:
 ~~~~~~~~~
 The example is one of our "Push Button" actions. It is designed to work with our special hardware.
 But it could without this.
 All You need is an Arduino compatible board (Uno, Nano, Mega, ...) a WS281x LED stripe with
 at least 16 LEDs and a push button connected to pin D7 and ground.

 The DIN pin of the first LED must be connected to D6.

 Without the special hardware the LEDs are flashing confusing. Check the video https://vimeo.com/311006857
 to see how it should look like.

 The following table helps to understand the LEDs

 LED Channels:
 0:   Sound
 1:   Additional outputs on the Sound Modul (Not used)
 2-6: Fire LEDs
 7:   Smoke generator (C1=Rauch, C3=Rote Status LED)
 8,9: Fire truck
 15:  Button LED (If used without the compiler switch USE_KEYS_4017)

 The first LED normally triggers the sound module. If the example is used without the compiler
 switch USE_KEYS_4017 the LED is turned on for a short moment in different colors if a new sound
 should be played.
*/


//#define USE_KEYS_4017  // Enable this line if you want to use this example with the PushButtonAction modul
                         // like shown in the video https://vimeo.com/311006857
                         // If the line is disabled a normal button is used which is connected to D7 and ground.

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#define NUM_LEDS_1           32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN_1         6   // Pin D6 is connected to the LED stripe
#else 
#error this example does not support this plattform
#endif


#ifdef USE_KEYS_4017
   #include <TimerOne.h> // The TimerOne library must be installed in addition if you got the error message "..fatal error: TimerOne.h: No such file or directory"
   #include <DIO2.h>     // The library for fast digital I/O functions must be installed also
                         // Installation see "FastLED" installation above

   #define BUTTON_LED NUM_LEDS_1+1
   #define NUM_LEDS_2           6   // Number WS2811 modules for the second "stripe"
   #define LED_DO_PIN_2      A2  // Pin A2 is connected to the LED stripe 2
   #define CTR_CHANNELS_1    8   // Number of used counter channels for keyboard 1. Up to 10 if one CD4017 is used, up to 18 if two CD4017 are used, ...
   #define BUTTON_INP_LIST_1 A0  // Comma separated list of the button input pins (Example use A0-A3: 14, 15, 16, 17)   Attention: Not A6, A7 (See blow)
   #define CLK_PIN           A4  // Pin number used for the CD4017 clock (Example 18 = A4)
   #define RESET_PIN         A5  // Pin number used for the CD4017 reset (Example 19 = A5)
                                 // The digital pins D2..D13 could be used also.
                                 // Attention the analog pins A6 & A7 of the Nano can't be used as digital input/output
                                 // => They can't be used for to read the keys
   #include "Keys_4017.h"        // Keyboard library which uses the CD4017 counter to save Arduino pins. Attention: The pins (CLK_PIN, ...) must be defined prior.

   #define NUM_LEDS         NUM_LEDS_1+NUM_LEDS_2 // Necessary for the checking of the LED count in the MobaLedLib

#else // USE_KEYS_4017 not defines
   #define SWITCH0_PIN      7    // Pin D7 is connected to push button 0
   #define NUM_LEDS         NUM_LEDS_1
   #define BUTTON_LED       15
#endif


// **** Macro definition: Fire truck with WS2811 module connected to separate LED's ****
#define Fire_truck(LED,    InCh)                                                                                       \
  Blink2    (LED+1, C2,    InCh, 500 ms, 500 ms, 50,  255) /* Vorderlicht Blinken Dunkel/Hell                       */ \
  BlueLight1(LED+1, C1,    InCh)                           /* Blaulicht rechts                                      */ \
  BlueLight2(LED+1, C3,    InCh)                           /* Blaulicht Links (Andere Periode damit nicht synchron) */ \
  Blink2    (LED,   C3,    InCh, 500 ms, 500 ms,  0,  255) /* Blinker hinten                                        */ \
  Const     (LED,   C1,    InCh,                  0,   25) /* Ruecklicht                                            */

// 1. Tastendruck: Feuer LEDs
// 2. Tastendruck: Rauch
// 3. Tastendruck: Feuerwehr Sirene + Licht
// 4. Tastendruck: Naechster Sound


#define StatusLEDs 20

// Dateien auf der SD-Karte
// 1:  Martinshorn 1.wav
// 2:  Sirene faehrt vorbei.mp3
// 3:  salamisound-3902659-polizei-martinshorn-sirene-3.mp3
// 4:  salamisound-1660551-brennendes-haus.mp3
// 5:  salamisound-7047521-polizeisirene-martinshorn-6.mp3
// 6:  006.mp3
// 7:  007.mp3
// 8:  Stille.mp3

#define Burning_House(LED0, B_LED, B_LED_Cx, InNr, TmpNr, Timeout)                                                     \
            PushButton_w_LED_0_3(B_LED, B_LED_Cx, InNr, TmpNr, 0, 0, 1, 0, Timeout)                                    \
            Logic      (TmpNr+4,    TmpNr+3 AND InNr)        /* Sound is active when TmpNr+3 and the input = 1 */      \
            Logic      (TmpNr+2,    TmpNr+2 OR TmpNr+3)      /* TmpNr+2 is also active if TmpNr+3 is active */         \
            Logic      (TmpNr+1,    TmpNr+1 OR TmpNr+2)      /* TmpNr+1 is also active if TmpNr+2 is active */         \
            Fire       (LED0+1,     TmpNr+1, 5, 50)                                                                    \
            Const      (LED0+7, C1, TmpNr+2, 0,255) /* Rauch */                                                        \
            Const      (LED0+7, C3, TmpNr+2, 0,63)  /* Status LED Rauch */                                             \
            Fire_truck (LED0+8,     TmpNr+3)                                                                           \
          /*Sound_Next_of_N(LED0+0, TmpNr+4, 5)*/   /* Sound 1-5 nacheinander */                                       \
            Sound_PlayRandom(LED0+0,TmpNr+4, 5)     /* Sound zufaellig */                                              \
            Sound_Seq8(      LED0+0,TmpNr+0)        /* Sound aus (Stille.mp3) */
#if 0
            Const      (StatusLEDs+0,C_ALL,   TmpNr+0, 0, 10)                 /* Debug: */                             \
            Const      (StatusLEDs+1,C_ALL,   TmpNr+1, 0, 10)                 /* Debug: */                             \
            Const      (StatusLEDs+2,C_ALL,   TmpNr+2, 0, 10)                 /* Debug: */                             \
            Const      (StatusLEDs+3,C_ALL,   TmpNr+3, 0, 10)                 /* Debug: */                             \
            Const      (StatusLEDs+4,C_ALL,   TmpNr+4, 0, 10)                 /* Debug: */
#endif


//*******************************************************************
// *** Configuration array which defines the behavior of the sound module which is addressed like a LED ***
MobaLedLib_Configuration()
  {
  //               LED0, B_LED,        B_LED_Cx, InNr, TmpNr, Timeout)
  Burning_House(   0,    BUTTON_LED, C1,       3,    200,    2 Min)
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
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN_1>(leds,            NUM_LEDS_1);

#ifdef USE_KEYS_4017
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN_2>(leds+NUM_LEDS_1, NUM_LEDS_2);
  Keys_4017_Setup(); // Initialize the keyboard scanning process
#else
  pinMode(SWITCH0_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin
#endif
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
#ifdef USE_KEYS_4017
  MobaLedLib_Copy_to_InpStruct(Keys_Array_1, KEYS_ARRAY_BYTE_SIZE_1, 0);  // Copy the key states to the input structure
#else
  MobaLedLib.Set_Input(3, !digitalRead(SWITCH0_PIN));
#endif

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
        C0 | [ ]A0       / N \       D9[ ]~|   B1   -> push button 2 --.
        C1 | [ ]A1      /  A  \      D8[ ] |   B0   -> push button 1 --o-- GND
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   -> push button 0 --'
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
           | [ ]5v     | A |                9[ ]~|   .   -> push button 2 (B)  --.
           | [ ]GND   -| R |-               8[B] |   B0  -> push button 1 (G)  --o-- GND
           | [ ]GND   -| D |-                    |                               |
           | [ ]Vin   -| U |-               7[A] |   D7  -> push button 0 (R)  --'
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

