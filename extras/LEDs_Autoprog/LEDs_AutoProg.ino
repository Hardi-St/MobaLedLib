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


 Autogenerate program for the LEDs Arduino                                                 by Hardi   04.09.19:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 The excel program Prog_Generator_MobaLedLib.xlsm is a graphical user interface for the MobaLedLib.
 It could generate the program to control LEDs and other WS281x components without programming
 knowledge. The user simply fills a table with all wanted functions and start the "Send to Arduino"
 button. The Excel program generates the header file "LEDs_AutoProg.h" which is linked together
 with this program and send to the nano.

 This program could read commands from a railway central using DCC, Selectrix or CAN.

 The Excel program is opened by copying the following line into the address bar of
 Windows Explorer:
 %USERPROFILE%\Documents\Arduino\libraries\Mobaledlib\examples\23_B.LEDs_AutoProg\Prog_Generator_MobaLedLib.xlsm




 Hardware:
 ~~~~~~~~~

 - DCC or Selectrix:    (CAN Bus see below)
   ~~~~~~~~~~~~~~~~~
   The project is used with two two Arduino compatible boards (Uno, Nano, Mega, ...) if the
   commands are read from DCC or Selectrix.
   For DCC an opto coppler (6N137), a diode (1n148) and a 1K resistor is needed.
   For Selectrix two 22K resistors and a DIN5 connector is needed.

   The two Arduinos are connected by three wires (TX1-RX1, GND, A1).
   The A1 line is used to disable the serial transmission while the LEDs are updated.

   The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).
   An optional heartbeat LED could be connected to pin D3 togeter with a 470 ohm resistor.

                       .-----------------------------------------------------.
                       |         DCC or Selectrix                            |  The DCC or Selectrix program
 Optocopler is         |           Arduino Nano: +-----+                     |  must be sendto the Arduino
 used for DCC          |            +------------| USB |------------+        |  at first to make sure that the
 only              +5V |            |            +-----+            |        |  TX line is disabled. Otherwise
                    ^  |            | [ ]D13/SCK        MISO/D12[ ] |        |  the LED Arduino can't be flashed
            ______  |  |            | [ ]3.3V           MOSI/D11[ ]~|        |
           |1     |-'  |            | [ ]V.ref     ___    SS/D10[ ]~|        |
 ---[1K]---|      |    |            | [ ]A0       / N \       D9[ ]~|        |
 t      .--|      |----'  .---------| [ ]A1      /  A  \      D8[ ] |        |
 o      |  |______|-.     |         | [ ]A2      \  N  /      D7[ ] |        |            Selectrix DIN5 connector
        |   6N137   |     |         | [ ]A3       \_0_/       D6[ ]~|        *----[22K]---- Pin 1
 r      |          GND    |         | [ ]A4/SDA               D5[ ]~|        |              Pin 2 --- GND
 a     \/ 1N148           |         | [ ]A5/SCL               D4[ ] |-------------[22K]---- Pin 4
 i     --                 |    +5V  | [ ]A6              INT1/D3[ ]~|-HB LED | with 470 ohm resistor (Opt.)
 l      |                 |     ^   | [ ]A7              INT0/D2[ ] |--------'
 -------'                 |     '---| [ ]5V                  GND[ ] |
                          |         | [ ]RST                 RST[ ] |
                          |  .-GND--| [ ]GND   5V MOSI GND   TX1[ ] |--------*---[3.9K]--.
                          |  |      | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |        |           |
                          |  |      |          [ ] [ ] [ ]          |        |          GND
                          |  |      |          MISO SCK RST         |        |
                          |  |      | NANO-V3                       |        |
                          |  |      +-------------------------------+        |
                          |  |                                               |
                          |  |                                               |
                          |  | LED-Arduino Nano: +-----+                     |
                          |  |      +------------| USB |------------+        |
                          |  |      |            +-----+            |        |
                          |  |      | [ ]D13/SCK        MISO/D12[ ] |        |
                          |  |      | [ ]3.3V           MOSI/D11[ ]~|        |
                          |  |      | [ ]V.ref     ___    SS/D10[ ]~|        |  <== This second program must be send
                          |  |      | [ ]A0       / N \       D9[ ]~|        |      to the LED-Arduino
          SEND_DISABLE -> '---------| [ ]A1      /  A  \      D8[ ] |        |      (23_B.DCC_Rail_Decoder_Receiver.ino)
          line               |      | [ ]A2      \  N  /      D7[ ] |        |
                             |      | [ ]A3       \_0_/       D6[ ]~|---LEDs | WS281x
                             |      | [ ]A4/SDA               D5[ ]~|        |
                             |      | [ ]A5/SCL               D4[ ] |        |
                             | +5V  | [ ]A6              INT1/D3[ ]~|        |
                             |  ^   | [ ]A7              INT0/D2[ ] |        |
                             |  '---| [ ]5V                  GND[ ] |        |
                             |      | [ ]RST                 RST[ ] |        |
                             '-GND--| [ ]GND   5V MOSI GND   TX1[ ] |        |
                                    | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |--------'
                                    |          [ ] [ ] [ ]          |
                                    |          MISO SCK RST         |
                                    | NANO-V3                       |
                                    +-------------------------------+
   The +5V supply lines of both arduinos could be connected together if they are powerd from one power supply (one USB).


 - CAN Bus:
   ~~~~~~~
   If the program is used with the Maerklin CAN bus only one Arduino compatible board (Uno, Nano, Mega, ...)
   and a MCP2515 CAN Module is needed (~1.50 Eu Google: "mcp2515 CAN Module China").

   The DIN pin of the WS2811 module is connected to pin D6 (LED_DO_PIN).

     .-----------------------------------------------------.
     | Arduino Nano:    +-----+                            |  +--------------------+
     |     +------------| USB |------------+               |  |INT MCP2515 CAN     |
     |     |            +-----+            |               '--|SCK  ____           |
     '--B5-| [ ]D13/SCK        MISO/D12[ ] |---B4-----\/------|SI- |    |CAN     .-|
           | [ ]3.3V           MOSI/D11[ ]~|---B3-----/\------|SO- |    |      H |o|--- CAN H   to the Maerklin MS2
           | [ ]V.ref     ___    SS/D10[ ]~|---B2-------------|CS- |    |      : |o|--- CAN L   Attention: The CAN bus to the MS2 must be isolated !
        C0 | [ ]A0       / N \       D9[ ]~|   B1             |GND |____|      L '-|            Otherwise the components may be damaged.
        C1 | [ ]A1      /  A  \      D8[ ] |   B0         +5V-|VCC            .. J1|            It's also possible to use a common ground.
        C2 | [ ]A2      \  N  /      D7[ ] |   D7             +--------------------+
        C3 | [ ]A3       \_0_/       D6[ ]~|---D6------> LEDs WS281x                            Don't connect the Arduino ground with one
        C4 | [ ]A4/SDA               D5[ ]~|   D5                                               of the rails!
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


 Adapter to connect the ESP32 to the Mainboard
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 ESP32    LED / DCC Arduino
 25       D9         Right  switch
 26       D8         Middle switch
 12       D7         Left   switch
 27       D6         LED Bus 0
 32       A4         LED Bus 1
 16       D3         LED 1 (Left   switch) or LED Bus 2
 14       D4         LED 2 (Middle switch) or LED Bus 3
 18       D13        LED 3 (Right  switch) or LED Bus 4 
 19       D12        LED Bus 5
 23       D11        LED Bus 6
 0        D5         LED Bus 7  (not available on 30 pin ESP32 module)
 13       D2         DCC Signal   with voltage divider 510/1K if Selctrix is used
 15       A0         CLOCK_K    (RX2), with 3,3V -> 5V level shifter
 34       A2         BUTTONS         , with 1K/2K voltage divider
 33       A3         RESET_K         , with 3,3V -> 5V level shifter
 39       A5         Analog buttons or KEY_80, with 1K/2K voltage divider, change to 100k/220k when using as analog buttons
 36       A6         Analog buttons  , with 100K/220K voltage divider
 35       A7         Day & Night     , with 100K/220K voltage divider
 2        -          Onboard LED
 21       -          SDA I2C Display
 22       -          SCL I2C Display
 EN                  1uF Capacitor to GND to fix boot problems at some ESP boards (See: https://github.com/espressif/esptool/wiki/ESP32-Boot-Mode-Selection)
 4        (A1)       CAN Rx or A1 via Jumper or Selectrix, with optional voltage divider
 5        -          CAN Tx
 1        D0         USB Tx
 3        D1         USB Rx
 
   Voltage divider
        R1
5V   --[1K]--*---->-  ESP (3.3V)
             |
            |2| R2      
            |K|         
             |          
            _|_




 Adapter to connect the Raspberry PICO to the Mainboard

 !! The PICO Leds BUS must have consecutive pin numbers !!
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 PICO    LED / DCC Arduino
 8        D9         Right  switch
 9        D8         Middle switch
 10       D7         Left   switch
 0        D6         LED Bus 0
(1)       A4         LED Bus 1
(2)                  LED Bus 2
(3)                  LED Bus 3
(4)                  LED Bus 4
(5)       D12        LED Bus 5
(6)       D11        LED Bus 6
(7)       -          LED Bus 7
 13       D3         LED 1 (Left   switch) 
 12       D4         LED 2 (Middle switch) 
 11       D5         LED 3 (Right  switch)  
 22       D2         DCC Signal   with voltage divider 510/1K if Selctrix is used
 14       A0         CLOCK_K    (RX2), with 3,3V -> 5V level shifter
 16       A2         BUTTONS         , with 1K/2K voltage divider
 15       A3         RESET_K         , with 3,3V -> 5V level shifter
          A5         Analog buttons or KEY_80, with 1K/2K voltage divider, change to 100k/220k when using as analog buttons
          A6         Analog buttons  , with 100K/220K voltage divider
          A7         Day & Night     , with 100K/220K voltage divider
 25       -          Onboard LED
          -          SDA I2C Display
          -          SCL I2C Display
          (A1)       CAN Rx or A1 via Jumper or Selectrix, with optional voltage divider
          -          CAN Tx
          D0         USB Tx
          D1         USB Rx
 40       5V         5V Supply
 
   Voltage divider
        R1
5V   --[1K]--*---->-  PICO (3.3V)
             |
            |2| R2      
            |K|         
             |          
            _|_

 Revision History:
 ~~~~~~~~~~~~~~~~~
 12.01.20:  - Improved the Check_Mainboard_Buttons() function
 01.05.20:  - Storing the status to the EEPROM (Functions from Juergen)
 05.05.20:  - Wait 1.5 seconds after booting to prevent flickering of the LEDs if the Arduino is detected from the excel program
 13.05.20:  - Started the SPI interface
 17.05.20:  - The SPI communication is no longer necessary since the TX LED Problem could also be
              solved wit a 3.9K resistor between TX and ground.
              The SPI functions are kept in the software but the are normally disabled by not defining USE_SPI_COM
 05.10.20:  - Added the Mainboard LED function
 07.10.20:  - Added DayAndNight Timer
 08.10.20:  - New method "LED_to_Var()" to set variables controlled by the LED values.
 09.10.20:  - Using port writes to speed up the Mainboard_LED function
            - Added additional pins to the Mainboard_LED function. Now nearly every pin could be
              used as LED pin (New channels 0, 5-16). See definition of the LEDx_PINs below
 10.10.20:  - Added the compiler switch "KeepDarknessCtr"
 11.10.20:  - New Mainboard_LED defines which end with the pin number (Example: Mainboard_LED_A0)
 14.11.20:  - Added the experimental ESP32 support
 19.01.21:  - Added DMX support by Juergen
 10.04.21:  - add new FarbTest protocol without need to reset CPU 
 21.04.21:  - Juergen: release ESP32 and DMX512 support
 23.04.21:  - ubit: Added ServoMP3 commands
 24.04.21:  - Juergen: add Pico support 
 25.04.21:  - Juergen: improve signaling of DCC status using onboard led (ESP & PICO)
 14.09.21:  - Juergen: add signalling of DCC status with ESP32 onboard led
 29.09.21:  - Juergen: add new feature to enable processing of extra commands outside the core library
 12.10.21:  - Juergen: add new feature to control sound modules attached to the mainboard  (currently limited to Arduino Nano and JQ6500)
*/

#ifdef ARDUINO_RASPBERRY_PI_PICO
  #include "pico/stdlib.h"
  #include <pico/multicore.h>
#endif

#include <Arduino.h>
#include "MP3.h"

#ifndef __LEDS_AUTOPROG_H__
  #define FASTLED_ESP32_FLASH_LOCK 1
  #include "LEDs_AutoProg.h"   // This file is generated by "Prog_Generator_MobaLedLib.xlsm"
#endif // __LEDS_AUTOPROG_H__

#ifndef ESP32  // The pin is not used by the ESP                                                              // 30.10.20: Juergen
  #define SEND_DISABLE_PIN    A1 // Pin A1 is used to stop the DCC, Selectrix, ... Arduino from sending RS232 characters
#endif

#ifdef ESP32                                                                                                  // 30.10.20: Juergen
  #include "esp_task_wdt.h"																					  // 05.03.21: Juergen - needed to reset watchdog timer while Farbtest is active
  #include <EEPROM.h>
  #define EEPROM_SIZE 512			// maximum size of the eeprom
  //#define EEPROM_OFFSET 0			// (the first 96 byte are reserved for WIFI configuration)  // 28.11.2020 comment out -> WIFI config no longer stored in EEPROM
  #ifdef USE_SX_INTERFACE
      #define SX_SIGNAL_PIN 4
      #define SX_CLOCK_PIN 13
      #define SX_STATUS_PIN  2  // Build in LED
      #include "SXInterface.h"
      #define USE_COMM_INTERFACE
  #else
  #define USE_DCC_INTERFACE
  #define DCC_STATUS_PIN  2  // Build in LED
  #include "DCCInterface.h"
      #define DCC_SIGNAL_PIN   13
      #define USE_COMM_INTERFACE
  #endif
  #include "InMemoryStream.h"

  #ifdef USE_SPI_COM
	#error "USE_SPI_COM can't be used on ESP32 platform"
  #endif
  
#endif

#ifdef ARDUINO_RASPBERRY_PI_PICO
  const uint DATA_IN_PIN = 29;
  const uint DATA_OUT_PIN = 28;
  const uint NUM_LEDS_TO_EMULATE = 1;
  #include "ws2811.hpp"
  #define USE_DCC_INTERFACE
  #define USE_COMM_INTERFACE
  #define DCC_STATUS_PIN  LED_BUILTIN
  #include "DCCInterface.h"
  #include "InMemoryStream.h"
  #ifdef USE_SPI_COM
  #error "USE_SPI_COM can't be used on ESP32 platform"
  #endif
  #define DCC_SIGNAL_PIN   22
  void ws281x_receive_thread();
  WS2811Client<NUM_LEDS_TO_EMULATE, GRB>* pWS2811;
#endif


#define LED_DO_PIN          6  // Pin D6 is connected to the LED stripe
#define CAN_CS_PIN          10 // Pin D10 is used as chip select for the CAN bus

#define DCC_INPSTRUCT_START 0  // Start number in the InpStructArray[]

#define SERIAL_BAUD     115200 // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program

#define DISABLE_SPI_DELAY 3000 // After this time the SPI pins are disabled if they are not used
                               // The programm also sends a command to the DCC/SX slave to deactivate his pins

#if defined(ESP32)                                                                                                  // 30.10.20: Juergen
  #define LED0_PIN    2          // Onboard Led
  #define LED1_PIN    16         // Left   Yellow LED on the mainboard
  #define LED2_PIN    14         // Middle White  LED on the mainboard
  #define LED3_PIN    0          // Right  Blue   LED on the mainboard
  #define LED4_PIN    33         // Green  HB LED between the Nanos
  #define LED5_PIN    34         // Buttons input KEYBRDx, Pin 7
  #define LED6_PIN    4          // KEY_80, Pin 8
  #define LED7_PIN    12         // KEY_80, Pin 2 AND left   button
  #define LED8_PIN    26         // KEY_80, Pin 3 AND middle button
  #define LED9_PIN    25         // KEY_80, Pin 4 AND right  button
  #define LED10_PIN   18         // KEY_80, Pin 5 (Can't be used togther with the CAN)
  #define LED11_PIN   23         // KEY_80, Pin 6          "
  #define LED12_PIN   19         // KEY_80, Pin 7          "
  #define LED13_PIN   5          // CLK Pin CAN Modul      "
  #define LED14_PIN   32         // KEY_80, Pin 11
  #define LED15_PIN   39         // KEY_80, Pin 12
  #define LED16_PIN   15         // KEYBRD, Pin 5
#elif defined(ARDUINO_RASPBERRY_PI_PICO)                                                                            // 24.04.21: Juergen
  #define LED1_PIN    13          // Left   Yellow LED on the mainboard
  #define LED2_PIN    12          // Middle White  LED on the mainboard
  #define LED3_PIN    11          // Right  Blue   LED on the mainboard
#else // Nano
  #define LED0_PIN    2          // LED Bus 2
  #define LED1_PIN    3          // Left   Yellow LED on the mainboard
  #define LED2_PIN    4          // Middle White  LED on the mainboard
  #define LED3_PIN    5          // Right  Blue   LED on the mainboard
  #define LED4_PIN    A3         // Green  HB LED between the Nanos
  #define LED5_PIN    A2         // Buttons input KEYBRDx, Pin 7
  #define LED6_PIN    A1         // KEY_80, Pin 8
  #define LED7_PIN    7          // KEY_80, Pin 2 AND left   button
  #define LED8_PIN    8          // KEY_80, Pin 3 AND middle button
  #define LED9_PIN    9          // KEY_80, Pin 4 AND right  button
  #define LED10_PIN   10         // KEY_80, Pin 5 (Can't be used togther with the CAN)
  #define LED11_PIN   11         // KEY_80, Pin 6          "
  #define LED12_PIN   12         // KEY_80, Pin 7          "
  #define LED13_PIN   13         // CLK Pin CAN Modul      "
  #define LED14_PIN   A4         // KEY_80, Pin 11
  #define LED15_PIN   A5         // KEY_80, Pin 12
  #define LED16_PIN   A0         // KEYBRD, Pin 5
#endif

/*
Abhaengig von der Konfiguration braucht die alte oder die neue Variante mehr Speicher

Sheet: MB Test 2 KEYBRD
Ohne USE_SWITCH_AND_LED_ARRAY
Der Sketch verwendet 19598 Bytes (60%) des Programmspeicherplatzes. Das Maximum sind 32256 Bytes.
Globale Variablen verwenden 672 Bytes (32%) des dynamischen Speichers, 1376 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

Mit USE_SWITCH_AND_LED_ARRAY
Der Sketch verwendet 19650 Bytes (60%) des Programmspeicherplatzes. Das Maximum sind 32256 Bytes.
Globale Variablen verwenden 672 Bytes (32%) des dynamischen Speichers, 1376 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.
                     -52

Sheet: MB Test 3 KEY_80
Ohne USE_SWITCH_AND_LED_ARRAY
Der Sketch verwendet 19914 Bytes (61%) des Programmspeicherplatzes. Das Maximum sind 32256 Bytes.
Globale Variablen verwenden 672 Bytes (32%) des dynamischen Speichers, 1376 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

Mit USE_SWITCH_AND_LED_ARRAY
Der Sketch verwendet 19892 Bytes (61%) des Programmspeicherplatzes. Das Maximum sind 32256 Bytes.
Globale Variablen verwenden 672 Bytes (32%) des dynamischen Speichers, 1376 Bytes für lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.
                     22
*/

/* Tests mit SEND_INPUTS
Deaktiviert: Sketch uses 22130 bytes (68%) of program storage space. Maximum is 32256 bytes. Global variables use 1355 bytes (66%) of dynamic memory, leaving 693 bytes for local variables. Maximum is 2048 bytes.
Aktiviert: 	 Sketch uses 22272 bytes (69%) of program storage space. Maximum is 32256 bytes. Global variables use 1355 bytes (66%) of dynamic memory, leaving 693 bytes for local variables. Maximum is 2048 bytes.
Benoetig als 142 byte
*/

#define USE_NEW_LED_ARRAY   1                                                                                 // 09.11.20:

#if defined TEST_TOGGLE_BUTTONS || defined TEST_PUSH_BUTTONS                                                  // 09.11.20:
  #error "The TEST_TOGGLE_BUTTONS and the TEST_PUSH_BUTTONS functions are no longer supported"
#endif

#ifdef READ_LDR                // Enable the darkness detection functions
  #include "Read_LDR.h"        // process the darkness sensor
  #ifndef LDR_PIN              // The LDR pin could be defined in the Prog_Generator
    #define LDR_PIN      A7    // Use A7 if the MobaLedLib "LEDs Main Module" is used
  #endif
#endif

#ifdef USE_CAN_AS_INPUT
  #ifdef ESP32
	  #include "CAN.h"			   // The Sandeep Mistry library must be installed (0.3.1)
	  
	  // START Workaround for filterExtended bug in Mistry library (0.3.1)
	  #define REG_BASE                   0x3ff6b000
	  #define REG_MOD                    0x00
	  #define REG_ACRn(n)                (0x10 + n)
	  #define REG_AMRn(n)                (0x14 + n)
	  
	  void writeRegister(uint8_t address, uint8_t value)
	  {
	    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);
	    *reg = value;
	  }
	  
	  void modifyRegister(uint8_t address, uint8_t mask, uint8_t value)
	  {
	    volatile uint32_t* reg = (volatile uint32_t*)(REG_BASE + address * 4);
	    *reg = (*reg & ~mask) | value;
	  }
	  
	  int filterExtended(long id, long mask)
	  {
	    id &= 0x1FFFFFFF;
	    mask = ~(mask & 0x1FFFFFFF);
	    modifyRegister(REG_MOD, 0x17, 0x01); // reset
	    writeRegister(REG_ACRn(0), id >> 21);
	    writeRegister(REG_ACRn(1), id >> 13);
	    writeRegister(REG_ACRn(2), id >> 5);
	    writeRegister(REG_ACRn(3), id << 5);
	    writeRegister(REG_AMRn(0), mask >> 21);
	    writeRegister(REG_AMRn(1), mask >> 13);
	    writeRegister(REG_AMRn(2), mask >> 5);
	    writeRegister(REG_AMRn(3), (mask << 5) |0x1f);
	    modifyRegister(REG_MOD, 0x17, 0x00); // normal
	    return 1;
	  }
	  // END Workaround for filterExtended bug in Mistry library (0.3.1)
	  
  #else
	  #include "mcp_can_nd.h"      // The MCP CAN library must be installed in addition if you got the error message "..fatal error: mcp_can_nd.h: No such file or directory"
								   // Here we use a local version where the debug mode is disabled (#define DEBUG_MODE 0).
								   // The original source could be downloaded from https://github.com/coryjfowler/MCP_CAN_lib
								   // It's important to disable the debug mode because the debug messages use a lot of RAM.
								   // To prevent this use the F() macro in print() functions like used here.
								   // If your program uses to much memory you get the following warning:
								   //   "Low memory available, stability problems may occur."
								   //
								   // The files are listed in the Arduino IDE, but they don't use memory if USE_CAN_AS_INPUT is disabled
	  #include <SPI.h>
	  #include "Add_Message_to_Filter_nd.h"
	  #ifndef LED_HEARTBEAT_PIN
		 #define LED_HEARTBEAT_PIN A3 // The build in LED can't be use because the pin is used as clock port for the SPI bus
	  #endif
   #endif
#else // not USE_CAN_AS_INPUT
  #ifndef LED_HEARTBEAT_PIN
    #if defined(ESP32)                                                                                              // 30.10.20: Juergen
      #define DCC_STATUS_PIN  2  // Build in LED
    #elif defined(ARDUINO_RASPBERRY_PI_PICO)                                                                        // 24.04.21: Juergen
      #define DCC_STATUS_PIN  LED_BUILTIN
    #else
      #define LED_HEARTBEAT_PIN  13 // Build in LED
    #endif
  #endif
#endif

#ifdef USE_SPI_COM                                                                                            // 13.05.20:
  #if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
  #error "USE_SPI_COM can't be used on this platform"
  #endif

  #ifdef USE_CAN_AS_INPUT
     #error "USE_CAN_AS_INPUT can't be used together with USE_CAN_AS_INPUT"
  #endif
  #ifndef LED_HEARTBEAT_PIN
    #define LED_HEARTBEAT_PIN A3 // The build in LED can't be use because the pin is used as clock port for the SPI bus
  #endif
  #include <SPI.h>
#endif // USE_SPI_COM

CRGB leds[NUM_LEDS];           // Define the array of leds
//int dcc=0; // Debug                                                                                           // 19.01.21: Juergen

#if defined(ENABLE_STORE_STATUS) && defined(_USE_STORE_STATUS)                                                // 19.05.20: Juergen
  void On_Callback(uint8_t CallbackType, uint8_t ValueId, uint8_t OldValue, uint8_t *NewValue);
#endif
#if _USE_EXT_PROC && defined(_ENABLE_EXT_PROC)                                                                    // 26.09.21: Juergen
  uint8_t Handle_Command(uint8_t Type, const uint8_t* arguments, bool process);
#endif

MobaLedLib_Prepare();
#define MobaLedLib (*pMobaLedLib)

#if defined LED_HEARTBEAT_PIN && LED_HEARTBEAT_PIN >= 0
  LED_Heartbeat_C LED_HeartBeat(LED_HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.
#endif

#if defined USE_SPI_COM || defined USE_LOCONET_INTERFACE || defined USE_COMM_INTERFACE                        // 12.11.20 Juergen use second buffer for DCC interface communication
  char Buffer[2][13] = {"",""};
#else
  char Buffer[1][13] = {""};
#endif

#ifdef ESP32                                                                                                  // 30.10.20: Juergen
  TaskHandle_t  MLLTaskHnd ;
  #include "esp_log.h"
#endif

#if defined USE_COMM_INTERFACE || defined USE_LOCONET_INTERFACE
InMemoryStream stream(256);
#endif
#if defined USE_COMM_INTERFACE 
CommInterface* commInterface;
#endif

bool Send_Disable_Pin_Active = 1;                                                                             // 13.05.20:


#if defined(ENABLE_STORE_STATUS) && defined(_USE_STORE_STATUS)                                                // 19.05.20:  New feature from Juergen
    #define EEPROM_START 0
    void StoreStatus(uint16_t EEPromAddr, uint8_t status);                                                    // 01.05.20:
#endif

// Forward defines to help the Arduino builder. They are important for the fast build of the ESP32
void Proc_Color_Cmd(const char *Buffer);
void Receive_LED_Color_per_RS232();
void Update_InputCh_if_Addr_exists(uint16_t ReceivedAddr, uint8_t Direction, uint8_t OutputPower);
bool Get_Char_from_RS232_or_SPI(char &c, uint8_t &Buff_Nr);
void Proc_Received_Char();
void Debug_Print_Fuses();
void setup();
void Set_Mainboard_LEDs();
void loop();
void MLLTask( void * parameter );
void MLLMainLoop();


#ifdef RECEIVE_LED_COLOR_PER_RS232
//-------------------------------------
void Proc_Color_Cmd(const char *Buffer)                                                                       // 03.11.19:
//-------------------------------------
{
  int Led, r, g, b, Leds, cnt;
  // #Lll rr gg bb nn
  if ((cnt = sscanf(Buffer, "L %x %x %x %x %x", &Led, &r, &g, &b, &Leds)) == 5)
       {
       //Serial.print("#L:"); Serial.print(Led);Serial.print(" r="); Serial.print(r); Serial.print(" g="); Serial.print(g); Serial.print(" b="); Serial.print(g); Serial.print(" Leds="); Serial.println(Leds);
       for (int i = Led; i < NUM_LEDS && i < Led+Leds; i++)    // 20.01.21:  Old: "i <= 255 && ..": Prevent crash if "Alle LEDs aus" is pressed in Farbtest.
           {
           leds[i].r = r;
           leds[i].g = g;
           leds[i].b = b;
           }
       FastLED.show();                       // Show the LEDs (send the leds[] array to the LED stripe)
       }
  else { Serial.print(F("#Wrong Cnt:'")); Serial.print(Buffer); Serial.print(F("':")); Serial.println(cnt); }

  //Serial.println(Buffer); //
}

void (* ResetArduino)(void) = 0; // Restart the Arduino
#ifdef ESP32                                                                                                  // 30.10.20: Juergen
  void MLLTask(void* parameter);
  void MLLMainLoop();
#endif

//--------------------------------
void Receive_LED_Color_per_RS232()                                                                            // 03.11.19:
//--------------------------------
{
#ifdef SEND_DISABLE_PIN
  if (Send_Disable_Pin_Active) digitalWrite(SEND_DISABLE_PIN, 1); // Tell the DCC Arduino to be quiet         // 13.05.20:  Added: if (Send_Disable_Pin_Active)
#endif
  char Buffer[20] = "";
#ifndef LEDS_PER_CHANNEL							 
  uint8_t JustStarted = 1;
#endif
  while (1)
     {
     #if defined LED_HEARTBEAT_PIN && LED_HEARTBEAT_PIN >= 0                                                  // 13.05.20:
       LED_HeartBeat.Update(300); // Fast Flash
     #endif
     if (Serial.available() > 0)
        {
        char c = Serial.read();
        switch (c)
           {
           case '#': *Buffer = '\0'; break;
           case '\n': // Proc buffer       (For tests with the serial console of Arduino use "Neue Zeile" and not "..(CR)" )
#ifndef LEDS_PER_CHANNEL							 
                      if (JustStarted)
                           JustStarted = 0;
                      else 
#endif					  
					       {
                           switch (*Buffer)
                             {
#ifdef LEDS_PER_CHANNEL							 
							 case '?': 
//                       char s[100]; sprintf(s, "#?%s%s", START_MSG,LEDS_PER_CHANNEL); Serial.println(s);  // Debug
                             
                             Serial.print("#?");															   // 13.03.21 Juergen inform farbtest about led count per channel	
										Serial.print(START_MSG);
										 Serial.println(LEDS_PER_CHANNEL);
										break;
                             case 'X': return;						                                                   // 13.03.21 Juergen stop Receive_LED_Color_per_RS232 without rebooting
#else						
                             case 'E': ResetArduino(); // Restart the Arduino
#endif
                             case 'L': Proc_Color_Cmd(Buffer); break;
                             default:  Serial.print(F("#Unknown cmd:'")); Serial.print(Buffer); Serial.println(F("'"));// Debug
                             }
                           }
                      break;
           default:   { // Add character to Buffer
                      uint8_t len = strlen(Buffer);
                      if (len < sizeof(Buffer)-1)
                           {
                           Buffer[len++] = c;
                           Buffer[len]   = '\0';
                           }
                      else {
                           *Buffer = '\0';
                           Serial.println(F("Buffer overflow")); // Debug
                           }
                      }
           }
        }
     #if defined(USE_DMX_PIN)                                                                                 // 20.01.21:
         dmxInterface.loop();
     #endif
#ifdef ESP32
         esp_task_wdt_reset();                                                                                    // 05.03.21: Juergen -  reset watchdog timer, because we are in an endless loop
         vTaskDelay(10);
         yield();
#endif
     }
}
#endif // RECEIVE_LED_COLOR_PER_RS232

#ifdef USE_EXT_ADDR
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


   //-----------------------------------------------------------------------------------------------
   void Update_InputCh_if_Addr_exists(uint16_t ReceivedAddr, uint8_t Direction, uint8_t OutputPower)
   //-----------------------------------------------------------------------------------------------
   {
     //Serial.print(F("Update_InputCh_if_Addr_exists ")); Serial.println(ReceivedAddr); // Debug
     //if (ReceivedAddr<1000) dcc++;  // Debug                                                                  // 19.01.21: Juergen
     uint16_t Channel = DCC_INPSTRUCT_START;
     for (const uint8_t *p = (const uint8_t*)Ext_Addr, *e = p + sizeof(Ext_Addr); p < e; )
         {
         uint16_t Raw_Addr = pgm_read_word_near(p); p+=2;
         uint16_t Addr     = Raw_Addr & ADDR_MSK;
         uint16_t InTyp    = Raw_Addr & ~ADDR_MSK;
         uint8_t  InCnt    = pgm_read_byte_near(p); p++;
         //Serial.print("Addr:"); Serial.print(Addr); Serial.print(" InTyp:"); Serial.print(InTyp,HEX); Serial.print(" InCnt:"); Serial.println(InCnt); // Debug
         uint16_t ChkBut = InTyp;

         for (uint8_t In = 0; ; )
             {
             //Serial.print(In); Serial.print(" "); // Debug
             //Serial.print("In="); Serial.print(In); Serial.print(" ChkBut="); Serial.print(ChkBut,HEX); Serial.print(" Dir "); Serial.println(Direction); // Debug
             if (Addr == ReceivedAddr) // Addr is matching
                  {
                  if (ChkBut == S_ONOFF) // OnOff switch
                       {
#ifdef COMMANDS_DEBUG					   
                       char s[48]; sprintf(s, "OnOff: Addr %i Channel[%i]=%i Pow=%i", Addr, Channel, Direction, OutputPower); Serial.println(s);  // Debug
#endif					   
                       if (OutputPower > 0)
                            {
                            MobaLedLib.Set_Input(Channel, Direction > 0);
                            }
                       return ;
                       }
                  else { // Push Button
                       #if TWO_BUTTONS_PER_ADDRESS // DCC and CAN use the direction to distinguish between "Red" and "Green"
                          if (   (ChkBut == B_RED   && Direction == 0)
                              || (ChkBut == B_GREEN && Direction >  0))
                       #else
                             // If one button per address is used every change generates a button press event
                             // => In this case no "if()" is needed
                       #endif
                             {
#ifdef COMMANDS_DEBUG					   
                             char s[48]; sprintf(s, "%s Button Addr %i: Channel[%i]=%i", ChkBut==B_RED?"Red":"Green", Addr, Channel,OutputPower); Serial.println(s); // Debug
#endif						
                             MobaLedLib.Set_Input(Channel, OutputPower);
                             #ifdef GEN_BUTTON_RELEASE
                               if (OutputPower)
                                  {
                                  if (LastTime && LastChannel != Channel) MobaLedLib.Set_Input(LastChannel, 0); // Send release
                                  LastTime = millis();
                                  LastChannel = Channel;
                                  }
                             #endif
                             return ;
                             }
                       }
                  }
             // Addr dosn't match => Check the next possible address / InTyp for this entry
             //Serial.print("Don't match In="); Serial.println(In); // Debug
             Channel++;
             In++;
             if (In < InCnt)
                {
                #if TWO_BUTTONS_PER_ADDRESS
                   if (ChkBut == S_ONOFF) Addr++;
                   else if (ChkBut == B_RED)    ChkBut = B_GREEN;
                   else if (ChkBut == B_GREEN) {ChkBut = B_RED;   Addr++; }
                #else
                   Addr++;
                #endif
                }
             else break; // Exit the for loop
             }
         }
#ifdef COMMANDS_DEBUG					   
     char s[35];sprintf(s, "Addr %i not found Dir:%i", ReceivedAddr, Direction); Serial.println(s);  // Debug
#endif	 
   }

#endif // USE_EXT_ADDR

     
#if defined(_ENABLE_EXT_PROC) && _USE_EXT_PROC                                                                     // 26.09.21: Juergen
   

uint8_t Handle_Command(uint8_t Type, const uint8_t* arguments, bool process)
{
  //{ char s[80]; sprintf(s, "Handle Command type %d argumentsMem %d.", Type, arguments); Serial.println(s); Serial.flush();} // Debug
  
#ifdef _ENABLE_EXT_PROC
  if (Type==SOUND_CHANNEL_TYPE_T) return soundProcessor.handle(arguments, process);
#endif
  return 0;
}
#endif

#if defined(ENABLE_STORE_STATUS) && defined(_USE_STORE_STATUS)                                                     // 19.05.20: Juergen
// if function returns TRUE the calling loop stops

   //-----------------------------------------------------------------------------------------------
   void ForAllStoreValues(uint8_t ValueType, uint8_t ValueId, uint8_t* Value, HandleValue_t handler)
   //-----------------------------------------------------------------------------------------------
   {
     uint16_t EEPromAddr = EEPROM_START;
     uint16_t tmp;
     for (const uint8_t *p = (const uint8_t*)Store_Values, *e = p + sizeof(Store_Values); p < e; )
         {
         tmp = pgm_read_word_near(p); p += 2;
         if (handler(ValueType, ValueId, Value, EEPromAddr, tmp>>8, tmp)) break;
         EEPromAddr++;
         }
   }

   //--------------------------------------------------------------------------------------------------------------------------------------
   bool Handle_Callback(uint8_t CallbackType, uint8_t ValueId, uint8_t* Value, uint16_t EEPromAddr, uint8_t TargetValueId, uint8_t Options)
   //--------------------------------------------------------------------------------------------------------------------------------------
   {
     #if defined(DEBUG_STORE_STATUS) && 1
       { char s[80]; sprintf(s, "Handle callback type %d for %d. %d@EEAdr %d=%d options=0x%02X", CallbackType, ValueId, TargetValueId, EEPromAddr, *Value, Options); Serial.println(s); Serial.flush();} // Debug
     #endif
     if (CallbackType == CT_COUNTER_CHANGED || CallbackType == CT_COUNTER_INITIAL)
        {
        if ((Options & IS_COUNTER) != IS_COUNTER) return false;
        if (ValueId != TargetValueId) return false;
        if (CallbackType == CT_COUNTER_INITIAL)
           {
#ifndef ESP32
           eeprom_busy_wait();
           *Value = eeprom_read_byte((const uint8_t*)EEPromAddr);
#else
           *Value = EEPROM.read(EEPromAddr);
#endif
           #if defined(DEBUG_STORE_STATUS) && 1
             { char s[80]; sprintf(s, "Initialize Counter %d@EEAdr %d=%d", ValueId, EEPromAddr, *Value); Serial.println(s); Serial.flush();} // Debug
           #endif
           }

        #if defined(DEBUG_STORE_STATUS) && 1
          { char s[80]; sprintf(s, "Store Counter %d@EEAdr %d=%d", ValueId, EEPromAddr, *Value); Serial.println(s); Serial.flush();} // Debug
        #endif
        StoreStatus(EEPromAddr, *Value);
        return true;
        }
     if (CallbackType == CT_CHANNEL_CHANGED)
        {
        bool IsToggle = (Options & IS_PULSE) != IS_PULSE;
        uint8_t InCnt = Options & InCnt_MSK;
        #if defined(DEBUG_STORE_STATUS) && 0
           Serial.print("Store Channel:"); Serial.print(StoreChannel); // Debug
           Serial.print(" IsToggle:");     Serial.print(IsToggle);     // Debug
           Serial.print(" InCnt:");        Serial.print(InCnt);        // Debug
           Serial.print(" Tmp:");          Serial.println(tmp);        // Debug
        #endif
        if (ValueId < TargetValueId || ValueId >= TargetValueId + InCnt) return false;
        uint8_t status = 0;

        if (IsToggle)
           {
           for (uint8_t cnt = InCnt; cnt > 0;)
               {
               cnt--;
               byte tmp = MobaLedLib.Get_Input(TargetValueId + cnt);
               tmp = (tmp == INP_ON || tmp == INP_TURNED_ON) ? 1 : 0;
               #if defined(DEBUG_STORE_STATUS) && 0
                   char s[80]; sprintf(s, "State of InCh %d=%d", InCh+cnt, tmp); Serial.println(s);  // Debug
               #endif
               status = (status << 1) + tmp;
               }
           #if defined(DEBUG_STORE_STATUS) && 0
               { char s[80]; sprintf(s, "New OnOff State for InCh %d@EEAdr %d=%d", InCh, EEPromAddr, status); Serial.println(s); Serial.flush();} // Debug
           #endif
           }
        else // pulse type
           {
           if (*Value != INP_ON && *Value != INP_TURNED_ON) return false;
           status = ValueId - TargetValueId;
           #if defined(DEBUG_STORE_STATUS) && 0
               { char s[80]; sprintf(s, "New Button State for InCh %d@EEAdr %d=%d", InCh, EEPromAddr, status); Serial.println(s); Serial.flush();} // Debug
           #endif
           }
        StoreStatus(EEPromAddr, status);
        return true;
        }

     return false;
   }

   //------------------------------------------------------------------------------------------
   void On_Callback(uint8_t CallbackType, uint8_t ValueId, uint8_t OldValue, uint8_t *NewValue)
   //------------------------------------------------------------------------------------------
   {
     if (     (CallbackType == CT_CHANNEL_CHANGED && ((OldValue > 0) != (*NewValue > 0)))
           || (CallbackType == CT_COUNTER_CHANGED &&  (OldValue != *NewValue))
           ||  CallbackType == CT_COUNTER_INITIAL)
        {
        #if defined(DEBUG_STORE_STATUS) && 1
           if (CallbackType == CT_CHANNEL_CHANGED)
              { char s[80]; sprintf(s, "On_Value_Changed: channel %d changed from %d to %d", ValueId, OldValue, *NewValue); Serial.println(s); Serial.flush();} // Debug
           else if (CallbackType == CT_COUNTER_CHANGED)
                   { char s[80]; sprintf(s, "On_Value_Changed: counter %d changed from %d to %d", ValueId, OldValue, *NewValue); Serial.println(s); Serial.flush();} // Debug
        #endif
        ForAllStoreValues(CallbackType, ValueId, NewValue, Handle_Callback);
        #if defined(DEBUG_STORE_STATUS) && 1
           if (CallbackType == CT_COUNTER_INITIAL)
              {
              char s[80]; sprintf(s, "On_Counter_Init: counter %d set to %d", ValueId, *NewValue); Serial.println(s);
              } // Debug
        #endif
        }
   };

   //-----------------------------------------------------------------------------------------------------------------------------------------
   bool Handle_Restore_Status(uint8_t ValueType, uint8_t ValueId, uint8_t* Value, uint16_t EEPromAddr, uint8_t TargetValueId, uint8_t Options)
   //-----------------------------------------------------------------------------------------------------------------------------------------
   {
     #if defined(DEBUG_STORE_STATUS) && 1
       { char s[80]; sprintf(s, " restore state for %d@EEAdr %d, options=0x%02X", TargetValueId, EEPromAddr, Options); Serial.println(s); Serial.flush();} // Debug
     #endif
     if ((Options & IS_COUNTER) == IS_COUNTER) return false;

#ifndef ESP32
     eeprom_busy_wait();
     uint8_t status = eeprom_read_byte((const uint8_t*)EEPromAddr);
#else
	 uint8_t status = EEPROM.read(EEPromAddr);
#endif
     uint8_t InCnt = Options & InCnt_MSK;
     if (status > (1 << InCnt))
        {
        #if defined(DEBUG_STORE_STATUS) && 1
          { char s[80]; sprintf(s, " don't restore state, stored status for InCh %d@EEAdr %d=%d is invalid", TargetValueId, EEPromAddr, status); Serial.println(s); Serial.flush();} // Debug
        #endif
        return false;
        }
     #if defined(DEBUG_STORE_STATUS) && 1
       { char s[80]; sprintf(s, " stored status for InCh %d@EEAdr %d=%d", TargetValueId, EEPromAddr, status); Serial.println(s); Serial.flush();} // Debug
     #endif
     if ((Options & IS_PULSE) != IS_PULSE)
          {
          for (uint8_t cnt = 0; cnt < InCnt; cnt++)
              {
              MobaLedLib.Set_Input(TargetValueId + cnt, status & 0x01);
              status = status >> 1;
              }
          }
     else {
          MobaLedLib.Set_Input(TargetValueId + status, 1);
          }
     return false;
   }

   //-----------------------------------------------------------------------------------------------
   void RestoreStatus()
   //-----------------------------------------------------------------------------------------------
   {
     #if defined(DEBUG_STORE_STATUS) && 1
       Serial.println("Restore last state begin"); // Debug
     #endif
     ForAllStoreValues(0, 0, 0, Handle_Restore_Status);
     #if defined(DEBUG_STORE_STATUS) && 1
       Serial.println("Restore last state done"); // Debug
     #endif
   }

   //------------------------------------------------------------------------------------------------------------------------------------
   void StoreStatus(uint16_t EEPromAddr, uint8_t status) // 17.04.20:
   //------------------------------------------------------------------------------------------------------------------------------------
   {
     #if defined(DEBUG_STORE_STATUS) && 0
       { char s[80];sprintf(s, "store status for EEAdr %i value:%i", EEPromAddr, value); Serial.println(s);}  // Debug
	#endif
#ifndef ESP32
     uint8_t eeValue = eeprom_read_byte((const uint8_t*)EEPromAddr);
#else
	 uint8_t eeValue = EEPROM.read(EEPromAddr);
#endif
     #if defined(DEBUG_STORE_STATUS) && 0
       { char s[80]; sprintf(s, " read ActualVal for EEAdr%i=%d", EEPromAddr, eeValue); Serial.println(s); }  // Debug
     #endif
     if (eeValue != status)
        {
        #if defined(DEBUG_STORE_STATUS) && 1
           { char s[80]; sprintf(s, " updating status for EEAdr %d=%d", EEPromAddr, status); Serial.println(s); Serial.flush();} // Debug
        #endif
#ifndef ESP32
        eeprom_busy_wait();
        eeprom_write_byte((uint8_t*)EEPromAddr, status);
#else
        EEPROM.write(EEPromAddr, status);
		EEPROM.commit();
#endif
        }
   }
#endif // ENABLE_STORE_STATUS                                                                                 // 01.05.20:


#if defined USE_RS232_OR_SPI_AS_INPUT

#if defined SEND_INPUTS
	#if defined START_SWITCHES_1
		#define START_SEND_CHANNEL START_SWITCHES_1
		#ifdef START_SWITCHES_2
			#define INCH_CNT START_SWITCHES_2+8-START_SWITCHES_1													// no expicit define in Leds_Autoprog.h -> in the meantime take this as a workaround
		#else
			#define TMP_INCH_CNT 32
		#endif
	#else
		#if defined START_SWITCHES_2
			#define START_SEND_CHANNEL START_SWITCHES_2
			#define INCH_CNT 8																			// align to byte size
		#else
			#define START_SEND_CHANNEL 0
			#define INCH_CNT 0
		#endif
	#endif
                                                                                                        // 12.11.21 Juergen remove debug output
//	#define STRINGIFY(s) XSTRINGIFY(s)
//	#define XSTRINGIFY(s) #s
//	#pragma message ("INCH_CNT=" STRINGIFY(INCH_CNT))
//	#pragma message ("START_SEND_CHANNEL=" STRINGIFY(START_SEND_CHANNEL))

 	void Send_Inputs(char type, bool forceSend)
	{
		#if INCH_CNT!= 0
			#define SEND_LEN ((uint8_t)((INCH_CNT+6)/7))+1														// add one byte for checksum
			uint8_t outByte = 0;
			uint8_t input;
			uint8_t chkSum = (uint8_t)~(SEND_LEN^0xD0);												        // compiler will make correct value out of it (saves memory)
			for (uint8_t i=0;i<INCH_CNT &&!forceSend;i++) {
				input = MobaLedLib.Get_Input(START_SEND_CHANNEL+i);
				forceSend |= (input==INP_TURNED_ON);
				forceSend |= (input==INP_TURNED_OFF);
			}
			if (!forceSend) return;

			Serial.write(0xD0);
			Serial.write(SEND_LEN);
			for (uint8_t i=0;i<INCH_CNT;i++) {																	// align to 7 bits

				input = MobaLedLib.Get_Input(START_SEND_CHANNEL+i);
				outByte = outByte<<1;
				if ((input == INP_ON || input == INP_TURNED_ON))
					outByte |= 0x01;
				if ((i%7)==6 || i==(INCH_CNT-1)) {																// every 7 bits or with the last bit
					//if (outByte<16)Serial.print("0");
					//Serial.print(outByte,HEX);
					Serial.write(outByte);
					chkSum ^= outByte;
					outByte=0;
				}
			}
			Serial.write(chkSum);		//checksum
		#endif
	}
#endif
   //----------------------------
   void Proc_Buffer(char *Buffer)                                                                             // 13.05.20:  Added: Buffer
   //----------------------------
   {
     uint16_t Addr, Direction, OutputPower, State;
#if defined COMMANDS_DEBUG
     Serial.print("Buffer received:"); Serial.println(Buffer); // Debug
#endif
     switch (*Buffer)
       {
     //case '@': if (sscanf(Buffer+1, "%i %x %x", &Addr, &Direction, &OutputPower) == 3)                      // 28.11.20: Old
       case '@': if (sscanf(Buffer+1, "%" SCNu16 " %" SCNu16 " %" SCNu16, &Addr, &Direction, &OutputPower) == 3)
                    {
                    Update_InputCh_if_Addr_exists(Addr+ADDR_OFFSET, Direction, OutputPower);                  // 26.09.19:  Added: ADDR_OFFSET
                    return ;
                    }
                 break;
     //case '$': if (sscanf(Buffer+1, "%i %x", &Addr, &State) == 2)                                           // 28.11.20: Old
       case '$': if (sscanf(Buffer+1, "%" SCNu16 " %" SCNu16, &Addr, &State) == 2)
                    {
                    // Not implemented yet
                    return;
                    }
                 break;
#if defined SEND_INPUTS
       case '?': Send_Inputs(Buffer[1], true);
                 return;
#endif
       }
     Serial.println(" Parse Error");
   }
#endif

#ifdef USE_SPI_COM                                                                                            // 13.05.20:
  /*
  SPI Kommunikation:
  - wird parallel zur RS232 Kommunikation benutzt, weil:
    - es sein kann, dass der Jumper J13 nicht verbunden wurde
    - die LED Kommandos von Windows weiterhin per RS232 kommen (Excel/Farbtest/...)
  - Wenn der SPI Modus funktioniert, dann wird die A1 Leitung deaktiviert.
    Das wird auf beiden Arduinos gemacht damit die Leitung fuer andere Zwecke genutzt werden kann
    Zu Programmstart schickt der LED Arduino eine 6 (ACK) und eine 0 zum DCC Arduino. Dieser
    Antwortet auf das zweite Zeichen mit 0xF9. Daran erkennt man, das die richtige Software
    auf dem anderen Arduino laeuft.

  - Beim SPI Bus werden gesendete und empfangene Daten gleichzeitig uebertragen.
    Der Master schickt ein Byte und gleichzeitig wird ein Byte vom Slave empfangen.
    => Der Slave muss das erste zu Sendende Byte bereits im SPDR Register ablegen bevor der Master
      die erste Anfrage schickt.

  - Das SPI Modul kann unabhaengig von USE_RS232_OR_SPI_AS_INPUT und RECEIVE_LED_COLOR_PER_RS232 aktiviert werden
    damit man dem DCC/Selectrix Nano sagen kann, dass er die A1 Leitung Leitungen freigeben soll
    damit diese im LED Programm benutzt werden koennen. Dazu schickt der LED Arduino das Byte 6 an
    den DCC/Selectrix Nano.

  - Automatische deaktivierung der SPI und der A1 Leitung:
    Der DCC/Selectrix Nano deaktiviert seine SPI Leitungen, wenn er drei Sekunden lang
    keine SPI Anfragen bekommt damit die Pins am DCC Arduino fuer die Schalter genutzt
    werden koennen.
    Der SPI Mode wird wieder aktiviert, wenn High am Pin 13 erkannt wird. Das passiert beim Booten
    des LED Arduinos automatisch. Beim neuen Bootloader blinkt die LED 3 mal mit 125 ms Periodendauer.
    Beim alten Bootloader ist sie einmal fuer 100 ms an. Aber das ist nur dann so wenn das LED Programm
    darauf installiert ist wenn das "BareMinimum" Programm installiert wird, dann blinkt nichts ?!?

  - Wenn Der DCC/Selectrix Nano einmal das SPI Kommando Nr 6 empfangen hat wird der A1 Pin deaktiviert.

  - Wenn der SPI Mode aktiv ist, dann koennen die eingebauten LEDs 'L' an D13 der Arduinos nicht benutzt
    werden weil sie gleichzeitig leuchten wuerden.

  Die SPI Kommunikation wird momentan nicht verwendet weil das TX LED Problem auch ueber einen 3.9K Widerstang
  der gehen Masse geschaltet ist loesen kann. => 3 Tage Arbeit fuer die Katz ;-(
  */

  //----------------
  void Disable_SPI()
  //----------------
  {
    SPI.end();
    pinMode(SS, INPUT);
    pinMode(11, INPUT);  // SPI SI pin
    pinMode(13, INPUT);  // SPI CLK pin
  }

  //---------------------------------------------------
  void Check_SPI_Slave(uint8_t Disable_SPI_if_Detected)
  //---------------------------------------------------
  // Check if the correct SPI slave is available
  // The check is performed every 200ms until a slave is detected
  // If Disable_SPI_if_Detected is set to 1 the SPI comunication in the LED Arduino is disabled
  {
    if (Send_Disable_Pin_Active)
       {
       static uint8_t LastCheck = 0;
       if ((uint8_t)((millis() & 0xFF) - LastCheck) > 200)
          {
          LastCheck = millis() & 0xFF;
          Read_SPI_Char(6); //+Disable_SPI_if_Detected); // The Slave will send 0xF9/0xFAwith the next request
          if (Read_SPI_Char(0) == 0xF9) //+Disable_SPI_if_Detected)
             {
             Serial.println(F("Disabled A1 function")); // Debug
             Send_Disable_Pin_Active = 0;
             pinMode(SEND_DISABLE_PIN, INPUT);
             if (Disable_SPI_if_Detected) Disable_SPI();
             }
          }
       }
  }

  //-------------------------------------
  uint8_t Read_SPI_Char(uint8_t SendChar)
  //-------------------------------------
  {
    //digitalWrite(SS, LOW);   // Starts communication with the SPI slave (DCC/Selectrix Arduino)                      // 12.11.20: Moved down
    delayMicroseconds(10);                 // Das ist noetig, weil sonst zeichen verloren gehen
    //if (Send_Disable_Pin_Active == 0)    // RS232 communication is disabled => A valid SPI answer has been receifed in Check_SPI_Slave()
         return SPI.transfer(SendChar);    // Send 0 to slave and receives value from slave
    //else return 0;
  }
#endif // USE_SPI_COM

#if defined USE_RS232_OR_SPI_AS_INPUT || defined RECEIVE_LED_COLOR_PER_RS232
      //--------------------------------------------------------
      bool Get_Char_from_RS232_or_SPI(char &c, uint8_t &Buff_Nr)
      //--------------------------------------------------------
      {
        //PORTB &= ~3; // Disable D8 & D9   Debug Speed Test
        //DDRB  |=  3; // OUTPUT
        //PORTB |=  1;

	#if defined USE_RS232_OR_SPI_AS_INPUT && defined USE_SPI_COM	// USE_SPI_COM is enabled if the DCC/SX arduino also uses SPI,
																																// but without DCC commands the USE_RS232_OR_SPI_AS_INPUT is not active
																																// In this case the DCC Arduino disables the SPI communication
																																// to be able to use the pins in the LED Arduino for switches

	c = Read_SPI_Char(0);
	if (c)
		{
		Buff_Nr = 1;
		//PORTB |= 3;        // Debug Speed Test
		return 1;
		}
	#endif

#if defined USE_COMM_INTERFACE || defined USE_LOCONET_INTERFACE                                                // Juergen: get Data from DCCInterface
    if (stream.available())
    {
      Buff_Nr = 1;                                                                                             // Juergen: ESP32 doesn't use SPI buffer, so re-use Buffe1
      c = stream.read();
      return 1;
    }
#endif

	if (Serial.available() > 0)
		{
		Buff_Nr = 0;
		c = Serial.read();
		//PORTB |= 3;   // Debug Speed Test
		return 1;
		}
		//PORTB &= ~3;  // Debug Speed Test
		return 0;
	}

   //-----------------------
   void Proc_Received_Char()
   //-----------------------
   // Receive DCC Accessory Turnout messages:
   //  "@ 319 00 01\n":  Addr, Direction, OutputPower
   //
   // and DCC Signal Output messages:
   //  "$ 123 00\n":     Addr, State
   // For tests with the serial monitor use "Neue Zeile" instead of "Zeilenumbruch (CR)"
   {
     #ifdef GEN_BUTTON_RELEASE                                                                                // 23.05.19:
       if (LastTime && millis()-LastTime > 400) // Use 1100 if no repeat is wanted
          {
          MobaLedLib.Set_Input(LastChannel, 0); // Send release
          LastTime = 0;
          // Serial.print(F("Release Button Channel[")); Serial.print(LastChannel); Serial.println("]=0"); // Debug
          }
     #endif

     char c;
     uint8_t Buff_Nr;
     while (Get_Char_from_RS232_or_SPI(c, Buff_Nr))
       {
       //Serial.print(c); // Debug
       switch (c)
          {
          #if defined RECEIVE_LED_COLOR_PER_RS232
             case '#': if (Buff_Nr==0) Receive_LED_Color_per_RS232(); // The first '#' is used to enable the serial color change mode  // 03.11.19:  // 13.05.20:  Added: if (Buff_Nr==0)
                       break;
          #endif
          #if defined USE_RS232_OR_SPI_AS_INPUT
            case '@':
#if defined SEND_INPUTS                                                                                       // 28.11.20: Juergen
            case '?':
#endif
            case '$': if (*Buffer[Buff_Nr]) Serial.println(F(" Unterm. Error"));                              // 13.05.20:  Added [Buff_Nr] to all Buffer usages in this and the following lines
                      *Buffer[Buff_Nr] = c; Buffer[Buff_Nr][1] = '\0';
                      break;
            default:  // Other characters
                      uint8_t ExpLen;
                      switch(*Buffer[Buff_Nr])
                         {
                         case '@': ExpLen = 11; break;
                         case '$': ExpLen =  8; break;
#if defined SEND_INPUTS                                                                                       // 28.11.20: Juergen
                         case '?': ExpLen =  2; break;
#endif
                         default : ExpLen =  0;
                         //Serial.print(c); // Enable this line to show status messages from the DCC-Arduino
                         }
                      if (ExpLen)
                         {
                         uint8_t len = strlen(Buffer[Buff_Nr]);
                         if (len < ExpLen)
                              {
                              Buffer[Buff_Nr][len++] = c;
                              Buffer[Buff_Nr][len]   = '\0';
                              }
                         else {
                              if (c != '\n')
                                   Serial.println(F(" Length Error"));
                              else Proc_Buffer(Buffer[Buff_Nr]);
                              *Buffer[Buff_Nr] = '\0';
                              }
                         }
          #endif // USE_RS232_OR_SPI_AS_INPUT
          }
       }
   }
#endif // USE_RS232_OR_SPI_AS_INPUT || RECEIVE_LED_COLOR_PER_RS232

#ifdef USE_CAN_AS_INPUT
   #ifndef ESP32
	MCP_CAN  CAN(CAN_CS_PIN);
   #endif
   bool CAN_ok = false;

   //-----------------------------------
   void Proc_Accessory(uint8_t *rxBuf)
   //-----------------------------------
   // Process accessory CAN messages
   // See: https://www.maerklin.de/fileadmin/media/produkte/CS2_can-protokoll_1-0.pdf
   {
     uint8_t Pos     = rxBuf[4];  // 0 = Aus, Rund, Rot, Rechts, HP0 / 1 = Ein, Gruen, Gerade, HP1 / 2 = Gelb, Links, HP2 / 3 = Weiss, SH0
     uint8_t Current = rxBuf[5];  // 0 = Ausschalten, 1-31 Einschalten mit Dimmwert (sofern Protokoll dies unterstuetzt), 1 = ein
     uint16_t Loc_ID = ((rxBuf[2]<<8) + rxBuf[3]) & 0x7FF;  // Adresses from 0 - 0x7FF are possible (2048 adresses)
     //Serial.print("Loc_ID:"); Serial.print(Loc_ID+1); Serial.print(" Pos:"); Serial.print(Pos); Serial.print(" Current:"); Serial.println(Current); // Debug
     Update_InputCh_if_Addr_exists(Loc_ID+1, Pos, Current);
   }

   //----------------
   void Process_CAN()
   //----------------
   {
   uint8_t  len;
   uint32_t rxId;
   uint8_t rxBuf[8];
   #ifdef ESP32
      if (CAN_ok)
		{
        if ((len=CAN.parsePacket())>0)
          {
          //Serial.print("packet with id 0x");
          //Serial.print(CAN.packetId(), HEX);
          rxId = CAN.packetId();
          CAN.readBytes(rxBuf, len);
   #else
   if (CAN_ok && CAN.checkReceive() == CAN_MSGAVAIL)
      {
      if ( CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
         {
   #endif
#ifdef COMMANDS_DEBUG
         Serial.print("CAN: "); Serial.print(rxId,HEX); Serial.print(" Len="); Serial.print(len); // Debug
		 for (uint8_t i=0;i<len;i++) 
		 {
			if (i==0) Serial.print(" Data ");
			if (rxBuf[i]<16) Serial.print("0");
			Serial.print(rxBuf[i], HEX);
			Serial.print(" ");
		 }
		 Serial.println();
#endif
         if (((rxId>>16) & 0xFF) == 0x16) Proc_Accessory(rxBuf); // Check if it's a accessory message
         }
      }
   }
#endif // USE_CAN_AS_INPUT

#if !defined(ESP32) && !defined(ARDUINO_AVR_NANO_EVERY) && !defined(ARDUINO_RASPBERRY_PI_PICO)                // 19.01.21: Juergen: Added Every
  #include <avr/boot.h>
  //----------------------
  void Debug_Print_Fuses()                                                                                    // 29.10.20:
  //----------------------
  // https://arduino.stackexchange.com/questions/24859/how-do-i-read-the-fuse-bits-from-within-my-sketch
  {
    cli();
    uint8_t highBits = boot_lock_fuse_bits_get(GET_HIGH_FUSE_BITS);
    sei();
    Serial.print(F("HFuse:")); Serial.println(highBits, HEX);
  }
#endif

#ifdef USE_ESP32_EXTENSIONS
#include "ESP32Extensions.h"
#endif

//#include "Eigene_Ergaenzungen.h"

//-----------
void setup(){
//-----------
  Serial.begin(SERIAL_BAUD); // Communication with the DCC-Arduino must be fast
#ifdef ESP32
  if (!EEPROM.begin(EEPROM_SIZE))                                                                             // 19.01.21: Juergen: Old: 100
  {
    Serial.println("failed to initialise EEPROM");
  }
  esp_log_level_set("*", ESP_LOG_NONE);
#endif
  MobaLedLibPtr_CreateEx(leds
#if _USE_STORE_STATUS
  #if defined(ENABLE_STORE_STATUS)                                                                             // 19.05.20: Juergen
    , On_Callback
  #else
    , NULL
  #endif
#endif
#if _USE_EXT_PROC                                                                                              // 26.09.21: Juergen
  #if defined(_ENABLE_EXT_PROC)
    , Handle_Command
  #else
    , NULL
  #endif
#endif
    );
  #ifdef SETUP_FASTLED // Use a special FastLED Setup macro defined in the LEDs_AutoProg.h                    // 26.04.20:
    SETUP_FASTLED();

    #ifdef COLOR_CORRECTION                                                                                   // 17.04.20:  New feature from Juergen
      controller0.setCorrection(COLOR_CORRECTION); // Attention: Can't be used with Servos, Sound Modules, Charliplexing, ...
    #endif
  #else
	#ifdef USE_WS2811                                                                                     // 19.01.21: Juergen
		CLEDController& controller = FastLED.addLeds<WS2811, LED_DO_PIN, RGB>(leds, NUM_LEDS); // Initialize the FastLED library
	#else
		CLEDController& controller = FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library
	#endif

    controller.clearLeds(256);                                                                                // 13.04.20: Uses only 12 Bytes!

    #ifdef COLOR_CORRECTION                                                                                   // 17.04.20:  New feature from Juergen
      controller.setCorrection(COLOR_CORRECTION); // Attention: Can't be used with Servos, Sound Modules, Charliplexing, ...
    #endif
  #endif

  #ifdef START_MSG
    Serial.println(F(START_MSG));
  #endif
  // Debug_Print_Fuses();

  #ifdef RECEIVE_LED_COLOR_PER_RS232                                     // Send the number of available LEDs to the Python program
    #ifndef LEDS_PER_CHANNEL
	  Serial.print(F("#Color Test LED cnt:")); Serial.println(NUM_LEDS); // Without this message the program fails with the message
	#endif
  #endif                                                                 //   "Error ARDUINO is not answering"

//  #define GCC_VERSION (__GNUC__ * 10000L + __GNUC_MINOR__ * 100L + __GNUC_PATCHLEVEL__)
//  Serial.print(F("GCC_VERSION:")); Serial.println(GCC_VERSION);


  #if defined USE_RS232_OR_SPI_AS_INPUT && defined SEND_DISABLE_PIN
    pinMode(SEND_DISABLE_PIN, OUTPUT);
    digitalWrite(SEND_DISABLE_PIN, 0);
  #endif

  #ifdef USE_CAN_AS_INPUT

	// *** Initialize the CAN bus ***
    #ifdef ESP32																							
		CAN.setPins(4, 5);																						// 08.03.21 Juergen
		if (CAN.begin(250E3)) 
			 {
			 //if (CAN.filterExtended(0x00160000, 0x1FFF0000))													// 08.03.21 Juergen: the original implementation of filterExtended has a bug, so need to do it outselfs
			 if (filterExtended(0x160000, 0x1FFF0000))
			 {
			 	 Serial.println(F("Filter OK!"));
			 }
			 else
			 {
			 	 Serial.println(F("Filter failed!"));
			 }
			 
    		 Serial.println(F("CAN Init OK!"));
			 CAN_ok = true;
			 }
	#else
		if (CAN.begin(MCP_STDEXT, CAN_250KBPS, MCP_8MHZ) == CAN_OK) // init CAN bus, baudrate: 250k@8MHz
			 {
			 Serial.println(F("CAN Init OK!"));
			 CAN_ok = true;
			 CAN.setMode(MCP_NORMAL); // Important to use the filters
			 // Set the message filters
			 Add_Message_to_Filter(CAN, 0x80000000 | 0x0016FFFF, 0); // Filter 0 initializes the CAN chip
			 Add_Message_to_Filter(CAN, 0x80000000 | 0x00160000, 6); // Filter >= 6 uses also channel 0
			 }                                                       // => Filter is addapte to pass both messages
	#endif
    else Serial.println(F("CAN Init Fail!"));                    //    => Messages matching 0x0016???? are passed
  #endif

  #ifdef USE_SPI_COM                                                                                          // 13.05.20:
    SPI.begin();                         // Begins the SPI commnuication
    SPI.setClockDivider(SPI_CLOCK_DIV8); // Sets clock for SPI communication at 8 (16/8=2Mhz)
    digitalWrite(SS,HIGH);               // Setting SlaveSelect as HIGH (So master doesnt connnect with slave)
    digitalWrite(SS, LOW);               // Starts communication with the SPI slave (DCC/Selectrix Arduino)   // 11.11.20: Juergen

  #endif

  Set_Start_Values(MobaLedLib); // The start values are defined in the "MobaLedLib.h" file if entered by the user

  #if defined(ENABLE_STORE_STATUS) && defined(_USE_STORE_STATUS)                                              // 19.05.20: Juergen
    RestoreStatus();
  #endif

  #if defined TEST_TOGGLE_BUTTONS || defined TEST_PUSH_BUTTONS
    Setup_Toggle_Buttons();
  #endif

  #ifdef READ_LDR
    Init_DarknessSensor(LDR_PIN); // Attention: The analogRead() function can't be used together with the darkness sensor !
  #endif

  //#else                                                                                                     // 04.11.20:  Disabled
    #ifdef USE_ADDITIONAL_SETUP_PROC
      Additional_Setup_Proc();                                                                                // 31.03.20:
    #endif
  //#endif

  #ifdef DUMP_EEPROM_ON_START                                                                                 // 01.05.20:
    char tmp[5];

    Serial.println("Dump off EEProm content");
    for (uint16_t eeAddr = 0; eeAddr < 1024; eeAddr++)
    {
        if ((eeAddr&0x0f) == 0)
        {
            Serial.println();
            sprintf(tmp, "%04x", eeAddr);
            Serial.print(tmp);
        }
        eeprom_busy_wait();
        uint8_t value = eeprom_read_byte((const uint8_t*)eeAddr);
        sprintf(tmp, " %02x", value);
        Serial.print(tmp);
    }
    Serial.println();
  #endif

  if (millis() < 1500) delay(1500 - millis()); // Wait to prevent flickering if the Arduino is detected from the excel program  // 05.05.20:

#if  USE_NEW_LED_ARRAY                                                                                        // 30.10.20:
  #if defined(Mainboard_LED0) || defined(Mainboard_LED_D2)
	FastPin<LED0_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED1) || defined(Mainboard_LED_D3)
	FastPin<LED1_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED2) || defined(Mainboard_LED_D4)
    FastPin<LED2_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED3) || defined(Mainboard_LED_D5)
    FastPin<LED3_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED4) || defined(Mainboard_LED_A3) // Could also be used with the CAN heart beat LED because the LED is overwritten
    FastPin<LED4_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED5) || defined(Mainboard_LED_A2)
    FastPin<LED5_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED6) || defined(Mainboard_LED_A1)
    FastPin<LED6_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED7) || defined(Mainboard_LED_D7)
    FastPin<LED7_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED8) || defined(Mainboard_LED_D8)
    FastPin<LED8_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED9) || defined(Mainboard_LED_D9)
    FastPin<LED9_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED10) || defined(Mainboard_LED_D10)
    FastPin<LED10_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED11) || defined(Mainboard_LED_D11)
    FastPin<LED11_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED12) || defined(Mainboard_LED_D12)
    FastPin<LED12_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED13) || defined(Mainboard_LED_D13)
    FastPin<LED13_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED14) || defined(Mainboard_LED_A4)
    FastPin<LED14_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED15) || defined(Mainboard_LED_A5)
    FastPin<LED15_PIN>::setOutput();
  #endif
  #if defined(Mainboard_LED16) || defined(Mainboard_LED_A0)
    FastPin<LED16_PIN>::setOutput();
  #endif
#else // USE_NEW_LED_ARRAY not defined
  #if defined(Mainboard_LED0) || defined(Mainboard_LED_D2)
    pinMode(LED0_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED1) || defined(Mainboard_LED_D3)
    pinMode(LED1_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED2) || defined(Mainboard_LED_D4)
    pinMode(LED2_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED3) || defined(Mainboard_LED_D5)
    pinMode(LED3_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED4) || defined(Mainboard_LED_A3) // Could also be used with the CAN heart beat LED because the LED is overwritten
    pinMode(LED4_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED5) || defined(Mainboard_LED_A2)
    pinMode(LED5_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED6) || defined(Mainboard_LED_A1)
    pinMode(LED6_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED7) || defined(Mainboard_LED_D7)
    pinMode(LED7_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED8) || defined(Mainboard_LED_D8)
    pinMode(LED8_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED9) || defined(Mainboard_LED_D9)
    pinMode(LED9_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED10) || defined(Mainboard_LED_D10)
    pinMode(LED10_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED11) || defined(Mainboard_LED_D11)
    pinMode(LED11_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED12) || defined(Mainboard_LED_D12)
    pinMode(LED12_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED13) || defined(Mainboard_LED_D13)
    pinMode(LED13_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED14) || defined(Mainboard_LED_A4)
    pinMode(LED14_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED15) || defined(Mainboard_LED_A5)
    pinMode(LED15_PIN, OUTPUT);
  #endif
  #if defined(Mainboard_LED16) || defined(Mainboard_LED_A0)
    pinMode(LED16_PIN, OUTPUT);
  #endif
#endif

#ifdef USE_DCC_INTERFACE
  #ifndef DCC_STATUS_PIN
  #define DCC_STATUS_PIN -1
  #endif
  DCCInterface* interf = new DCCInterface();
  commInterface = interf;
  interf->setup(DCC_SIGNAL_PIN, DCC_STATUS_PIN, stream, 
#ifdef NO_DCC_PULLUP
  false
#else
  true
#endif    
  );
#endif
#ifdef ESP32                                                                                                  // 30.10.20: Juergen

  #ifdef USE_SX_INTERFACE
    #ifndef SX_STATUS_PIN
      #define SX_STATUS_PIN -1
    #endif
    SXInterface* interf = new SXInterface();
    commInterface = interf;
    interf->setup(SX_SIGNAL_PIN, SX_CLOCK_PIN, SX_STATUS_PIN, stream);
  #endif  
  
  #if !defined(Mainboard_LED1)																				  // 12.11.20: Juergen initialize Mainboard leds
	#if defined(USE_NEW_LED_ARRAY)																			  // if mainboard led isn't configured at all just clear the output
		FastPin<LED1_PIN>::setInput();
	#else
		pinMode(LED1_PIN, INPUT);
    #endif
  #endif
  #if !defined(Mainboard_LED2)
	#if defined(USE_NEW_LED_ARRAY)
		FastPin<LED2_PIN>::setInput();
	#else
		pinMode(LED2_PIN, INPUT);
    #endif
  #endif
  #if !defined(Mainboard_LED3)
	#if defined(USE_NEW_LED_ARRAY)
		FastPin<LED3_PIN>::setInput();
	#else
		pinMode(LED3_PIN, INPUT);
    #endif
  #endif

  #ifdef USE_ESP32_EXTENSIONS
     setupESP32Extensions();
  #endif

  xTaskCreatePinnedToCore(MLLTask,"MLL",4096,NULL,1,&MLLTaskHnd,0);
  //xTaskCreatePinnedToCore(CoreTask1,"CPU_1",1000,NULL,1,&Core0TaskHnd,1);
#endif

#if defined(USE_DMX_PIN)                                                                                      // 19.01.21: Juergen
  dmxInterface.setup(USE_DMX_PIN,&(leds[DMX_LED_OFFSET].r), DMX_CHANNEL_COUNT);
#endif

  //Eigene_setup();
}

#if defined READ_LDR && defined READ_LDR_DEBUG
  //---------------------------
  void Debug_Print_LDR_Values()
  //---------------------------
  {
    // Debug print the LDR values
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
#endif // READ_LDR_DEBUG
#if USE_NEW_LED_ARRAY                                                                                         // 30.10.20: Juergen
  #define FAST_SET_MAINBOARDLED(LEDNr)                     \
          if (MobaLedLib.Get_Input(Mainboard_LED##LEDNr))  \
               FastPin<LED##LEDNr##_PIN>::hi();            \
          else FastPin<LED##LEDNr##_PIN>::lo();
#else
  #if 1
    // Fast way to write the mainboard LEDs
    // It's not possible to use the INP_TURNED_ON / ..OFF because the variables are cleared at the end of
    // the Set_Mainboard_LEDs()
    // => Implement a special callback if the LEDs should be written onli in case of a change of the input variable
    //
    // The fast methode uses 11 Bytes more
    #define FAST_SET_MAINBOARDLED(LEDNr, PortLetter, Mask)                     \
        {                                                                      \
        uint8_t Inp = MobaLedLib.Get_Input(Mainboard_LED##LEDNr);              \
        if (Inp == INP_ON)                                                     \
           {                                                                   \
           PORT##PortLetter = PORT##PortLetter | Mask;                         \
           }                                                                   \
        else if (Inp == INP_OFF)                                               \
           {                                                                   \
           PORT##PortLetter = PORT##PortLetter & ~Mask;                        \
           }                                                                   \
        }
  #else
    #define FAST_SET_MAINBOARDLED(LEDNr, PortLetter, Mask) digitalWrite(LED##LEDNr##_PIN, MobaLedLib.Get_Input(Mainboard_LED##LEDNr))
  #endif
#endif

//-----------------------
void Set_Mainboard_LEDs()
//-----------------------
{
#if USE_NEW_LED_ARRAY                                                                                         // 30.10.20: Juergen
  #if defined(Mainboard_LED0)
	FAST_SET_MAINBOARDLED(0);
  #endif
  #if defined(Mainboard_LED1)
	FAST_SET_MAINBOARDLED(1);
  #endif
  #if defined(Mainboard_LED2)
	FAST_SET_MAINBOARDLED(2);
  #endif
  #if defined(Mainboard_LED3)
	FAST_SET_MAINBOARDLED(3);
  #endif
  #if defined(Mainboard_LED4)
    FAST_SET_MAINBOARDLED(4);
  #endif
  #if defined(Mainboard_LED5)
    FAST_SET_MAINBOARDLED(5);
  #endif
  #if defined(Mainboard_LED6)
    FAST_SET_MAINBOARDLED(6);
  #endif
  #if defined(Mainboard_LED7)
    FAST_SET_MAINBOARDLED(7);
  #endif
  #if defined(Mainboard_LED8)
    FAST_SET_MAINBOARDLED(8);
  #endif
  #if defined(Mainboard_LED9)
    FAST_SET_MAINBOARDLED(9);
  #endif
  #if defined(Mainboard_LED10)
    FAST_SET_MAINBOARDLED(10);
  #endif
  #if defined(Mainboard_LED11)
    FAST_SET_MAINBOARDLED(11);
  #endif
  #if defined(Mainboard_LED12)
    FAST_SET_MAINBOARDLED(12);
  #endif
  #if defined(Mainboard_LED13)
    FAST_SET_MAINBOARDLED(13);
  #endif
  #if defined(Mainboard_LED14)
    FAST_SET_MAINBOARDLED(14);
  #endif
  #if defined(Mainboard_LED15)
    FAST_SET_MAINBOARDLED(15);
  #endif
  #if defined(Mainboard_LED16)
    FAST_SET_MAINBOARDLED(16);
  #endif
#else // USE_NEW_LED_ARRAY not active
  #if defined(Mainboard_LED0)
    FAST_SET_MAINBOARDLED(0,    D, B00000100); // D2  = PortD2
  #endif
  #if defined(Mainboard_LED_D2)
    FAST_SET_MAINBOARDLED(_D2,  D, B00000100); // D2  = PortD2
  #endif

  #if defined(Mainboard_LED1)
    FAST_SET_MAINBOARDLED(1,    D, B00001000); // D3  = PortD3
  #endif
  #if defined(Mainboard_LED_D3)
    FAST_SET_MAINBOARDLED(_D3,  D, B00001000); // D3  = PortD3
  #endif

  #if defined(Mainboard_LED2)
    FAST_SET_MAINBOARDLED(2,    D, B00010000); // D4  = PortD4
  #endif
  #if defined(Mainboard_LED_D4)
    FAST_SET_MAINBOARDLED(_D4,  D, B00010000); // D4  = PortD4                                                // 25.10.20:  Old: _D2
  #endif

  #if defined(Mainboard_LED3)
    FAST_SET_MAINBOARDLED(3,    D, B00100000); // D5  = PortD5
  #endif
  #if defined(Mainboard_LED_D5)
    FAST_SET_MAINBOARDLED(_D5,  D, B00100000); // D5  = PortD5
  #endif

  #if defined(Mainboard_LED4)
    FAST_SET_MAINBOARDLED(4,    C, B00001000); // A3  = PortC3
  #endif
  #if defined(Mainboard_LED_A3)
    FAST_SET_MAINBOARDLED(_A3,  C, B00001000); // A3  = PortC3
  #endif

  #if defined(Mainboard_LED5)
    FAST_SET_MAINBOARDLED(5,    C, B00000100); // A2  = PortC2
  #endif
  #if defined(Mainboard_LED_A2)
    FAST_SET_MAINBOARDLED(_A2,  C, B00000100); // A2  = PortC2
  #endif

  #if defined(Mainboard_LED6)
    FAST_SET_MAINBOARDLED(6,    C, B00000010); // A1  = PortC1
  #endif
  #if defined(Mainboard_LED_A1)
    FAST_SET_MAINBOARDLED(_A1,  C, B00000010); // A1  = PortC1
  #endif

  #if defined(Mainboard_LED7)
    FAST_SET_MAINBOARDLED(7,    D, B10000000); // D7  = PortD7
  #endif
  #if defined(Mainboard_LED_D7)
    FAST_SET_MAINBOARDLED(_D7,  D, B10000000); // D7  = PortD7
  #endif

  #if defined(Mainboard_LED8)
    FAST_SET_MAINBOARDLED(8,    B, B00000001); // D8  = PortB0
  #endif
  #if defined(Mainboard_LED_D8)
    FAST_SET_MAINBOARDLED(_D8,  B, B00000001); // D8  = PortB0
  #endif

  #if defined(Mainboard_LED9)
    FAST_SET_MAINBOARDLED(9,    B, B00000010); // D9  = PortB1
  #endif
  #if defined(Mainboard_LED_D9)
    FAST_SET_MAINBOARDLED(_D9,  B, B00000010); // D9  = PortB1
  #endif

  #if defined(Mainboard_LED10)
    FAST_SET_MAINBOARDLED(10,   B, B00000100); // D10 = PortB2
  #endif
  #if defined(Mainboard_LED_D10)
    FAST_SET_MAINBOARDLED(_D10, B, B00000100); // D10 = PortB2
  #endif

  #if defined(Mainboard_LED11)
    FAST_SET_MAINBOARDLED(11,   B, B00001000); // D11 = PortB3
  #endif
  #if defined(Mainboard_LED_D11)
    FAST_SET_MAINBOARDLED(_D11, B, B00001000); // D11 = PortB3
  #endif

  #if defined(Mainboard_LED12)
    FAST_SET_MAINBOARDLED(12,   B, B00010000); // D12 = PortB4
  #endif
  #if defined(Mainboard_LED_D12)
    FAST_SET_MAINBOARDLED(_D12, B, B00010000); // D12 = PortB4
  #endif

  #if defined(Mainboard_LED13)
    FAST_SET_MAINBOARDLED(13,   B, B00100000); // D13 = PortB5
  #endif
  #if defined(Mainboard_LED_D13)
    FAST_SET_MAINBOARDLED(_D13, B, B00100000); // D13 = PortB5
  #endif

  #if defined(Mainboard_LED14)
    FAST_SET_MAINBOARDLED(14,   C, B00010000); // A4  = PortC4
  #endif
  #if defined(Mainboard_LED_A4)
    FAST_SET_MAINBOARDLED(_A4,  C, B00010000); // A4  = PortC4
  #endif

  #if defined(Mainboard_LED15)
    FAST_SET_MAINBOARDLED(15,   C, B00100000); // A5  = PortC5
  #endif
  #if defined(Mainboard_LED_A5)
    FAST_SET_MAINBOARDLED(_A5,  C, B00100000); // A5  = PortC5
  #endif

  #if defined(Mainboard_LED16)
    FAST_SET_MAINBOARDLED(16,   C, B00000001); // A0  = PortC0
  #endif
  #if defined(Mainboard_LED_A0)
    FAST_SET_MAINBOARDLED(_A0,  C, B00000001); // A0  = PortC0
  #endif
#endif
}


#ifdef DayAndNightTimer_Period                                                                                // 07.10.20:

  // Wenn ein InCh angegeben ist, dann startet wechselt der Zaehler bei
  //   einer steigende Flanke in den Nacht Modus (DayState = SunSet)
  //   einer fallende  Flanke in den Nacht Modus (DayState = SunRise)
  // Wenn kein InCh angegeben wurde, dann Startet er immer neu am Ende.

  // Normalerweise wird der Darkness Zaehler bei einer Flanke auf den
  // Eetsprechenden Startwert gesetzt:
  //   steigende Flanke: Darkness = 0;   (Maximale Helligkeit)
  //   fallende  Flanke: Darkness = 255: (Minimale Helligkeit)
  // Wenn KeepDarknessCtr definiert ist, dann wird der Darkness Zaehler
  // bei einer Flanke nicht auf den entsprechenden Startwert gesetzt.
  // Dadurch Ergibt sich kein Sprung im Zaehlerstand.
  // Aber dadurch wird die Zeit "gespiegelt". Aus 18 Uhr wird 6 Uhr.
  // Es kann aber sein, dass dadurch die Synchronisation Probleme macht

  //----------------------------
  void Update_DayAndNightTimer()
  //----------------------------
  {
    #ifdef DayAndNightTimer_InCh // The Day/Night change is triggered by a variable
      uint8_t Inp = MobaLedLib.Get_Input(DayAndNightTimer_InCh);
      static uint8_t OldInp = INP_OFF;  // ToDo: Speichern des letzten Wertes ?
      if (OldInp != Inp) // INP_TURNED_ON / ..OFF can't be used because it's cleared in the lib
         {
         OldInp = Inp;
         if (Inp == INP_ON)
              {
              DayState = SunSet;
              #ifndef KeepDarknessCtr // Normaly the Darkness is set to the start values
                 Darkness = 0;
              #endif
              }
         else {
              DayState = SunRise;
              #ifndef KeepDarknessCtr
                 Darkness = 255;
              #endif
              }
         }
    #endif

    static uint32_t Next_Time_Update = DayAndNightTimer_Period;
    if (millis() > Next_Time_Update)
       {
       Next_Time_Update += DayAndNightTimer_Period;
       if (DayState == SunSet)
            {
            if (Darkness < 255)
                 {
                 DayState = SunSet;
                 Darkness++;
                 }
            #ifndef DayAndNightTimer_InCh
              else DayState = SunRise;
            #endif
            }
       else { // DayState = SunRise or Unknown
            if (Darkness > 0)
                 Darkness--;
            #ifndef DayAndNightTimer_InCh
              else DayState = SunSet;
            #endif
            }
       // Serial.print(Darkness); Serial.print(DayState==SunSet?" SunSet  ":" SunRise "); Serial.print(DayState); Serial.print(" ");// Debug
       #ifdef DayAndNightTimer_Debug
         uint16_t Minutes = ((uint32_t)Darkness * 12*60) / 255;
         if (DayState <= SunSet)
              Minutes =  12*60 + Minutes;
         else Minutes =  12*60 - Minutes;
         char TimeStr[6];
         sprintf(TimeStr, "%3i: %2i:%02i", Darkness, Minutes/60, Minutes%60);
         Serial.println(TimeStr);
       #endif
       }
  }
#endif


#ifdef USE_LED_TO_VAR                                                                                         // 08.10.20:
  //-------------------
  void Update_LED2Var()
  //-------------------
  // Uses 194 bytes + 4 bytes per processed LED
  {
       {
       for (uint8_t i = 0; i < sizeof(LED2Var_Tab)/sizeof(LED2Var_Tab_T); i++)
           {
           const LED2Var_Tab_T *p = &LED2Var_Tab[i];
           uint8_t Var_Nr         = pgm_read_byte_near(&p->Var_Nr);
           uint8_t LED_Nr         = pgm_read_byte_near(&p->LED_Nr);
           uint8_t Offset_and_Typ = pgm_read_byte_near(&p->Offset_and_Typ);
           uint8_t Val            = pgm_read_byte_near(&p->Val);
           uint8_t Offset         = Offset_and_Typ >> 3;
           int8_t  Typ            = Offset_and_Typ & 0x07;
           uint8_t LED_Val        = leds[LED_Nr][Offset];

           uint8_t Res;
           switch (Typ)
              {
              case T_GREATER_THAN:   Res =  (LED_Val >  Val); break;
              case T_LESS_THEN:      Res =  (LED_Val <  Val); break;
              default:                                                                                        // 28.11.20: Juergen
              case T_EQUAL_THEN:     Res =  (LED_Val == Val); break;
              case T_NOT_EQUAL_THEN: Res =  (LED_Val != Val); break;
              case T_BIN_MASK:       Res =  (LED_Val &  Val); break;
              case T_NOT_BIN_MASK:   Res = !(LED_Val &  Val); break;
              }
           MobaLedLib.Set_Input(Var_Nr, Res);
           #if 0 // Debug
              uint32_t PERIOD = 500;
              static uint32_t Disp = PERIOD;
              if (millis() >= Disp)
                 {
                 char s[40];
                 sprintf(s, "%3i: V%-3i L%-3i O%i T%i V%-3i Act%-3i R%i", i, Var_Nr, LED_Nr, Offset, Typ, Val, LED_Val, Res);
                 Serial.println(s);
                 if (i == sizeof(LED2Var_Tab)/sizeof(LED2Var_Tab_T) - 1) Disp += 1000;
                 }
           #endif
           }
       }
  }
#endif // USE_LED_TO_VAR

//-----------
void loop(){
//-----------
#ifdef USE_COMM_INTERFACE
  commInterface->process();
#endif

#ifndef ESP32
  MLLMainLoop();
#endif

#ifdef USE_ESP32_EXTENSIONS
  loopESP32Extensions();
#endif
  
#ifdef Additional_Loop_Proc2						// 08.10.21: Juergen: add low prority loop, on multicore platforms running on seperate core 
  Additional_Loop_Proc2();                                                                                        // 26.09.21: Juergen
#endif
}

#ifdef ESP32
//--------------------------------
void MLLTask( void * parameter ) {
//--------------------------------
  while(1) {
    MLLMainLoop();
    yield();
    delay (1);
  }
}
#endif


//-----------------
void MLLMainLoop(){
//-----------------
  //Eigene_loop();
  #ifdef Additional_Loop_Proc
    #ifdef USE_SPI_COM
    if (millis() > DISABLE_SPI_DELAY + 50) // Wait until the SPI pins are disabled in case they are not used
    #endif
       {
       Additional_Loop_Proc();                                                                                // 31.03.20:
       }
  #endif

  #if defined LED_HEARTBEAT_PIN && LED_HEARTBEAT_PIN >= 0                                                     // 13.05.20:
    LED_HeartBeat.Update();
  #endif

  #if defined SEND_INPUTS                                                                                     // 28.11.20: Juergen
     Send_Inputs('*', false);
  #endif

  MobaLedLib.Update();                  // Update the LEDs in the configuration

  #ifdef DayAndNightTimer_Period                                                                              // 07.10.20:
    Update_DayAndNightTimer();
  #endif

  #if defined USE_RS232_OR_SPI_AS_INPUT || defined RECEIVE_LED_COLOR_PER_RS232
    Proc_Received_Char();               // Process characters received from the RS232 (DCC, Selectrix, ... Arduino)
  #endif

  #if defined USE_SPI_COM               // Check if the SPI Slave is available to be able to use the A1 pin       // 15.05.20:
    #if defined USE_RS232_OR_SPI_AS_INPUT      // If SPI communication is used this check is always sent in case the DCC Arduino was restarted
       Check_SPI_Slave(0);              // The slave will deactivate the pull-Up resistor in the A1 line
    #else
      if (millis() < DISABLE_SPI_DELAY) // Give the DCC/SX Arduino some time to boot
           Check_SPI_Slave(1);          // The slave will deactivate the pull-Up resistor in the A1 line
      else if (Send_Disable_Pin_Active)
              {
              Send_Disable_Pin_Active = 0;
              Disable_SPI();
              }
    #endif
  #endif

  #if defined USE_RS232_OR_SPI_AS_INPUT && defined SEND_DISABLE_PIN
    if (Send_Disable_Pin_Active) digitalWrite(SEND_DISABLE_PIN, 1);  // Stop the sending of the DCC-Arduino because the RS232 interrupts are not processed during the FastLED.show() command  // 13.05.20:  Added: if (Send_Disable_Pin_Active)
  #endif

  #if defined TEST_TOGGLE_BUTTONS || defined TEST_PUSH_BUTTONS
    Check_Mainboard_Buttons();
  #endif


  #ifdef USE_CAN_AS_INPUT
    Process_CAN();                      // Read the messages from the CAN bus and write the InpStructArray[]
  #endif

  Set_Mainboard_LEDs();                 // Turn on/off the LEDs on the mainboard if configured

  FastLED.show();                       // Show the LEDs (send the leds[] array to the LED stripe)

#if defined(USE_DMX_PIN)                                                                                      // 19.01.21: Juergen
  dmxInterface.loop();
#endif


  #if defined USE_RS232_OR_SPI_AS_INPUT && defined SEND_DISABLE_PIN
    if (Send_Disable_Pin_Active) digitalWrite(SEND_DISABLE_PIN, 0);  // Allow the sending of the DCC commands again  // 13.05.20:  Added: if (Send_Disable_Pin_Active)
  #endif

  #if defined READ_LDR && defined READ_LDR_DEBUG
    Debug_Print_LDR_Values();           // Debug print the LDR values
  #endif

  #ifdef USE_LED_TO_VAR                                                                                       // 08.10.20:
    Update_LED2Var();
  #endif

  #if 0 // Debug
    static unsigned long lastMillis = 0;
    static unsigned long lastMillis2 = 0;
    static int cnt=0;
    cnt++;
    if ((millis()-lastMillis)>=1000)
       {
       if (cnt<90)
          {
          Serial.print("updates in 1000ms: ");
          Serial.println(cnt);
       }
       cnt=0;

       lastMillis=millis();
       //for (int i=0;i<NUM_LEDS;i++){ leds[i].r = random(0,255);leds[i].g = random(0,255);leds[i].b = random(0,255); }
       }
    if ((millis()-lastMillis2)>=100)
       {
       lastMillis2=millis();
       //for (int i=0;i<NUM_LEDS;i++){ leds[i].r = random(0,255);leds[i].g = random(0,255);leds[i].b = random(0,255); }
       //for (int i=42;i<NUM_LEDS;i++){ leds[i].r++;leds[i].g++;leds[i].b++;   }
       }
  #endif
}
