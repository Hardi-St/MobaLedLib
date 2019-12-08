/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018, 2019  Hardi Stengelin: MobaLedLib@gmx.de

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

 FLASH usage: 146

*/

//:::::::::::::::::::
class LED_Heartbeat_C
//:::::::::::::::::::
{
public:

//----------------------------
LED_Heartbeat_C(uint8_t PinNr) // Constructor
//----------------------------
// The internal LED (13) can't be used if the SPI bus is used (CAN bus)
  : LED_Pin(PinNr),
    HB_Time(0)
{
  pinMode(LED_Pin, OUTPUT);
}

//---------------------------------
void Update(uint16_t Period = 1000)                                                                           // 03.11.19:  Added: Period
//---------------------------------
{
  uint8_t t4 = millis()>>2;
  if ((uint8_t)(t4 - HB_Time) >= Period/8)
     {
     HB_Time = t4;
     digitalWrite(LED_Pin, !digitalRead(LED_Pin));
     }
}

private:
  uint8_t     LED_Pin;      //  1 Byte
  uint8_t     HB_Time;      //  1 Byte
};                          // -------
                            //  2 Byte



