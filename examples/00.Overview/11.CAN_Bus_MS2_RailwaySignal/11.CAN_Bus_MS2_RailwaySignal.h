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


 CAN Bus from MS2 controlled Railway Signal                                                by Hardi   16.11.18
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes how to use the CAN bus from a Maerklin MS2 to control a Railway Light Signal.
 The example uses a MCP2515 CAN modula connected to the Arduino.
 For more details about the pattern function check the prior example.

 This example shows how a Railway Light Signal could be implemented. Two signal should be and show
 the following signal aspects:

 Entry signal:                                        Departure signal:
   Hp0         Hp1         Hp2                          Hp0         Hp1         Hp2        Hp0+SH1    @ = active, O = not active
  ______      ______      ______                       ______      ______      ______      ______
 /      \    /      \    /      \                     /      \    /      \    /      \    /      \
 |    0 |    |    @ |    |    @ |       Green         | O    |    | @    |    | @    |    | O    |    Green
 |      |    |      |    |      |                     | @  @ |    | O  O |    | O  O |    | @  O |    Red    Red
 |      |    |      |    |      |                     |    . |    |    . |    |    . |    |    * |    White           * = active, . = not active
 |      |    |      |    |      |                     | .    |    | .    |    | .    |    | *    |           White
 | @  0 |    | 0  0 |    | 0  @ |  Red  Yellow        | O    |    | O    |    | @    |    | O    |    Yellow
 '------'    '------'    '------'                     '------'    '------'    '------'    '------'
    ||          ||          ||                           ||          ||          ||          ||

 The lights fade slowly from one aspect to the other which looks quite nice.


 In contrast to the last example, the signal should be controlled with a Maerklin mobile station over the
 CAN bus.


 Test with RGB LED Stripe:
 ~~~~~~~~~~~~~~~~~~~~~~~~~
 To be able to test the example without a special light signal two lines are available below.
 The first could be used for demonstration with an RGB LED stripe (Signal3_RGB).

 The second line (Signal3) is designed for the usage with a real light signal.
 It is controlled by a WS2811 module.
 Attention: "Viessmann multiplex" signals could not be used. I'm planing to implement a
            multiplex driver with a ATTiny which costs about 2 Euro. But therefore I need a
            signal from Viessmann to test it. Maybe there is someone out there who donates one...

 By default the RGB example is enabled in the configuration (Change the comments to use the example
 with a real signal).

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Video:
 ~~~~~~
 This video demonstrates the example:
   https://vimeo.com/308898588

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and one WS2811 module. For tests a LED stripe could also be used.
 In addition a MCP2515 CAN Modula is needed (~1.50 Eu Google: "mcp2515 CAN Module China").

 The DIN pin of the WS2811 module is connected to pin D6 (LED_DO_PIN).

     .-----------------------------------------------------.
     | Arduino Nano:    +-----+                            |  +--------------------+
     |     +------------| USB |------------+               |  |INT MCP2515 CAN     |
     |     |            +-----+            |               '--|SCK  ____           |
     '--B5-| [ ]D13/SCK        MISO/D12[ ] |---B4-----\/------|SI- |    |CAN     .-|
           | [ ]3.3V           MOSI/D11[ ]~|---B3-----/\------|SO- |    |      H |o|--- CAN H   to the Maerklin MS2
           | [ ]V.ref     ___    SS/D10[ ]~|---B2-------------|CS- |    |      : |o|--- CAN L   Attention: The CAN bus to the MS2 must be isolated !
        C0 | [ ]A0       / N \       D9[ ]~|   B1             |GND |____|      L '-|                       Otherwise the components may be damaged.
        C1 | [ ]A1      /  A  \      D8[ ] |   B0         +5V-|VCC            .. J1|                       It's also possible to use a common ground.
        C2 | [ ]A2      \  N  /      D7[ ] |   D7             +--------------------+                       Don't connect the Arduino ground with one
        C3 | [ ]A3       \_0_/       D6[ ]~|---D6------.                                                   of the rails!
        C4 | [ ]A4/SDA               D5[ ]~|   D5      |                     Entry signal:
        C5 | [ ]A5/SCL               D4[ ] |   D4      |       ------------
           | [ ]A6              INT1/D3[ ]~|   D3      |  +5V--+  WS2811  +-C3--(Yellow)-.
           | [ ]A7              INT0/D2[ ] |   D2      '--DIN--+    5V    +-C1---(Red )--o
  +5V------| [ ]5V                  GND[ ] |           .--DOut-+  Bottom  +--------------o
        C6 | [ ]RST                 RST[ ] |   C6      |  GND--+   side   +-C2--(Green)--'
  GND------| [ ]GND   5V MOSI GND   TX1[ ] |   D0      |       ------------
           | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1      |
           |          [ ] [ ] [ ]          |           |                     Departure signal:
           |          MISO SCK RST         |           |       ------------
           | NANO-V3                       |           |  +5V--+  WS2811  +-C3--(Yellow)-.
           +-------------------------------+           '--DIN--+    5V    +-C1---(Red1)--o
                                                       .--DOut-+  Bottom  +--------------o
                                                       |  GND--+   side   +-C2--(Green)--o
                                                       |       ------------              |
                                                       |       ------------              |
                                                       |  +5V--+  WS2811  +-C3           |
                                                       '--DIN--+    5V    +-C1---(Red2)--o
                                                       .--DOut-+  Bottom  +              |
                                                       |  GND--+   side   +-C2--(White)--'
                                                       |       ------------
                                               (to the next WS281x)

 The connection to the Maerklin MS2 could be made with a seperate cable or by adding a connector to
 the "Gleisbox". Both is described here:
   http://www.skrauss.de/modellbahn/gboxcan.html
 in case the page has been moved here a link to the main page:
   http://www.skrauss.de/modellbahn/index.html sub page "Gleisbox als Zentrale" / "CAN-Anschluss"

 Attention: The CAN bus to the MS2 must be isolated !
            Otherwise the components may be damaged.
            It's also possible to use a common ground.
            Don't connect the Arduino ground with one
            of the rails!

*/

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#include <mcp_can.h>     // The MCP CAN library must be installed in addition if you got the error message "..fatal error: mcp_can.h: No such file or directory" https://github.com/coryjfowler/MCP_CAN_lib
                         // Download the ZIP file from https://github.com/coryjfowler/MCP_CAN_lib
                         // Arduino IDE: Sketch / Include library / Add .ZIP library...                 Deutsche IDE: Sketch / Bibliothek einbinden / .ZIP Bibliothek hinzufuegen...
                         //              Navigate to the download directory and select the file                       Zum Download Verzeichnis werchseln und die Datei auswaehlen
                         //
                         // Attention: Disable the debug in mcp_can_dfs.h by setting the following define to 0:
                         //              #define DEBUG_MODE 0
                         //            This is important because the debug messages use a lot of RAM.
                         //            To prevent this use the F() macro in print() functions like used here.
                         //            If your program uses to much memory you get the following warning:
                         //              "Low memory available, stability problems may occur."
#include <SPI.h>
#include "Add_Message_to_Filter.h"


#define NUM_LEDS     32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#else 
	#error this example does not support this plattform
#endif
#define CAN_CS_PIN   10  // Pin D10 is used as chip select for the CAN bus

#define LED_HEARTBEAT_PIN 17 // The build in LED can't be use because the pin is used as clock port for the SPI bus

// Define which accessoires CAN messages should be used.
#define DCC_FIRST_LOC_ID     0   // First local ID which should be copied to the InpStructArray[] of the MobaLedLib
#define DCC_LAST_LOC_ID      19  // Last    "                         "
#define DCC_INPSTRUCT_START  0   // Start number in the InpStructArray[]

MCP_CAN  CAN(CAN_CS_PIN);

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
  Dep_Signal4_RGB(8)      // Start LED 8: Demonstration with a LED stripe
//Dep_Signal4(    1)      // Start LED channel 1: Real departure signal connected to two  WS2811 modules

  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];       // Define the array of leds

MobaLedLib_Create(leds);   // Define the MobaLedLib instance

LED_Heartbeat_C LED_Heartbeat(LED_HEARTBEAT_PIN); // The build in LED can't be use because the pin is used as clock port for the SPI bus

bool CAN_ok = false;

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  Serial.begin(9600); // Attention: The serial monitor in the Arduino IDE must use the same baudrate

  // *** Initialize the CAN bus ***
  if (CAN.begin(MCP_STDEXT, CAN_250KBPS, MCP_8MHZ) == CAN_OK) // init CAN bus, baudrate: 250k@8MHz
       {
       Serial.println(F("CAN Init OK!"));
       CAN_ok = true;
       CAN.setMode(MCP_NORMAL); // Important to use the filters
       // Set the message filters
       Add_Message_to_Filter(CAN, 0x80000000 | 0x0016FFFF, 0); // Filter 0 initializes the CAN chip
       Add_Message_to_Filter(CAN, 0x80000000 | 0x00160000, 6); // Filter >= 6 uses also channel 0
       }                                                       // => Filter is addapte to pass both messages
  else Serial.println(F("CAN Init Fail!"));                    //    => Messages matching 0x0016???? are passed

  MobaLedLib.Set_Input(0, 1); // Set the default value for the entry light signal at power on
  MobaLedLib.Set_Input(4, 1); // Set the default value for the departure light signal at power on
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously

  Process_CAN();          // Read the messages from the CAN bus and write the InpStructArray[]

  MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)

  LED_Heartbeat.Update(); // Update the heartbeat LED. This must be called periodically in the loop() function.
}


//-----------------------------------
void Proc_Accessoires(uint8_t *rxBuf)
//-----------------------------------
// Process accessoires CAN messages
// See: https://www.maerklin.de/fileadmin/media/produkte/CS2_can-protokoll_1-0.pdf
{
  uint8_t Pos     = rxBuf[4];  // 0 = Aus, Rund, Rot, Rechts, HP0 / 1 = Ein, Gruen, Gerade, HP1 / 2 = Gelb, Links, HP2 / 3 = Weiss, SH0
  uint8_t Current = rxBuf[5];  // 0 = Ausschalten, 1-31 Einschalten mit Dimmwert (sofern Protokoll dies unterstuetzt), 1 = ein
  uint16_t Loc_ID = ((rxBuf[2]<<8) + rxBuf[3]) & 0x7FF;  // Adresses from 0 - 0x7FF are possible (2048 adresses)

  // The InpStructArray[] is enabled if Current is > 0, otherwise it's disabled
  if (Loc_ID >= DCC_FIRST_LOC_ID && Loc_ID <= DCC_LAST_LOC_ID)
     MobaLedLib.Set_Input(((Loc_ID - DCC_FIRST_LOC_ID)*2) + DCC_INPSTRUCT_START + Pos, Current);
}

//----------------
void Process_CAN()
//----------------
{
uint8_t  len;
uint32_t rxId;
uint8_t rxBuf[8];
if (CAN_ok && CAN.checkReceive() == CAN_MSGAVAIL)
   {
   if (CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
      {
      if (((rxId>>16) & 0xFF) == 0x16) Proc_Accessoires(rxBuf); // Check if it's a accessoires message
      }
   }
}


/*

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

