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
#include "SoundChannelMacros.h"

/****************************************/
/* DEBUG_SOUND_CHANNEL is bit coded     */
/*                                      */
/* 0x01 show generic command executing  */
/* 0x02 show sound command executing    */
/* 0x04 show sound command details      */
/* 0x08 show bytes sent to serial line  */
/* 0x80 show all command handling calls */
/****************************************/

#ifndef DEBUG_SOUND_CHANNEL
  //#define DEBUG_SOUND_CHANNEL 0x0f
#endif

class SoundPlayerPro
{
  public:
  virtual void handle(uint8_t command, const uint8_t* arguments) = 0;
  virtual bool process() = 0;
  
#if (DEBUG_SOUND_CHANNEL&0x08)==0x08
  void dump(const uint8_t *buffer, size_t size)
  {
    while(size--)
    {
      uint8_t by = *(buffer++);
      if (by<16) 
        Serial.print(" 0");
      else
        Serial.print(" ");
        
      Serial.print(by,HEX);
    } 
  }
#endif  
};

#include "SoundPlayerProTiny.h"

class SoundProcessorPro
{
  private:
  SoundPlayerPro** soundPlayers;
  uint8_t playerCount;
  unsigned long lastMillis = 0;
  
  public:
  SoundProcessorPro(SoundPlayerPro* soundPlayers[], uint8_t playerCount)
  {
    this->soundPlayers = soundPlayers;
    this->playerCount = playerCount;
#if defined(ESP32)
  #if ESP_IDF_VERSION_MAJOR<4
    vPortCPUInitializeMutex(&soundProcessor_mutex);
  #else
    soundProcessor_mutex=portMUX_INITIALIZER_UNLOCKED;
  #endif
  
#endif
  }

  static uint8_t GetSoundCommandLength(uint8_t cmd)
  {
    // commands 0..7 have no argument, command 8..13 have one argument, others have two arguments
    if (cmd>=14) return 4;
    if (cmd>=8) return 3;
    return 2;
  }
  
  uint8_t handle(const uint8_t* arguments, bool doProcess)
  {
      
    // upper 8 = cmdAndIndex, lower 8 = InCh
    uint16_t tmp = pgm_read_word_near(arguments);
    uint16_t cmdAndIndex = tmp>>8;
    uint8_t len = GetSoundCommandLength(cmdAndIndex&0x0f);
    
#if (DEBUG_SOUND_CHANNEL&0x80)==0x80
    { char s[80]; sprintf(s, "Command %d on module %d.", cmdAndIndex&0x0f, (cmdAndIndex >>4)&0x0f); Serial.println(s); } // Debug
#endif      
    if (doProcess)
    {
      if (pMobaLedLib!=NULL && pMobaLedLib->Get_Input(tmp&0xff)==INP_TURNED_ON)
      {
#if (DEBUG_SOUND_CHANNEL&0x01)==0x01
        { char s[80]; sprintf(s, "Command %d on module %d added to queue.", cmdAndIndex&0x0f, (cmdAndIndex>>4)&0x0f); Serial.println(s); Serial.flush();} // Debug
#endif        
        soundPlayers[(cmdAndIndex>>4)&0x0f]->handle(cmdAndIndex&0x0f, arguments+2);        
      }
    }      
    return len;
  }
  
  // check the command queue and send out max. one message per process call
  bool process()
  {
    if ((millis()-lastMillis)<10) return true;
    lastMillis = millis();
    
    for ( uint8_t idx=3; idx<min(4,playerCount); idx++)
    {
        soundPlayers[idx]->process();
    }
    return true;    
  }
  
  /*
  uint8_t handlePro(const uint8_t* arguments, bool doProcess)
  {
      
    // upper 8 = cmdAndIndex, lower 8 = InCh
    uint16_t tmp = pgm_read_word_near(arguments);
    uint16_t cmdAndIndex = tmp>>8;
    uint8_t len = GetSoundCommandLength(cmdAndIndex&0x0f);
 
#if (DEBUG_SOUND_CHANNEL&0x80)==0x80
    { char s[80]; sprintf(s, "Command %d on module %d.", cmdAndIndex&0x0f, (cmdAndIndex >>4)&0x0f); Serial.println(s); } // Debug
#endif      
    if (doProcess)
    {
      if (pMobaLedLib!=NULL && pMobaLedLib->Get_Input(tmp&0xff)==INP_TURNED_ON)
      {
#if (DEBUG_SOUND_CHANNEL&0x01)==0x01
        { char s[80]; sprintf(s, "Command %d on module %d added to queue.", cmdAndIndex&0x0f, (cmdAndIndex>>4)&0x0f); Serial.println(s); Serial.flush();} // Debug
#endif        
        SoundPlayerPro* sp = soundPlayers[cmdAndIndex>>6)];

        SOUNDPROCESSOR_MUTEX_ENTER
        sp->handle(cmd, arguments, len);
        SOUNDPROCESSOR_MUTEX_EXIT;
      }
    }      
    return len;
  }*/
};
