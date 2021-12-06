#ifndef DCCINTERFACE_H
#define DCCINTERFACE_H

#include <EEPROM.h>
#include <NmraDcc.h>
#include "CommInterface.h"

class DCCInterface : public CommInterface
{
public:
	virtual void	setup(int DCCSignalPin, int statusLedPin, InMemoryStream& stream, bool enablePullup = true);
	virtual void	process();
};

#endif