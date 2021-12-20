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


 Sound                                                                                     by Hardi   13.11.18
 ~~~~~

 This example demonstrates the usage of the MobaLedLib with a sound module. It uses the  extremely inexpensive
 module MP3-TF-16P (1Eu from China) which could be controlled with a WS2811 module to play different sounds.
 In this case two "LED colors" trigger the inputs of the sound module.

 The example could also be used with te JQ6500 sound module.
 Enable / disable the "USE_MP3_TF_16P_SOUND_MODULE" line below to define the used module.

 In this way sounds could be controlled with the same single wire as all LED's on the model railway.
 Several sound modules could be placed in the model landscape and play atmospheric sounds. This could
 be animals, vehicles, church bells, railway station messages, ...

 The example uses three push buttons to generate the signals for the sound module. They are connected
 to D7-D9 with the common pin connected to ground.

 Sounds could also generated automatically. The second St. Andrews Cross example will shows how this
 is implemented. (The example is not fininished yet)

 Sound files:
 ~~~~~~~~~~~~
 According to the documentation of the MP3-TF-16P module (http://www.picaxe.com/docs/spe033.pdf) the sound
 files have to be named in a special way: "\00\001.mp3". This naming convention is NOT used if the sounds
 are started with the inputs pins (IO1, IO2, ADKEY1, ADKEY2) as in this example.
 The documentation uses "SegmentN" instead of file names in the "Key ports" section. The reason is that the
 module plays the first file which has been copied to the SD-card when ADKEY1 is connected to ground.
 It plays the second file if the ADKEY1 is connected to ground by a 3K resistor...
 The name of the files and the directory is not important. The MP3-TF-16P module uses the sequence in
 the FAT directory.
 Attention: If a file is deleted the entry in the directory deleted. If an other file is copied to the
 SD-card it will be listed in this free entry. => It's played at the sequence number of the deleted file.
 To view the sequence of the files in the FAT directory the command prompt could be used (cmd.exe).
 Enter "dir F:" in the command window to view the sequence (F: has to be replaced by the used drive letter).
 Don't trust the windows explorer it always show a sorted list.

 For the first tests sound files with spoken numbers could be used.
   https://vocapp.com/german-numbers-1-20-flashcards-26526
 (Right click to the speaker symbol to download the files.)

 Attention: There is an error in the MP3-TF-16P documentation. The next segment is played with a 24K resistor
            and the previous segment with a 15K resistor. The resistores are mixed up in the documentation.


 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration lines and
 eventual the macros and adapt the first LED to avoid overlapping (First parameter in the configuration line).

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...), a WS2811 module and a
 MP3-TF-16P sound module, a SD card, a speaker and three push buttons. In addition some electronic components
 are needed to filter the WS2811 signals:
   MP3-TF-16P                            JQ6500
   ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
   2x 1.5 K 1% resistors                 1x 470 ohm 1% resistors
   2x  33 ohm  resistors                 1x  33 ohm    resistors
   1x   1 ohm  resistor                  1x   1 ohm    resistor
   2x  22 uF   electrolyte capacitors    1x  22 uF     electrolyte capacitors
   1x 470 uF   electrolyte capacitor     1x 470 uF     electrolyte capacitor
   2x 0.1 uF   capacitor                 2x 0.1 uF     capacitor
   1x L78L33   voltage regualtor         1x L78L33     voltage regualtor

 To test the software a LED stripe could be connected to pin D6. The first LED should flash briefly
 when a button is pressed.

 The sound modules are connected to the WS2811 module in the following way:

 MP3-TF-16P sound module:
 ~~~~~~~~~~~~~~~~~~~~~~~~                      L78L33
                                               _____     3.3V
                                          +5V--\___/---o-----o-[1K5]----.
       .---[1ohm]--- +5V                         |     |     |          |
       |                                         |    ===    '-[1K5]-.  |
       |             -------\_/-------           |     |0.1uF        |  |
       o---o-------- +  MP3-TF-16P   +          ---   ---            |  |
       |   |         +  -----------  +          GND   GND            |  |
      ~~~ ===        + |           | +                               |  |       ------------
470uF ###  |0.1uF    + |           | + ADKEY2---------.              |  |       B  ------  +-- +5V
       |   |         + |           | +-ADKEY1---o-----------[33 ]----o----------R |WS2811| +-- DIN   -> Arduino D6
      --- ---   SPK_1+ |  SD-Card  | +          |     |                 |       + |  5V  | +-- DOut
      GND GND   GND  + |           | +          |     o-----[33 ]-------o-------G  ------  +-- GND
                SPK_2+ |           | +          |     |                         ------------
                     -----------------         ~~~   ~~~
                                               ###   ###  2x 22uF
                                                |     |
                                               ---   ---
                                               GND   GND


 JQ6500 sound module:
 ~~~~~~~~~~~~~~~~~~~~

    +5V ---[1ohm]---.                         L78L33
                    |                         _____     3.3V
       .------------o--------------------o----\___/---o----[470]--.
       |                                 |      |     |           |
       |                                ===     |    ===          |
       |                                 |0.1uF |     |0.1uF      |
       |                                ---    ---   ---          |
       |             ----------------   GND    GND   GND          |       ------------
       |             +   JQ65000    +                             |       B  ------  +-- +5V
       |             +   .-----.    +-ADKEY1---o-----------[33 ]--o-------R |WS2811| +-- DIN   -> Arduino D6
       |        GND--+   |     |    +-GND      |                          + |  5V  | +-- DOut
       o-------------+   |     |    +          |                          G  ------  +-- GND
       |             +   '-----'    +          |                          ------------
      ~~~            +              +         ~~~
470uF ###       SPK_1+     ___      +         ### 22 uF
       |        SPK_2+    |USB|     +          |
      ---            ----------------         ---
      GND                                     GND

 The buttons are connected to D7-D9. The DIN port of the WS2811 module is connected to D6.

 In the "extras" directory of the library there is a file S3PO_Modul_WS2811.zip which contains
 the schematic and printed circuit board for this example.
*/
#define USE_MP3_TF_16P_SOUND_MODULE // Enable this line to use the MP3-TF-16P sound module
                                    // If the line is disabled the JQ6500 sound module is used

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
#define SWITCH0_PIN  7   // Pin D7 is connected to push button 0
#define SWITCH1_PIN  8   // Pin D8 is connected to push button 1
#define SWITCH2_PIN  9   // Pin D9 is connected to push button 2


//*******************************************************************
// *** Configuration array which defines the behavior of the sound module which is addressed like a LED ***
MobaLedLib_Configuration()
  {//                    LED:                "LED" number in the stripe which is used to control the sound module
   //                     |  InCh:           Input channel. The inputs are read in below using the digitalRead() function.
   //                     |  |  MaxSoundNr:  Number of the maximal played sound file
   //                     |  |  |
#ifdef USE_MP3_TF_16P_SOUND_MODULE
  Sound_Next_of_N(        0, 0, 5)        // Play the next sound file if the button is pressed. The 5  defines the maximal played sound number in the range of 1..14.
  Sound_PlayRandom(       0, 1, 14)       // Play a random sound file if the button is pressed. The 14 defines the maximal played sound number in the range of 1..14.
  Sound_Seq7(             0, 2)           // Play sound file 7 if the button is pressed. Other files could be played by using the numbers 1..14 after "Sound_Seq" (See documentation)
#else // JQ6500 sound module
  Sound_JQ6500_Next_of_N( 0, 0, 3)        // Play the next sound file if the button is pressed. The 3  defines the maximal played sound number in the range of 1..5.
  Sound_JQ6500_PlayRandom(0, 1, 5)        // Play a random sound file if the button is pressed. The 5 defines the maximal played sound number in the range of 1..5.
  Sound_JQ6500_Seq2(      0, 2)           // Play sound file 2 if the button is pressed. Other files could be played by using the numbers 1..5 after "Sound_Seq" (See documentation)
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

  pinMode(SWITCH0_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pins
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
  pinMode(SWITCH2_PIN, INPUT_PULLUP);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib.Set_Input(0, !digitalRead(SWITCH0_PIN));
  MobaLedLib.Set_Input(1, !digitalRead(SWITCH1_PIN));
  MobaLedLib.Set_Input(2, !digitalRead(SWITCH2_PIN));

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

