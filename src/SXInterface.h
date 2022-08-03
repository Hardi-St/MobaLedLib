#ifndef SXINTERFACE_H
#define SXINTERFACE_H

#include "CommInterface.h"
#include "SX20.h"

#define SX_FILTER_COUNT     1                 // Filter to eliminate noise. Disabled if set to 0.
// 1: Changed byte must be repeated one tine before it's send to the LED Arduino
// The reaction time will be increased if a greater number is used.
// 3: => reaction time 231ms - 308ms
#define SX_MAX_USED_CHANNEL_NUMBER  104

class SXInterface : public CommInterface
{
private:

public:
  virtual void	setup(int SXSignalPin, int SXClockPin, int statusLedPin, InMemoryStream& stream);
  virtual void	process();
  static void   isr();
private:	
  static void   print2Buf(uint16_t addr, uint8_t dir, uint8_t pow);
  static void   printSXValue(int i,int data, byte old);
  static void   checkByte(uint8_t i, uint8_t d);  

  static byte   oldSx[MAX_CHANNEL_NUMBER];
#if SX_FILTER_COUNT > 0
  static byte   newSx[MAX_CHANNEL_NUMBER];
  static byte   cntSx[MAX_CHANNEL_NUMBER];
#endif
};

#endif