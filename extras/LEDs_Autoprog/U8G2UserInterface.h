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
10.11.20:  Version 1.0 (Jürgen)
18.01.25:  Version 1.1 (Jürgen)
	 
*/
#ifdef ESP32

#include <U8g2lib.h>
#ifdef U8X8_HAVE_HW_I2C
#include <Wire.h>
#endif

#include "UserInterface.h"

class U8G2UserInterface : public UserInterface
{
	bool connected = false;
	bool update = false;
	String ipAddress;
	uint32_t lastUIUpdate = 0;
	int delayCount = -1;    // display of faults is turned off
	int reviveCount = 0;    
	int lastDelayCount = -1;
	int lastReviveCount = 0;    
  U8G2* dsp;
  
public:		
	U8G2UserInterface(U8G2* display)
  {
    dsp = display;
  }
	
	void setup() 
	{
		dsp->begin();

		dsp->clearBuffer();
		dsp->setFont(u8g2_font_logisoso18_tf);	// choose a suitable font
		dsp->setFontPosTop();
		dsp->drawStr(4, 16, "MobaLedLib");	// write something to the internal memory
		dsp->drawStr(34, 40, "ESP32");	// write something to the internal memory
		dsp->sendBuffer();					// transfer internal memory to the display
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

  void setCounters(int delayCount, int reviveCount)
  {
    this->delayCount = delayCount;
    this->reviveCount = reviveCount;
    update = true;
  }
	void loop() 
	{
    if (delayCount!=-1)     // display of fault count is on?
    {
      if (lastUIUpdate==0 || ((millis()-lastUIUpdate)>60*1000) || lastDelayCount!=delayCount || lastReviveCount!=reviveCount)
      {
        lastDelayCount=delayCount;
        lastReviveCount=reviveCount;
        
        char buffer[20];
        lastUIUpdate = millis();
        sprintf(&buffer[0], "%03d:%02d %d %d", lastUIUpdate/(60*60*1000), (lastUIUpdate/(60*1000)%60), reviveCount, delayCount);
        dsp->clearBuffer();
        dsp->setFont(u8g2_font_ncenB10_tr);	// choose a suitable font
        dsp->drawStr(4, 0, "Uptime/Fail");	
        dsp->drawStr(4,40, &buffer[0]);	
        dsp->sendBuffer();
      }
      return;
    }
    else
    {  
		if (!update) return;
		
		update = false;
		dsp->clearBuffer();
		dsp->setFont(u8g2_font_ncenB10_tr);	// choose a suitable font
		dsp->drawStr(0, 0, connected ? "WIFI connected" : "network error");
		dsp->drawStr(0, 16, ipAddress.c_str());
		dsp->sendBuffer();
    }
	}
};

UserInterface* getUserInterface() 
{
#if OLED_TYP==1         // 1.3"  I2C - for ESP32 adapter board
  return new U8G2UserInterface(new U8G2_SSD1306_128X64_NONAME_F_SW_I2C(U8G2_R0, SCL, SDA));
#elif OLED_TYP==2        // 0.96" I2C - for ESP32 mainboard
  return new U8G2UserInterface(new U8G2_SH1106_128X64_NONAME_F_HW_I2C(U8G2_R0, SCL, SDA));
#else
   #error "OLED_TYP not supported yet ;-("
  return NULL;
#endif
}
#endif