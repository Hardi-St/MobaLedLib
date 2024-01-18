#ifndef LNETINTERFACE_H
#define LNETINTERFACE_H

#include "CommInterface.h"
#include <LocoNetESP32.h>

class LNetInterface : public CommInterface
{
public:
  virtual void	setup(int LNetRxPin, int LNetTxPin, int statusLedPin, InMemoryStream& stream, uint8_t xCoreID);
  virtual void	process();
private:	
  static void   print2Buf(uint16_t addr, uint8_t dir, uint8_t pow);
private:	
  LocoNetESP32*      pLocoNetPhy;
  LocoNetBus         bus;
  LocoNetDispatcher* pParser; 
  bool               cablePlugged = true;     // start with cable plugged
  uint32_t           lastHighTime;
};
#endif