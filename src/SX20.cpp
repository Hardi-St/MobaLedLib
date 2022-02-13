/*
 * SX20.cpp
 *
 *  Created on: 22.08.2019
 *  Changed on: 
 *  Version:    1.00
 *  Copyright:  Wilfried Maier
 *  Origin:		derived from Michael Blank www.opensx.net
 *
 *
 *    This version is an improved timming-version
 *    it corresponds to the SX-definitions
 *
 *	  Reading:  reads the dataline (T1) at the rising edge of T0
 *    Writing:  writes D at the rising edge of T0 until the end of T0
 *              for minimum half time of T0


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
 Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

 */

#ifdef ESP32

#include <Arduino.h>
#include "SX20.h"

SX20::SX20() {
}

void SX20::init(int SXSignalPin, int SXClockPin) {
	// initialize function
	// initialize pins and variables
	// and start looking for a SYNC signal

  _signalPin = SXSignalPin;
  
	pinMode(SXClockPin, INPUT);      // SX-clock is an input
	pinMode(_signalPin,INPUT);      // SX-data is also an input


	// reset sx variable to zero
	for (int i=0; i<112;i++) _sx[i]=0;
	_adrCount =0;

	// start always with search for SYNC
	_state = SYNC;
	_zeroCount = 0;
}

uint8_t SX20::readChannel(uint8_t channel) {
  // returns the value of a selectrix channel
  if (channel < MAX_CHANNEL_NUMBER) return _sx[channel];
  return 0;
}

void IRAM_ATTR SX20::switchAdr() {
	// a SYNC signal was received, now look for a valid
	// base address
	switch(_adrCount) {
		case 0:   // this is the GLEISSPANNUNG bit
			_track_power = _bit;
			break; // ignore
		case 1:
			break;
		case 4:
			break; // ignore
		case 2:  // B3
			bitWrite(_baseAdr,3,_bit);
			break;
		case 3:  // B2
			bitWrite(_baseAdr,2,_bit);
			break;
		case 5:  // B1
			bitWrite(_baseAdr,1,_bit);
			break;
		case 6:  // B0
			bitWrite(_baseAdr,0,_bit);
			break;
		case 7: // last "1"
			// _baseAdr is complete !

			// advance to next state - next we are looking
			// for the 7 data bytes (i.e. 7 SX Channels)
			_state = DATA;

			_dataFrameCount = 0;
			_dataBitCount = 0;
			_data=0;
			break;
	}
}

void IRAM_ATTR SX20::switchData() {
	// continue reading _data
	// a total of 7 DATA blocks will be received
	// for a certain base-address

	// calc sx channel from baseAdr and dataFrameCount
	_channel = (15-_baseAdr) + ((6-_dataFrameCount)<<4);
  
	switch(_dataBitCount) {
	case 2:  // "Trenn_bits"
	case 5:
	case 8:
		_dataBitCount++;
		break; // ignore
	case 0:  // D0
		_data=0;
		bitWrite(_data,0,_bit);
		_dataBitCount++;

		break;
	case 1:  // D1
		bitWrite(_data,1,_bit);
		_dataBitCount++;
		break;
	case 3:  // D2
		bitWrite(_data,2,_bit);
		_dataBitCount++;
		break;
	case 4:  // D3
		bitWrite(_data,3,_bit);
		_dataBitCount++;
		break;
	case 6:  // D4
		bitWrite(_data,4,_bit);
		_dataBitCount++;
		break;
	case 7:  // D5
		bitWrite(_data,5,_bit);
		_dataBitCount++;
		break;
	case 9:  // D6
		bitWrite(_data,6,_bit);
		_dataBitCount++;
		break;
	case 10: // D7
		bitWrite(_data,7,_bit);
		_dataBitCount++;
		break;

	case 11:  // == MAX_DATABITCOUNT
      // _bit value should always equal HIGH, if not set SYNC again
		if (_bit == 0)  {
			_dataFrameCount = 0;
			_state = SYNC;
			_zeroCount = 0;
			_dataBitCount = 0;
			break;
		}

		// copy _data byte to SX _channel
		_sx[_channel] = _data;

		// increment dataFrameCount to move on the next DATA byte
		// check, if we already reached the last DATA block - in this
		// case move on to the next SX-Datenpaket, i.e. look for SYNC
		_dataFrameCount ++;
		if (_dataFrameCount == MAX_DATACOUNT) {
			// and move on to SYNC _state
			_dataFrameCount=0;
			_state =SYNC;
			_zeroCount = 0;
			_dataBitCount=0;
      _lastPacketTime = millis();
		} else {
			_dataBitCount = 0;  // reset _bit counter
			_data = 0;
			// continue with reading next _data uint8_t
		}
	}  //end switch/case _dataBitCount
}


unsigned long SX20::lastPacketTime()
{
  return _lastPacketTime;  
}

bool SX20::trackPower()              // true if track power is on
{
  return _track_power;
}

void IRAM_ATTR SX20::isr() {

 	// interrupt service routine (AVR INT0)
	// driven by RISING clock signal T0 (SX-Pin)

	// measured max duration at 4MHz CPU clock: 5..18 usec  (i.e. less than 50usec!)
	// at 8 Mhz CPU clock (2.2.2012) with added "debounce" bitRead statements and blink led max 6.3...10 usec

	// 3 different states are distinguished
	//     1. SNYC = looking for a SYNC signal
	//     2. ADDR = (after SYNC received) look for base address (0..15)
	//     3. DATA = (after ADDR decoded) decode the 7 data-bytes

   _bit = digitalRead(_signalPin);    // reading the T1 data pin

	switch(_state) {
	case SYNC:
		if (_bit == LOW)  {
			_zeroCount++;
		} else {
			if (_zeroCount == 3)  {        // sync bits 0 0 0 1 found
				_state = ADDR;         // advance to next state
				_baseAdr = 0;   //init_vars_and_ports
				_adrCount = 0;  //init_vars_and_ports


			} else {  // no valid sync, try again ...
				_zeroCount = 0;       // reset _zeroCounter
			} // endif _zeroCount
		}  // endif _bit==LOW
		break;
	case ADDR:
		switchAdr();
		_adrCount++;
		break;
	case DATA:
		switchData();
	}
}

#endif // ESP32