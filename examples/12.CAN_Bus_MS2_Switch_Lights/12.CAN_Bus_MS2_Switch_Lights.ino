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


 CAN Bus from MS2 controlled Lights                                                        by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes how to use the CAN bus from a Maerklin MS2 to controls the lights of houses.
 It is based on the example "03.Switched_Houses", but here the lights could be turned on and off
 with the MS2 over the CAN.
 The example uses a MCP2515 CAN modula connected to the Arduino.

 For details to the House() function check the "03.Switched_Houses" example

 Attention: It takes some time (up to 2.5 minutes) to see changes. The people in the houses
            don't run from room to room and turnig the lights on and off.
            Change the #defines HOUSE_MIN_T and HOUSE_MAX_T below to modify the update rates.


 Other examples:
 ~~~~~~~~~~~~~~~
 The CAN input function in this example is different than the one used in the example
 "11.CAN_Bus_MS2_RailwaySignal". Here one InpStructArray[] entry is controlled by
 two buttons on the MS2. If the "Red" button is pressed the channel is turned off.
 It's turned on again with the "Green" button.

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
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...) and a
 and a WS2812 LED stripe or better houses with lights based on WS2812 LEDs.

 In addition a MCP2515 CAN Modula is needed (~1.50 Eu Google: "mcp2515 CAN Module China").

 The DIN pin of the LED Strile is connected to pin D6 (LED_DO_PIN).

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
        C3 | [ ]A3       \_0_/       D6[ ]~|---D6---> to the LED stripe.                                   of the rails!
        C4 | [ ]A4/SDA               D5[ ]~|   D5
        C5 | [ ]A5/SCL               D4[ ] |   D4
           | [ ]A6              INT1/D3[ ]~|   D3
           | [ ]A7              INT0/D2[ ] |   D2
  +5V------| [ ]5V                  GND[ ] |
        C6 | [ ]RST                 RST[ ] |   C6
  GND------| [ ]GND   5V MOSI GND   TX1[ ] |   D0
           | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
           |          [ ] [ ] [ ]          |
           |          MISO SCK RST         |
           | NANO-V3                       |
           +-------------------------------+

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

#include "mcp_can_nd.h"  // The MCP CAN library must be installed in addition if you got the error message "..fatal error: mcp_can.h: No such file or directory" https://github.com/coryjfowler/MCP_CAN_lib
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
#include "Add_Message_to_Filter_nd.h"


#define NUM_LEDS     32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#else 
#error this example does not support this plattform
#endif
#define CAN_CS_PIN   10  // Pin D10 is used as chip select for the CAN bus

#define LED_HEARTBEAT_PIN 17 // The build in LED can't be use because the pin is used as clock port for the SPI bus

// Define which accessoires CAN messages should be used.
#define DCC_FIRST_LOC_ID_SW     0   // First local ID which should be copied to the InpStructArray[] of the MobaLedLib
#define DCC_LAST_LOC_ID_SW      19  // Last    "                         "
#define DCC_INPSTRUCT_START_SW  0   // Start number in the InpStructArray[]

MCP_CAN  CAN(CAN_CS_PIN);


#define INCH0        0   // Define names for the input channels to be able to change them easily.
#define INCH1        1   // In this small example this is not necessary, but it's useful in a
#define INCH2        2   // large configuration.
#define INCH3        3

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
  //    |             |      |      |   |
  Const(HOUSE_C_SHOP, C_ALL, INCH3, 10, 255) // The light in the shop is not turned off, it's just dimmed down in the evening if nobody is out on the street
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

  // The "Red" channel disables the InpStructArray[], the "Green" channel enables it
  if (Loc_ID >= DCC_FIRST_LOC_ID_SW && Loc_ID <= DCC_LAST_LOC_ID_SW && Current > 0)
     MobaLedLib.Set_Input(Loc_ID - DCC_FIRST_LOC_ID_SW + DCC_INPSTRUCT_START_SW, Pos);
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

