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


Selectrix rail signal decoder for ESP with thread save read buffer      06.12.2021 Jürgen
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


Documents:
~~~~~~~~~~

Revision History :
~~~~~~~~~~~~~~~~~
06.12.21:  Versions 1.0 (Jürgen)
18.12.21:  fix signal reappear detection bug
02.01.22:  Juergen add support for DCC receive on LED Arduino
*/

#include "CommInterface.h"
#include "Helpers.h"

static uint8_t       Error               = 0;             
static unsigned long NextStatusFlash     = 0;
static unsigned long LastSignalTime      = 0;
static int           StatusLedPin        = -1;

//                                       States On    Off   On  Off  On   Off
static uint16_t OK_Flash_Table[]          = { 2,     1500, 1500                     };
#ifndef __AVR__                                                                             // AVR doesn't use a send buffer
static uint16_t BufferFull_Flash_Table[]  = { 6,     100,  100, 100, 100, 100, 400  };
#endif
static uint16_t NoSignal_Flash_Table[]    = { 2,     50,   450                      };
static uint8_t  FlashState = 1;

#ifndef __AVR__                                                                             // AVR doesn't use a send buffer
InMemoryStream* CommInterface::pStream;

//---------------------------------
void CommInterface::addToSendBuffer(const char *s)
//---------------------------------
// Attention this is called in the interrupt
{
  if (!pStream->write(s))
  {
    Error = 50;   // show this state at least 5 seconds
  }
}

//-----------

void CommInterface::setup(int statusLedPin, InMemoryStream& stream){
  //-----------
  pStream = &stream;
  StatusLedPin = statusLedPin;
  if (StatusLedPin>=0) pinMode(StatusLedPin, OUTPUT);
}
#else
void CommInterface::setup(int statusLedPin){
  //-----------
  StatusLedPin = statusLedPin;
  if (StatusLedPin>=0) pinMode(StatusLedPin, OUTPUT);
}
#endif

//---------------------------------
void CommInterface::processErrorLed()
//---------------------------------
{
  if (StatusLedPin<0) return;
  
  static uint16_t *Flash_Table_p     = OK_Flash_Table;
  static uint16_t *Old_Flash_Table_p = OK_Flash_Table;
  static unsigned long NextCheck = 0;

  auto t = millis();
  if (t >= NextCheck) // Check the Status and the error every 100 ms
  {
    NextCheck = t + 100;
    if ((millis()-LastSignalTime) > 1000)
    {
      LastSignalTime = 0;
      Error = 0;              // don't show an error when signal comes back
    }
    if (LastSignalTime==0)   // no SX signal
    {
      Flash_Table_p = NoSignal_Flash_Table;
    }
#ifndef __AVR__                                                                             // AVR doesn't use a send buffer
    else if (Error) // Fast flash frequency if an buffer overflow was detected
    {
      Flash_Table_p = BufferFull_Flash_Table;
      Error--; // Decrement the error counter if the communication is working again
    }
#endif    
    else 
    {
      Flash_Table_p = OK_Flash_Table;
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
void CommInterface::process(){
  processErrorLed();
}

void CommInterface::setLastSignalTime(unsigned long lastSignalTime) 
{
  if ((millis()-lastSignalTime) > 1000)
  {
    LastSignalTime = 0;
    Error = 0;              // don't show an error when signal comes back
  }
  else
  {
    LastSignalTime = lastSignalTime;
  }
}
