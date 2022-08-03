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


 Sound and Keyboard                                                                        by Hardi   24.11.18
 ~~~~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with a MP3-TF-16P or a JQ6500 sound module
 together with the keyboard module "Keys_4017.h". For details see the sound example and the
 switches_80_and_more example.
 Enable / disable the "USE_MP3_TF_16P_SOUND_MODULE" line below to define the used module.

 It uses the keys which are read in with the CD4017 to test the sound functions (PushButtonAction_4017.zip).

 The first 14 keys (key 0 .. key 13) could be used to play the different sounds.
 If the JQ6500 sound module is used only the first 5 keys (Key 0..4) could be used
 to play the different sounds.
 Other keys:
   Key 16: Prev sound
   Key 17: Next sound
   Key 18: Next sound of 3
   Key 19: Play Random sound
   Key 20: Pause                 (Not with the JQ6500 sound modul)
   Key 21: Loop                    "        "           "
   Key 22: toggle the Play Mode    "        "           "
   Key 24: Dec Volume
   Key 25: Inc Volume

 In the "extras" directory of the library there are the schematics and printed circuit boards for
 this example (S3PO_Modul_WS2811.zip).
 */

#define USE_MP3_TF_16P_SOUND_MODULE // Enable this line to use the MP3-TF-16P sound module
                                    // If the line is disabled the JQ6500 sound module is used

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include <TimerOne.h>    // The TimerOne library must be installed in addition if you got the error message "..fatal error: TimerOne.h: No such file or directory"
#include <DIO2.h>        // The library for fast digital I/O functions must be installed also
                         // Installation see "FastLED" installation above

#include "MobaLedLib.h"  // Use the Moba Led Library

#define NUM_LEDS     32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#else 
#error this example does not support this plattform
#endif
#define CTR_CHANNELS_1    10                   // Number of used counter channels for keyboard 1. Up to 10 if one 4017 is used, up to 18 if two CD4017 are used, ...
#define BUTTON_INP_LIST_1 2,7,8,9,10,11,12,A1  // Comma separated list of the button input pins (Example use A0-A3: 14, 15, 16, 17)   Attention: Not A6, A7 (See blow)
#define CLK_PIN           A4                   // Pin number used for the CD4017 clock (Example 18 = A4)
#define RESET_PIN         A5                   // Pin number used for the CD4017 reset (Example 19 = A5)
                                               // The digital pins D2..D13 could be used also.
                                               // Attention the analog pins A6 & A7 of the Nano can't be used as digital input/output !
                                               // => They can't be used for to read the keys

#include "Keys_4017.h"   // Keyboard library which uses the CD4017 counter to save Arduino pins. Attention: The pins (CLK_PIN, ...) must be defined prior.


//*******************************************************************
// *** Configuration array which defines the behavior of the sound module which is addressed like a LED ***
MobaLedLib_Configuration()
  {//                    LED:                "LED" number in the stripe which is used to control the sound module
   //                     |  InCh:           Input channel. The inputs are read in below using the digitalRead() function.
   //                     |  |
#ifdef USE_MP3_TF_16P_SOUND_MODULE
  Sound_Seq1(             0, 0)           // Play sound file 1 if the button 0 is pressed.
  Sound_Seq2(             0, 1)
  Sound_Seq3(             0, 2)
  Sound_Seq4(             0, 3)
  Sound_Seq5(             0, 4)
  Sound_Seq6(             0, 5)
  Sound_Seq7(             0, 6)
  Sound_Seq8(             0, 7)
  Sound_Seq9(             0, 8)
  Sound_Seq10(            0, 9)
  Sound_Seq11(            0, 10)
  Sound_Seq12(            0, 11)
  Sound_Seq13(            0, 12)
  Sound_Seq14(            0, 13)
  // Button row 3
  Sound_Prev(             0, 16)
  Sound_Next(             0, 17)
  Sound_Next_of_N(        0, 18, 5)       // Play the next sound file if the button is pressed. The 5  defines the maximal played sound number in the range of 1..14.
  Sound_PlayRandom(       0, 19, 14)      // Play a random sound file if the button is pressed. The 14 defines the maximal played sound number in the range of 1..14.
  Sound_PausePlay(        0, 20)
  Sound_Loop(             0, 21)
  Sound_PlayMode(         0, 22)
  // Button row 4
  Sound_DecVol(           0, 24, 1)
  Sound_IncVol(           0, 25, 1)

#else // JQ6500 sound modul
  Sound_JQ6500_Seq1(      0, 0)           // Play sound file 1 if the button 0 is pressed.
  Sound_JQ6500_Seq2(      0, 1)
  Sound_JQ6500_Seq3(      0, 2)
  Sound_JQ6500_Seq4(      0, 3)
  Sound_JQ6500_Seq5(      0, 4)
  // Button row 3
  Sound_JQ6500_Prev(      0, 16)
  Sound_JQ6500_Next(      0, 17)
  Sound_JQ6500_Next_of_N( 0, 18, 3)       // Play the next sound file if the button is pressed. The 5  defines the maximal played sound number in the range of 1..5.
  Sound_JQ6500_PlayRandom(0, 19, 5)       // Play a random sound file if the button is pressed. The 14 defines the maximal played sound number in the range of 1..5.
  // Button row 4
  Sound_JQ6500_DecVol(    0, 24, 1)
  Sound_JQ6500_IncVol(    0, 25, 1)
  Andreaskreuz(           0,C2,SI_1)      // Usage example of the remaining two outputs
#endif

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

  Keys_4017_Setup(); // Initialize the keyboard scanning process
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib_Copy_to_InpStruct(Keys_Array_1, KEYS_ARRAY_BYTE_SIZE_1, 0);  // Copy the key states to the input structure

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

