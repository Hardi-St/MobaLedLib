#ifndef DCCINTERFACE_H
#define DCCINTERFACE_H

#include <EEPROM.h>
#include <NmraDcc.h>
#include "CommInterface.h"

// possible behavior of sending a button release
#define GEN_OFF  0      // never simulate a button release
#define GEN_AUTOMATIC 1 // automatic detect necessity to simulate a button release
#define GEN_ON 2        // always simulate a button release

#ifndef GEN_BUTTON_RELEASE_DETECTION_SECONDS
  #define GEN_BUTTON_RELEASE_DETECTION_SECONDS 30
#endif 

class DCCInterface : public CommInterface
{
public:
// dual core CPUs use a stream to exchange data between cores
#if !defined(__AVR__)                                                                                                  // 02.01.22: Juergen add support for DCC receive on LED Arduino
	virtual void	setup(int DCCSignalPin, int statusLedPin, InMemoryStream& stream, bool enablePullup = true, 
  uint8_t sendButtonReleaseMode = GEN_OFF);                                                                            // 09.04.23: possible behavior of sending a button release
#else
	virtual void	setup(int DCCSignalPin, int statusLedPin, bool enablePullup = true,
  uint8_t sendButtonReleaseMode = GEN_OFF);                                                                            // 09.04.23: possible behavior of sending a button release
#endif
	virtual void	process();
  
public:
  static void StoreReleaseMode(bool isOn);
public:
  static uint32_t recentButtonPressTime;
  static bool releaseModeChangeDetection;
  static bool sendButtonReleaseMode;
public:
  static uint8_t DCCLastDirection;
  static uint16_t DCCLastAddr;
  static uint32_t DCCLastTime;
  static uint8_t  release_mode;   // 0 = off, 1 = on, 2=detection phase
};

#endif