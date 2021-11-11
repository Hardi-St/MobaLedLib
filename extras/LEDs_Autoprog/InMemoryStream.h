#ifndef INMEMORYSTREAM_H
#define INMEMORYSTREAM_H

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)

#if defined(ARDUINO) && ARDUINO >= 100
#include "Arduino.h"
#endif

class InMemoryStream
{
	
public:
	InMemoryStream(int queueSize);
	char	read();
	int 	available();
	bool  write(const char *s);
	bool  write(char s);
};
#endif //defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
#endif //INMEMORYSTREAM
