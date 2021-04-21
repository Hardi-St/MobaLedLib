#ifndef INMEMORYSTREAM_H
#define INMEMORYSTREAM_H

#ifdef ESP32

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
#endif //ESP32
#endif //INMEMORYSTREAM
