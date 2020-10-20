/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de

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

 Debug printf function
 ~~~~~~~~~~~~~~~~~~~~~

 which could be enabled with the compiler switch _PRINT_DEBUG_MESSAGES.
 If it's disabled no memory is used.

*/

#include "Dprintf.h"

#ifdef _PRINT_DEBUG_MESSAGES
  #include "HardwareSerial.h"  // Defines "Serial"

  #if defined __AVR_ATtiny85__ && defined DEBUG_TINY
     #include "ATtinySerialOut.h"
     #include <avr/pgmspace.h> // needed for PSTR()
  #endif


  //-------------------------------------------------------
  void Dprintf_proc(const __FlashStringHelper *format, ...)  // uses 812 byte FLASH and 173 Bytes RAM if enabled ;-(
  //-------------------------------------------------------
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

    #if defined __AVR_ATtiny85__ && defined DEBUG_TINY
       writeString(buf);
    #else
       Serial.print(buf);
    #endif
  }
#endif

