/*
MobaLedLib: LED library for model railways
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Copyright (C) 2023 Jürgen Winkler: MobaLedLib@gmx.at

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

The Loconet(TM) Protocol is (C) by digitrax. This code is developed using the LocoNet2 library,
which doesn't contain a license for Loconet(TM).

This code is provided solely for non-commercial private use by Digitrax customers.
see https://www.digitrax.com/static/apps/cms/media/documents/loconet/loconetpersonaledition.pdf

 
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Documents:
  ~~~~~~~~~~
 - https://www.digitrax.com/static/apps/cms/media/documents/loconet/loconetpersonaledition.pdf
 - https://www.opendcc.de/info/dcc/dcc.html
 - https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
 - https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
*/
/*
Revision History :
~~~~~~~~~~~~~~~~~
17.12.23:  Versions 1.0 (Jürgen)
           LNet rail signal decoder for ESP with thread save read buffer
*/

#if defined(ESP32)

#include "LNetInterface.h"
#include "Helpers.h"

//-----------
void LNetInterface::setup(int LNetRxPin, int LNetTxPin, int statusLedPin, InMemoryStream& stream, uint8_t xCoreID)
//-----------
{
  lastHighTime = millis();
  setSignalLossTimeout(60000);
  
  // start assuming to be connected    
  setPluggedIn(true);
  setLastSignalTime(millis());    
  
  CommInterface::setup(statusLedPin, stream);
  CommInterface::addToSendBuffer((char*)"\nLNet_Interface\n");          // Don't use Serial.print because characters get lost if the LED-Arduino updates the LEDs
  pLocoNetPhy = new LocoNetESP32(&bus, LNetRxPin, LNetTxPin, 2, xCoreID);
  pParser = new LocoNetDispatcher(&bus);
  pLocoNetPhy->begin();
  
  pParser->onPacket(CALLBACK_FOR_ALL_OPCODES, [](const lnMsg *rxPacket) 
  {
    CommInterface::setLastSignalTime(millis());
  });

  pParser->onSwitchRequest([](uint16_t address, bool output, bool direction) 
  {
    print2Buf(address, direction, output);
    if_printf("notifySwitchRequest: %u %s %s\n", address, direction ? "Closed" : "Thrown", output ? "On" : "Off");
  });
  if_printf("LNetInterface using pin %d/%d has been started.\r\n", LNetRxPin, LNetTxPin);
}

//-----------
void LNetInterface::process(){
  CommInterface::process();
  if (digitalRead(4) == LOW)
  {
    if (cablePlugged && millis() - lastHighTime > 1000)
    {
      cablePlugged = false;
      setPluggedIn(cablePlugged);
      if_printf("cable unplugged\n");
    }
  }
  else
  {
    lastHighTime = millis();
    if (!cablePlugged)
    {
      cablePlugged = true;
      setPluggedIn(cablePlugged);
      if_printf("cable plugged in\n");
    }
  }  
}

//-----------------------------------------------------
void LNetInterface::print2Buf(uint16_t addr, uint8_t dir, uint8_t pow)
//-----------------------------------------------------
{
  char s[20];
  sprintf(s, "@%4i %02X %02X\n", addr, dir, pow?1:0);
  LNetInterface::addToSendBuffer(s);
}
#endif