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

 Dprintf.h
 ~~~~~~~~~

 This module contains functions for debug printing. It is enabled with the
 compiler switch _PRINT_DEBUG_MESSAGES.

*/
#ifndef _Dprintf_h_
#define _Dprintf_h_

#include "Lib_Config.h"

//#define Nprintf(Format, ...) Dprintf_proc(F(Format), ##__VA_ARGS__)   // Normal printf function which is allways active

#ifdef _PRINT_DEBUG_MESSAGES
   // F() makro saves dynamic memory
   #define Dprintf(Format, ...) Dprintf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
   // Memory based on 01.09.18:
   // Flash: 18426   59%
   // Ram:     740   36%
#else
   #define DPRINTF(Str)
   #define Dprintf(Format, ...)
   // Flash: 16534   53%     save: 1892
   // Ram:     720   35%     save: 20
#endif


#include <stdarg.h>
#include <WString.h>

void Dprintf_proc(const __FlashStringHelper *format, ...);

#endif



