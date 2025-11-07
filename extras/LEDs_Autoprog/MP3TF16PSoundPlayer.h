#ifndef MP3TF16PSoundPlayer_H
#define MP3TF16PSoundPlayer_H

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
02.11.21:  Versions 1.0 (Jürgen)

TODO: why does MP3-TF-16P doesn't work on Pin D13?

*/

#ifndef DELAY_AFTER_MP3TF16P_PACKET
#define DELAY_AFTER_MP3TF16P_PACKET 100
#endif

/*
  
  MP3-TF-16P Integration
  
  Module also know as DFPlayer
  
  The module is available with different chips mounted
  
  1 YX5200-24SS
  2 YX5300
  3 GD3200B 201124565B
  4 AA19HF8328-94
  5 MH2024K-24SS/MH2024K-16SS
  
  4 & 5 seam to work with MLL Analog control, 3 works with MLL with some hardware and software adaptions  (3.0.0 Beta >D3)
  
  1 and 2 are not tested yet, but that one's seem to work just fine with the standard commands.
  (http://www.thebackshed.com/forum/ViewTopic_mobile.php?FID=16&TID=11977)
  
  5 has a problem with the serial interface, some messages fail when sending it with the correct protocol (PlayIndex, SetVolume..). 
  For these messages the two checksum bytes must be ommited.
  
  
  see also https://www.stummiforum.de/t165060f7-MobaLedLib-LEDs-Servos-Sound-Stammtisch-am-5.html#msg2325467
*/

#define DFPLAYER_SEND_LENGTH 10
#define Stack_Header 0
#define Stack_Version 1
#define Stack_Length 2
#define Stack_Command 3
#define Stack_ACK 4
#define Stack_Parameter 5
#define Stack_CheckSum 7
#define Stack_End 9

#define MP3TF16P_CMD_PLAY_IDX 0x03
#define MP3TF16P_CMD_VOL_SET  0x06
#define MP3TF16P_CMD_VOL_UP   0x04
#define MP3TF16P_CMD_VOL_DN   0x05
#define MP3TF16P_CMD_PLAY     0x0D
#define MP3TF16P_CMD_PAUSE    0x0E
#define MP3TF16P_CMD_LOOP_ALL 0x11    // Set all repeat playback
#define MP3TF16P_CMD_LOOP_RND 0x18    // Set random playback 
#define MP3TF16P_CMD_LOOP_OFF 0x19    // Set repeat playback of current track
#define MP3TF16P_CMD_LOOP_TRK 0x08    // Specify single repeat playback

//#define MP3TF16P_CMD_PLAY_MP3_FILE 0x12   // Play track "xxxx" in the folder "MP3"
//#define MP3TF16P_CMD_PLAY_FOLDER_FILE 0x14   // Play track "xxxx" in the folder "yy"

#define MP3TF16P_MODULE_MASK 0x0f

class MP3TF16PSoundPlayer : public SoundPlayer
{
  private:
  uint8_t moduleIdAndParams;
  uint8_t lastTrack = 0;
  
  public:
  static uint8_t buffer[DFPLAYER_SEND_LENGTH];
  static uint8_t sendLen;
    
  // moduleIdAndParams: lower 4 bits are the moduleId (MP3TF16P_MODULE_MASK)
  //                    bit 7: if set use checksum workaround for MH2024K-24SS modules
  MP3TF16PSoundPlayer(uint8_t moduleIdAndParams, SOFTWARE_SERIAL_TYPE* serialLine)
  {
    this->moduleIdAndParams = moduleIdAndParams;
    this->serialLine = serialLine;
  }  
  void process(const uint8_t command, const uint8_t* arguments) override 
  {
    uint8_t argument = arguments[0];
#if (DEBUG_SOUND_CHANNEL&0x02)==0x02
    { char s[80]; sprintf(s, "MP3TF16PSoundPlayer(%d) process command %d.", moduleIdAndParams&MP3TF16P_MODULE_MASK, command); Serial.println(s); Serial.flush();} // Debug
#endif      
    sendLen = DFPLAYER_SEND_LENGTH;
    uint8_t cmd;
    switch(command)
    {
      case SOUND_CHANNEL_CMD_PLAY_TRACK: // command 8: PlayTrack, argument TrackNumber
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "MP3TF16PSoundPlayer(%d) play trackIndex %d.", moduleIdAndParams&MP3TF16P_MODULE_MASK, argument); Serial.println(s); Serial.flush();} // Debug
#endif          
          // 7E FF 06 03 00 00 01 FE F7 EF  Play Idx
          // 7E FF 06 12 00 00 01 FE E8 EF  Play File# in MP3 Folder
        cmd = MP3TF16P_CMD_PLAY_IDX; 
        lastTrack = argument;
        break;
      case SOUND_CHANNEL_CMD_SET_VOLUME: // command 9: Set Volume to percentage level
      {
        uint16_t vol = (argument*30)/100;
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "MP3TF16PSoundPlayer(%d) set volume %d.", moduleIdAndParams&MP3TF16P_MODULE_MASK, argument); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = MP3TF16P_CMD_VOL_SET; 
        argument = vol;
        break;
      }
      case SOUND_CHANNEL_CMD_INCREASE_VOLUME: // command 0: Increase volume
      case SOUND_CHANNEL_CMD_DECREASE_VOLUME: // command 1: decrease volume
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "MP3TF16PSoundPlayer(%d) volume %s.", moduleIdAndParams&MP3TF16P_MODULE_MASK, command ? "DOWN" : "UP"); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = MP3TF16P_CMD_VOL_UP + command;  
        argument = 0;
        break;
        
      case SOUND_CHANNEL_CMD_PAUSE: // command 2: pause
      case SOUND_CHANNEL_CMD_CONTINUE: // command 3: continue
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "MP3TF16PSoundPlayer(%d) %s.", moduleIdAndParams&MP3TF16P_MODULE_MASK, command==2 ? "PAUSE" : "CONTINUE"); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = MP3TF16P_CMD_PLAY+SOUND_CHANNEL_CMD_CONTINUE-command; 
        argument = 0;
        break;

      case SOUND_CHANNEL_CMD_LOOP_MODE: // command 9: Set loop mode
      {
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "MP3TF16PSoundPlayer(%d) set loop mode %d.", moduleIdAndParams&MP3TF16P_MODULE_MASK, argument); Serial.println(s); Serial.flush();} // Debug
#endif          
        switch(argument)
        {
          case LOOP_ALL: cmd = MP3TF16P_CMD_LOOP_ALL; argument = 0; break;
          case LOOP_RND: cmd = MP3TF16P_CMD_LOOP_RND; argument = 0; break;

          case LOOP_OFF: cmd = MP3TF16P_CMD_LOOP_OFF; argument = 1; break;
          case LOOP_ONE: cmd = MP3TF16P_CMD_LOOP_TRK; argument = lastTrack; break;
          default: return;
        }
        break;
      }
      default: return;              // skip this unknown package
    }

    buffer[Stack_Command] = cmd; 
    buffer[Stack_Parameter+1] = argument;
    
    // workaround for bug in MH2024K-24SS - see comments above
    if (moduleIdAndParams&0x80) {
      sendLen -= 2;
    }
    else
    {
      //Checksum (2 bytes) = 0xFFFF–(Ver.+Length+CMD+Feedback+Para_MSB+Para_LSB)+1
      uint16_t chk = 0xFFFF - (0xFF+0x06+cmd+0+argument-1);
      buffer[Stack_CheckSum] = chk >>8;
      buffer[Stack_CheckSum+1] = chk;
    }
    buffer[sendLen-1] = 0xEF;

#if (DEBUG_SOUND_CHANNEL&0x08)==0x08
    Serial.print("MP3TF16PSoundPlayer sends");
    dump(&(MP3TF16PSoundPlayer::buffer[0]),sendLen);
    Serial.println();
#endif    
    serialLine->write(&(MP3TF16PSoundPlayer::buffer[0]),sendLen);
    waitUntil = millis()+DELAY_AFTER_MP3TF16P_PACKET;
    return; 
  }
  
};
uint8_t MP3TF16PSoundPlayer::buffer[DFPLAYER_SEND_LENGTH] = {0x7E, 0xFF, 06, 00, 00, 00, 00, 00, 00, 0xEF};
uint8_t MP3TF16PSoundPlayer::sendLen;

class MP3TF16PNoCRCSoundPlayer : public MP3TF16PSoundPlayer
{
public:
  // moduleIdAndParams: lower 4 bits are the moduleId (MP3TF16P_MODULE_MASK)
  //                    bit 7: if set use checksum workaround for MH2024K-24SS modules
  MP3TF16PNoCRCSoundPlayer(uint8_t moduleIdAndParams, SOFTWARE_SERIAL_TYPE* serialLine)
    : MP3TF16PSoundPlayer(moduleIdAndParams|0x80, serialLine)
  {
    
  }
};

#endif
