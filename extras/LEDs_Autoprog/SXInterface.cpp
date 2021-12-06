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
- https://www.mec-arnsdorf.de/index.php/selectrix/selectrix-protokoll/

Revision History :
~~~~~~~~~~~~~~~~~
06.12.21:  Versions 1.0 (Jürgen)
*/

#if defined(ESP32)

#include "SXInterface.h"
#include "SX20.h"
#include "Helpers.h"

#define MAX_USED_CHANNEL_NUMBER   104

static SX20 sx;                                // Instance of the Selectrix class
byte SXInterface::oldSx[MAX_CHANNEL_NUMBER];

//-----------
void SXInterface::setup(int SXSignalPin, int SXClockPin, int statusLedPin, InMemoryStream& stream)
//-----------
{
  CommInterface::setup(statusLedPin, stream);
  CommInterface::addToSendBuffer((char*)"\nSelectrix_Interface\n");          // Don't use Serial.print because characters get lost if the LED-Arduinio updates the LEDs
  
  for (int i=0; i<MAX_USED_CHANNEL_NUMBER; i++)
  {
    oldSx[i]=0;   // initialize to zero
    #if FILTER_COUNT > 0
    cntSx[i]=0;
    #endif
  }
  
  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  sx.init(SXSignalPin, SXClockPin);

  attachInterrupt(digitalPinToInterrupt(SXClockPin), SXInterface::isr, RISING);      // only the rising edge may be triggered
  
  printf("SXInterface using pin %d/%4 has been started.\r\n", SXSignalPin, SXClockPin);
}

void SXInterface::isr()
{
  sx.isr();
}

//----------------------------------------------------------------------------------------------
void SXInterface::checkByte(uint8_t i, uint8_t d)
//----------------------------------------------------------------------------------------------
{
#if FILTER_COUNT > 0
  if (oldSx[i] != d)  // data have changed on SX bus
  {
    if (cntSx[i]++ == 0) // First time the change is detected
    newSx[i] = d;   // Store the new value
    else 
    {
      if (newSx[i] == d) // Still the same byte ?
      {
        //Serial.print(millis()); Serial.print("cntSx="); Serial.println(cntSx[i]);
        if (cntSx[i] >= FILTER_COUNT)
        {
          printSXValue(i, d, oldSx[i]);   // send new value to serial port
          oldSx[i] = d;
          cntSx[i] = 0;
        }
      }
      else 
      {
        cntSx[i] = 1;
        newSx[i] = d;
      }
    }
  }
  else cntSx[i] = 0;
#else // Filter is disabled
  if (oldSx[i] != d)                                // data have changed on SX bus
  {
    printSXValue(i, d, oldSx[i]);                // send new value to serial port
    oldSx[i] = d;
  }
#endif // FILTER_COUNT
}

uint8_t lastD = 0;
//-----------
void SXInterface::process(){

  for (int i=0; i<MAX_USED_CHANNEL_NUMBER; i++)
  {
    byte d=sx.readChannel(i);
    checkByte(i, d);
    /*      if (i<1 && d!=lastD)
      {
        lastD=d;
        if (d<16) Serial.print("0");
        Serial.print(d,HEX);
        Serial.print(" ");
      }*/
  }

  setLastSignalTime(sx.lastPacketTime());
  CommInterface::process();                                                                             // 13.05.20:
}

//-----------------------------------------------------
void SXInterface::print2Buf(uint16_t addr, uint8_t dir, uint8_t pow)
//-----------------------------------------------------
{
  char s[20];
  sprintf(s, "@%4i %02X %02X\n", addr, dir, pow?1:0);
  SXInterface::addToSendBuffer(s);
}

//-----------------------------------------
void SXInterface::printSXValue(int i,int data, byte old)
//-----------------------------------------
{
  // One selectrix channel is mapped to 8 DCC adresses.
  // selectrix 1 => DCC  4-11
  //           2 => DCC 12-19
  // Only the "key pressed" signal is generated. It's not possible to detect the release
  uint16_t addr   = i*8;
  uint8_t changed = data ^ old;
  for (uint8_t ci = 0; ci < 8; ci++)
  {
    if (i >= 0 && i <= 99 && changed & B00000001) print2Buf(addr, data & B00000001, sx.trackPower());	// WM geaendert auf >= 0
    addr++;
    data    >>= 1;
    changed >>= 1;
  }
}
#endif