#ifndef JQ6500SoundPlayer_H
#define JQ6500SoundPlayer_H

#ifndef DELAY_AFTER_JQ6500_PACKET
#define DELAY_AFTER_JQ6500_PACKET 15
#endif

// The JQ6500 sound module is delivered with various sound chips
// the original JQ6500 Module has a JQ6500 sound chip on board
// also we can find JQ6500 moduls with cips AA20HGN403 or AA20HFJ648 or ???
// the so fast tested modules with AA* chip behave different,  serial messages must be sent without any breaks. If the delay between two bytes exceeds 600us the packet is ignored by the chip
//
// by default MLL sends one byte in each main loop cycle to ensure that LED stream is not interrupted for too long. Sending all byte at once delays the main loop by ~5ms, which should be avoided
// but with JQ6500 modules having the AA* chips onboard we need to send a full packet at once, because the main loop cylce time is >2ms.

// sound chips tests so fast
// JQ6500: works fine, even with delays
// AA20HGN403: no delay accepted
// AA20HFJ648: no delay accepted

#define JQ6500_CMD_BEGIN    0x7E
#define JQ6500_CMD_END      0xEF
#define JQ6500_CMD_PLAY_IDX 0x03
#define JQ6500_CMD_VOL_UP   0x04
#define JQ6500_CMD_VOL_DN   0x05
#define JQ6500_CMD_VOL_SET  0x06
#define JQ6500_CMD_PLAY     0x0D
#define JQ6500_CMD_PAUSE    0x0E
#define JQ6500_CMD_LOOP_SET 0x11

class JQ6500SoundPlayer : public SoundPlayer
{
  private:
#ifdef DEBUG_SOUND_CHANNEL
  uint8_t moduleId;
#endif  
  
  public:
  static uint8_t buffer[6];
  JQ6500SoundPlayer(uint8_t moduleId, SOFTWARE_SERIAL_TYPE* serialLine)
  {
#ifdef DEBUG_SOUND_CHANNEL
    this->moduleId = moduleId;
#endif    
    this->serialLine = serialLine;
    buffer[0] = JQ6500_CMD_BEGIN;
  }
  
  // process a command and send data out to the module
  // return new wait timeout timestamp 
  
  void process(const uint8_t command, const uint8_t* arguments) override 
  {

    uint8_t argument = arguments[0];
#if (DEBUG_SOUND_CHANNEL&0x02)==0x02
    { char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) process command %d.", moduleId, command); Serial.println(s); Serial.flush();} // Debug
#endif      
    uint8_t argumentCount = 1;
    uint8_t cmd;
    switch(command)
    {
      case SOUND_CHANNEL_CMD_PLAY_TRACK: // command 8: PlayTrack, argument TrackNumber
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) play trackIndex %d.", moduleId, argument); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = JQ6500_CMD_PLAY_IDX; 
        argumentCount++;
        buffer[4] = argument;
        argument = 0;
        break;
      case SOUND_CHANNEL_CMD_SET_VOLUME: // command 9: Set Volume to percentage level
      {
        uint16_t vol = (argument*30)/100;
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) set volume %d.", moduleId, argument); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = JQ6500_CMD_VOL_SET; 
        argument = vol;
        break;
      }
      case SOUND_CHANNEL_CMD_LOOP_MODE: // command 9: Set loop mode
      {
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) set loop mode %d.", moduleId, argument); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = JQ6500_CMD_LOOP_SET; 
        break;
      }
        
      case SOUND_CHANNEL_CMD_INCREASE_VOLUME: // command 0: Increase volume
      case SOUND_CHANNEL_CMD_DECREASE_VOLUME: // command 1: decrease volume
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) volume %s.", moduleId, command ? "DOWN" : "UP"); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = JQ6500_CMD_VOL_UP + command; 
        argumentCount--;
        break;

      case SOUND_CHANNEL_CMD_PAUSE: // command 2: pause
      case SOUND_CHANNEL_CMD_CONTINUE: // command 3: continue
#if (DEBUG_SOUND_CHANNEL&0x04)==0x04
        { char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) %s.", moduleId, command==2 ? "PAUSE" : "CONTINUE"); Serial.println(s); Serial.flush();} // Debug
#endif          
        cmd = JQ6500_CMD_PLAY+SOUND_CHANNEL_CMD_CONTINUE-command; 
        argumentCount--;
        break;

      default: return;              // skip this unknown package
    }
    buffer[1] = argumentCount+2;
    buffer[2] = cmd; 
    buffer[3] = argument;
    buffer[3+argumentCount] = JQ6500_CMD_END;
#if (DEBUG_SOUND_CHANNEL&0x08)==0x08
    char s[80]; sprintf(s, "JQ6500SoundPlayer(%d) sends", moduleId); 
    Serial.print(s);
    dump(&(JQ6500SoundPlayer::buffer[0]),JQ6500SoundPlayer::buffer[1]+2);
    Serial.println();
#endif    
    serialLine->write(&(JQ6500SoundPlayer::buffer[0]),JQ6500SoundPlayer::buffer[1]+2);
    waitUntil = millis()+DELAY_AFTER_JQ6500_PACKET;
  }
};

uint8_t JQ6500SoundPlayer::buffer[6];

#endif