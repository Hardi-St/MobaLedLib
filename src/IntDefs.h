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

 IntDefs.h
 ~~~~~~~~~

 This file contains internal definitions for the MobaLedLib

*/

#ifndef __OUTPUT_FUNCT_INTDEFS_H__
#define __OUTPUT_FUNCT_INTDEFS_H__

// Parameter numbers for the different functions
#define P_INPCHANEL_NO_CM      1  // If the funktion has no channel mask
#define P_INPCHANEL_W_CM       2  // If the function has a channel mask


#define P_LEDNR                0
#define P_CHANELMSK            1
#define P_INPCHANEL            P_INPCHANEL_W_CM  // 2

#define P_CONST_VAL0           3   // Const
#define P_CONST_VAL1           4   //  "

#define P_DFLASH_PAUSE         3
#define P_DFLASH_TON           4
#define P_DFLASH_TOFF          5
#define P_DFLASH_VAL0          6
#define P_DFLASH_VAL1          7
#define P_DFLASH_OFF           8


#define P_HOUSE_LED            0
#define P_HOUSE_INCH           P_INPCHANEL_NO_CM  // 1
#define P_HOUSE_ON_MIN         2
#define P_HOUSE_ON_MAX         3
#define P_HOUSE_MIN_T          4
#define P_HOUSE_MAX_T          5
#define P_HOUSE_CNT            6
#define P_HOUSE_ROOM_0         7

#define P_FIRE_LED0            0
#define P_FIRE_INCH            P_INPCHANEL_NO_CM  // 1
#define P_FIRE_LEDCNT          2
#define P_FIRE_BRIGHT          3

#define P_BUTTON_LED           0
#define P_BUTTON_CHMSK         1
#define P_BUTTON_INCH          P_INPCHANEL_W_CM   // 2
#define P_BUTTON_DURLO         3
#define P_BUTTON_DURHI         4
#define P_BUTTON_VAL0          5
#define P_BUTTON_VAL1          6

#define P_PATERN_LED           0
#define P_PATERN_NSTRU         1
#define P_PATERN_INCH          P_INPCHANEL_W_CM   // 2
#define P_PATERN_ENABLE        3
#define P_PATERN_LEDS          4 //  -+
#define P_PATERN_VAL0          5 //   +  Attention: LEDs, Val0, Val1, Off are read at once.
#define P_PATERN_VAL1          6 //   +  => Don't change the sequence of the parameters
#define P_PATERN_OFF           7 //  -+
#define P_PATERN_MODE          8
#define P_PATERN_T1_L          9
#define P_PATERN_T1_H          10

//#define Random(    DestVar, Inp, MinTime, MaxTime, MinOn, MaxOn) RANDOM_T,  DestVar, Inp, _T2B(MinTime), _T2B(MaxTime), _T2B(MinOn), _T2B(MaxOn),
//#define RandMux(   DestVar1, DestVarN, Inp, MinTime, MaxTime)    RANDMUX_T, DestVar1, DestVarN, Inp, _T2B(MinTime), _T2B(MaxTime),


#define P_RANDOM_DSTVAR        0
#define P_RANDOM_INP           P_INPCHANEL_NO_CM  // 1
#define P_RANDOM_MODE          2
#define P_RANDOM_MINTIME_L     3
#define P_RANDOM_MINTIME_H     4
#define P_RANDOM_MAXTIME_L     5    // must be direct after P_RANDOM_MINTIME_H
#define P_RANDOM_MAXTIME_H     6
#define P_RANDOM_MINON_L       7
#define P_RANDOM_MINON_H       8
#define P_RANDOM_MAXON_L       9    // must be direct after P_RANDOM_MINON_H
#define P_RANDOM_MAXON_H       10
#define EP_RANDOM_INCREMENT    11   // Insert additional parameters before

#define P_RANDMUX_DSTVAR1      0
#define P_RANDMUX_DSTVARN      1
#define P_RANDMUX_INP          P_INPCHANEL_W_CM   // 2
#define P_RANDMUX_MODE         3
#define P_RANDMUX_MINTIME_L    4
#define P_RANDMUX_MINTIME_H    5
#define P_RANDMUX_MAXTIME_L    6    // must be direct after P_RANDMUX_MINTIME_H
#define P_RANDMUX_MAXTIME_H    7
#define EP_RANDMUX_INCREMENT   8    // Insert additional parameters before


#define P_WELDING_LED          0
#define P_WELDING_INP          1
#define EP_WELDING_INCREMENT   2    // Insert additional parameters before

#define P_COPYLED_LED          0
#define P_COPYLED_INP          1
#define P_COPYLED_SRCLED       2
#define EP_COPYLED_INCREMENT   3    // Insert additional parameters before

#define P_SCHEDULE_DSTVAR1     0
#define P_SCHEDULE_DSTVARN     1
#define P_SCHEDULE_INCH        2
#define P_SCHEDULE_START       3
#define P_SCHEDULE_END         4
#define EP_SCHEDULE_INCREMENT  5    // Insert additional parameters before

#define P_COUNT_MODE_L         0
#define P_COUNT_MODE_H         1
#define P_COUNT_INP            2
#define P_COUNT_ENABLE         3
#define P_COUNT_TIMEOUT_L      4
#define P_COUNT_TIMEOUT_H      5
#define P_COUNT_DEST_COUNT     6
#define P_COUNT_FIRST_DEST     7


#define PT_INACTIVE 255


// Constants for AnalogPattern
#define SHIFT_FF 16
#define MIN_M    -2147483647
#define MAX_M     2147483647


typedef struct
    {
    uint8_t         Last_t;
    uint8_t         dt;
    } TimerData_T;

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    } TimerData16_T;

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         State;
    } PatternData_T;        // 5 Byte

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         State;
    uint16_t        Start_t;
    } AnalogPatternData_T;  // 7 Byte

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         flickertimes;  // 5 Bytes
    } WeldingData_T;

enum DayState_E : uint8_t
    {
    Unknown = 0,
    SunSet,
    SunRise
    } ;

typedef struct
    {
    uint8_t         SwitchVal;
    uint8_t         ToDoCnt;
    DayState_E      OldDayState;
    } Schedule_T;


typedef struct
    {
    uint8_t         Hue;
    uint8_t         Sat;
    uint8_t         Val;
    } HSV_T;

typedef struct                                                                                                // 07.11.18:
    {
    uint8_t         Val;
    uint8_t         Changed:1;     // Is cleared in New_Local_Var()
    uint8_t         ExtUpdated:1;  // Is set by external functions. In Set_Local_Var() this flag is copied to Changed and cleared
    } ControlVar_t;

typedef struct
    {
    uint32_t        Cnt;
    uint32_t        On;
    uint16_t        PrintStat;
    } House_Stat_T;

typedef struct
    {
    uint16_t        Last_t;
    uint8_t         Counter;
    } Counter_T;

typedef struct
    {
    uint16_t LEDNr;
    uint16_t Start;
    uint16_t Duration;
    } Sound_Dat_t;   // 6 Byte


#endif // __OUTPUT_FUNCT_INTDEFS_H__

