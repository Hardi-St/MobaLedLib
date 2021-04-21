/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2020 Jürgen Winkler: MobaLedLib@gmx.at

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


 DCC rail signal decoder for ESP with thread save read buffer      18.10.2020 Jürgen
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


 Documents:
 ~~~~~~~~~~
 - https://www.opendcc.de/info/dcc/dcc.html
 - https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
 - https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
 - https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/
 - https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

Revision History :
~~~~~~~~~~~~~~~~~
18.10.20:  Versions 1.0 (Jürgen)
*/

#ifdef ESP32
// Disable the warning:                                                                                       // 26.12.19:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
// and:
//   ... warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <NmraDcc.h>
#pragma GCC diagnostic pop

#include "DCCInterface.h"

static NmraDcc  Dcc ;                                // Instance of the NmraDcc class

static uint8_t          Error           = 0;
static uint32_t         NextStatusFlash = 0;
static int 				StatusLedPin	= -1;
InMemoryStream* DCCInterface::pStream;


//                                       States On    Off   On  Off  On   Off
static uint16_t RS232_Flash_Table[]         = { 2,     1500, 1500                     };
static uint16_t BufferFull_Flash_Table[]    = { 2,     250,  250                      };
static uint16_t PriorBuddFull_Flash_Table[] = { 2,     50,   450                      };
static uint8_t  FlashState = 1;

//---------------------------------
void DCCInterface::addToSendBuffer(const char *s)
//---------------------------------
// Attention this is called in the interrupt
{
	if (!pStream->write(s))
	{
	  if (Error < 16) Error += 2; // buffer overflow	
	}
}

#include <stdarg.h>
#include <WString.h>

#define printf(Format, ...) printf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

//-------------------------------------------------------
void printf_proc(const __FlashStringHelper *format, ...)
//-------------------------------------------------------
// Achtung: Es durfen keine Zeichen über die serielle Schnittstelle ausgegeben werden wenn der LED Arduino
//          programmiert wird und die Beiden über die TX Leitung verbunden sind. Die serielle Schnittstelle
//          muss nach jeder Ausgabe abgeschaltet werden. Sonst zieht der TX Ausgang dieses Nanos die
//          RX Leitung des LED Arduinos auf 5V.
//          Bei der normalen Kommunikation wird das über die A1 Leitung zwischen den beiden Rechnern gesteuert.
//          Wenn der SPI Mode aktiviert ist, dann wird die A1 Leitung als Input geschaltet damit sie auf dem
//          LED Arduino als Eingang für die Schalter genutzt werden kann.
{
#ifdef DEBUG_INTERFACE	
     char buf[50];
     va_list ap;
     va_start(ap, format);
     #ifdef __AVR__
        vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
     #else
        vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
     #endif
     va_end(ap);
     Serial.print(buf);
#endif		 
}


//-------------------------------------------------------------------------------------
void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
//-------------------------------------------------------------------------------------
// This function is called whenever a normal DCC Turnout Packet is received
{
  //if (!OutputPower) return ; // debug: Simulate the Lenz LZV100 behavior which doesn't send the button release signal
	char s[20];
  sprintf(s, "@%4i %02X %02X\n", Addr, Direction, OutputPower);
  DCCInterface::addToSendBuffer(s);
  //printf("%4i notifyDccAccTurnoutOutput: %i, %i, %02X\r\n", millis(), Addr, Direction, OutputPower);
}

//---------------------------------------------------------
void notifyDccSigOutputState( uint16_t Addr, uint8_t State)
//---------------------------------------------------------
// notifyDccSigOutputState() Callback for a signal aspect accessory decoder.
//                      Defined in S-9.2.1 as the Extended Accessory Decoder Control Packet.
{
  char s[20];
  sprintf(s, "$%4i %02X\n", Addr, State); // Bei der CAN Geschichte hab ich herausgefunden dass es 2048 Adressen gibt. Ich hoffe das stimmt...
  DCCInterface::addToSendBuffer(s);
  //printf("notifyDccSigState: %i,%02X\r\n", Addr, State) ;
}

//-----------
void DCCInterface::setup(int DCCSignalPin, int statusLedPin, InMemoryStream& stream){
//-----------
  pStream = &stream;
  StatusLedPin = statusLedPin;

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  Dcc.pin(DCCSignalPin, 1);

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // ToDo: Was bedeuten die Konstanten ?

  //addToSendBuffer("Init Done\r\n"); // This message is send to the LED Arduino over RS232 or SPI (If the Arduino is already active)
  if (StatusLedPin>=0) pinMode(StatusLedPin, OUTPUT);
  
  printf("DCCInterface using pin %d has been started.\r\n", DCCSignalPin);
}

//---------------------------------
void DCCInterface::processErrorLed()
//---------------------------------
{
  if (StatusLedPin<0) return;
  
  static uint16_t *Flash_Table_p     = RS232_Flash_Table;
  static uint16_t *Old_Flash_Table_p = RS232_Flash_Table;
  static uint32_t NextCheck = 0;

  uint32_t t = millis();
  if (t >= NextCheck) // Check the Status and the error every 100 ms
     {
     NextCheck = t + 100;
     if (Error) // Fast flash frequency if an buffer overflow was detected
          {
          if (Error > 1)
               Flash_Table_p = BufferFull_Flash_Table;
          else Flash_Table_p = PriorBuddFull_Flash_Table; // Prior error detected but not longer activ
          }
     else {
             Flash_Table_p = RS232_Flash_Table;
          }

     if (Old_Flash_Table_p != Flash_Table_p) // If the state has changed show it immediately
        {
        Old_Flash_Table_p = Flash_Table_p;
        NextStatusFlash = 0;
        FlashState = 1;
        }
     }
  if (t >= NextStatusFlash) // Flash the status LED
     {
     if (FlashState > Flash_Table_p[0]) FlashState = 1;
     NextStatusFlash = t + Flash_Table_p[FlashState];
     digitalWrite(StatusLedPin, FlashState%2);
     FlashState++;
     }
}

//-----------
void DCCInterface::process(){
//-----------
  Dcc.process(); // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  processErrorLed();                                                                             // 13.05.20:
}
#endif