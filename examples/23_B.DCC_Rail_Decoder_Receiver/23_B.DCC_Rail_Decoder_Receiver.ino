/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018, 2019  Hardi Stengelin: MobaLedLib@gmx.de

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


 DCC rail signal decoder and transmitter to LED Arduino                                    by Hardi   14.01.19
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes how to read DCC signals (Digital Command Control) from the rail signal
 of a model railway and transmit the accessory and the signalling commands to this Arduino.
 To use this example the program "23_A.DCC_Rail_Decoder_Transmitter.ino" on a the second Arduino.
 See the documentation in this program for more details.

 This program is the part for the LED-Arduino.
 To change the behavior of the LEDs only this program has to be modified. The program on the DCC-Arduino
 don't has to be changed.

 The program uses the build in LED of the Arduino as a heartbeat. It's normal if the LED is flashing
 slowly (1 Hz). Don't mix up this with LED on the DCC-Arduino which is used to show buffer overrun error
 with a fast flashing signal.

 The program reads in the DCC accessory signals and maps them to the input variables of the MobaLedLib.
 The constants DCC_FIRST_LOC_ID, DCC_LAST_LOC_ID and DCC_INPSTRUCT_START below define which DCC adresses
 are mapped to which variables in the InpStructArray[].

 Hardware:
 ~~~~~~~~~
 See "23_A.DCC_Rail_Decoder_Transmitter.ino"
*/

#define SEND_DISABLE_PIN      A1
#define ERROR_LED_PIN         13

#define LED_HEARTBEAT_PIN     13

#define NUM_LEDS              32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#define LED_DO_PIN            6   // Pin D6 is connected to the LED stripe

// Define which accessoires CAN messages should be used.
#define DCC_FIRST_LOC_ID      0   // First local ID which should be copied to the InpStructArray[] of the MobaLedLib
#define DCC_LAST_LOC_ID       19  // Last    "                         "
#define DCC_INPSTRUCT_START   0   // Start number in the InpStructArray[]

#define SERIAL_BAUD      115200   // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"

// *** Macros ***
// Entry signal with 3 aspects
#define Entry_Signal3_RGB(LED)   XPatternT1(LED,160,SI_LocalVar,9,0,128,0,0,500 ms,64,4,104,0  ,63,191,191)
#define Entry_Signal3(LED)       XPatternT1(LED,224,SI_LocalVar,3,0,128,0,0,500 ms,145,1  ,63,191,191)

// Departure signal with 4 aspects
#define Dep_Signal4_RGB(LED)     XPatternT1(LED,12,SI_LocalVar,18,0,128,0,0,500 ms,0,240,0,15,0,0,0,0,0,240,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,240,15,0,240,0,0,16,17,17,1,0  ,63,191,191,191)
#define Dep_Signal4(LED)         XPatternT1(LED,12,SI_LocalVar,5,0,128,0,0,500 ms,15,240,0,15,0,240,15,240,0,16  ,63,191,191,191)



//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  InCh_to_TmpVar(0, 3)
  Entry_Signal3_RGB(4)    // Start LED 4: Demonstration with a LED stripe
//Entry_Signal3(    0)    // Start LED channel 0: Real signal connected to a WS2811 module

  InCh_to_TmpVar(4, 7)
  Dep_Signal4_RGB(11)     // Start LED 11: Demonstration with a LED stripe
//Dep_Signal4(    1)      // Start LED channel 1: Real departure signal connected to two  WS2811 modules

  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];       // Define the array of leds

MobaLedLib_Create(leds);   // Define the MobaLedLib instance
LED_Heartbeat_C LED_HeartBeat(LED_HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.

char Buffer[12] = "";      // Store the received messages from the DCC-Arduino

//----------------
void Proc_Buffer()
//----------------
{
  uint16_t Addr, Direction, OutputPower, State;
  char s[25];
  switch (*Buffer)
    {
    case '@': if (sscanf(Buffer+1, "%i %x %x", &Addr, &Direction, &OutputPower) == 3)
                 {
                 Addr--; // Internally the address starts with 0
                 if (Addr >= DCC_FIRST_LOC_ID && Addr <= DCC_LAST_LOC_ID)
                     MobaLedLib.Set_Input(((Addr - DCC_FIRST_LOC_ID)*2) + DCC_INPSTRUCT_START + Direction, OutputPower);
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
  while (Serial.available() > 0)
     {
     char c = Serial.read();
     //Serial.print(c); // Debug
     switch (c)
        {
        case '@':
        case '$': if (*Buffer) Serial.println(F(" Unterm. Error"));
                  *Buffer = c; Buffer[1] = '\0'; break;
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

//-----------
void setup(){
//-----------
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  Serial.begin(SERIAL_BAUD); // Communication with the DCC-Arduino must be fast

  Serial.println(F("DCC_Rail_Decoder_Transmitter Example B"));

  pinMode(SEND_DISABLE_PIN, OUTPUT);
  digitalWrite(SEND_DISABLE_PIN, 0);

  MobaLedLib.Set_Input(0, 1); // Set the default value for the entry light signal at power on
  MobaLedLib.Set_Input(4, 1); // Set the default value for the departure light signal at power on
}

//-----------
void loop(){
//-----------
  LED_HeartBeat.Update();

  MobaLedLib.Update();               // Update the LEDs in the configuration

  Proc_Received_Char();              // Process characters received from the DCC-Arduino

  digitalWrite(SEND_DISABLE_PIN, 1);  // Stop the sending of the DCC-Arduino because the RS232 interrupts are not processed during the FastLED.show() command
  FastLED.show();                    // Show the LEDs (send the leds[] array to the LED stripe)
  digitalWrite(SEND_DISABLE_PIN, 0);  // Allow the sanding of the DCC commands again

}

