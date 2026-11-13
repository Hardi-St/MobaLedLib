#pragma once

class SoundPlayer 
{
  protected:
  unsigned long waitUntil = 0;
  uint8_t type = 0;

public:
  virtual void process(const uint8_t command, const uint8_t* arguments) = 0;
virtual void loop() {};
  bool available() 
  {
#if (DEBUG_SOUND_CHANNEL&0x01)==0x01
        { char s[80]; sprintf(s, "available wait %dms %d", waitUntil==0 ? 0 : waitUntil-millis(),millis()>=waitUntil); Serial.println(s); Serial.flush();} // Debug
#endif        
    
    return waitUntil==0 || millis()>=waitUntil; 
  }
  SOFTWARE_SERIAL_TYPE* serialLine;

  uint8_t GetType() 
  { 
    return type; 
  }
    
  protected:
  
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
