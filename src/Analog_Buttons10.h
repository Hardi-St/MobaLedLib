#ifndef _ANALOG_BUTTONS10_H_
#define _ANALOG_BUTTONS10_H_
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

 Hardware:
 ~~~~~~~~~
 This modul could be used to read up to 10 push buttons which are connected to one                        Main board
 analog input of an arduino.                                                                        .........................
                                                                                                    :
                                                                                                    :    +5V
 Only two wires are needed to read in 10 buttons.                                                   :     |
 The buttons are connected like this:                                                               :    [R] 22K
                                                                                                    :     |
  .---------o---------o---------o---------o---------o---------o---------o---------o---------o-------+-----o------- A6 Nano
  |         |         |         |         |         |         |         |         |         |       :     |
  *         *         *         *         *         *         *         *         *         *       :     |
 /         /         /         /         /         /         /         /         /         /        :    === 1uF
  |         |         |         |         |         |         |         |         |         |       :     |
 [R]       [R]       [R]       [R]       [R]       [R]       [R]       [R]       [R]       [R]      :     |
  | 0 Ohm   |2k2      |4k7      |8k2      |15k      |22k      |33k      |47k      |68k      |150k   :     |
  '---------o---------o---------o---------o---------o---------o---------o---------o---------o-------+-----o------- GND
                                                                                                    :
                                                                                                    '.......................
 Attention don't forgot the 22K resistor to +5V and the 1uF capacitor on the main board
 next to the Nano.

*/

#ifdef ESP32
	#include "AnalogScannerESP32.h"
  extern AnalogScannerESP32 scanner;
#else
	#include <AnalogScanner.h>
  extern AnalogScanner scanner;
#endif

// Table containing the delta from one compare value to the next. We don't store the 16 bit values to save memory.
#ifdef ESP32
//const PROGMEM uint8_t DeltaTab[10] = { 33, 76, 101, 123, 114, 102, 95, 70, 114, 98};
const PROGMEM uint8_t DeltaTab[10] = { 30, 74, 96, 120, 114, 98, 88, 80, 145, 80 };
/*
ESP32 ADC Values - minimum per key
	
Key	Jürgen 	Dominik
01	00		00
02	62		57
03	152		143
04	263		240
05	399		376
06	491		468
07	602		565
08	681		642
09	828		717
10	910		856
*/ 

#else
const PROGMEM uint8_t DeltaTab[10] = { 88, 86, 89, 113, 112, 96, 88, 77, 94, 101};
#endif

//:::::::::::::::::::::::
class Analog_Buttons10_C   // Attention this class will be moved to the library in the next version
//:::::::::::::::::::::::
{
public:
  //-------------------------------------
  Analog_Buttons10_C(uint8_t AnalogPinNr) // Constructor
  //-------------------------------------
  {
    APin = AnalogPinNr;
    LastCheck = 0;
    LastKey   = 0xFFFF;
    SameCnt   = 0;
  }

  //------------
  uint16_t Get()
  //------------
  // Must be called periodic in the loop() function
  {
    uint8_t t = millis() % 0xFF;
    if ((uint8_t)(t - LastCheck) > 20) // Check every 20 ms
       {
       LastCheck = t;
       uint16_t Key = Get_Act_Key();
       if (Key == LastKey)
					{
          if (SameCnt<4) SameCnt++;
					}
       else 
					{
					SameCnt = 0;
					}
       LastKey = Key;
       }
    if (SameCnt >=127)
       {
       SameCnt = 4;
       }			 
    if (SameCnt >= 4)    // Wenn 4 mal hintereinander der gleiche Wert gelesen wird, dann wird der Taster als gedrueckt gemeldet
         return LastKey; // Das ist wichtig weil der analog Wert mit einem 1uF Kondensator gefiltert wird
    else return 0;
  }

//uint16_t maxVal = 0;
private:
  //--------------------
  uint16_t Get_Act_Key()
  //--------------------
 {
    const uint8_t *p = DeltaTab; // Table containing the delta from one compare value to the next
    uint16_t Cmp = 0;
    uint16_t Bit = 1;
    uint16_t val = scanner.getValue(APin);
		//if (val>maxVal) maxVal = val;
    while (Bit < 513)
        {
        Cmp += pgm_read_byte_near(p);
        if (val < Cmp)
				{
					//Serial.printf("cmp=%d, val=%d, bit=%d, maxVal=%d\n", Cmp, val, Bit, maxVal);
					return Bit;
				}
        p++;
        Bit <<= 1;
        }
    return 0;
  }

private:
  uint8_t  APin;
  uint8_t  LastCheck;
  uint16_t LastKey;
  uint8_t  SameCnt;
};
#endif // _ANALOG_BUTTONS10_H_
