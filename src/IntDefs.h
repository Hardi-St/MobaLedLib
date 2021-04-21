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

 IntDefs.h
 ~~~~~~~~~

 This file contains internal definitions for the MobaLedLib

*/

#ifndef __OUTPUT_FUNCT_INTDEFS_H__
#define __OUTPUT_FUNCT_INTDEFS_H__

#ifdef ESP32
	// enable the feature to use two byte for LED adress
	#define LONG_LED_ADDR
#endif

#ifdef LONG_LED_ADDR		// use two byte for adressing the LED, allows up to 16384 Leds :-)
							// still two bits reserved for future use
							
// number of extra bytes to count for each Led adress
#define ADD_WORD_OFFSET 1
// a led nubmer is store in a 16 bit variable
#define ledNr_t uint16_t
// marco for reading the led number from configuration array
// read two bytes
#define pgm_read_led_nr pgm_read_word_near

#else					    // use one byte for adressing the LED, allows up to 256 Leds
// number of extra bytes to count for each Led adress
#define ADD_WORD_OFFSET 0
// a led nubmer is store in an 8 bit variable
#define ledNr_t uint8_t
// marco for reading the led number from configuration array
// read one byte
#define pgm_read_led_nr pgm_read_byte_near
#endif

// Parameter numbers for the different functions
#define P_INPCHANEL_NO_CM      1  // If the funktion has no channel mask
#define P_INPCHANEL_W_CM       2  // If the function has a channel mask


#define P_LEDNR                0
#define P_CHANELMSK            (1+ADD_WORD_OFFSET)
#define P_INPCHANEL            (P_INPCHANEL_W_CM+ADD_WORD_OFFSET)  // 2

#define P_CONST_VAL0           (3+ADD_WORD_OFFSET)   // Const
#define P_CONST_VAL1           (4+ADD_WORD_OFFSET)   //  "

#define P_DFLASH_PAUSE         (3+ADD_WORD_OFFSET)
#define P_DFLASH_TON           (4+ADD_WORD_OFFSET)
#define P_DFLASH_TOFF          (5+ADD_WORD_OFFSET)
#define P_DFLASH_VAL0          (6+ADD_WORD_OFFSET)
#define P_DFLASH_VAL1          (7+ADD_WORD_OFFSET)
#define P_DFLASH_OFF           (8+ADD_WORD_OFFSET)


#define P_HOUSE_LED            0
#define P_HOUSE_INCH           (P_INPCHANEL_NO_CM+ADD_WORD_OFFSET)  // 1
#define P_HOUSE_ON_MIN         (2+ADD_WORD_OFFSET)
#define P_HOUSE_ON_MAX         (3+ADD_WORD_OFFSET)
#define P_HOUSE_MIN_T          (4+ADD_WORD_OFFSET)
#define P_HOUSE_MAX_T          (5+ADD_WORD_OFFSET)
#define P_HOUSE_CNT            (6+ADD_WORD_OFFSET)
#define P_HOUSE_ROOM_0         (7+ADD_WORD_OFFSET)

#define P_FIRE_LED0            0
#define P_FIRE_INCH            (P_INPCHANEL_NO_CM+ADD_WORD_OFFSET) // 1
#define P_FIRE_LEDCNT          (2+ADD_WORD_OFFSET)
#define P_FIRE_BRIGHT          (3+ADD_WORD_OFFSET)

#define P_BUTTON_LED           0
#define P_BUTTON_CHMSK         (1+ADD_WORD_OFFSET)
#define P_BUTTON_INCH          (P_INPCHANEL_W_CM+ADD_WORD_OFFSET)   // 2
#define P_BUTTON_DURLO         (3+ADD_WORD_OFFSET)
#define P_BUTTON_DURHI         (4+ADD_WORD_OFFSET)
#define P_BUTTON_VAL0          (5+ADD_WORD_OFFSET)
#define P_BUTTON_VAL1          (6+ADD_WORD_OFFSET)

#define P_PATERN_LED           0
#define P_PATERN_NSTRU         (1+ADD_WORD_OFFSET)
#define P_PATERN_INCH          (P_INPCHANEL_W_CM+ADD_WORD_OFFSET)   // 2
#define P_PATERN_ENABLE        (3+ADD_WORD_OFFSET)
#define P_PATERN_LEDS          (4+ADD_WORD_OFFSET) //  -+
#define P_PATERN_VAL0          (5+ADD_WORD_OFFSET) //   +  Attention: LEDs, Val0, Val1, Off are read at once.
#define P_PATERN_VAL1          (6+ADD_WORD_OFFSET) //   +  => Don't change the sequence of the parameters
#define P_PATERN_OFF           (7+ADD_WORD_OFFSET) //  -+
#define P_PATERN_MODE          (8+ADD_WORD_OFFSET)
#define P_PATERN_T1_L          (9+ADD_WORD_OFFSET)
#define P_PATERN_T1_H          (10+ADD_WORD_OFFSET)

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
#define P_RANDMUX_INP          (P_INPCHANEL_W_CM+ADD_WORD_OFFSET)   // 2
#define P_RANDMUX_MODE         (3+ADD_WORD_OFFSET)
#define P_RANDMUX_MINTIME_L    (4+ADD_WORD_OFFSET)
#define P_RANDMUX_MINTIME_H    (5+ADD_WORD_OFFSET)
#define P_RANDMUX_MAXTIME_L    (6+ADD_WORD_OFFSET)    // must be direct after P_RANDMUX_MINTIME_H
#define P_RANDMUX_MAXTIME_H    (7+ADD_WORD_OFFSET)
#define EP_RANDMUX_INCREMENT   (8+ADD_WORD_OFFSET)    // Insert additional parameters before


#define P_WELDING_LED          0
#define P_WELDING_INP          (1+ADD_WORD_OFFSET)
#define EP_WELDING_INCREMENT   (2+ADD_WORD_OFFSET)    // Insert additional parameters before

#define P_COPYLED_LED          0
#define P_COPYLED_INP          (1+ADD_WORD_OFFSET)
#define P_COPYLED_SRCLED       (2+ADD_WORD_OFFSET)
#define EP_COPYLED_INCREMENT   (3+ADD_WORD_OFFSET*2)    // Insert additional parameters before

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

// Callback types for Callback_t                                                                              // 01.05.20:
#define CT_CHANNEL_CHANGED 0
#define CT_COUNTER_CHANGED 1
#define CT_COUNTER_INITIAL 2

#define PT_INACTIVE 255


// Constants for AnalogPattern
#define SHIFT_FF 16
#define MIN_M    -2147483647
#define MAX_M     2147483647


typedef struct
    {
    uint8_t         Last_t;
    uint8_t         dt;
    }  __attribute__ ((packed)) TimerData_T;

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    }  __attribute__ ((packed)) TimerData16_T;

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         State;
    } __attribute__ ((packed)) PatternData_T;        // 5 Byte

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         State;
    uint16_t        Start_t;
    } __attribute__ ((packed)) AnalogPatternData_T;  // 7 Byte

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         flickertimes;  // 5 Bytes
    }  __attribute__ ((packed)) WeldingData_T;

enum DayState_E : uint8_t
    {
    Unknown = 0,
    SunSet  = 1,  // don't change the sequence
    SunRise = 2
    } ;

typedef struct
    {
    uint8_t         SwitchVal;
    uint8_t         ToDoCnt;
    DayState_E      OldDayState;
    }  __attribute__ ((packed)) Schedule_T;


typedef struct
    {
    uint8_t         Hue;
    uint8_t         Sat;
    uint8_t         Val;
    }  __attribute__ ((packed)) HSV_T;

typedef struct                                                                                                // 07.11.18:
    {
    uint8_t         Val;
    uint8_t         Changed:1;     // Is cleared in New_Local_Var()
    uint8_t         ExtUpdated:1;  // Is set by external functions. In Set_Local_Var() this flag is copied to Changed and cleared
    }  __attribute__ ((packed)) ControlVar_t;

typedef struct
    {
    uint32_t        Cnt;
    uint32_t        On;
    uint16_t        PrintStat;
    }  __attribute__ ((packed)) House_Stat_T;

typedef struct
    {
    uint16_t        Last_t;
    uint8_t         Counter;
    }  __attribute__ ((packed)) Counter_T;

typedef struct
    {
    uint16_t LEDNr;
    uint16_t Start;
    uint16_t Duration;
    }  __attribute__ ((packed)) Sound_Dat_t;   // 6 Byte

typedef struct                                                                                                // 10.06.20:
    {
    uint8_t  Candle_Min_Hue;
    uint8_t  Candle_Max_Hue;
    uint8_t  Candle_Min_BrightnessD;
    uint8_t  Candle_Max_BrightnessD;
    uint8_t  Candle_Min_Brightness;
    uint8_t  Candle_Max_Brightness;
    uint8_t  Candle_Change_Probability;
    uint8_t  Candle_Chg_Hue;
    uint8_t  Candle_Time_Dark;
    }  __attribute__ ((packed)) Candle_Dat_T;

typedef struct
    {
    #if _USE_SET_TVTAB                                                                                        // 10.01.20:
      union {
            struct {
                   uint8_t r, g, b;
                   } __attribute__ ((packed)) ;
            uint8_t raw [3];
            } ;
    #else
      uint8_t         r, g, b;
    #endif
      uint8_t         Last_t;
      uint8_t         dt;
    }  __attribute__ ((packed)) TV_Dat_T;     // 5 Byte

typedef struct
      {
      uint8_t Start:1;
      uint8_t SwitchMode:1;
      } __attribute__ ((packed)) AD_Flags_T;
	
#endif // __OUTPUT_FUNCT_INTDEFS_H__

