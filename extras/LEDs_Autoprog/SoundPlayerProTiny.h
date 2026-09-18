#pragma once

/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2025  Hardi Stengelin: MobaLedLib@gmx.de
 
 this file: Copyright (C) 2025 Jürgen Winkler: MobaLedLib@a1.net

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
15.03.25:  Versions 1.0 (Jürgen)

*/

#include "SoundProcessor.h"
#include "SoundChannelMacros.h"

extern MobaLedLib_C* pMobaLedLib;
#ifndef _SOUND_PROBUFFER_SIZE
#define _SOUND_PROBUFFER_SIZE 15*3    // 5 commands for 3 modules
#endif

class SoundModulePro
{
  protected:
    ledNr_t ledOffset;
    uint8_t ledBuffer[_SOUND_PROBUFFER_SIZE];
    uint8_t count = 0;
    uint16_t moduleConfiguration = 0;
  
  public:
    // moduleConfiguration contains 3 3 bit value specifying the hardware types of the sound module
    // in bits 0..8
    // bits 9..16 are reserved
     
    SoundModulePro(ledNr_t ledOffset, uint16_t moduleConfiguration)
    {
      this->ledOffset = ledOffset;  
      this->moduleConfiguration = moduleConfiguration;      
      SetLeds(0,moduleConfiguration, moduleConfiguration>>8);
    }
 
    // check the command queue and send out max. one message per process call
    void loop()
    {
      // check for commands in queue
      if (count<3)
      {
        if (pMobaLedLib->leds[ledOffset].red !=1) 
        {
  #if (DEBUG_SOUND_CHANNEL&0x08)==0x08
        { char s[80]; sprintf(s, "clear ledBuffer[%d], moduleConfiguration=%d %d", ledOffset, moduleConfiguration, moduleConfiguration>>8); Serial.println(s); Serial.flush();} // Debug
  #endif
          SetLeds(1,moduleConfiguration, moduleConfiguration>>8);
        }
      }        
      else
      {
      
  #if (DEBUG_SOUND_CHANNEL&0x08)==0x08
        { char s[80]; sprintf(s, "ledBuffer[%d] count = %d val = %d %d %d.", ledOffset, count, ledBuffer[0], ledBuffer[1], ledBuffer[2]); Serial.println(s); Serial.flush();} // Debug
  #endif
        SetLeds(ledBuffer[0], ledBuffer[1], ledBuffer[2]);
        count -= 3;
        memmove(ledBuffer, ledBuffer+3, count);
      }
    }  
  
    // index has range 0..3   2 bit
    // cmd has range 0..15    4 bit
    // arg1 has range 0..255  8 bit
    // arg2 has range 0..31   5 bit
    
    void AddToBuffer(uint8_t index, uint8_t cmd, uint8_t arg1, uint8_t arg2)
    {
      if ((count+3)<=_SOUND_PROBUFFER_SIZE)
      {
        // bits 4..5 reserved for future use
        ledBuffer[count++] = (cmd & 0x0F) | (index << 6);

        ledBuffer[count++] = arg1;

        // bits 5..7 reserved for checksum
        ledBuffer[count++] = arg2 & 0x1f;
      }
      // else todo?
    }
    
  protected:
    void SetLeds(uint8_t cmd, uint8_t arg1, uint8_t arg2)
    {
      if ((count+3)>_SOUND_PROBUFFER_SIZE)
      {
         // todo
        return;
      }      
      uint8_t arg2_payload = arg2 & 0x1F; // Mask to keep only bits 0-4 (0b00011111)
      uint8_t sum = (uint8_t)(cmd + arg1 + arg2_payload);
      uint8_t hybrid_value = sum ^ (cmd ^ 0x55) ^ (arg1 ^ 0x07) ^ (arg2_payload ^ 0x33);
      uint8_t checksum = (uint8_t)(hybrid_value & 0x07);
    
      pMobaLedLib->leds[ledOffset].red   = cmd;
      pMobaLedLib->leds[ledOffset].green = arg1;
      pMobaLedLib->leds[ledOffset].blue = arg2_payload | (checksum << 5);
    }
};

class SoundPlayerPro : public SoundPlayer
{
  protected:
    uint8_t playerIndex;
    SoundModulePro* soundModulePro;
  
  public:
    // moduleIndex is the index of the sound hardware attached to the SoundPlayerModule
    SoundPlayerPro(SoundModulePro* module, uint8_t playerIndex)
    {
      this->playerIndex = playerIndex;  
      this->soundModulePro = module;
      this->type = 1;
    }
 
    void process(uint8_t command, const uint8_t* arguments) override
    {
        uint8_t arg0 = *arguments;
        uint8_t arg1 = *(arguments+1);
#if (DEBUG_SOUND_CHANNEL&0x08)==0x08
        { char s[80]; sprintf(s, "add to ledBuffer(%d) command = %d arg0=%d arg1=%d.", playerIndex, command, arg0, arg1); Serial.println(s); Serial.flush();} // Debug
#endif
      if (SoundProcessor::GetSoundCommandLength(command) == 2)
      {
        // one byte commands are sent as command 0, subcommand in arg1
        soundModulePro->AddToBuffer(playerIndex+1, 0, command, 0);
      }
      else
      {
        soundModulePro->AddToBuffer(playerIndex+1, command, arg0, arg1);
      }
      // else TODO
    }
    
    void loop() override
    {
      if (playerIndex==0) soundModulePro->loop();      
    }  
};
