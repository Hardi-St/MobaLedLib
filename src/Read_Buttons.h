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


 Read_Buttons.h
 ~~~~~~~~~~~~~~

 This module implements the simple button input function Read_Buttons()
 It returns a bit mask of up to 6 buttons together with a flag whitch defines the
 button press time.
 The module detects the pressing if one or multible buttons.
 To use the function the array Buttons[] must be defined in the main program like:
   const uint8_t Buttons[] = { 7, 8, 9 }; // Up to 6 button pins
 and the setup_Buttons() function must be called in the setup() function
*/

#ifndef __READ_BUTTONS_H__
#define __READ_BUTTONS_H__

#include <Arduino.h>

#define BUTTON_PRESSED            LOW   // Use LOW if the buttons are connected to GND. Otherwise use HIGH. Attention: If the buttons are connected to 5V external 10K pull down resistors must be used.
#define BUTTON_LONG_TIME          1500  // Time [ms]. If buttons pressed longer Read_Buttons() returns BUTTON_LONG_FLAG | buttons
#define BUTTON_MIDDLE_TIME        400   // Time [ms]. If buttons pressed longer but shorter than BUTTON_LONG_TIME Read_Buttons() returns BUTTON_MIDDLE_FLAG
#define BUTTON_REPEAT_PERIOD      100   // Repeat time if the button is pressed longer than BUTTON_LONG_TIME


#define BUTTON_SHORT_FLAG         0x00
#define BUTTON_MIDDLE_FLAG        0x40
#define BUTTON_LONG_FLAG          0x80
#define BUTTON_REP_FLAG           0xC0
#define BUTTON_TIME_MASK          (BUTTON_LONG_FLAG | BUTTON_MIDDLE_FLAG | BUTTON_SHORT_FLAG | BUTTON_REP_FLAG)

const char FlagTxt[4][7] = { "SHORT", "MIDDLE", "LONG", "REP" };
#define DEBUG_PRINT_BUTTONS(key) Dprintf("Key: 0x%02X | BUTTON_%s_FLAG\n", key & ~BUTTON_TIME_MASK, FlagTxt[key >> 6])

//------------------
void Setup_Buttons()
//------------------
{
  #if BUTTON_PRESSED == LOW
    for (uint8_t i  = 0; i < sizeof(Buttons); i++) pinMode(Buttons[i], INPUT_PULLUP); // Pins der Taster als Eingang verwenden, Widerstand nach plus, weil Schalter nach Masse kommt
  #endif
}

//----------------
bool Key_Pressed()                                                                                            // 27.08.18:
//----------------
{
  for (uint8_t i = 0, Mask = 1; i < sizeof(Buttons); i++, Mask<<=1)
      if (digitalRead(Buttons[i]) == BUTTON_PRESSED) return true;
  return false;
}

//--------------------
uint8_t Read_Buttons()                                                                                        // 04.08.18:
//--------------------
// Read up to 6 buttons which are connected to the inputs of the arduino
// Returns a bitmask which contains the pressed buttons together with a
// flag (see above) which contains the key press duration.
//
// digitalRead() takes about 5 us. This could be improved by using direct port access
{
  static uint8_t  Old_Inp = 0;
  static uint32_t LastTime;
  uint8_t Inp = 0, ret = 0;
  for (uint8_t i = 0, Mask = 1; i < sizeof(Buttons); i++, Mask<<=1)
      {
      if (digitalRead(Buttons[i]) == BUTTON_PRESSED)
           Inp |=  Mask;
      else Inp &= ~Mask;
      }

  if (Old_Inp == 0)
       {
       if (Inp) // Button Press start dedected
          {
          LastTime = millis();
          }
       Old_Inp = Inp; // Add the pressed buttons to Old_Inp
       }
  else { // Old_Inp != 0
       uint32_t Duration = millis() - LastTime;
       if (Inp == 0) // Key released
            {
            if ((Old_Inp & BUTTON_LONG_FLAG) == 0) // Check if a long press time was reported before
               {
               ret = Old_Inp;
               if (Duration > BUTTON_MIDDLE_TIME)
                  ret |= BUTTON_MIDDLE_FLAG;
               }
            Old_Inp = 0;
            }
       else { // Button is still pressed
            if (Duration >= BUTTON_LONG_TIME)
               {
               if ((Old_Inp & BUTTON_TIME_MASK) == 0) // Button is pressed the first time longer then BUTTON_LONG_TIME
                    {
                    Old_Inp = ret = BUTTON_LONG_FLAG | Old_Inp;
                    }
               else {
                    Old_Inp &= ~BUTTON_TIME_MASK;
                    ret = (Old_Inp |= BUTTON_REP_FLAG);
                    }
               LastTime += BUTTON_REPEAT_PERIOD;
               }
            else Old_Inp |= Inp; // Add the pressed buttons to Old_Inp to be able to detect multiple buttons
            }
       }
  return ret;
}


#endif // __READ_BUTTONS_H__

