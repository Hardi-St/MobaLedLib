/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de
 
 this file: Copyright (C) 2020 Jürgen Winkler: MobaLedLib@a1.net

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
 
 
 this module implements the MLL Zentrale UserInterface on a SSD1306 display
 it uses U8g2lib.h
 
 see https://github.com/olikraus/u8g2/wiki/u8g2reference
 fonts: https://github.com/olikraus/u8g2/wiki/fntlistall
 
Revision History :
~~~~~~~~~~~~~~~~~
10.11.20:  Versions 1.0 (Jürgen)
	 
*/
#ifdef ESP32

#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "UserInterface.h"

U8G2_SSD1306_128X64_NONAME_F_SW_I2C dsp(U8G2_R0, /* clock=*/ SCL, /* data=*/ SDA, /* reset=*/ U8X8_PIN_NONE);   // All Boards without Reset of the Display

class SSD1306UI : public UserInterface
{
	bool connected = false;
	bool update = false;
	String ipAddress;
public:		
	SSD1306UI() {};
	
	void setup() 
	{
		dsp.begin();

		dsp.clearBuffer();
		dsp.setFont(u8g2_font_logisoso18_tf);	// choose a suitable font
		dsp.setFontPosTop();
		dsp.drawStr(4, 16, "MobaLedLib");	// write something to the internal memory
		dsp.drawStr(34, 40, "ESP32");	// write something to the internal memory
		dsp.sendBuffer();					// transfer internal memory to the display
	}

	void setConnected(bool connected) 
	{
	  this->connected = connected;
		update = true;
	}

	void setIPAddress(String ipAddress)
	{
	  this->ipAddress = ipAddress;
		update = true;
	}

	void loop() 
	{
		if (!update) return;
		
		update = false;
		dsp.clearBuffer();
		dsp.setFont(u8g2_font_ncenB10_tr);	// choose a suitable font
		dsp.drawStr(0, 0, connected ? "WIFI connected" : "network error");
		dsp.drawStr(0, 16, ipAddress.c_str());
		dsp.sendBuffer();
	}
};

static SSD1306UI ssd1306UI;

UserInterface* getUserInterface() 
{
	return &ssd1306UI;
}
#endif