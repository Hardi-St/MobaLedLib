/*
 * SX20.h
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

#ifndef SX20_H_
#define SX20_H_

#include <inttypes.h>
  

// define arduino pins, ports and bits
#if defined (__AVR_ATmega328P__) // default settings for Arduino Pro Mini 5V 16MHz

	#define SX_CLK            2			// T0 - line
	#define SX_CLK_DDR        DDRD
	#define SX_CLK_PORT       PORTD
	#define SX_CLK_PORTPIN    PORTD2
	#define SX_CLK_PINREG     PIND

	#define SX_DATA           4			// T1 - line
	#define SX_DATA_DDR       DDRD
	#define SX_DATA_PORT      PORTD
	#define SX_DATA_PORTPIN   PORTD4
	#define SX_DATA_PINREG    PIND

	#define SX_OUT            6			// D - line
	#define SX_OUT_DDR        DDRD
	#define SX_OUT_PORT       PORTD
	#define SX_OUT_PORTPIN    PORTD6

	#define SCOPE            A4
	#define SCOPE_DDR        DDRC
	#define SCOPE_PORT       PORTC
	#define SCOPE_PORTPIN    PORTC4

#else
	#error "--> CPU settings not supported in SX20.h <--"
#endif

// defines for state machine
#define SYNC  0
#define ADDR  1
#define DATA  2

#define MAX_DATACOUNT    7		// 7 dataframes in 1 SYNC Channel
#define MAX_DATABITCOUNT 12		// 12 bits in 1 frame
#define MAX_CHANNEL_NUMBER 112	// SX channels

class SX20 {
public:
    SX20();
    void init(void);
    void init(uint8_t);    
    uint8_t writing();
    uint8_t getTrackBit();
    void setTrackBit(uint8_t);
    void isr(void);           
	uint8_t readChannel(uint8_t);
	void writeChannel(uint8_t , uint8_t );
	void SetBit (uint8_t , uint8_t );
	void ClearBit (uint8_t , uint8_t);


private:
    void switchAdr(void);
    void switchData(void);

    uint8_t _toggle;
    uint8_t _zeroCount;
    uint8_t _adrCount;
    uint8_t _state;
    uint8_t _dataBitCount;    // bit counting
    uint8_t _dataFrameCount;  // frame counting
    uint8_t _trackBit;

    uint8_t _data;            // 1 data uint8_t
    uint8_t _baseAdr;         // base address
    uint8_t _triggerAdr;      // SX address to trigger scope
    uint8_t _scopeFlag;       // generate scope trigger signal if != 0

    volatile uint8_t _sx_write_channel;	// stores channel number to write to
    volatile uint8_t _sx_write_data;	// data to write
	volatile uint8_t _sx_bit_register;	// Bit-Positionen
	volatile uint8_t _track_power;

    volatile uint8_t _sx_write_busy;	// waiting to send data, cannot accept new data
    uint8_t _sx_writing;				// currently writing a byte
    
    uint8_t _bit;						// bit to store the dataline
    uint8_t _sx[MAX_CHANNEL_NUMBER];	// to store the SX data

    uint8_t _channel;   // channel from 0 to 15, B3..B0 in sync data
    // 0  0  0  1  X   1  B3  B2  1  B1  B0  1 == sync frame of 12 bits
    

    /* SX Timing
     1   Bit             50 us
     1   Kanal          600 us (= 12 Bit)
     1   Grundrahmen    ca. 4,8 ms
     1   Gesamtrahmen   ca.  80 ms (= 16 Grundrahmen)  */
};

#endif /* SX20_H_ */
