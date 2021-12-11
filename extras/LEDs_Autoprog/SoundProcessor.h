#ifndef SoundProcessor_H
#define SoundProcessor_H

/* 
mit Beta 3.0.0L
Sketch uses 22462 bytes 
Global variables use 726 bytes

mit MP3TF und Random extension
Sketch uses 22510 bytes
Global variables use 714 bytes

after refactor
Sketch uses 22478 bytes
Global variables use 712 bytes

*/

/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de
 
 this file: Copyright (C) 2021 Jürgen Winkler: MobaLedLib@a1.net

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
07.10.21:  Versions 1.0 (Jürgen)

TODO: why does MP3-TF-16P doesn't work on Pin D13?

*/

#if defined(ESP32)
  #include <SoftwareSerial.h>
  #define SOFTWARE_SERIAL_TYPE SoftwareSerial
  #define SOFTWARE_SERIAL(pin) new SOFTWARE_SERIAL_TYPE(-1, txPin)
#else
  #include "SoftwareSerialTX.h"
  #define SOFTWARE_SERIAL_TYPE SoftwareSerialTX
  #define SOFTWARE_SERIAL(pin) new SOFTWARE_SERIAL_TYPE(txPin)
#endif

#if defined(ESP32)
	static portMUX_TYPE			soundProcessor_mutex;
	#define SOUNDPROCESSOR_MUTEX_ENTER   portENTER_CRITICAL(&soundProcessor_mutex);
	#define SOUNDPROCESSOR_MUTEX_EXIT    portEXIT_CRITICAL(&soundProcessor_mutex);
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
	auto_init_mutex(soundProcessor_mutex);
	#define SOUNDPROCESSOR_MUTEX_ENTER   mutex_enter_blocking(&soundProcessor_mutex);
	#define SOUNDPROCESSOR_MUTEX_EXIT    mutex_exit(&soundProcessor_mutex);
#else  
	#define SOUNDPROCESSOR_MUTEX_ENTER   
	#define SOUNDPROCESSOR_MUTEX_EXIT    
#endif

  
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

extern MobaLedLib_C* MobaLedLib;

class SoundPlayer 
{
  public:
  virtual void process(const uint8_t command, const uint8_t* arguments) = 0;
  bool available() 
  {
#if (DEBUG_SOUND_CHANNEL&0x01)==0x01
        { char s[80]; sprintf(s, "available wait %dms %d", waitUntil==0 ? 0 : waitUntil-millis(),millis()>=waitUntil); Serial.println(s); Serial.flush();} // Debug
#endif        
    
    return waitUntil==0 || millis()>=waitUntil; 
  }
  SOFTWARE_SERIAL_TYPE* serialLine;

  protected:
  unsigned long waitUntil = 0;
    
  
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

#include "JQ6500SoundPlayer.h"
#include "MP3TF16PSoundPlayer.h"


class SoundProcessor
{
  private:
  uint8_t* commandBuffer;
  uint8_t  capacity;
  uint8_t  count = 0;
  SoundPlayer** soundPlayers;
  
  public:
  SoundProcessor(uint8_t* commandBuffer, uint8_t capacity, SoundPlayer* soundPlayers[])
  {
    this->commandBuffer = commandBuffer; 
    this->capacity = capacity;
    this->soundPlayers = soundPlayers;
#if defined(ESP32)
    vPortCPUInitializeMutex(&soundProcessor_mutex);
#endif
  }

  static SOFTWARE_SERIAL_TYPE* CreateSoftwareSerial(const byte txPin, uint16_t baudrate)
  {
      SOFTWARE_SERIAL_TYPE* result = SOFTWARE_SERIAL(txPin);
      result->begin(baudrate);
      return result;
  }
  
  uint8_t handle(const uint8_t* arguments, bool doProcess)
  {
      
    // upper 8 = cmdAndIndex, lower 8 = InCh
    uint16_t tmp = pgm_read_word_near(arguments);
    uint16_t cmdAndIndex = tmp>>8;
    uint8_t len = GetSoundCommandLength(cmdAndIndex&0x0f);
    
    while (count+len>=capacity)
    {
       if (!process()) delay(1);
    }
    
#if (DEBUG_SOUND_CHANNEL&0x80)==0x80
    { char s[80]; sprintf(s, "Command %d on module %d.", cmdAndIndex&0x0f, (cmdAndIndex >>4)&0x0f); Serial.println(s); } // Debug
#endif      
    if (doProcess)
    {
      if (MobaLedLib!=NULL && MobaLedLib->Get_Input(tmp&0xff)==INP_TURNED_ON)
      {
#if (DEBUG_SOUND_CHANNEL&0x01)==0x01
        { char s[80]; sprintf(s, "Command %d on module %d added to queue.", cmdAndIndex&0x0f, (cmdAndIndex>>4)&0x0f); Serial.println(s); Serial.flush();} // Debug
#endif        
        SOUNDPROCESSOR_MUTEX_ENTER;
        for (int i=1;i<=len;i++)
        {
          commandBuffer[count++] = pgm_read_byte_near(arguments+i);
        }
        SOUNDPROCESSOR_MUTEX_EXIT;
      }
    }      
    return len;
  }
  
  private:
    
  static uint8_t GetSoundCommandLength(uint8_t cmd)
  {
    // commands 0..7 have no argument, command 8..13 have one argument, others have two arguments
    if (cmd>=14) return 4;
    if (cmd>=8) return 3;
    return 2;
  }
  
  public:
  
    
  // check the command queue and send out max. one message per process call
  bool process()
  {
      // check for commands in queue
      if (!count) return true;
      
#if (DEBUG_SOUND_CHANNEL&0x08)==0x08
      { char s[80]; sprintf(s, "commandBuffer count = %d.", count); Serial.println(s); Serial.flush();} // Debug
#endif
      uint8_t cmd = commandBuffer[0]&0x0f;
      uint8_t len = GetSoundCommandLength(cmd);
      
      // create a "PlayTrackIndex" message out of the PlayRandom
      if (cmd==SOUND_CHANNEL_CMD_PLAY_RANDOM)   // play random track
      {
        commandBuffer[1] += random8(commandBuffer[2]);      // the argument contain the trackNumber -> add the random number
        cmd = SOUND_CHANNEL_CMD_PLAY_TRACK;
      }
      
      SoundPlayer* sp = soundPlayers[(commandBuffer[0]>>4)&0x0f];
      if (sp->available())
      {
          sp->process(cmd, &commandBuffer[1]);
          count -= len;
          memmove(commandBuffer, commandBuffer+len, count);
          return true;
      }
      return false;
  }
};

#endif
