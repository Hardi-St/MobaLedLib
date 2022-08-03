/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de

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

 Lib_Config.h
 ~~~~~~~~~~~~

 This file contains configuration variables to change the bahavior of the MobaLedLib.

*/
#ifndef __LIB_CONFIG_H__
#define __LIB_CONFIG_H__

#include <Arduino.h>

// *** Defines to enable certain functions of the library ***

//#define _TEST_BUTTONS                 // Enable this to use three buttons connected to the Arduino pins for testing (04.08.18: FLASH: 1390, RAM 18 without DEBUG_PRINT_BUTTONS())

//#define _PRINT_DEBUG_MESSAGES         // Enable this line to print debug messages to the PC (USB)

//#define _DUMP_CONFIG					// Enable this line to print the configuration memory on startup

//#define _FAST_TIMES                   // Enable this for debugging with fast timing for the houses

//#define _HOUSE_STATISTIC              // Calculate the average number of active lights in the houses


#define _USE_LOCAL_TX                   // The variables t4w, tw and t10 are calculated in each function instead of calculating them once in Int_Update()
                                        // Saves 3 Bytes RAM, but uses 42 bytes FLASH

#define _USE_SEP_CONST                  // Use a separate Const() function instead of the Pattern function. This saves
                                        // 5 byte RAM and 10 Byte FLASH per line, but the separate function needs 144 bytes
                                        // FLASH for the program code ;-(
                                        // If more than 14 Const() lines are used and if not enough RAM is available this
                                        // line could be enabled.
                                        // On the other hand the separate Const() function ist faster 7us / 17us.
                                        // 18.11.18:  Enabled because of the saved RAM

#define _NEW_ROOM_COL // Saves 162 Byte FLASH and 43 Bytes RAM

#define _NEW_INITIALIZE                 // allows to restore input state after constructor before first initialize run    


// Memory usage tests / optimisation                  Flash  Ram   Attention _PRINT_DEBUG_MESSAGES must be disabled
#define _USE_COUNTER        1           // 28.10.18:    592    0
#define _USE_HOUSE          1           // 28.10.18:   2924   18?
#define _USE_SET_COLTAB     1           // 09.12.18:
#define _USE_SET_TVTAB      1           // 10.01.20:    176    4
#define _USE_DEF_NEON       1           // 12.01.20:     96  -31 ?
#define _USE_PATTERN        1           // 28.10.18:   2096    0
#define _USE_FIRE           1           // 28.10.18:    594    0
#define _USE_HSV_GROUP      1           // 28.10.18:     62    0
#define _USE_LOCAL_VAR      1           // 07.11.18:     ??
#define _USE_USE_GLOBALVAR  1           // 26.11.18:    124    2
#define _USE_INCH_TO_VAR    1           // 25.11.18:     ??
#define _USE_LOGIC          1           // 28.10.18:    172    0
#define _USE_RANDOM         1           // 28.10.18:    292    0
#define _USE_RANMUX         1           // 28.10.18:    364    0
#define _USE_WELDING        1           // 28.10.18:    430    0
#define _USE_COPYLED        1           // 28.10.18:    116    0
#define _USE_SCHEDULE       1           // 28.10.18:    264    0
#define _USE_STORE_STATUS   1           // 22.04.20:    138    5                                              // 19.05.20: Juergen
#define _USE_INCH_TRIGGER   1           // 08.06.20:    -12    0
#define _USE_CANDLE         1           // 10.06.20:    352    2                                              // 09.06.20:
#define _USE_EXT_PROC       1           // 26.09.21:     48    4                                              // 26.09.21: Juergen
                                        //             8800    Sum

// Hardware pin definitions

// Only used if _TEST_BUTTONS is enabled:
const uint8_t Buttons[] = { 7, 8, 9 }; // Up to 6 button pins (_TEST_BUTTONS must be defined)
#define LED_LEFT_PIN      3            // Yellow LED above the Left button
#define LED_MID_PIN       4            // White LED above the middle button
#define LED_RIGHT_PIN     5            // Blue LED above the Right button
#define RES_INP_PIN       15




#ifdef _FAST_TIMES  // Fast times for tests
  #ifndef HOUSE_MIN_T
    #define HOUSE_MIN_T     1            // Minimal time [s] to the next event (1..255)
  #endif
  #ifndef HOUSE_MAX_T
    #define HOUSE_MAX_T     5            // Maximal random time [s]              "
  #endif
#else // Normal times
  #ifndef HOUSE_MIN_T
    #define HOUSE_MIN_T     50           // Minimal time [s] to the next event (1..255)
  #endif
  #ifndef HOUSE_MAX_T
    #define HOUSE_MAX_T     150          // Maximal random time [s]              "
  #endif
#endif

#ifdef _HOUSE_STATISTIC
  #undef HOUSE_MIN_T
  #undef HOUSE_MAX_T
  #define HOUSE_MIN_T     1            // Minimal time [s] to the next event (1..255)
  #define HOUSE_MAX_T     1            // Maximal random time [s]              "
#endif

#define GAS_MIN_T         2            // Minimal time [s] to the next event (1..255)
#define GAS_MAX_T         5            // Maximal random time [s]                 "

#endif // __LIB_CONFIG_H__
