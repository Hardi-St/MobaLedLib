#ifndef DCCINTERFACE_H
#define DCCINTERFACE_H

#include <EEPROM.h>
#include <NmraDcc.h>
#include "CommInterface.h"

class DCCInterface : public CommInterface
{
public:
// dual core CPUs use a stream to exchange data between cores
#if !defined(__AVR__)                                                                                                  // 02.01.22: Juergen add support for DCC receive on LED Arduino
	virtual void	setup(int DCCSignalPin, int statusLedPin, InMemoryStream& stream, bool enablePullup = true);
#else
	virtual void	setup(int DCCSignalPin, int statusLedPin, bool enablePullup = true);
#endif
	virtual void	process();
};

#endif