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
*/

#ifndef DMXINTERFACE_H
#define DMXINTERFACE_H

#include <Arduino.h>

#ifdef ESP32
#include <driver/uart.h>
#define URAT_USED UART_NUM_1			// use UART1 
#endif

#if !defined(__AVR__) && !defined(ESP32) && !defined(__arm__)
	#error this library does not support this plattform
#endif

class DMXInterface
{
private:
  uint8_t* buffer;
  uint16_t numberOfChannels;
	unsigned long lastSend = 0;
#if  defined(__AVR__)
	volatile uint8_t *dmxPort;
	uint8_t dmxBit = 0;
	uint8_t dmxPin = 3;
#elif defined(ESP32)
	const char nullChar = 0;
#endif
	void dmxSendByte(uint8_t value);
	
public:
	void setup(uint8_t DMXSignalPin, uint8_t* buffer, uint16_t numberOfChannels);
	void loop();
	void sendSynchron(uint8_t* buffer, uint16_t numberOfChannels);
};

#endif
