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


 DDC and Sound                                                                             by Hardi   19.01.21
 ~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with a MP3-TF-16P or a JQ6500 sound
 module which is controlled by DDC commands.
 Enable / disable the "USE_MP3_TF_16P_SOUND_MODULE" line below to define the used module.

 To use this example the program "23_A.DCC_Rail_Decoder_Transmitter.ino" must be running on an
 other Arduino which is connected to this Arduino by two signal wires (and GND).

 See the documentation in the program "23_A.DCC_Rail_Decoder_Transmitter.ino" for more details.

 The DCC buttons and adresses are described in the configuration below.

 In the "extras" directory of the library there are the schematics this example.

 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php


 */

#define USE_MP3_TF_16P_SOUND_MODULE   // Enable this line to use the MP3-TF-16P sound module
                                      // If the line is disabled the JQ6500 sound module is used
                                      // Attention there are different WS2811 modules ´with different
                                      // output pins (Blue and Green swapped) and different characteristics


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

#define SEND_DISABLE_PIN      A1

// Define which accessorie CAN messages should be used.
#define DCC_FIRST_LOC_ID      1                      // First local ID which should be copied to the InpStructArray[] of the MobaLedLib
#define DCC_FIRST_TOGGLE_ID   (11+DCC_FIRST_LOC_ID)  // DCC adresses greater equal this number are used to toggle an entry in the InpStructArray[]
                                                     // The DCC adresses smaller than this number are treated as momentarry events.
                                                     // In this example all channels are momentarry buttons.
                                                     // (DCC_FIRST_TOGGLE_ID > DCC_LAST_LOC_ID)
#define DCC_LAST_LOC_ID       (10+DCC_FIRST_LOC_ID)  // Last local ID which should be copied to the InpStructArray[] of the MobaLedLib

#define DCC_INPSTRUCT_START   0   // Start number in the InpStructArray[]

#define SERIAL_BAUD      115200   // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program


//*******************************************************************
// *** Configuration array which defines the behavior of the sound module which is addressed like a LED ***
MobaLedLib_Configuration()
  {//                    LED:                "LED" number in the stripe which is used to control the sound module
   //                     |  InCh:           Input channel. The inputs are read in below using the digitalRead() function.
   //                     |  |
#ifdef USE_MP3_TF_16P_SOUND_MODULE
  Sound_Seq1(             0, 0)      // Play sound file 1 if the red button of DCC address 1 is pressed.
  Sound_Seq2(             0, 1)      //  "          "   2        green   "      "          1      "
  Sound_Seq3(             0, 2)      //  "          "   3        red     "      "          2      "
  Sound_Seq4(             0, 3)      //  "          "   4        green   "      "          2      "
  Sound_Seq5(             0, 4)      //  "          "   5        red     "      "          3      "
  Sound_Seq6(             0, 5)      //  "          "   6        green   "      "          3      "
  Sound_Seq7(             0, 6)      //  "          "   7        red     "      "          4      "
  Sound_Seq8(             0, 7)      //  "          "   8        green   "      "          4      "
  Sound_Seq9(             0, 8)      //  "          "   9        red     "      "          5      "
  Sound_Seq10(            0, 9)      //  "          "   10       green   "      "          5      "
  Sound_Seq11(            0, 10)     //  "          "   11       red     "      "          6      "
  Sound_Seq12(            0, 11)     //  "          "   12       green   "      "          6      "
  Sound_Seq13(            0, 12)     //  "          "   13       red     "      "          7      "
  Sound_Seq14(            0, 13)     //  "          "   14       green   "      "          7      "
  Sound_Prev(             0, 14)     // Play the previous sound file if the  red  button, addr 8 is pressed.
  Sound_Next(             0, 15)     // Play the next sound file if the     green button, addr 8 is pressed.
  Sound_Next_of_N(        0, 16, 5)  // Play the next sound file on N if the red  button, addr 9 is pressed. The 5  defines the maximal played sound number in the range of 1..14.
  Sound_PlayRandom(       0, 17, 14) // Play a random sound file if the     green button, addr 9 is pressed. The 14 defines the maximal played sound number in the range of 1..14.
  Sound_DecVol(           0, 18, 1)  // Decremet  the volume if the          red  button, addr 10 is pressed.
  Sound_IncVol(           0, 19, 1)  // Increment the volume if the         green button, addr 10 is pressed.
  Sound_PausePlay(        0, 20)
  Sound_Loop(             0, 21)
  Sound_PlayMode(         0, 22)

#else // JQ6500 sound module
  // Unfortunately there are two different WS2811 modules
  // Rename .. _JQ6500_ with _JQ6500_BG_ if the module with swapped Green/Blue LEDs is used
  //
  Sound_JQ6500_Seq1(      0, 0)      // Play sound file 1 if the red button of DCC address 1 is pressed.
  Sound_JQ6500_Seq2(      0, 1)      //  "          "   2        green   "      "          1      "
  Sound_JQ6500_Seq3(      0, 2)      //  "          "   3        red     "      "          2      "
  Sound_JQ6500_Seq4(      0, 3)      //  "          "   4        green   "      "          2      "
  Sound_JQ6500_Seq5(      0, 4)      //  "          "   5        red     "      "          3      "
  Sound_JQ6500_Prev(      0, 6)      // Play the previous sound file the     red  button, addr 4 is pressed.
  Sound_JQ6500_Next(      0, 7)      // Play the next sound file if         green button, addr 4 is pressed.
  Sound_JQ6500_Next_of_N( 0, 8, 3)   // Play the next sound file of N if the red  button, addr 5 is pressed. The 5  defines the maximal played sound number in the range of 1..5.
  Sound_JQ6500_PlayRandom(0, 9, 5)   // Play a random sound file if the     green button, addr 5 is pressed. The 14 defines the maximal played sound number in the range of 1..5.
  Sound_JQ6500_DecVol(    0, 10, 1)  // Decremet  the volume if the          red  button, addr 6 is pressed.
  Sound_JQ6500_IncVol(    0, 11, 1)  // Increment the volume if the         green button, addr 6 is pressed.

  Andreaskreuz(           0,C2,SI_1) // Usage example of the remaining two outputs
#endif
  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];     // Define the array of leds

MobaLedLib_Create(leds); // Define the MobaLedLib instance

#include "Serial_Inputs.h" // Use the serial monitor to test LED colors and brightness values

LED_Heartbeat_C LED_Heartbeat(LED_BUILTIN); // Use the build in LED as heartbeat

char Buffer[12] = "";      // Store the received messages from the DCC-Arduino
/*
 Problem:
 Bei der Lenz Zentrale LZV100 von Rolf wird keine Message beim
 loslassen des Tasters geschickt ;-(
 Das fuehrt dazu, dass die mit den Tasten verknuepften Ausgaenge nicht mehr aus gehen.
 Als Abhilfe wird die letzte Taste und der Zeitpunkt zu dem sie empfangen wurde
 gespeichert. Nach 400ms wird automatisch das Taste losgelassen Ereignis generiert.
 Wenn eine andere Taste empfangen wird wird die alte Taste ebenfalls "losgelassen".
*/
#define GEN_BUTTON_RELEASE
#ifdef GEN_BUTTON_RELEASE
  uint8_t  LastChannel;
  uint32_t LastTime = 0;
#endif

//----------------
void Proc_Buffer()
//----------------
{
  uint16_t Addr, Direction, OutputPower, State;
  //Serial.println(Buffer); // Debug
  switch (*Buffer)
    {
    case '@': if (sscanf(Buffer+1, "%i %x %x", &Addr, &Direction, &OutputPower) == 3)
                 {
                 //Addr--; // Internally the address starts with 0
                 if (Addr >= DCC_FIRST_LOC_ID)
                    {
                    if (Addr < DCC_FIRST_TOGGLE_ID)
                         {
                         uint8_t Channel = ((Addr - DCC_FIRST_LOC_ID)*2) + DCC_INPSTRUCT_START + Direction;
                         //char s[30];                                      // Debug
                         //sprintf(s, "InpNr %i %i", Channel, OutputPower); // Debug  23.05.19:  Added: OutputPower
                         //Serial.println(s);                               // Debug
                         MobaLedLib.Set_Input(Channel, OutputPower);

                         #ifdef GEN_BUTTON_RELEASE                                                            // 23.05.19:
                           if (OutputPower)
                              {
                              if (LastTime && LastChannel != Channel) MobaLedLib.Set_Input(LastChannel, 0); // Send release
                              LastTime = millis();
                              LastChannel = Channel;
                              }
                         #endif
                         }
                    else if (Addr <= DCC_LAST_LOC_ID) // Addr >= DCC_FIRST_TOGGLE_ID
                            {                                        // ~~~~~~~~~~~ 18 ~~~~~~~~~~~~~~~~~~~~~~~~~~
                            uint8_t dst = Addr - DCC_FIRST_TOGGLE_ID + (DCC_FIRST_TOGGLE_ID - DCC_FIRST_LOC_ID)*2 + DCC_INPSTRUCT_START;
                            //char s[30];                                                                       // Debug
                            //sprintf(s, "Addr %i Dir %i Pow %i dst %i", Addr, Direction, OutputPower, dst);    // Debug
                            //Serial.println(s);                                                                // Debug
                            if (OutputPower > 0)
                               MobaLedLib.Set_Input(dst, Direction > 0);
                            }
                    }
                 return;
                 }
              break;
    case '$': if (sscanf(Buffer+1, "%i %x", &Addr, &State) == 2)
                 {
                 // Not implemented yet
                 return;
                 }
              break;
    }
  Serial.println(" Parse Error");
}

//-----------------------
void Proc_Received_Char()
//-----------------------
// Receive DCC Accessory Turnout messages:
//  "@ 319 00 01\n":  Addr, Direction, OutputPower
//
// and DCC Signal Output messages:
//  "$ 123 00\n":     Addr, State
{
  #ifdef GEN_BUTTON_RELEASE                                                                                   // 23.05.19:
    if (LastTime && millis()-LastTime > 400) // Use 1100 if no repeat is wanted
       {
       MobaLedLib.Set_Input(LastChannel, 0); // Send release
       LastTime = 0;
       Serial.print(F("Timeout "));
       Serial.println(millis());
       }
  #endif

  while (Serial.available() > 0)
     {
     char c = Serial.read();
     //Serial.print(c); // Debug
     switch (c)
        {
        case '@':
        case '$': if (*Buffer) Serial.println(F(" Unterm. Error"));
                  *Buffer = c; Buffer[1] = '\0'; break;
        #ifdef __SERIAL_INPUTS_H__
          case '\247': Proc_Serial_Input(c); break;    // /247 = Paragraph sign must be entered to start the debug function to test the LED colors and brightness values
        #endif
        default:  // Other characters
                  uint8_t ExpLen;
                  switch(*Buffer)
                     {
                     case '@': ExpLen = 11; break;
                     case '$': ExpLen =  8; break;
                     default : ExpLen =  0;
                               Serial.print(c); // Enable this line to show status messages from the DCC-Arduino
                     }
                  if (ExpLen)
                     {
                     uint8_t len = strlen(Buffer);
                     if (len < ExpLen)
                          {
                          Buffer[len++] = c;
                          Buffer[len]   = '\0';
                          }
                     else {
                          if (c != '\n')
                               Serial.println(F(" Length Error"));
                          else Proc_Buffer();
                          *Buffer = '\0';
                          }
                     }
        }
     }
}

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  Serial.begin(SERIAL_BAUD); // Communication with the DCC-Arduino must be fast

  Serial.print(F("DCC and Sound Example with "));
  #ifdef USE_MP3_TF_16P_SOUND_MODULE
     Serial.println(F("MP3-TF-16P"));
  #else
     Serial.println(F("JQ6500"));
  #endif

  pinMode(SEND_DISABLE_PIN, OUTPUT);
  digitalWrite(SEND_DISABLE_PIN, 0);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//

  MobaLedLib.Update();                // Update the LEDs in the configuration

  Proc_Received_Char();               // Process characters received from the DCC-Arduino

  digitalWrite(SEND_DISABLE_PIN, 1);  // Stop the sending of the DCC-Arduino because the RS232 interrupts are not processed during the FastLED.show() command
  FastLED.show();                     // Show the LEDs (send the leds[] array to the LED stripe)
  digitalWrite(SEND_DISABLE_PIN, 0);  // Allow the sanding of the DCC commands again


  LED_Heartbeat.Update();             // Update the heartbeat LED. This must be called periodically in the loop() function.
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

