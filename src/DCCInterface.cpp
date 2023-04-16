/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de
 Copyright (C) 2021 - 2023  Jürgen Winkler: MobaLedLib@gmx.at

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------------------------------------


 DCC rail signal decoder for ESP with thread save read buffer      18.10.2020 Jürgen
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~


 Documents:
 ~~~~~~~~~~
 - https://www.opendcc.de/info/dcc/dcc.html
 - https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
 - https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
 - https://lastminuteengineers.com/handling-esp32-gpio-interrupts-tutorial/
 - https://randomnerdtutorials.com/esp32-pinout-reference-gpios/

Revision History :
~~~~~~~~~~~~~~~~~
18.10.20:  Versions 1.0 (Jürgen)
24.04.21:  Add PICO support (Jürgen)
25.04.21:  Improve DCC signal detection and led display (Jürgen)
02.01.21:  add support for DCC receive on LED Arduino
04.03.23:  Add GEN_BUTTON_RELEASE_COM feature that generates a "Button Release" in case the DCC central doesn't do it e.g. Lenz LZV100
*/

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO) || defined(__AVR__)                                  // 02.01.22: Juergen add support for DCC receive on LED Arduino
// Disable the warning:                                                                                       // 26.12.19:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
// and:
//   ... warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
//

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <NmraDcc.h>
#pragma GCC diagnostic pop

#include "DCCInterface.h"
#include "Helpers.h"

static NmraDcc  Dcc ;                                // Instance of the NmraDcc class

#include <EEPROM.h>
uint32_t DCCInterface::recentButtonPressTime  = 0;
bool     DCCInterface::releaseModeChangeDetection = false;
uint8_t  DCCInterface::DCCLastDirection;
uint16_t DCCInterface::DCCLastAddr;
uint32_t DCCInterface::DCCLastTime = 0;
uint8_t  DCCInterface::release_mode;   // 0 = off, 1=detection phase, 2 = on
#define DEBUG_GEN_BUTTON_RELEASE
  
#if defined(__AVR__)                                                                                         // 02.01.22: Juergen add support for DCC receive on LED Arduino
  // forward declaration..
  void Update_InputCh_if_Addr_exists(uint16_t ReceivedAddr, uint8_t Direction, uint8_t OutputPower);
#endif

void send( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
{
  if (OutputPower)
  {
    if (DCCInterface::DCCLastTime && (DCCInterface::DCCLastAddr != Addr || DCCInterface::DCCLastDirection != Direction)) 
    {
      if (DCCInterface::release_mode!=0) send(DCCInterface::DCCLastAddr, DCCInterface::DCCLastDirection, 0);
    }
    DCCInterface::DCCLastTime = millis();
    DCCInterface::DCCLastAddr = Addr;
    DCCInterface::DCCLastDirection = Direction;
  }

#ifdef __AVR__                                                                                               // 02.01.22: Juergen add support for DCC receive on LED Arduino
  Update_InputCh_if_Addr_exists(Addr, Direction, OutputPower);  
#else
  //if (!OutputPower) return ; // debug: Simulate the Lenz LZV100 behavior which doesn't send the button release signal
	char s[20];
  sprintf(s, "@%4i %02X %02X\n", Addr, Direction, OutputPower);
  CommInterface::addToSendBuffer(s);
  if_printf("%4i notifyDccAccTurnoutOutput: %i, %i, %02X\r\n", millis(), Addr, Direction, OutputPower);
#endif
}
  
//-------------------------------------------------------------------------------------
void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
//-------------------------------------------------------------------------------------
// This function is called whenever a normal DCC Turnout Packet is received
{
  // automatic detection
  if (DCCInterface::release_mode==GEN_AUTOMATIC)
  {
    if (OutputPower==0)   // got a turn off
    {
      // turn off one time detectection
#ifdef DEBUG_GEN_BUTTON_RELEASE
      if (DCCInterface::releaseModeChangeDetection) if_printf("GEN_BUTTON_RELEASE automatic mode change detection turned OFF\r\n");
#endif
      DCCInterface::releaseModeChangeDetection = false;
      
      // turn OFF the automatic generation of button release  
      if (DCCInterface::release_mode!=0)
      {
        DCCInterface::release_mode = 0;
        DCCInterface::StoreReleaseMode(false);
#ifdef DEBUG_GEN_BUTTON_RELEASE
      if_printf("button release detected, GEN_BUTTON_RELEASE turned OFF\r\n");
#endif
      }
    }
    else
    {
      // store last Button On Time
      if (DCCInterface::recentButtonPressTime==0) DCCInterface::recentButtonPressTime = millis();
    }
  }
  send(Addr, Direction, OutputPower);
}

#ifndef __AVR__                                                                                             // 02.01.22: Juergen add support for DCC receive on LED Arduino
//---------------------------------------------------------
void notifyDccSigOutputState( uint16_t Addr, uint8_t State)
//---------------------------------------------------------
// notifyDccSigOutputState() Callback for a signal aspect accessory decoder.
//                      Defined in S-9.2.1 as the Extended Accessory Decoder Control Packet.
{
  char s[20];
  sprintf(s, "$%4i %02X\n", Addr, State); // Bei der CAN Geschichte hab ich herausgefunden dass es 2048 Adressen gibt. Ich hoffe das stimmt...
  CommInterface::addToSendBuffer(s);
  if_printf("notifyDccSigState: %i,%02X\r\n", Addr, State) ;
}
#endif

void notifyDccMsg( DCC_MSG * Msg )
{
  CommInterface::setLastSignalTime(millis());
}

//-----------
void DCCInterface::setup(
    int DCCSignalPin, 
    int statusLedPin, 
#if !defined(__AVR__)                                                                                     // 02.01.22: Juergen add support for DCC receive on LED Arduino
    InMemoryStream& stream, 
#endif
    bool enablePullup,
    uint8_t sendButtonReleaseMode)                                                                        // 09.04.23: possible behavior of sending a button release
{
//-----------
  
#if !defined(__AVR__)                                                                                     // 02.01.22: Juergen add support for DCC receive on LED Arduino
  CommInterface::setup(statusLedPin, stream);
#else
  CommInterface::setup(statusLedPin);

#endif  
  
  DCCInterface::release_mode = sendButtonReleaseMode;
  if (sendButtonReleaseMode==GEN_AUTOMATIC) 
  {
    DCCInterface::releaseModeChangeDetection = true;
#ifdef DEBUG_GEN_BUTTON_RELEASE
    if_printf("GEN_BUTTON_RELEASE automatic mode change detection turned ON (%i seconds)\r\n", GEN_BUTTON_RELEASE_DETECTION_SECONDS);
#endif
    switch(EEPROM.read(0))
    {
      case 0x55: // automatic send is OFF
        DCCInterface::release_mode = 0;
#ifdef DEBUG_GEN_BUTTON_RELEASE
        if_printf("GEN_BUTTON_RELEASE mode from EEPROM is OFF\r\n");
#endif
        break;
      case 0xAA: // automatic send is ON
        DCCInterface::release_mode = 2;
#ifdef DEBUG_GEN_BUTTON_RELEASE
        if_printf("GEN_BUTTON_RELEASE mode from EEPROM is ON\r\n");
#endif
        break;
      default:
#ifdef DEBUG_GEN_BUTTON_RELEASE
        if_printf("GEN_BUTTON_RELEASE mode from EEPROM is undefined\r\n");
#endif
        DCCInterface::release_mode = 1;  // automatic discover phase
        break;
    }
  }
  #ifdef DEBUG_GEN_BUTTON_RELEASE
  switch(DCCInterface::release_mode)
  {
    case 0: 
      if_printf("GEN_BUTTON_RELEASE mode is OFF\r\n");
      break;
    case 1: 
      if_printf("GEN_BUTTON_RELEASE mode is AUTOMATIC\r\n");
      break;
    case 2: 
      if_printf("GEN_BUTTON_RELEASE mode is ON\r\n");
      break;
  }
  #endif
  
  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  Dcc.pin(DCCSignalPin, enablePullup ? 1 : 0);

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // ToDo: Was bedeuten die Konstanten ?

  //addToSendBuffer("Init Done\r\n"); // This message is send to the LED Arduino over RS232 or SPI (If the Arduino is already active)
  
  if_printf("DCCInterface using pin %d has been started.\r\n", DCCSignalPin);
}

//-----------
void DCCInterface::process(){
//-----------
  if (DCCInterface::release_mode!=0)
  {
    if (DCCInterface::DCCLastTime && millis()-DCCInterface::DCCLastTime > 400) // Use 1100 if no repeat is wanted
    {
#ifdef DEBUG_GEN_BUTTON_RELEASE
      if_printf("GEN_BUTTON_RELEASE sends release for %4i %i\r\n", DCCInterface::DCCLastAddr, DCCInterface::DCCLastDirection);
#endif
      send(DCCInterface::DCCLastAddr, DCCInterface::DCCLastDirection, 0);
      DCCInterface::DCCLastTime = 0;
      // Serial.print(F("Release Button Addr ")); Serial.print(DCCInterface::DCCLastAddr); Serial.print("/");Serial.println(DCCInterface::DCCLastDirection); // Debug
    }
  }

  /* one time detection after process startup
     applies only if automatic detection is enabled 

     use-case: a customer may change the DCC central, and the changed central
     may have a different behavior for sending button releases

     so EEPROM stored value may no longer be correct
     after program restart it still operates in mode stored in EEPROM
     but a one-time mode detection is active, that looks for a button release DCC packet 
     within 30 seconds after the first button press

     Note: this must be a one time detection, because a customer may use DCC switches,
     e.g. for turning a light on (for long time). In this case no button release will occur 
     as long the function is turned on.

     Side effect: in case you use a central that generates a button release, but the first and ONLY
     DCC packet after startup is a Switch turned on, without a release within 30 seconds
     the mode is set to automatic generation of button release and the switch is turned off. This wrong
     situation is fixed with the first button release DCC packet

     */
  if (DCCInterface::releaseModeChangeDetection)      
  {
    if (DCCInterface::recentButtonPressTime && millis()-DCCInterface::recentButtonPressTime > (1000*GEN_BUTTON_RELEASE_DETECTION_SECONDS)) // no button release within 30 seconds
    {
      DCCInterface::recentButtonPressTime = 0;
      // turn ON the automatic generation of button release  
      if (DCCInterface::release_mode!=2)
      {
        DCCInterface::release_mode = 2;
        DCCInterface::StoreReleaseMode(true);
        DCCInterface::releaseModeChangeDetection = false;
  #ifdef DEBUG_GEN_BUTTON_RELEASE
        if_printf("no button release within %i seconds, GEN_BUTTON_RELEASE turned ON\r\n", GEN_BUTTON_RELEASE_DETECTION_SECONDS);
        if_printf("GEN_BUTTON_RELEASE automatic mode change detection turned OFF\r\n");
#endif
        // and turn off the switch
        if (DCCInterface::DCCLastTime) 
        {
          send(DCCInterface::DCCLastAddr, DCCInterface::DCCLastDirection, 0);
          DCCInterface::DCCLastTime = 0;
        }

      }
    }
  }
  Dcc.process(); // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation
  CommInterface::process();                                                                             // 13.05.20:
}
        
void DCCInterface::StoreReleaseMode(bool isOn)
{
#if !defined(ESP32) && !defined(ARDUINO_RASPBERRY_PI_PICO)
    eeprom_busy_wait();
    eeprom_write_byte((uint8_t*)0, isOn?0xaa:0x55);
#else
    EEPROM.write(0, isOn?0xaa:0x55);
		EEPROM.commit();
#endif
  
}
#endif