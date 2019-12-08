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
                          |  .-GND--| [ ]GND   5V MOSI GND   TX1[ ] |--------.
                          |  |      | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |        |
                          |  |      |          [ ] [ ] [ ]          |        |
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

*/

#define SEND_DISABLE_PIN    A1 // Pin A1 is used to stop the DCC, Selectrix, ... Arduino from sending RS232 characters


#define LED_DO_PIN          6  // Pin D6 is connected to the LED stripe
#define CAN_CS_PIN          10 // Pin D10 is used as chip select for the CAN bus

#define DCC_INPSTRUCT_START 0  // Start number in the InpStructArray[]

#define SERIAL_BAUD     115200 // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program

#define FASTLED_INTERNAL       // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"           // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                               // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                               //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                               //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"



#define TestEntrySignal3_RGB(LED, InCh)     InCh_to_TmpVar(InCh, 4)                                                      \
                                            XPatternT1(LED,64,SI_LocalVar,9,0,128,0,0,500 ms,64,4,104,16,16,128,0  ,63,191,191,192,0,1)

#define TestDepSignal4(LED, InCh)           InCh_to_TmpVar(InCh, 7)                                                      \
                                            XPatternT1(LED,12,SI_LocalVar,5,0,45,0,0,500 ms,15,240,0,15,0,240,15,240,0,16  ,63,191,191,191)

#include "LEDs_AutoProg.h"     // This file is generated by "Prog_Generator_MobaLedLib.xlsm"


#ifdef READ_LDR                // Enable the darkness detection functions
  #include "Read_LDR.h"        // process the darkness sensor
  #ifndef LDR_PIN              // The LDR pin could be defined in the Prog_Generator
    #define LDR_PIN      A7    // Use A7 if the MobaLedLib "LEDs Main Module" is used
  #endif
#endif

#ifdef USE_CAN_AS_INPUT
  #include "mcp_can_nd.h"      // The MCP CAN library must be installed in addition if you got the error message "..fatal error: mcp_can_nd.h: No such file or directory"
                               // Here we use a local version where the debug mode is disabled (#define DEBUG_MODE 0).
                               // The original source could be downloaded from https://github.com/coryjfowler/MCP_CAN_lib
                               // It's important to disable the debug mode because the debug messages use a lot of RAM.
                               // To prevent this use the F() macro in print() functions like used here.
                               // If your program uses to much memory you get the following warning:
                               //   "Low memory available, stability problems may occur."
                               //
                               // The files are listed in the Arduino IDE, but they tont use memory if USE_CAN_AS_INPUT is disabled
  #include <SPI.h>
  #include "Add_Message_to_Filter_nd.h"

  #define LED_HEARTBEAT_PIN 17 // The build in LED can't be use because the pin is used as clock port for the SPI bus
#else // not USE_CAN_AS_INPUT
  #define LED_HEARTBEAT_PIN 13 // Build in LED
#endif

CRGB leds[NUM_LEDS];           // Define the array of leds

MobaLedLib_Create(leds);       // Define the MobaLedLib instance
LED_Heartbeat_C LED_HeartBeat(LED_HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.

char Buffer[13] = "";


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
       for (int i = Led; i <= 255 && i < Led+Leds; i++)
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

//--------------------------------
void Receive_LED_Color_per_RS232()                                                                            // 03.11.19:
//--------------------------------
{
  digitalWrite(SEND_DISABLE_PIN, 1); // Tell the DCC Arduino to be quiet

  char Buffer[20] = "";
  uint8_t JustStarted = 1;
  while (1)
     {
     LED_HeartBeat.Update(300); // Fast Flash
     if (Serial.available() > 0)
        {
        char c = Serial.read();
        switch (c)
           {
           case '#': *Buffer = '\0'; break;
           case '\n': // Proc buffer       (For tests with the serial console of Arduino use "Neue Zeile" and not "..(CR)" )
                      if (JustStarted)
                           JustStarted = 0;
                      else {
                           switch (*Buffer)
                             {
                             case 'L': Proc_Color_Cmd(Buffer); break;
                             case 'E': ResetArduino(); // Restart the Arduino
                                       break;
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
     }
}
#endif

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
                       //char s[30]; sprintf(s, "OnOff: Addr %i Channel[%i]=%i Pow=%i", Addr, Channel, Direction, OutputPower); Serial.println(s);  // Debug
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
                             //char s[30]; sprintf(s, "%s Button Addr %i: Channel[%i]=%i", ChkBut==B_RED?"Red":"Green", Addr, Channel,OutputPower); Serial.println(s); // Debug
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
     //char s[30];sprintf(s, "Addr %i not found Dir:%i", ReceivedAddr, Direction); Serial.println(s);  // Debug
   }
#endif // USE_EXT_ADDR

#if defined USE_RS232_AS_INPUT
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
                    Update_InputCh_if_Addr_exists(Addr+ADDR_OFFSET, Direction, OutputPower);                  // 26.09.19:  Added: ADDR_OFFSET
                    return ;
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
#endif

#if defined USE_RS232_AS_INPUT || defined RECEIVE_LED_COLOR_PER_RS232
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

     while (Serial.available() > 0)
        {
        char c = Serial.read();
        // Serial.print(c); // Debug
        switch (c)
           {
           #if defined RECEIVE_LED_COLOR_PER_RS232
              case '#': Receive_LED_Color_per_RS232(); break; // The first '#' is used to enable the serial color change mode  // 03.11.19:
           #endif
           #if defined USE_RS232_AS_INPUT
             case '@':
             case '$': if (*Buffer) Serial.println(F(" Unterm. Error"));
                       *Buffer = c; Buffer[1] = '\0';
                       break;
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
           #endif // USE_RS232_AS_INPUT
           }
        }
   }
#endif // USE_RS232_AS_INPUT || RECEIVE_LED_COLOR_PER_RS232

#ifdef USE_CAN_AS_INPUT

   MCP_CAN  CAN(CAN_CS_PIN);
   bool CAN_ok = false;

   //-----------------------------------
   void Proc_Accessoires(uint8_t *rxBuf)
   //-----------------------------------
   // Process accessoires CAN messages
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
   if (CAN_ok && CAN.checkReceive() == CAN_MSGAVAIL)
      {
      if (CAN.readMsgBuf(&rxId, &len, rxBuf) == CAN_OK)
         {
         //Serial.print(rxId,HEX); Serial.print(" "); Serial.println(len); // Debug
         if (((rxId>>16) & 0xFF) == 0x16) Proc_Accessoires(rxBuf); // Check if it's a accessoires message
         }
      }
   }
#endif // USE_CAN_AS_INPUT


#ifdef TEST_TOGGLE_BUTTONS                                                                                    // 06.10.19:

   #ifndef TOGGLE_BUTTONS_INCH
     #define TOGGLE_BUTTONS_INCH 0
   #endif

   #define LED0_PIN    3
   #define LED1_PIN    4
   #define LED2_PIN    5
   #define SWITCH0_PIN 7
   #define SWITCH1_PIN 8
   #define SWITCH2_PIN 9

   //-------------------------
   void Setup_Toggle_Buttons()
   //-------------------------
   {
     pinMode(LED0_PIN,    OUTPUT);
     pinMode(LED1_PIN,    OUTPUT);
     pinMode(LED2_PIN,    OUTPUT);
     pinMode(SWITCH0_PIN, INPUT_PULLUP);
     pinMode(SWITCH1_PIN, INPUT_PULLUP);
     pinMode(SWITCH2_PIN, INPUT_PULLUP);
   }

   //-------------------------
   void Check_Toggle_Buttons()
   //-------------------------
   // Use the 3 buttons of the main board to toggle input channels
   {
     static uint32_t NextCheck = 0;
     static uint8_t  LastKey   = 0xFF;
     if (millis() >= NextCheck)
        {
        uint8_t Mask = 1;
        NextCheck = millis() + 50;
        for (uint8_t KeyNr = SWITCH0_PIN, LEDNr = LED0_PIN, InChNr = TOGGLE_BUTTONS_INCH; KeyNr <= SWITCH2_PIN; KeyNr++, LEDNr++, InChNr++)
           {
           uint8_t Key = digitalRead(KeyNr);
           if (Key != (LastKey & Mask))
              {
              if (Key)
                   { LastKey |=  Mask; }
              else { LastKey &= ~Mask; digitalWrite(LEDNr, !digitalRead(LEDNr)); }
              NextCheck = millis() + 300;
              MobaLedLib.Set_Input(InChNr, digitalRead(LEDNr));
              }
           Mask <<= 1;
           }
        }
   }
#endif // TEST_TOGGLE_BUTTONS

//-----------
void setup(){
//-----------
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  Serial.begin(SERIAL_BAUD); // Communication with the DCC-Arduino must be fast

  #ifdef START_MSG
    Serial.println(F(START_MSG));
  #endif

  #ifdef RECEIVE_LED_COLOR_PER_RS232                                   // Send the number of available LEDs to the Python program
    Serial.print(F("#Color Test LED cnt:")); Serial.println(NUM_LEDS); // Without this message the program fails with the message
  #endif                                                               //   "Error ARDUINO is not answering"

//  #define GCC_VERSION (__GNUC__ * 10000L + __GNUC_MINOR__ * 100L + __GNUC_PATCHLEVEL__)
//  Serial.print("GCC_VERSION:"); Serial.println(GCC_VERSION);


  #ifdef USE_RS232_AS_INPUT
    pinMode(SEND_DISABLE_PIN, OUTPUT);
    digitalWrite(SEND_DISABLE_PIN, 0);
  #endif

  #ifdef USE_CAN_AS_INPUT
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
  #endif

  Set_Start_Values(MobaLedLib); // The start values are defined in the "MobaLedLib.h" file if entered by the user

  #ifdef TEST_TOGGLE_BUTTONS
    Setup_Toggle_Buttons();
  #endif

  #ifdef READ_LDR
    Init_DarknessSensor(LDR_PIN); // Attention: The analogRead() function can't be used together with the darkness sensor !
  #endif
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

//-----------
void loop(){
//-----------
  #ifdef TEST_TOGGLE_BUTTONS
    Check_Toggle_Buttons();
  #endif

  LED_HeartBeat.Update();

  MobaLedLib.Update();                  // Update the LEDs in the configuration

  #if defined USE_RS232_AS_INPUT || defined RECEIVE_LED_COLOR_PER_RS232
    Proc_Received_Char();               // Process characters received from the RS232 (DCC, Selectrix, ... Arduino)
  #endif
  #if defined USE_RS232_AS_INPUT
    digitalWrite(SEND_DISABLE_PIN, 1);  // Stop the sending of the DCC-Arduino because the RS232 interrupts are not processed during the FastLED.show() command
  #endif

  #ifdef USE_CAN_AS_INPUT
    Process_CAN();                      // Read the messages from the CAN bus and write the InpStructArray[]
  #endif

  FastLED.show();                       // Show the LEDs (send the leds[] array to the LED stripe)

  #ifdef USE_RS232_AS_INPUT
    digitalWrite(SEND_DISABLE_PIN, 0);  // Allow the sending of the DCC commands again
  #endif

  #if defined READ_LDR && defined READ_LDR_DEBUG
    Debug_Print_LDR_Values();           // Debug print the LDR values
  #endif
}

