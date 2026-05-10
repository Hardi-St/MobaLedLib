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
    // the callback is called every time an accessory command should be processed
    // receivedAddr: 1-2048
    // direction:    0:Thrown 1:Closed
    // outputPower:  0:Off 1:On
    // channel:      the first channel in the extAddrTable to be checked
    // extAddrTable: the table with the action definition
    // extAddrCount: number of actions in table
    //
    // returns 
    // 0  .. the command was processed, no further handling in core needed
    // >0 .. continue core processing
    virtual uint16_t onAccessoryCommand(MobaLedLib_C& mobaLedLib, uint16_t receivedAddr, uint8_t direction, uint8_t outputPower, uint16_t channel, const Ext_Addr_T extAddrTable[], uint16_t extAddrCount) { return receivedAddr; }
    
  protected:
    CRGB* Get_LEDPtr(MobaLedLib_C& mobaLedLib, ledNr_t ledNr)
    {
        return &mobaLedLib.leds[ledNr];
    };
};

#ifdef USE_EXTENSIONS_V2
class MLLExtensionV2 : public MLLExtension
{
	public:
    // the callback is called every time an accessory command should be processed
    // receivedAddr: 1-2048
    // direction:    0:Thrown 1:Closed
    // outputPower:  0:Off 1:On
    // channel:      the first channel in the extAddrTable to be checked
    // extAddrTable: the table with the action definition
    // extAddrCount: number of actions in table
    //
    // returns 
    // 0  .. the command was processed, no further handling in core needed
    // >0 .. continue core processing
    virtual uint16_t onAccessoryCommand(MobaLedLib_C& mobaLedLib, uint16_t receivedAddr, uint8_t direction, uint8_t outputPower, uint16_t channel, const Ext_Addr_T extAddrTable[], uint16_t extAddrCount) { return receivedAddr; }
};
#endif

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
#if (DEBUG_MLL_EXTENSIONS&0x81)==0x81
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
#if (DEBUG_MLL_EXTENSIONS&0x82)==0x82
    { char s[80]; sprintf(s, "%s calls loop for extension %d", DebugName, i); Serial.println(s); Serial.flush();} // Debug
#endif      
      extensions[i]->loop(mobaLedLib);
    } 
  }

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
  void loop2(MobaLedLib_C& mobaLedLib)
  {
#if (DEBUG_MLL_EXTENSIONS&0x04)==0x04
    { char s[80]; sprintf(s, "ExtensionProcessor calls loop2 for %d extensions", count); Serial.println(s); Serial.flush();} // Debug
#endif      
    for (int i=0;i<count; i++)
    {
#if (DEBUG_MLL_EXTENSIONS&0x84)==0x84
    { char s[80]; sprintf(s, "ExtensionProcessor calls loop2 for extension %d", i); Serial.println(s); Serial.flush();} // Debug
#endif      
      extensions[i]->loop2(mobaLedLib);
    } 
  }
#endif
  
#ifdef USE_EXTENSIONS_V2  
  uint16_t onAccessoryCommand(MobaLedLib_C& mobaLedLib, uint16_t receivedAddr, uint8_t direction, uint8_t outputPower, uint16_t channel, const Ext_Addr_T extAddrTable[], uint16_t extAddrCount)
  {
#if (DEBUG_MLL_EXTENSIONS&0x08)==0x08
    { char s[80]; sprintf(s, "%s calls onAccessoryCommand for %d extensions", DebugName, count); Serial.println(s); Serial.flush();} // Debug
#endif    
    for (int i=0;i<count; i++)
    {
#if (DEBUG_MLL_EXTENSIONS&0x88)==0x88
      { char s[80]; sprintf(s, "%s calls onAccessoryCommand(%d,%d,%d,%d,*,%d) for extension %d", DebugName, receivedAddr, direction, outputPower, channel, extAddrCount, i); Serial.println(s); Serial.flush();} // Debug
#endif      
      receivedAddr = extensions[i]->onAccessoryCommand(mobaLedLib, receivedAddr, direction, outputPower, channel, extAddrTable, extAddrCount);
      if (receivedAddr==0) return 0;
    } 
    return receivedAddr;
  }
#endif
  
};
#endif