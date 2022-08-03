#ifndef __MLLEXTENSION__
#define __MLLEXTENSION__

/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2022  Hardi Stengelin: MobaLedLib@gmx.de
 
 this file: Copyright (C) 2021-2022 Jürgen Winkler: MobaLedLib@a1.net

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
 
 
 Revision History :
~~~~~~~~~~~~~~~~~
17.11.21:  Versions 1.0 (Jürgen)

*/

#ifndef DEBUG_MLL_EXTENSIONS
  //#define DEBUG_MLL_EXTENSIONS 0x0f
#endif
#ifdef DEBUG_MLL_EXTENSIONS
  const char* DebugName = "ExtensionProcessor";
#endif  


class MLLExtension
{
  
	public:
    virtual void setup(MobaLedLib_C& mobaLedLib) = 0;
    // The main loop
    virtual void loop(MobaLedLib_C& mobaLedLib) {};
#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
    // On multicore CPUs the additional loop. FastLED and MobaLedLib is always processed in main (=other) loop
    virtual void loop2(MobaLedLib_C& mobaLedLib) {};
#endif    
  protected:
    CRGB* Get_LEDPtr(MobaLedLib_C& mobaLedLib, ledNr_t ledNr)
    {
        return &mobaLedLib.leds[ledNr];
    };
};

class ExtensionProcessor
{
  uint8_t  count = 0;
  MLLExtension** extensions;

  public:
  ExtensionProcessor(MLLExtension* extensions[], uint8_t count)
  {
#if (DEBUG_MLL_EXTENSIONS&0x01)==0x01
    { char s[80]; sprintf(s, "%s created with %d extensions", DebugName, count); Serial.println(s); Serial.flush();} // Debug
#endif      
    this->count = count;
    this->extensions = extensions;
  }
  
  virtual void setup(MobaLedLib_C& mobaLedLib)
  {
#if (DEBUG_MLL_EXTENSIONS&0x01)==0x01
    { char s[80]; sprintf(s, "%s calls setup for %d extensions", DebugName, count); Serial.println(s); Serial.flush();} // Debug
#endif      
    for (int i=0;i<count; i++)
    {
      extensions[i]->setup(mobaLedLib);
    }
#if (DEBUG_MLL_EXTENSIONS&0x01)==0x01
    { char s[80]; sprintf(s, "%s setup done", DebugName); Serial.println(s); Serial.flush();} // Debug
#endif      
  }
  
  virtual void loop(MobaLedLib_C& mobaLedLib)
  {
#if (DEBUG_MLL_EXTENSIONS&0x02)==0x02
    { char s[80]; sprintf(s, "%s calls loop for %d extensions", DebugName, count); Serial.println(s); Serial.flush();} // Debug
#endif      
    for (int i=0;i<count; i++)
    {
#if (DEBUG_MLL_EXTENSIONS&0x04)==0x04
    { char s[80]; sprintf(s, "%s calls loop for extension %d", DebugName, i); Serial.println(s); Serial.flush();} // Debug
#endif      
      extensions[i]->loop(mobaLedLib);
    } 
  }

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
  void loop2(MobaLedLib_C& mobaLedLib)
  {
#if (DEBUG_MLL_EXTENSIONS&0x02)==0x02
    { char s[80]; sprintf(s, "ExtensionProcessor calls loop2 for %d extensions", count); Serial.println(s); Serial.flush();} // Debug
#endif      
    for (int i=0;i<count; i++)
    {
#if (DEBUG_MLL_EXTENSIONS&0x04)==0x04
    { char s[80]; sprintf(s, "ExtensionProcessor calls loop2 for extension %d", i); Serial.println(s); Serial.flush();} // Debug
#endif      
      extensions[i]->loop2(mobaLedLib);
    } 
  }
#endif
  
};
#endif