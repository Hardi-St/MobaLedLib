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

 Printf.h
 ~~~~~~~~

 This module contains functions for formated printing with printf()

*/
#ifndef _Printf_h_
#define _Printf_h_

// F() makro saves dynamic memory
#define printf(Format, ...) printf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html


#include <stdarg.h>
#include <WString.h>

//------------------------------------------------------
void printf_proc(const __FlashStringHelper *format, ...)
//------------------------------------------------------
{
  char buf[50];
  va_list ap;
  va_start(ap, format);
  #ifdef __AVR__
     vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
  #else
     vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
  #endif

  va_end(ap);
  Serial.print(buf);
}

#endif // _Printf_h_



