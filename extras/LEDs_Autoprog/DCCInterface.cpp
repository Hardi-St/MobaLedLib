/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2021 Jürgen Winkler: MobaLedLib@gmx.at

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
24.04.21:  Add PICO support (Jürgen)
25.04.21:  Improve DCC signal detection and led display (Jürgen)
*/

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
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
#include "Helpers.h"

static NmraDcc  Dcc ;                                // Instance of the NmraDcc class

//-------------------------------------------------------------------------------------
void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
//-------------------------------------------------------------------------------------
// This function is called whenever a normal DCC Turnout Packet is received
{
  //if (!OutputPower) return ; // debug: Simulate the Lenz LZV100 behavior which doesn't send the button release signal
	char s[20];
  sprintf(s, "@%4i %02X %02X\n", Addr, Direction, OutputPower);
  CommInterface::addToSendBuffer(s);
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
  CommInterface::addToSendBuffer(s);
  //printf("notifyDccSigState: %i,%02X\r\n", Addr, State) ;
}

void notifyDccMsg( DCC_MSG * Msg )
{
  CommInterface::setLastSignalTime(millis());
}

//-----------
void DCCInterface::setup(int DCCSignalPin, int statusLedPin, InMemoryStream& stream, bool enablePullup)
{
//-----------
  
  CommInterface::setup(statusLedPin, stream);

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  Dcc.pin(DCCSignalPin, enablePullup ? 1 : 0);

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // ToDo: Was bedeuten die Konstanten ?

  //addToSendBuffer("Init Done\r\n"); // This message is send to the LED Arduino over RS232 or SPI (If the Arduino is already active)
  
  printf("DCCInterface using pin %d has been started.\r\n", DCCSignalPin);
}

//-----------
void DCCInterface::process(){
//-----------
  Dcc.process(); // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  CommInterface::process();                                                                             // 13.05.20:
}
#endif