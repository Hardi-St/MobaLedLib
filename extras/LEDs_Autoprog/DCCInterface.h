#ifndef DCCINTERFACE_H
#define DCCINTERFACE_H

#include <EEPROM.h>
#include <NmraDcc.h>
#include "InMemoryStream.h"

class DCCInterface
{
private:
	static InMemoryStream* pStream;

public:
	static void	setup(int DCCSignalPin, int statusLedPin, InMemoryStream& stream);
	static void	process();
	static char	read();
	static int 	available();
	static void addToSendBuffer(const char *s);
private:	
	static void processErrorLed();
};

#endif