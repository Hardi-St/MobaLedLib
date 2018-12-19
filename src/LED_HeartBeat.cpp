/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018  Hardi Stengelin: MobaLedLib@gmx.de

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


 LED heartbeat class
 ~~~~~~~~~~~~~~~~~~~

 Example usage:
   LED_Heartbeat_C LED_HeartBeat(LED_BUILTIN); // Initialize the heartbeat LED which is flashing if the program runs.
                                               // The internal LED (LED_BUILTIN = 13) can't be used if the SPI bus is
                                               // used (CAN BUS, ...)
   void loop()
   {
     LED_HeartBeat.Update() must be called periodic in the loop() function
   }

 An other way to implement the heartbeat function is the usage of a RGB LED. This could be located on the
 CPU module connected to the return signal of the LED stripe.
 Use the Blink() function in the configuration.

*/
#include "MobaLedLib.h"

//---------------------------------------------
LED_Heartbeat_C::LED_Heartbeat_C(uint8_t PinNr)
//---------------------------------------------
// The internal LED (13) can't be used if the CAN bus is enabled.
  : LED_Pin(PinNr),
    HB_State(0),
    HB_Time(0)
{
  pinMode(LED_Pin, OUTPUT);
}

//----------------------------
void LED_Heartbeat_C::Update()
//----------------------------
{
    if ((millis() - HB_Time) >= 500)
       {
       HB_Time = millis();
       HB_State = !HB_State;
       digitalWrite(LED_Pin, HB_State);
       }
}
