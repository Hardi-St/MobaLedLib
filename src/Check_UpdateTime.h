#ifndef _CHECK_UPDATETIME_H_
#define _CHECK_UPDATETIME_H_
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


 Check_UpdateTime.h
 ~~~~~~~~~~~~~~~~~~

 Include this headder file to measure the update rate including the calculation time
 The function Check_UpdateTime() must be added to the loop() function in the .ino file


 25.08.18:  Updaterate: 200 LEDs 130/s, 100 LEDs 218/s, 60 LEDs:299/s, 30 LEDs:399/s
 Update time for one LED (Theoretical) 30us  => 100 LEDs = 3 ms
 An update rate > 50/s is not necessary because our eys are to slow ;-(
*/

//---------------------
void Check_UpdateTime()
//---------------------
{
  static unsigned   Upd_Cnt = 0;
  static uint32_t   Next_Upd_Check = 5000;
    Upd_Cnt++;
    if (millis() >= Next_Upd_Check)
       {
       Serial.print(F("Upd Time "));
       Serial.print(5000/Upd_Cnt);
       Serial.println(F(" ms"));
       Upd_Cnt = 0;
       Next_Upd_Check = millis() + 5000;
       }
}

#endif // _CHECK_UPDATETIME_H_
