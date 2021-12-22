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


 Huge configuration                                                                        by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with a huge configuration array.
 It is used to check the performance and the memory usage of the library.
 The example uses the maximum number of 256 RGB LEDs = 768 single LEDs. They are used in
 a quite typical configuration:
   1  heartbeat LED
   1  fire truck with 6 RGB LEDs
   1  traffic light with 6 RGB LEDs
   2  construction warning lights with 9 LEDs each
   3  railway signals together 12 LEDs
   12 street lights
   40 houses (5 RGB LEDs per house)

 It uses the CAN BUS to control the railway signals.

 And there is still some FLASH and RAM available (08.12.18):
   Sketch uses 19602 bytes (63%) of program storage space. Maximum is 30720 bytes.
   Global variables use 1436 bytes (70%) of dynamic memory, leaving 612 bytes for local variables. Maximum is 2048 bytes.
 The memory usage may be different because it depends on all used librarys

 The update time is about 14 ms. That's faster than humans can recognize (20ms).

                                                                         \      /
 *****************************************************                    \('')/
 *                                                   *                     ####
 *  ==> The library meets all my expectations :-))   *                      ##
 *                                                   *                     /##\
 *****************************************************                     |  |
 nearly 9 months of work have been worthwhile                              |  |


 If all 256 RGB LEDs are active they will draw 14 A ! Therefore the FastLed function
 setMaxPowerInVoltsAndMilliamps() is used to limit the current in the setup() function.
 Attention: If the power supply is too weak this can cause reboots of the Arduino.


 Other examples:
 ~~~~~~~~~~~~~~~
 This example could NOT be combined with other MobaLedLib examples because it contains
 already everything...


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
 and a WS2812 LED stripe with at least 32 LEDs. To see the whole configuration 256
 LEDs are needed and a huge power supply.
 The RAM, FLASH and CPU time consumption is not influenced by the number of connected LEDs.
 => The performance tests are also valid if less LEDs are connected.

 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).

 All examples could also be used with the other LED stripe types which are supported
 by the FastLED library (See FastLED Blink example how to adapt the "FastLED.addLeds"
 line.).
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

#include "Check_UpdateTime.h"

#define NUM_LEDS     256 // Number of LEDs without spare channels (Maximal 256 RGB LEDs could be used)
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

//***************************** Macros ******************************

#define TestHouse_5(LED)     House(LED,  SI_1, 2,  5,      ROOM_DARK, ROOM_BRIGHT, ROOM_WARM_W, ROOM_TV0, NEON_LIGHT) // House with 5 rooms

#define TestHouse_10(LED)    TestHouse_5(LED+0)   \
                             TestHouse_5(LED+5)

#define TestHouse_20(LED)    TestHouse_10(LED+0 ) \
                             TestHouse_10(LED+10)

#define TestHouse_50(LED)    TestHouse_20(LED+0 ) \
                             TestHouse_20(LED+20) \
                             TestHouse_10(LED+40)
#define TestHouse_100(LED)   TestHouse_50(LED+0)  \
                             TestHouse_50(LED+50)
#define Fire_truckRGB(LED, InCh)                                                                                       \
  Blink2    (LED,   C_ALL, InCh, 500 ms, 500 ms, 15,  128) /* Vorderlicht Blinken Dunkel 15/ Hell 128 (Weis)        */ \
  BlueLight1(LED+1, C3,    InCh)                           /* Blaulicht rechts                                      */ \
  BlueLight2(LED+2, C3,    InCh)                           /* Blaulicht Links (Andere Periode damit nicht synchron) */ \
  Blink2    (LED+3, C12,   InCh, 500 ms, 500 ms,  0,  128) /* Blinker hinten (Gelb)                                 */ \
  Const     (LED+4, C1,    InCh,                  0,   25) /* Ruecklicht     (Rot)                                  */

#define RGB_AmpelX_Fade_IO(LED, InNr)   /* Fading Traffic light for tests with RGB LEDs */                                                                                                                        \
           APatternT8(LED,0,InNr,18,0,255,0,PF_NO_SWITCH_OFF,200 ms,2 Sec,200 ms,1 Sec,200 ms,10 Sec,200 ms,3 Sec,1,2,4,8,144,33,64,134,0,128,2,0,10,128,33,0,134,0,1,2,4,8,16,32,67,128,12,1,0,5,0,20,0,67,0,12) \
           APatternT4(LED,0,InNr,18,0,255,0,PF_INVERT_INP | PF_NO_SWITCH_OFF,100 ms,300 ms,100 ms,500 ms,24,48,96,192,0,0,0,0,0)

#define RGB_ConstrWarnLight(LED,InNr)   APatternT2(LED,0,InNr,26,0,255,0,0,100 ms,200 ms,3,0,0,12,0,0,176,1,0,192,6,0,0,219,0,0,108,3,0,176,109,0,192,182,1,0,219,54,0,108,219,0,176,109,27,192,182,109,0,219,182,13,108,219,54,176,109,219,198,182,109,27,219,182,109,111,219,182,13,0,0,0,0,0,0,3,0,0,12,0,0,128,1,0,0,6,0,0,192,0,0,0,3,0,0,96,0,0,128,1,0,0,48,0,0,192,0,0,0,24,0,0,96,0,0,0,12,0,0,48,0,0,0,6,0,0,24,0,0,0,3,0,0,12,0,0,0,0,0,0)

#define Entry_Signal3_RGB(LED)   XPatternT1(LED,160,SI_LocalVar,9,0,128,0,0,500 ms,64,4,104,0  ,63,191,191)

#define Dep_Signal4_RGB(LED)     XPatternT1(LED,12,SI_LocalVar,18,0,128,0,0,500 ms,0,240,0,15,0,0,0,0,0,240,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,240,15,0,240,0,0,16,17,17,1,0  ,63,191,191,191)

#define GasLightsRGB_6(LED)      GasLights(LED,   SI_1, GAS_LIGHT,  GAS_LIGHT,  GAS_LIGHTD,  GAS_LIGHT,  GAS_LIGHT,  GAS_LIGHT)  // RGB stripe or light bulbs with parrallel used ouputs


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  RGB_Heartbeat       (0)           // 1 LED
  Fire_truckRGB       (1,  SI_1)    // 5 LEDs
  RGB_AmpelX_Fade_IO  (6,  SI_1)    // 6 LEDs
  RGB_ConstrWarnLight (12, SI_1)    // 9 LEDs

  InCh_to_TmpVar(0, 3)
  Entry_Signal3_RGB   (21)          // 3 LEDs

  InCh_to_TmpVar(4, 7)
  Dep_Signal4_RGB     (24)           // 6 LEDs

  InCh_to_TmpVar(8, 11)
  Entry_Signal3_RGB   (30)           // 3 LEDs

  RGB_ConstrWarnLight (33, SI_1)     // 9 LEDs
  AndreaskrRGB        (42,  SI_1)    // 2 LEDs
  GasLightsRGB_6      (45)           // 6 LEDs
  GasLightsRGB_6      (50)           // 6 LEDs

  TestHouse_100       (56)           // 100 LEDs / 20 Houses
  TestHouse_100       (156)          // 100 LEDs / 20 Houses
  //                   256
  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];     // Define the array of leds

MobaLedLib_Create(leds); // Define the MobaLedLib instance

LED_Heartbeat_C LED_Heartbeat(LED_HEARTBEAT_PIN); // The build in LED can't be use because the pin is used as clock port for the SPI bus

bool CAN_ok = false;

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  FastLED.setMaxPowerInVoltsAndMilliamps(5, 1500);       // limit the power used by the LEDs to 1.5A at 5V

  Serial.begin(9600); // Attention: The serial monitor in the Arduino IDE must use the same baudrate
  Serial.println(F("Started Huge MobaLedLib example"));

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

  Serial.print(F("Config size:    ")); Serial.println(sizeof(Config));
  Serial.print(F("Config_RAM size:")); Serial.println(sizeof(Config_RAM));


  // Set the default value for the light signals at power on
  MobaLedLib.Set_Input(0,  1);
  MobaLedLib.Set_Input(4,  1);
  MobaLedLib.Set_Input(8,  1);
  MobaLedLib.Set_Input(12, 1);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  Process_CAN();          // Read the messages from the CAN bus and write the InpStructArray[]

  MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)

  LED_Heartbeat.Update(); // Update the heartbeat LED. This must be called periodically in the loop() function.

  Check_UpdateTime();     // Measure the update time and print it to the serial monitor
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
