#pragma once

#define EEPROM_START 64                                                                                   // 18.05.23:  reserve first 32 byte for global storage
#define EEPROM_SIZE 1024			// maximum size of the eeprom same as for ATMEGA328

#if defined(ESP32)
  #include "EEPROM.h"
  #if defined(NO_SERIAL_OPTPUT)                                                                              // 17.03.26:
    #define PERSISTENCE_SETUP(size) EEPROM.begin(size);
  #else
    #define PERSISTENCE_SETUP(size) if (!EEPROM.begin(size)) Serial.println("failed to initialize EEPROM");
  #endif  
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
  #include "EEPROM.h"
  #define PERSISTENCE_SETUP(size) EEPROM.begin(size);
#else
  #define PERSISTENCE_SETUP(size) ;
#endif

#define PERSISTENCE_WRITE(addr, data) eeprom_write(addr, data)

#if !defined(ESP32) && !defined(ARDUINO_RASPBERRY_PI_PICO)
#define PERSISTENCE_READ(addr) eeprom_read(addr)

uint8_t eeprom_read(int addr)
{
  eeprom_busy_wait();
  return eeprom_read_byte((const uint8_t*)addr);
}
void eeprom_write(int addr, uint8_t data)
{
  eeprom_busy_wait();
  eeprom_write_byte((uint8_t*)addr, data);
}

#else
#define PERSISTENCE_READ(addr) EEPROM.read(addr)
void eeprom_write(int addr, uint8_t data)
{
  EEPROM.write(addr, data);
  EEPROM.commit();
}

#endif
