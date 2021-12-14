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


 MobaLedLib.h
 ~~~~~~~~~~~~

 This is the main headder file for the MobaLedLib.

*/
#ifndef _MOBALEDLIB_H_
#define _MOBALEDLIB_H_

#define MobaLedLib_Ver  3.0.0   // Adapt also library.properties if changed

#define FASTLED_INTERNAL        // Disable version number message in FastLED library (looks like an error)

#ifdef ARDUINO_RASPBERRY_PI_PICO
#include "PicoFastLED.h"				// Juergen: a small self made implementation of FastLED for PICO (only FastLED code needed by MLL)
#else
#include "FastLED.h"            // The FastLED library must be installed in addition if you got the
                                // error message "..fatal error: FastLED.h: No such file or directory"
                                // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                                //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                                //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#endif
#include "Dprintf.h"            // Debug Ausgaben

#include "Lib_Config.h"         // Library configurarion
#include "IntDefs.h"            // Internal defines

#include "LED_HeartBeat.h"

#include "Macros.h"


#ifdef _HOUSE_STATISTIC
  #define RAMH RAM12     // Additional RAM for statistics: sizeof(House_Stat_T)
#else
  #define RAMH RAM2
#endif

/*
 Wenn die Const() Funktion ueber die Pattern Funktion implementiert werden,
 dann benoetigt das 13 Byte pro Konstante und 5 Bytes RAM.
 Die Proc_Const() Funktion benoetigt 6 Byte pro Zeile und 188 Bytes C-Code
 => Aus der Sicht des FLASH lohnt es die Proc_Const() Funktion zu verwenden
   wenn mehr als 26 Const() Zeilen verwendet werden.
 Man koennte die Pattern Funktion so erweitern, dass sie keinen RAM und keine Zeiten benutzt
 Das wurde 2 Bytes FLASH sparen
*/



#ifdef _USE_SEP_CONST
 #define Const(         LED,Cx,InCh,Val0, Val1)               CONST_T, _CHKL(LED)+RAM0,Cx,_ChkIn(InCh),Val0, Val1,               // 6 Bytes / 13 Bytes mit der Pattern Funktion + 5 Bytes RAM
#else
 #define Const(         LED,Cx,InCh,Val0, Val1)               PatternT1(LED,_NStru(Cx,   1,1),InCh,_Cx2LedCnt(Cx),Val0,Val1,Val0,0,0 Sec, _Cx2P_BLINK(Cx))
#endif

 #define ConstRGB(      LED,InCh, R0, G0, B0, R1, G1, B1)     Const(LED, C1, InCh, R0, R1) Const(LED, C2, InCh, G0, G1) Const(LED, C3, InCh, B0, B1)


#define  House(         LED,InCh, On_Min,On_Limit, ...)                     HOUSE_T,     _CHKL(LED)+RAMH,_ChkIn(InCh),On_Min,     On_Limit,HOUSE_MIN_T,HOUSE_MAX_T,COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,  // Variable number of Room types
#define  HouseT(        LED,InCh, On_Min,On_Limit,Min_T,Max_T,...)          HOUSE_T,     _CHKL(LED)+RAMH,_ChkIn(InCh),On_Min,     On_Limit,Min_T,      Max_T,      COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,  // Variable number of Room types
#define  GasLights(     LED,InCh, ...)                                      HOUSE_T,     _CHKL(LED)+RAMH,_ChkIn(InCh),127,        255,     GAS_MIN_T,  GAS_MAX_T,  COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,  // Variable number of Gas lights
#define  House_Inv(     LED,InCh, On_Min,On_Limit, ...)                     HOUSE_T,     _CHKL(LED)+RAMH,_ChkIn(InCh),On_Min|0x80,On_Limit,HOUSE_MIN_T,HOUSE_MAX_T,COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,  // Variable number of Room types
#define  HouseT_Inv(    LED,InCh, On_Min,On_Limit,Min_T,Max_T,...)          HOUSE_T,     _CHKL(LED)+RAMH,_ChkIn(InCh),On_Min|0x80,On_Limit,Min_T,      Max_T,      COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,  // Variable number of Room types
#define  GasLights_Inv( LED,InCh, ...)                                      HOUSE_T,     _CHKL(LED)+RAMH,_ChkIn(InCh),127   |0x80,255,     GAS_MIN_T,  GAS_MAX_T,  COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,  // Variable number of Gas lights
#define  Fire(          LED,InCh, LedCnt, Brightness)                       FIRE_T,      _CHKL(LED)+RAM1+RAMN(LedCnt),_ChkIn(InCh),LedCnt,Brightness,
#ifdef _NEW_ROOM_COL
 #define Set_ColTab(    r0,g0,b0,r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4,r5,g5,b5,r6,g6,b6,r7,g7,b7,r8,g8,b8,r9,g9,b9,r10,g10,b10,r11,g11,b11,r12,g12,b12,r13,g13,b13,r14,g14,b14,r15,g15,b15,r16,g16,b16) SET_COLTAB_T, r0,g0,b0,r1,g1,b1,r2,g2,b2,r3,g3,b3,r4,g4,b4,r5,g5,b5,r6,g6,b6,r7,g7,b7,r8,g8,b8,r9,g9,b9,r10,g10,b10,r11,g11,b11,r12,g12,b12,r13,g13,b13,r14,g14,b14,r15,g15,b15,r16,g16,b16,
#endif

#if _USE_CANDLE                                                                                               // 10.06.20:
 #define Set_CandleTab(Min_Hue, Max_Hue, Min_BrightnessD, Max_BrightnessD, Min_Brightness, Max_Brightness, Change_Probability, Chg_Hue, DarkProb)  SET_CANDLETAB_T, Min_Hue, Max_Hue, Min_BrightnessD, Max_BrightnessD, Min_Brightness, Max_Brightness, Change_Probability, Chg_Hue, DarkProb,
#endif

#if _USE_SET_TVTAB                                                                                            // 10.01.20:
  #define Set_TV_COL1(  InCh, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R_Min, R_Max, G_Min, G_Max, B_Min, B_Max)   SET_TV_TAB_T, InCh, 0x00, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R_Min, R_Max, G_Min, G_Max, B_Min, B_Max,
  #define Set_TV_COL2(  InCh, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R_Min, R_Max, G_Min, G_Max, B_Min, B_Max)   SET_TV_TAB_T, InCh, 0x01, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R_Min, R_Max, G_Min, G_Max, B_Min, B_Max,
  #define Set_TV_BW1(   InCh, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R,            G,            B)              SET_TV_TAB_T, InCh, 0x00, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R,     R,     G,     G,     B,     B,
  #define Set_TV_BW2(   InCh, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R,            G,            B)              SET_TV_TAB_T, InCh, 0x01, Update_t_Min, Update_t_Max, Min_Brightness, Max_Brightness, R,     R,     G,     G,     B,     B,
#endif

#if _USE_DEF_NEON                                                                                             // 12.01.20:
  #define Set_Def_Neon( InCh, Rand_On, RandOff, Min_Def)                    SET_DEF_NEON_T, InCh, Rand_On, RandOff, Min_Def,
#endif

#define _CHECK_MODE_RF_NOT_SAME(Mode, DstVar1, DstVarN) (((Mode)&RF_NOT_SAME) && (DstVarN) - (DstVar1) == 0) ? ((Mode) & ~RF_NOT_SAME) : (Mode) // Prevent endless loop if DstVar1 == DstVarN     // 13.01.20:

#define  Logic(         DstVar, ...)                                        LOGIC_T,     DstVar, COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,
#define  Counter(       Mode, InCh, Enable, TimeOut, ...)                   COUNTER_T,   _W2B(Mode), InCh+RAM3, Enable, _T2B((TimeOut)/16), COUNT_VARARGS(__VA_ARGS__), __VA_ARGS__,      // TimeOut maximal 17 Min
#define  RandMux(       DstVar1, DstVarN, InCh, Mode, MinTime, MaxTime)     RANDMUX_T,   DstVar1+RAM4, DstVarN, InCh, _CHECK_MODE_RF_NOT_SAME(Mode, DstVar1, DstVarN) | _RAND_MODE2((MinTime), (MaxTime)),                   _Tx2B2(MinTime, MaxTime),               _Tx2B2(MaxTime, MinTime),
#define  RandCntMux(    DstVar1, DstVarN, InCh, Mode, MinTime, MaxTime)     RandMux(DstVar1, DstVarN, InCh, Mode|RF_SEQ, MinTime, MaxTime)
#define  Random(        DstVar, InCh, Mode, MinTime, MaxTime, MinOn, MaxOn) RANDOM_T,    DstVar +RAM4,          InCh, (Mode) | _RAND_MODE4((MinTime), (MaxTime), (MinOn), (MaxOn)), _Tx2B4(MinTime, MaxTime, MinOn, MaxOn), _Tx2B4(MaxTime, MinTime, MinOn, MaxOn), _Tx2B4(MinOn, MaxOn, MinTime, MaxTime), _Tx2B4(MaxOn, MinOn, MinTime, MaxTime),
#define  WeldingCont(   LED, InCh)                                          WELDING_CONT_T, _CHKL(LED)+RAM5, InCh,
#define  Welding(       LED, InCh)                                          WELDING_T,      _CHKL(LED)+RAM5, InCh,
#define  RandWelding(   LED, InCh, Var, MinTime, MaxTime, MinOn, MaxOn)     Random(Var, InCh, RM_NORMAL, MinTime, MaxTime, MinOn, MaxOn) Welding(LED, Var)



#define  CopyLED(       LED, InCh, SrcLED)                                  COPYLED_T,   _CHKL(LED), InCh, _CHKL(SrcLED),
#define  Schedule(      DstVar1, DstVarN, EnableCh, Start, End)             SCHEDULE_T,  DstVar1+RAM3, DstVarN, EnableCh, Start, End,  // Zeit- oder Helligkeitsgesteuertes Ein- und Ausschalten von Variablen.
#define  New_HSV_Group()                                                    NEW_HSV_GROUP_T+RAM3,
#define  New_Local_Var()                                                    NEW_LOCAL_VAR_T+RAM2,             // 07.11.18:
#define  Use_GlobalVar( GlobVarNr)                                          USE_GLOBALVAR_T, GlobVarNr,
#if _USE_INCH_TRIGGER                                                                                         // 02.06.20:
  #define  I2X_USE_LOCALVAR    0x80
  #define  I2X_USE_START1      0x40
  #define  InCh_to_TmpVar(FirstInCh, InCh_Cnt)                              INCH_TO_X_VAR_T,      FirstInCh, (InCh_Cnt-1),                   // 31.05.20:  J: EndInCh is now 0..63 instead of 1..64
  #define  InCh_to_TmpVar1(FirstInCh, InCh_Cnt)                             INCH_TO_X_VAR_T,      FirstInCh, (InCh_Cnt-1)|I2X_USE_START1,    // 07.05.20: // 31.05.20:  J: EndInCh is now 0..63 instead of 1..64
  #define  InCh_to_LocalVar(FirstInCh, InCh_Cnt)                            INCH_TO_X_VAR_T+RAM1, FirstInCh, (InCh_Cnt-1)|I2X_USE_LOCALVAR,
  #define  InCh_to_LocalVar1(FirstInCh, InCh_Cnt)                           INCH_TO_X_VAR_T+RAM1, FirstInCh, (InCh_Cnt-1)|I2X_USE_LOCALVAR|I2X_USE_START1,
  #define  Bin_InCh_to_TmpVar(FirstInCh, InCh_Cnt)                          BIN_INCH_TO_TMPVAR_T,  FirstInCh, (InCh_Cnt-1),     // 18.01.19: // 31.05.20:  J: EndInCh is now 0..7 instead of 1..8
  #define  Bin_InCh_to_TmpVar1(FirstInCh, InCh_Cnt)                         BIN_INCH_TO_TMPVAR_T,  FirstInCh, (InCh_Cnt-1)|0x40,// 07.05.20: // 31.05.20:  J: EndInCh is now 0..7 instead of 1..8
#else
  #define  InCh_to_TmpVar(FirstInCh, InCh_Cnt)                              INCH_TO_TMPVAR_T,  FirstInCh, InCh_Cnt,
  #define  InCh_to_TmpVar1(FirstInCh, InCh_Cnt)                             INCH_TO_TMPVAR1_T, FirstInCh, InCh_Cnt,     // 07.05.20:
  #define  Bin_InCh_to_TmpVar(FirstInCh, InCh_Cnt)                          BIN_INCH_TO_TMPVAR_T,  FirstInCh, InCh_Cnt, // 18.01.19:
  #define  Bin_InCh_to_TmpVar1(FirstInCh, InCh_Cnt)                         BIN_INCH_TO_TMPVAR1_T, FirstInCh, InCh_Cnt, // 07.05.20:
#endif
#define  Button(        LED,Cx,InCh,Duration,Val0, Val1)      PatternT1(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx),Val0,Val1,Val0,PM_SEQUENZ_W_ABORT+PF_SLOW,Duration/16,   _Cx2P_BLINK(Cx))
#define  ButtonNOff(    LED,Cx,InCh,Duration,Val0, Val1)      PatternT1(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx),Val0,Val1,Val0,PM_SEQUENZ_NO_RESTART+PF_SLOW,Duration/16,_Cx2P_BLINK(Cx)) // 12.03.19:
#define  Blinker(       LED,Cx,InCh,Period)                   PatternT1(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx), 0,  255, 0,   PF_SLOW,(Period)/32,                      _Cx2P_BLINK(Cx))     // Blinker  31.10.20:  Added PF_SLOW and /16 to all Blinker functions
#define  BlinkerInvInp( LED,Cx,InCh,Period)                   PatternT1(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx), 0,  255, 0,   PF_SLOW+PF_INVERT_INP,(Period)/32,        _Cx2P_BLINK(Cx))     // Blinker with inverse input
#define  BlinkerHD(     LED,Cx,InCh,Period)                   PatternT1(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx),50,  255, 0,   PF_SLOW, (Period)/32,                     _Cx2P_BLINK(Cx))     // Hell/Dunkel Blinken
#define  Blink2(        LED,Cx,InCh,Pause,Act,Val0,Val1)      PatternT2(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx),Val0,Val1,0,   PF_SLOW, Act/16,     Pause/16,            _Cx2P_BLINK(Cx))     // Blinken mit zwei verschiedenen Helligkeiten
#define  Blink3(        LED,Cx,InCh,Pause,Act,Val0,Val1,Off)  PatternT2(LED,_NStru(Cx,   2,1),InCh,_Cx2LedCnt(Cx),Val0,Val1,Off, PF_SLOW, Act/16,     Pause/16,            _Cx2P_BLINK(Cx))     // Blinken mit zwei verschiedenen Helligkeiten und einen Wert wenn der Blinker aus ist
#define  BlueLight1(    LED,Cx,InCh)                          PatternT4(LED,_NStru(Cx,   4,1),InCh,_Cx2LedCnt(Cx),0,   255, 0,0,  48 ms, 74 ms,  48 ms, 496 ms,_Cx2P_DBLFL(Cx))     // Blaulicht
#define  BlueLight2(    LED,Cx,InCh)                          PatternT4(LED,_NStru(Cx,   4,1),InCh,_Cx2LedCnt(Cx),0,   255, 0,0,  24 ms, 74 ms,  24 ms, 512 ms,_Cx2P_DBLFL(Cx))     // Blaulicht (Andere Periode damit nicht synchron)
#define  Leuchtfeuer(   LED,Cx,InCh)                          PatternT4(LED,_NStru(Cx,   4,1),InCh,_Cx2LedCnt(Cx),0,   255, 0,0,1000 ms,500 ms,1000 ms,1500 ms,_Cx2P_DBLFL(Cx))     // Leuchtfeuer fuer Windrad
#define  LeuchtfeuerALL(LED,InCh)                             PatternT4(LED,_NStru(C_ALL,4,1),InCh,_Cx2LedCnt(C_ALL),0,255, 0,0,1000 ms,500 ms,1000 ms,1500 ms,0b11000111, 0b00000001)// Leuchtfeuer fuer Windrad
#define  Andreaskreuz(  LED,Cx,InCh)                         APatternT1(LED,_NStru(Cx,   0,1),InCh,2,0,255,0,0,250 ms,105)                                                          // Nur fuer einzelne LEDs (C1, C2, C3).
#define  AndreaskrRGB(  LED,InCh)                            APatternT1(LED,0,                InCh,4,0,255,0,0,250 ms,129,24)

// 20.05.20:
// Flashing time see: 2:22.. in https://www.youtube.com/watch?v=aEwmf3gXAQk
#define AndreaskrLT(LED, Single_Cx, InCh, MaxBrightness, LT_Time,FadeTime, OnTime)         \
                                                              Bin_InCh_to_TmpVar(InCh, 1)  \
                                                              XPatternT9(LED,_Cx2StCh(Single_Cx)+132,SI_LocalVar,2,0,MaxBrightness,0,0,FadeTime,FadeTime,LT_Time,FadeTime,OnTime,OnTime,FadeTime,OnTime,FadeTime,240,15,48,204,3  ,63,128,0,0,2,64,0,0,65)

#define AndreaskrLT3(LED, Single_Cx, InCh, MaxBrightness, LT_Time,FadeTime, OnTime)        \
                                                              Bin_InCh_to_TmpVar(InCh, 1)  \
                                                              XPatternT9(LED,_Cx2StCh(Single_Cx)+68,SI_LocalVar,3,0,MaxBrightness,0,0,FadeTime,FadeTime,LT_Time,FadeTime,OnTime,OnTime,FadeTime,OnTime,FadeTime,192,243,0,192,196,48,19  ,63,128,0,0,2,64,0,0,65)

#define AndreaskrLT_RGB(LED, InCh, MaxBrightness, LT_Time,FadeTime, OnTime)                \
                                                              Bin_InCh_to_TmpVar(InCh, 1)  \
                                                              XPatternT9(LED,4,SI_LocalVar,4,0,MaxBrightness,0,0,FadeTime,FadeTime,LT_Time,FadeTime,OnTime,OnTime,FadeTime,OnTime,FadeTime,0,195,195,0,0,3,192,192,3  ,63,128,0,0,2,64,0,0,65)

#define AndreaskrLT3_RGB(LED, InCh, MaxBrightness, LT_Time,FadeTime, OnTime)               \
                                                              Bin_InCh_to_TmpVar(InCh, 1)  \
                                                              XPatternT9(LED,196,SI_LocalVar,9,0,MaxBrightness,0,0,FadeTime,FadeTime,LT_Time,FadeTime,OnTime,OnTime,FadeTime,OnTime,FadeTime,0,0,12,3,48,12,0,0,0,0,0,12,64,5,12,0,48,0,3,80,1  ,63,128,0,0,2,64,0,0,65)

#define  RGB_AmpelX(    LED,InCh)                             PatternT4(LED,0,InCh,18,0,255,0,0,3 Sec,1 Sec,10 Sec,3 Sec,1,2,100,8,0,40,0,134,0,1,2,4,200,16,0,80,0,12)             // Ampel fuer Kreuzung mit 6 RGB LEDs
#define  RGB_AmpelXFade(LED,InCh)                            APatternT8(LED,0,InCh,18,0,255,0,0,200,2 Sec,200,1 Sec,200,10 Sec,200,3 Sec,1,2,4,8,144,33,64,134,0,128,2,0,10,128,33,0,134,0,1,2,4,8,16,32,67,128,12,1,0,5,0,20,0,67,0,12)
#define  RGB_AmpelX_A(  LED,InCh)                             PatternT12(LED,0,InCh,17,0,255,0,0,2 Sec,2 sec,10 Sec,500,500,500,500,500,500,500,500,2 Sec,1,2,50,4,0,10,0,16,0,40,0,64,0,160,0,0,1,128,2,0,4,0,10,192,16,16,32,32,64,70,0,192,0,0,1,0,3,0,4,0,12,0,16,0,48,0,64,0,192,0,24)

#define  AmpelX(        LED,InCh)                             PatternT4(LED,0,InCh,6,0, 255,0,0,3 Sec,1 Sec,10 Sec,3 Sec,201,194,40,73,22,70)                                       // Ampel fuer Kreuzung mit 6 einzelnen LEDs (Achtung keine RGB LEDs)

#define  Flash(         LED, Cx, InCh, Var, MinTime, MaxTime) Random(Var, InCh, RM_NORMAL, MinTime, MaxTime, 30 ms, 30 ms) Const(LED, Cx, Var, 0, 255)                                         // Zufaelliges Blitzlicht (Fotograf)

#define  Def_Neon_Misha(LED, InCh, Var, Min_Delay, Max_Delay) Random(Var, InCh, RM_NORMAL, Min_Delay, Max_Delay, 500 ms, 2000 ms)  \
                                                              PatternT8(LED,8, Var,3,1,128,0,0,300 ms,100 ms,300 ms,100 ms,4 sec,400 ms,100 ms,100 ms,0,254,3,248,191,136,17,128,100)



#define ConstrWarnLightRGB6(   LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT12(LED,28,InCh,18,0,255,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,MaxBrightness,MaxBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MaxBrightness,MaxBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MaxBrightness,MaxBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MaxBrightness,MaxBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MaxBrightness,MaxBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MaxBrightness,MaxBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0,MinBrightness,MinBrightness,0)

#define ConstrWarnLightFlash3( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT6( LED,128,InCh,3, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitE,129,64,0,0,0)
#define ConstrWarnLightFlash4( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT8( LED,0,  InCh,4, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,2,4,8,0,0)
#define ConstrWarnLightFlash5( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT10(LED,128,InCh,5, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,8,64,0,2,16,0,0)
#define ConstrWarnLightFlash6( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT12(LED,0,  InCh,6, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,32,0,4,128,0,16,0,2)
#define ConstrWarnLightFlash7( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT14(LED,192,InCh,7, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,128,0,64,0,32,0,16,0,8,0,4,0)
#define ConstrWarnLightFlash8( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT16(LED,0,  InCh,8, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,2,0,4,0,8,0,16,0,32,0,64,0,128,0)
#define ConstrWarnLightFlash9( LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT18(LED,192,InCh,9, MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,8,0,64,0,0,2,0,16,0,128,0,0,4,0,32,0,0,1,0)
#define ConstrWarnLightFlash10(LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT20(LED,0,  InCh,10,MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,32,0,0,4,0,128,0,0,16,0,0,2,0,64,0,0,8,0,0,1,0,32,0)
#define ConstrWarnLightFlash11(LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT22(LED,192,InCh,11,MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,128,0,0,64,0,0,32,0,0,16,0,0,8,0,0,4,0,0,2,0,0,1,0,128,0,0,64,0,0)
#define ConstrWarnLightFlash12(LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT24(LED,0,  InCh,12,MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,0,2,0,0,4,0,0,8,0,0,16,0,0,32,0,0,64,0,0,128,0,0,0,1,0,0,2,0,0,4,0,0,8,0)
#define ConstrWarnLightFlash13(LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT26(LED,192,InCh,13,MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,0,8,0,0,64,0,0,0,2,0,0,16,0,0,128,0,0,0,4,0,0,32,0,0,0,1,0,0,8,0,0,64,0,0,0,2,0,0,16,0,0)
#define ConstrWarnLightFlash14(LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT28(LED,0,  InCh,14,MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,0,32,0,0,0,4,0,0,128,0,0,0,16,0,0,0,2,0,0,64,0,0,0,8,0,0,0,1,0,0,32,0,0,0,4,0,0,128,0,0,0,16,0,0,0,2,0)
#define ConstrWarnLightFlash15(LED,InCh, MinBrightness, MaxBrightness, OnT, WaitT, WaitE)      PatternT30(LED,192,InCh,15,MinBrightness,MaxBrightness,0,0,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitT,OnT,WaitE,1,0,0,128,0,0,0,64,0,0,0,32,0,0,0,16,0,0,0,8,0,0,0,4,0,0,0,2,0,0,0,1,0,0,128,0,0,0,64,0,0,0,32,0,0,0,16,0,0,0,8,0,0,0,4,0,0)

#define ConstrWarnLight3(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT4( LED,128,InCh,3, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,WaitE,17,1,0,0,0)
#define ConstrWarnLight4(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT5( LED,0,  InCh,4, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,WaitE,33,132,0,0,0,0)
#define ConstrWarnLight5(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT6( LED,128,InCh,5, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,WaitE,65,16,4,1,0,0,0,0)
#define ConstrWarnLight6(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT7( LED,0,  InCh,6, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,129,64,32,16,8,0,0,0,0)
#define ConstrWarnLight7(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT8( LED,128,InCh,7, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,1,1,1,1,1,1,0,0,0,0)
#define ConstrWarnLight8(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT9( LED,0,  InCh,8, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,2,4,8,16,32,64,128,0,0,0,0)
#define ConstrWarnLight9(      LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT10(LED,128,InCh,9, MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,4,16,64,0,1,4,16,64,0,1,0,0,0)
#define ConstrWarnLight10(     LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT11(LED,0,  InCh,10,MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,8,64,0,2,16,128,0,4,32,0,1,8,0,0)
#define ConstrWarnLight11(     LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT12(LED,128,InCh,11,MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,16,0,1,16,0,1,16,0,1,16,0,1,16,0,1,0)
#define ConstrWarnLight12(     LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT13(LED,128,InCh,12,MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,32,0,4,128,0,16,0,2,64,0,8,0,1,32,0,4,128,0,0)
#define ConstrWarnLight13(     LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT14(LED,64, InCh,13,MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,64,0,16,0,4,0,1,64,0,16,0,4,0,1,64,0,16,0,4,0,1,0)
#define ConstrWarnLight14(     LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT15(LED,192,InCh,14,MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,128,0,64,0,32,0,16,0,8,0,4,0,2,0,1,128,0,64,0,32,0,16,0,8,0,0)
#define ConstrWarnLight15(     LED,InCh, MinBrightness, MaxBrightness, OnT, WaitE)             PatternT16(LED,0,  InCh,15,MinBrightness,MaxBrightness,0,0,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,OnT,WaitE,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0)

// Control the charlieplexing Modul
#define Charlie_Buttons(LED, InCh, States)    InCh_to_TmpVar(InCh, States) \
                                              PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,1 ms,15,15,15,35,35,35,55,55,55,75,75,75,95,95,95,115,115,115,135,135,135,155,155,155,175,175,175,195,195,195,215,215,215  ,63,191,191,191,191,191,191,191,191,191,191)
// Old: Channels G & B (35 Bytes)  New 46 Bytes) 31.10.20:
//                                            PatternT1(LED,29,SI_LocalVar,2,0,255,0,0,1 ms,15,15,35,35,55,55,75,75,95,95,115,115,135,135,155,155,175,175,195,195,215,215  ,63,191,191,191,191,191,191,191,191,191,191)


#define _GET_BIN_SIZE(n)  (n<=1?0:(n<=2?1:(n<=4)?2:(n<=8?3:(n<=16?4:999)))) // Number of bits needed for n states (n up to 16)
#define Charlie_Binary(LED, InCh, BinStates)  Bin_InCh_to_TmpVar(InCh, _GET_BIN_SIZE((BinStates))) \
                                              PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,1 ms,15,15,15,35,35,35,55,55,55,75,75,75,95,95,95,115,115,115,135,135,135,155,155,155,175,175,175,195,195,195,215,215,215  ,63,191,191,191,191,191,191,191,191,191,191)
// Old: Channels G & B (35 Bytes)  New 46 Bytes) 31.10.20:
//                                            PatternT1(LED,29,SI_LocalVar,2,0,255,0,0,1 ms,15,15,35,35,55,55,75,75,95,95,115,115,135,135,155,155,175,175,195,195,215,215  ,63,191,191,191,191,191,191,191,191,191,191)


// Entry signal with 3 aspects
#define EntrySignal3_RGB(LED, InCh)                 EntrySignal3_RGB_BT(LED,InCh,128,125 ms)
#define EntrySignal3_RGB_B(LED, InCh,MaxBrightness) EntrySignal3_RGB_BT(LED,InCh,MaxBrightness,125 ms)        // 01.05.20: Added by Juergen
#define EntrySignal3_RGB_T(LED, InCh,FadeTime)      EntrySignal3_RGB_BT(LED,InCh,128,FadeTime)                //    "
#define EntrySignal3_RGB_BT(LED, InCh,MaxBrightness,FadeTime)  \
                                                    InCh_to_TmpVar(InCh, 3)      /* 29.10.19:  Old: 500 ms */                    \
                                                    XPatternT1(LED,160,SI_LocalVar,9,0,MaxBrightness,0,0,FadeTime,64,4,104,0  ,63,191,191)  // 01.05.20: Juergen added parameter MaxBrightness and FadeTime

#define EntrySignal3(LED, InCh, MaxB128)                   InCh_to_TmpVar(InCh, 3)      /* 29.10.19:  Old: 500 ms */                    \
                                                           XPatternT1(LED,224,SI_LocalVar,3,0,MaxB128,0,0,125 ms,145,1  ,63,191,191)

// Departure signal with 4 aspects
#define DepSignal4_RGB(LED, InCh, MaxB128)                 InCh_to_TmpVar(InCh, 4)      /* 29.10.19:  Old: 7 Input channels, 500 ms */  \
                                                           XPatternT1(LED,12,SI_LocalVar,18,0,MaxB128,0,0,125 ms,0,240,0,15,0,0,0,0,0,240,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,240,15,0,240,0,0,16,17,17,1,0  ,63,191,191,191)

#define DepSignal4(LED, InCh, MaxB128)                     InCh_to_TmpVar(InCh, 4)      /* 29.10.19:  Old: 7 Input channels, 500 ms */  \
                                                           XPatternT1(LED,12,SI_LocalVar,5,0,MaxB128,0,0,125 ms,15,240,0,15,0,240,15,240,0,16  ,63,191,191,191)

// Entry signal with 3 aspects
#define EntrySignal3Bin_RGB(LED, InCh, MaxB128)            Bin_InCh_to_TmpVar(InCh, 2)  /* 29.10.19:  Old: 500 ms */                    \
                                                           XPatternT1(LED,160,SI_LocalVar,9,0,MaxB128,0,0,500 ms,64,4,104,0  ,63,191,191)

#define EntrySignal3Bin(LED, InCh, MaxB128)                Bin_InCh_to_TmpVar(InCh, 2)  /* 29.10.19:  Old: 500 ms */                    \
                                                           XPatternT1(LED,224,SI_LocalVar,3,0,MaxB128,0,0,125 ms,145,1  ,63,191,191)

// Departure signal with 4 aspects
#define DepSignal4Bin_RGB(LED, InCh, MaxB128)              Bin_InCh_to_TmpVar(InCh, 2)  /* 29.10.19:  Old: 500 ms */                    \
                                                           XPatternT1(LED,12,SI_LocalVar,18,0,MaxB128,0,0,125 ms,0,240,0,15,0,0,0,0,0,240,0,0,0,0,0,0,0,0,240,0,0,0,0,0,0,240,15,0,240,0,0,16,17,17,1,0  ,63,191,191,191)

#define DepSignal4Bin(LED, InCh, MaxB128)                  Bin_InCh_to_TmpVar(InCh, 2)  /* 29.10.19:  Old: 500 ms */                    \
                                                           XPatternT1(LED,12,SI_LocalVar,5,0,MaxB128,0,0,125 ms,15,240,0,15,0,240,15,240,0,16  ,63,191,191,191)

// 09.06.20:  New signals
#define KS_Vorsignal_Zs3V_RGB(LED, InCh, MaxB128)           InCh_to_LocalVar(InCh, 3)                                            \
                                                            XPatternT9(LED,128,SI_LocalVar,12,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,0,48,224,0,0,1,0,0,237,208,14,236,192,14  ,0,63,128,63,128,64,0,0,1)

#define KS_Vorsignal_Zs3V(LED, InCh, MaxB128)               InCh_to_LocalVar(InCh, 3)                                            \
                                                            XPatternT9(LED,128,SI_LocalVar,4,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,144,32,224,206,12  ,0,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs1_RGB(LED, InCh, MaxB128)      InCh_to_LocalVar(InCh, 4)                                  \
                                                            XPatternT11(LED,128,SI_LocalVar,12,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,0,128,0,0,0,8,0,112,8,0,128,224,8,142,0,8,0  ,0,63,128,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs1(LED, InCh, MaxB128)          InCh_to_LocalVar(InCh, 4)                                  \
                                                            XPatternT11(LED,128,SI_LocalVar,4,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,32,64,80,160,42,2  ,0,63,128,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs6_Zs1_RGB(LED, InCh, MaxB128)  InCh_to_LocalVar(InCh, 4)                                  \
                                                            XPatternT11(LED,96,SI_LocalVar,15,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,0,0,4,0,0,0,16,0,0,56,132,3,0,16,28,8,14,4,0,2,0  ,0,63,128,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs6_Zs1(LED, InCh, MaxB128)      InCh_to_LocalVar(InCh, 4)                                  \
                                                            XPatternT11(LED,32,SI_LocalVar,5,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,64,0,2,42,80,74,8  ,0,63,128,63,128,63,128,64,0,0,1)

// 18.01.21:  Signals from Matthias (schma29)
#define HS_5l_RGB(LED, InCh, MaxB64)                        InCh_to_TmpVar(InCh, 4) \
                                                            XPatternT1(LED,12,SI_LocalVar,15,0,MaxB64,0,0,500 ms,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,240,10,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,240,255  ,0,63,128,63,128,63,128,63)
#define HS_5r_RGB(LED, InCh, MaxB64)                        InCh_to_TmpVar(InCh, 4) \
                                                            XPatternT1(LED,12,SI_LocalVar,15,0,MaxB64,0,0,500 ms,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,240,10,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,240,255  ,0,63,128,63,128,63,128,63)
#define HS_5_Plus_RGB(LED, InCh, MaxB64)                    InCh_to_TmpVar(InCh, 8) \
                                                            XPatternT1(LED,12,SI_LocalVar,27,0,MaxB64,0,0,500 ms,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,5,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,128,136,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,5,0,0,4,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,112,5,0,0,4,0,0,0,52,0,0,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,52,0,0,53,0,0  ,0,63,128,63,128,63,128,63,128,63,128,63,128,63,128,63)
#define Gleissperrsignal_RGB(LED, InCh, MaxB32)             InCh_to_TmpVar(InCh, 2) \
                                                            XPatternT1(LED,12,SI_LocalVar,12,0,MaxB32,0,0,600 ms,0,0,0,0,0,0,0,64,0,0,64,0,0,0,0,0,0,0,51,3,0,51,3,0  ,0,63,128,63)
#define HS_Einfach_RGB(LED, InCh, MaxB32)                   InCh_to_TmpVar(InCh, 3) \
                                                            XPatternT1(LED,12,SI_LocalVar,12,0,MaxB32,0,0,500 ms,0,0,0,0,0,0,0,240,0,0,128,0,0,0,0,0,0,0,0,0,0,192,0,0,0,0,0,0,0,0,191,0,0,192,0,0  ,0,63,128,63,128,63)
#define Vorsignal_RGB(LED, InCh, MaxB16)                    InCh_to_TmpVar(InCh, 2) \
                                                            XPatternT1(LED,12,SI_LocalVar,12,0,MaxB16,0,0,500 ms,0,0,0,0,0,0,0,128,0,0,128,0,0,0,0,0,0,0,122,0,0,122,0,0  ,0,63,128,63)
#define HP_2_2_RGB(LED, InCh, MaxB16)                       InCh_to_TmpVar(InCh, 2) \
                                                            XPatternT1(LED,12,SI_LocalVar,12,0,MaxB16,0,0,500 ms,0,0,0,0,0,0,0,128,0,8,0,0,0,0,0,0,0,0,128,0,0,0,0,8  ,0,63,128,63)
// Binary versions
#define KS_Vorsignal_Zs3VBin_RGB(LED, InCh, MaxB128)        Bin_InCh_to_TmpVar(InCh, 2)                                            \
                                                            XPatternT9(LED,128,SI_LocalVar,12,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,0,48,224,0,0,1,0,0,237,208,14,236,192,14  ,0,63,128,63,128,64,0,0,1)

#define KS_Vorsignal_Zs3VBin(LED, InCh, MaxB128)            Bin_InCh_to_TmpVar(InCh, 2)                                            \
                                                            XPatternT9(LED,128,SI_LocalVar,4,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,144,32,224,206,12  ,0,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs1Bin_RGB(LED, InCh, MaxB128)   Bin_InCh_to_TmpVar(InCh, 2)                                  \
                                                            XPatternT11(LED,128,SI_LocalVar,12,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,0,128,0,0,0,8,0,112,8,0,128,224,8,142,0,8,0  ,0,63,128,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs1Bin(LED, InCh, MaxB128)       Bin_InCh_to_TmpVar(InCh, 2)                                  \
                                                            XPatternT11(LED,128,SI_LocalVar,4,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,32,64,80,160,42,2  ,0,63,128,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs6_Zs1Bin_RGB(LED, InCh, MaxB128) Bin_InCh_to_TmpVar(InCh, 2)                                  \
                                                              XPatternT11(LED,96,SI_LocalVar,15,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,0,0,4,0,0,0,16,0,0,56,132,3,0,16,28,8,14,4,0,2,0  ,0,63,128,63,128,63,128,64,0,0,1)

#define KS_Hauptsignal_Zs3_Zs6_Zs1Bin(LED, InCh, MaxB128)     Bin_InCh_to_TmpVar(InCh, 2)                                  \
                                                              XPatternT11(LED,32,SI_LocalVar,5,0,MaxB128,0,0,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,500 ms,200 ms,500 ms,200 ms,64,0,2,42,80,74,8  ,0,63,128,63,128,63,128,64,0,0,1)

// 18.01.21:  Signals from Matthias (schma29)
#define HS_5lBin_RGB(LED, InCh, MaxB64)            Bin_InCh_to_TmpVar(InCh, 2) \
                                                   XPatternT1(LED,12,SI_LocalVar,15,0,MaxB64,0,0,500 ms,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,240,10,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,240,255  ,0,63,128,63,128,63,128,63)
#define HS_5rBin_RGB(LED, InCh, MaxB64)            Bin_InCh_to_TmpVar(InCh, 2) \
                                                   XPatternT1(LED,12,SI_LocalVar,15,0,MaxB64,0,0,500 ms,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,0,0,0,0,0,0,0,240,10,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,240,255  ,0,63,128,63,128,63,128,63)
#define HS_5_PlusBin_RGB(LED, InCh, MaxB64)        Bin_InCh_to_TmpVar(InCh, 3) \
                                                   XPatternT1(LED,12,SI_LocalVar,27,0,MaxB64,0,0,500 ms,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,8,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,5,0,0,4,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,8,0,0,0,128,136,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,112,5,0,0,4,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,112,5,0,0,4,0,0,0,52,0,0,53,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,0,0,3,0,0,3,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,4,0,0,0,52,0,0,53,0,0  ,0,63,128,63,128,63,128,63,128,63,128,63,128,63,128,63)
#define GleissperrsignalBin_RGB(LED, InCh, MaxB32) Bin_InCh_to_TmpVar(InCh, 1) \
                                                   XPatternT1(LED,12,SI_LocalVar,12,0,MaxB32,0,0,600 ms,0,0,0,0,0,0,0,64,0,0,64,0,0,0,0,0,0,0,51,3,0,51,3,0  ,0,63,128,63)
#define HS_EinfachBin_RGB(LED, InCh, MaxB32)       Bin_InCh_to_TmpVar(InCh, 2) \
                                                   XPatternT1(LED,12,SI_LocalVar,12,0,MaxB32,0,0,500 ms,0,0,0,0,0,0,0,240,0,0,128,0,0,0,0,0,0,0,0,0,0,192,0,0,0,0,0,0,0,0,191,0,0,192,0,0  ,0,63,128,63,128,63)
#define VorsignalBin_RGB(LED, InCh, MaxB16)        Bin_InCh_to_TmpVar(InCh, 1) \
                                                   XPatternT1(LED,12,SI_LocalVar,12,0,MaxB16,0,0,500 ms,0,0,0,0,0,0,0,128,0,0,128,0,0,0,0,0,0,0,122,0,0,122,0,0  ,0,63,128,63)
#define HP_2_2Bin_RGB(LED, InCh, MaxB16)           Bin_InCh_to_TmpVar(InCh, 1) \
                                                   XPatternT1(LED,12,SI_LocalVar,12,0,MaxB16,0,0,500 ms,0,0,0,0,0,0,0,128,0,8,0,0,0,0,0,0,0,0,128,0,0,0,0,8  ,0,63,128,63)



#define  ButtonFunc(                DstVar, InCh, Duration)                      Random(DstVar, InCh, RF_STAY_ON, 0, 0, (Duration), (Duration))             // DstVar is turned on if InCh is activated and stays on for duration (Static (Not Edge) retiggerable mono flop)

// 13.04.20:  Swapped parameter of the RS_FlipFlop's because it makes more sense if the first parameter is R like the name RS_FlipFlop.
//            In addition the first parameter of a DCC command is "Red" = Reset
//            In the excel sheet the standard parameter (#InCh) is connected to the S input because it is more likely that
//            several FlipFlops are disabled with a common Rest variable
//            To Use a RS-flip-flop with DCC the "GREEN" channel has to be assigned to

// Single DstVar (Q)
#define  MonoFlop(                  DstVar, InCh, Duration)                      Counter(_CM_RS_FlipFlop1,               InCh,   SI_0,   Duration, DstVar)  // Retrigger with rising endge (Like ButtonFunc, but edge triggered)
#define  MonoFlopReset(             DstVar, InCh, R_InCh, Duration)              Counter(_CM_RS_FlipFlop1,               InCh,   R_InCh, Duration, DstVar)  // Retrigger with rising endge with additional Reset input
#define  MonoFlopLongReset(         DstVar, InCh, Duration)                      Counter(_CM_RS_FlipFlop1|CF_RESET_LONG, InCh,   SI_0,   Duration, DstVar)  // Retrigger with rising endge, Long press disables
#define  RS_FlipFlop(               DstVar, R_InCh, S_InCh)                      Counter(_CM_RS_FlipFlop1,               S_InCh, R_InCh, 0,        DstVar)  // Edge tiggered RS Flip Flop
#define  RS_FlipFlopTimeout(        DstVar, R_InCh, S_InCh, Timeout)             Counter(_CM_RS_FlipFlop1,               S_InCh, R_InCh, Timeout,  DstVar)  // Edge tiggered RS Flip Flop and timeout
#define  T_FlipFlopReset(           DstVar, T_InCh, R_InCh)                      Counter(_CM_T_FlipFlop1,                T_InCh, R_InCh, 0,        DstVar)  // Toggle Flip Flop with reset
#define  T_FlipFlopResetTimeout(    DstVar, T_InCh, R_InCh, Timeout)             Counter(_CM_T_FlipFlop1,                T_InCh, R_InCh, Timeout,  DstVar)  // Toggle Flip Flop with restart and timeout

// Single DstVar which is controlled inverse (Q')  => DstVar is active at the begining and when a timeout timeout occoures. It's deactivated with the trigger
#define  MonoFlopInv(               DstVar, InCh, Duration)                      Counter(_CM_RS_FlipFlop2,               InCh,   SI_0,   Duration, DstVar,  DstVar)  // Retrigger with rising endge (Like ButtonFunc, but edge triggered)
#define  MonoFlopInvLongReset(      DstVar, InCh, Duration)                      Counter(_CM_RS_FlipFlop2|CF_RESET_LONG, InCh,   SI_0,   Duration, DstVar,  DstVar)  // Retrigger with rising endge, Long press disables
#define  RS_FlipFlopInv(            DstVar, R_InCh, S_InCh)                      Counter(_CM_RS_FlipFlop2,               S_InCh, R_InCh, 0,        DstVar,  DstVar)  // Edge tiggered RS Flip Flop
#define  RS_FlipFlopInvTimeout(     DstVar, R_InCh, S_InCh, Timeout)             Counter(_CM_RS_FlipFlop2,               S_InCh, R_InCh, Timeout,  DstVar,  DstVar)  // Edge tiggered RS Flip Flop and timeout
#define  T_FlipFlopInvReset(        DstVar, T_InCh, R_InCh)                      Counter(_CM_T_FlipFlopReset2,           T_InCh, R_InCh, 0,        DstVar,  DstVar)  // Toggle Flip Flop with reset Q'
#define  T_FlipFlopInvResetTimeout( DstVar, T_InCh, R_InCh, Timeout)             Counter(_CM_T_FlipFlopReset2,           T_InCh, R_InCh, Timeout,  DstVar,  DstVar)  // Toggle Flip Flop with restart and timeout Q'

// Dual DstVar (Q and Q')
#define  MonoFlop2(                 DstVar0, DstVar1, InCh, Duration)            Counter(_CM_RS_FlipFlop2,               InCh,   SI_0,   Duration, DstVar0, DstVar1)  // Two Outputs, retrigger with rising endge (Like ButtonFunc, but edge triggered)
#define  MonoFlop2LongReset(        DstVar0, DstVar1, InCh, Duration)            Counter(_CM_RS_FlipFlop2|CF_RESET_LONG, InCh,   SI_0,   Duration, DstVar0, DstVar1)  // Two Outputs, retrigger with rising endge, Long press disables
#define  RS_FlipFlop2(              DstVar0, DstVar1, R_InCh, S_InCh)            Counter(_CM_RS_FlipFlop2,               S_InCh, R_InCh, 0,        DstVar0, DstVar1)  // Edge tiggered RS Flip Flop
#define  RS_FlipFlop2Timeout(       DstVar0, DstVar1, R_InCh, S_InCh, Timeout)   Counter(_CM_RS_FlipFlop2,               S_InCh, R_InCh, Timeout,  DstVar0, DstVar1)  // Edge tiggered RS Flip Flop and timeout
#define  T_FlipFlop2Reset(          DstVar0, DstVar1, T_InCh, R_InCh)            Counter(_CM_T_FlipFlopReset2,           T_InCh, R_InCh, 0,        DstVar0, DstVar1)  // Toggle Flip Flop with reset
#define  T_FlipFlop2ResetTimeout(   DstVar0, DstVar1, T_InCh, R_InCh, Timeout)   Counter(_CM_T_FlipFlopReset2,           T_InCh, R_InCh, Timeout,  DstVar0, DstVar1)  // Toggle Flip Flop with restart and timeout

#define  RGB_Heartbeat(LED)                                           \
             New_HSV_Group()                                          \
             APatternT2(LED,192,SI_1,1,0,255,0,PM_HSV,60 Sek,0 ms,1)  \
             APatternT1(LED, 194,SI_1,1,10,255,0,PM_HSV|PF_EASEINOUT,1 Sek,1)   // Nicht ganz aus gehen weil es sonst bei manchen Farben flackert

#define  RGB_Heartbeat2(LED, MinBrightness, MaxBrightness)            \
             New_HSV_Group()                                          \
             APatternT2(LED,192,SI_1,1,0,255,0,PM_HSV,60 Sek,0 ms,1)  \
             APatternT1(LED, 194,SI_1,1,MinBrightness,MaxBrightness,0,PM_HSV|PF_EASEINOUT,1 Sek,1)            // 18.01.19:
                                                                                                              // 14.12.21: add additional heartbeat macro
#define  RGB_Heartbeat_Color(LED, MinBrightness, MaxBrightness, Color, Duration) \
             New_HSV_Group()                                          \
             APatternT1(LED,224,SI_1,1,Color,Color,0,PM_HSV,0 ms,1)  \
             APatternT1(LED, 194,SI_1,1,MinBrightness,MaxBrightness,0,PM_HSV|PF_EASEINOUT,Duration,1)       

// Push Button functions which count the button press and activates temporary variables
// The button flashes n times if the button was pressed n times.
// Parameter:
//   B_LED:    LED Number of the LED in the PUSH button (Example NUM_LEDS_1)
//   B_LED_Cx: Channel of the LED (C1..C3)
//   InCh:     Number of the input channel which is used to read in the button
//   TmpNr:    First temporaty variable. There function PushButton_w_LED_0_2 uses 3 temporary variables. PushButton_w_LED_0_3 uses 4, ...
//   Rotate:   If the buton is pressed n+1 times the first temporary variable is activated again
//   Timeout:  Time when the push button action is disabled again
//

// 10.03.19:  Added B_LED_Cx to be able to use the functions with all single button LEDs (C_1, C_2 and C_3)
#define Status_Button_0_1(LED,B_LED_Cx, Val1)  PatternT1(LED,40 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,Val1,0,0,0.4 Sec,57,0  ,63,192,0,0,1)  // 01.04.20:
#define Status_Button_0_2(LED,B_LED_Cx, Val1)  PatternT1(LED,232+_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,Val1,0,0,0.4 Sec,57,128,227,0,0  ,63,192,0,0,1,192,0,0,0,0,2)
#define Status_Button_0_3(LED,B_LED_Cx, Val1)  PatternT1(LED,232+_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,Val1,0,0,0.4 Sec,57,128,227,0,142,227,0,0  ,63,192,0,0,1,192,0,0,0,0,2,192,0,0,0,0,0,0,3)
#define Status_Button_0_4(LED,B_LED_Cx, Val1)  PatternT1(LED,40 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,Val1,0,0,0.4 Sec,57,128,227,0,142,227,0,142,227,56,0  ,63,192,0,0,1,192,0,0,0,0,2,192,0,0,0,0,0,0,3,192,0,0,0,0,0,0,0,0,4)
#define Status_Button_0_5(LED,B_LED_Cx, Val1)  PatternT1(LED,168+_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,Val1,0,0,0.4 Sec,57,128,227,0,142,227,0,142,227,56,128,227,56,142,3,0  ,63,192,0,0,1,192,0,0,0,0,2,192,0,0,0,0,0,0,3,192,0,0,0,0,0,0,0,0,4,192,0,0,0,0,0,0,0,0,0,0,5)

// 01.04.20:  Buttons where the background light could be changed (The functions above have a const background light of 1/8 of Val1)
#define Status_ButtonBL_0_1(LED,B_LED_Cx,Val1,BackLight) PatternT1(LED,28 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,255,0,0,0.4 Sec,BackLight,Val1,0,0,0  ,63,192,0,0,1)
#define Status_ButtonBL_0_2(LED,B_LED_Cx,Val1,BackLight) PatternT1(LED,28 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,255,0,0,0.4 Sec,BackLight,Val1,0,0,0,Val1,0,Val1,0,0,0  ,63,192,0,0,1,192,0,0,0,0,2)
#define Status_ButtonBL_0_3(LED,B_LED_Cx,Val1,BackLight) PatternT1(LED,28 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,255,0,0,0.4 Sec,BackLight,Val1,0,0,0,Val1,0,Val1,0,0,0,Val1,0,Val1,0,Val1,0,0,0  ,63,192,0,0,1,192,0,0,0,0,2,192,0,0,0,0,0,0,3)
#define Status_ButtonBL_0_4(LED,B_LED_Cx,Val1,BackLight) PatternT1(LED,28 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,255,0,0,0.4 Sec,BackLight,Val1,0,0,0,Val1,0,Val1,0,0,0,Val1,0,Val1,0,Val1,0,0,0,Val1,0,Val1,0,Val1,0,Val1,0,0,0  ,63,192,0,0,1,192,0,0,0,0,2,192,0,0,0,0,0,0,3,192,0,0,0,0,0,0,0,0,4)
#define Status_ButtonBL_0_5(LED,B_LED_Cx,Val1,BackLight) PatternT1(LED,28 +_Cx2StCh(B_LED_Cx),SI_LocalVar,1,0,255,0,0,0.4 Sec,BackLight,Val1,0,0,0,Val1,0,Val1,0,0,0,Val1,0,Val1,0,Val1,0,0,0,Val1,0,Val1,0,Val1,0,Val1,0,0,0,Val1,0,Val1,0,Val1,0,Val1,0,Val1,0,0,0  ,63,192,0,0,1,192,0,0,0,0,2,192,0,0,0,0,0,0,3,192,0,0,0,0,0,0,0,0,4,192,0,0,0,0,0,0,0,0,0,0,5)


#define Status_Button_RGB_0_1(LED,R0,G0,B0,R10,G10,B10,R11,G11,B11)                                                                                                 PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,0.5 Sec,R0,G0,B0,R10,G10,B10,R11,G11,B11  ,63,192,1)
#define Status_Button_RGB_0_2(LED,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21)                                                                         PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,0.5 Sec,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21  ,63,192,1,192,2)
#define Status_Button_RGB_0_3(LED,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31)                                                 PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,0.5 Sec,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31  ,63,192,1,192,2,192,3)
#define Status_Button_RGB_0_4(LED,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41)                         PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,0.5 Sec,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41  ,63,192,1,192,2,192,3,192,4)
#define Status_Button_RGB_0_5(LED,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41,R50,G50,B50,R51,G51,B51) PatternT1(LED,28,SI_LocalVar,3,0,255,0,0,0.5 Sec,R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41,R50,G50,B50,R51,G51,B51  ,63,192,1,192,2,192,3,192,4,192,5)


// 28.04.20:
#define PushButton_0_1(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar, InCh, SI_1, Timeout, TmpNr+0, TmpNr+1)                                        \
            InCh_to_TmpVar(TmpNr, 1+1)

#define PushButton_0_2(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                               \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar, InCh, SI_1, Timeout, TmpNr+0, TmpNr+1, TmpNr+2)                              \
            InCh_to_TmpVar(TmpNr, 2+1)

#define PushButton_0_3(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                               \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar, InCh, SI_1, Timeout, TmpNr+0, TmpNr+1, TmpNr+2, TmpNr+3)                     \
            InCh_to_TmpVar(TmpNr, 3+1)

#define PushButton_0_4(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                               \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar, InCh, SI_1, Timeout, TmpNr+0, TmpNr+1, TmpNr+2, TmpNr+3, TmpNr+4)            \
            InCh_to_TmpVar(TmpNr, 4+1)

#define PushButton_0_5(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                               \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar, InCh, SI_1, Timeout, TmpNr+0, TmpNr+1, TmpNr+2, TmpNr+3, TmpNr+4, TmpNr+5)   \
            InCh_to_TmpVar(TmpNr, 5+1)

// 21.05.20:
#define PushButton2I_0_1(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                              \
            Logic(LocInCh, InCh+1 OR InCh2==SI_0?InCh+1:InCh2)                                                                                                                                  \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar|CF_INV_ENABLE, LocInCh, InCh, Timeout, TmpNr+0, TmpNr+1)                                       \
            InCh_to_TmpVar(TmpNr, 1+1)

#define PushButton2I_0_2(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                              \
            Logic(LocInCh, InCh+1 OR InCh2==SI_0?InCh+1:InCh2)                                                                                                                                  \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar|CF_INV_ENABLE, LocInCh, InCh, Timeout, TmpNr+0, TmpNr+1, TmpNr+2)                              \
            InCh_to_TmpVar(TmpNr, 2+1)

#define PushButton2I_0_3(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                              \
            Logic(LocInCh, InCh+1 OR InCh2==SI_0?InCh+1:InCh2)                                                                                                                                  \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar|CF_INV_ENABLE, LocInCh, InCh, Timeout, TmpNr+0, TmpNr+1, TmpNr+2, TmpNr+3)                     \
            InCh_to_TmpVar(TmpNr, 3+1)

#define PushButton2I_0_4(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                              \
            Logic(LocInCh, InCh+1 OR InCh2==SI_0?InCh+1:InCh2)                                                                                                                                  \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar|CF_INV_ENABLE, LocInCh, InCh, Timeout, TmpNr+0, TmpNr+1, TmpNr+2, TmpNr+3, TmpNr+4)            \
            InCh_to_TmpVar(TmpNr, 4+1)

#define PushButton2I_0_5(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                              \
            Logic(LocInCh, InCh+1 OR InCh2==SI_0?InCh+1:InCh2)                                                                                                                                  \
            Counter((Rotate?CF_ROTATE:0)|(Use0?0:CF_SKIP0)|(ResetLong?CF_RESET_LONG:0)|OptCtrPar|CF_INV_ENABLE, LocInCh, InCh, Timeout, TmpNr+0, TmpNr+1, TmpNr+2, TmpNr+3, TmpNr+4, TmpNr+5)   \
            InCh_to_TmpVar(TmpNr, 5+1)

// 01.04.20:  Added: PushButton_w_LED_0_1
// 13.04.20:  Added: Use0, ResetLong, OptCtrPar
#define PushButton_w_LED_0_1(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                        \
            PushButton_0_1(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_Button_0_1(B_LED, B_LED_Cx, 255)

#define PushButton_w_LED_0_2(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                        \
            PushButton_0_2(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_Button_0_2(B_LED, B_LED_Cx, 255)

#define PushButton_w_LED_0_3(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                        \
            PushButton_0_3(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_Button_0_3(B_LED, B_LED_Cx,255)

#define PushButton_w_LED_0_4(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                        \
            PushButton_0_4(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_Button_0_4(B_LED, B_LED_Cx,255)

#define PushButton_w_LED_0_5(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                        \
            PushButton_0_5(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_Button_0_5(B_LED, B_LED_Cx,255)

// 01.04.20:
#define PushButton_w_LED_BL_0_1(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton_0_1(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_1(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton_w_LED_BL_0_2(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton_0_2(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_2(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton_w_LED_BL_0_3(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton_0_3(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_3(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton_w_LED_BL_0_4(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton_0_4(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_4(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton_w_LED_BL_0_5(B_LED, B_LED_Cx, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton_0_5(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_5(B_LED, B_LED_Cx, Val1, BackLight)

// 28.04.20:
#define PushButton_RGB_0_1(B_LED, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11)                                                                                                 \
            PushButton_0_1(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_1(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11)

#define PushButton_RGB_0_2(B_LED, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21)                                                                         \
            PushButton_0_2(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_2(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21)

#define PushButton_RGB_0_3(B_LED, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31)                                                 \
            PushButton_0_3(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_3(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31)

#define PushButton_RGB_0_4(B_LED, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41)                         \
            PushButton_0_4(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_4(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41)

#define PushButton_RGB_0_5(B_LED, InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41,R50,G50,B50,R51,G51,B51) \
            PushButton_0_5(InCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_5(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41,R50,G50,B50,R51,G51,B51)

// 22.05.20:
#define PushButton2I_w_LED_BL_0_1(B_LED, B_LED_Cx, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton2I_0_1(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_1(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton2I_w_LED_BL_0_2(B_LED, B_LED_Cx, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton2I_0_2(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_2(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton2I_w_LED_BL_0_3(B_LED, B_LED_Cx, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton2I_0_3(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_3(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton2I_w_LED_BL_0_4(B_LED, B_LED_Cx, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton2I_0_4(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_4(B_LED, B_LED_Cx, Val1, BackLight)

#define PushButton2I_w_LED_BL_0_5(B_LED, B_LED_Cx, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, Val1, BackLight)                                                    \
            PushButton2I_0_5(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                           \
            Status_ButtonBL_0_5(B_LED, B_LED_Cx, Val1, BackLight)


#define PushButton2I_RGB_0_1(B_LED, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11)                                                                                                 \
            PushButton2I_0_1(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_1(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11)

#define PushButton2I_RGB_0_2(B_LED, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21)                                                                         \
            PushButton2I_0_2(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_2(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21)

#define PushButton2I_RGB_0_3(B_LED, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31)                                                 \
            PushButton2I_0_3(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_3(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31)

#define PushButton2I_RGB_0_4(B_LED, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41)                         \
            PushButton2I_0_4(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_4(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41)

#define PushButton2I_RGB_0_5(B_LED, InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41,R50,G50,B50,R51,G51,B51) \
            PushButton2I_0_5(InCh, InCh2, LocInCh, TmpNr, Rotate, Use0, ResetLong, OptCtrPar, Timeout)                                                                                                                                          \
            Status_Button_RGB_0_5(B_LED, R0,G0,B0,R10,G10,B10,R11,G11,B11,R20,G20,B20,R21,G21,B21,R30,G30,B30,R31,G31,B31,R40,G40,B40,R41,G41,B41,R50,G50,B50,R51,G51,B51)

#define Servo2(LED, InCh, Single_Cx, Servo1, Servo2, ServoT)                                                          \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT1(LED,_Cx2StCh(Single_Cx)+28,SI_LocalVar,1,0,255,0,0,ServoT,0,Servo1,Servo2  ,127,129,129)

#define Servo3(LED, InCh, Single_Cx, Servo1, Servo2, Servo3, ServoT)                                                  \
            InCh_to_TmpVar1(InCh, 3)                                                                                  \
            PatternT1(LED,_Cx2StCh(Single_Cx)+28,SI_LocalVar,1,0,255,0,0,ServoT,0,Servo1,Servo2,Servo3  ,127,129,129,129)

#define Servo4(LED, InCh, Single_Cx, Servo1, Servo2, Servo3, Servo4, ServoT)                                          \
            InCh_to_TmpVar1(InCh, 4)                                                                                  \
            PatternT1(LED,_Cx2StCh(Single_Cx)+28,SI_LocalVar,1,0,255,0,0,ServoT,0,Servo1,Servo2,Servo3,Servo4  ,127,129,129,129,129)

#define Servo5(LED, InCh, Single_Cx, Servo1, Servo2, Servo3, Servo4, Servo5, ServoT)                                  \
            InCh_to_TmpVar1(InCh, 5)                                                                                  \
            PatternT1(LED,_Cx2StCh(Single_Cx)+28,SI_LocalVar,1,0,255,0,0,10 Sec,0,Servo1,Servo2,Servo3,Servo4,Servo5  ,127,129,129,129,129,129)

// Herzstueck for the PCB Ver. 1.0 where the Red and Green Channels are swapped ;-(                           // 25.07.20:
#define Herz_BiRelais_V1_AB(LED, InCh, DelayHerz1)                                                                    \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,192,SI_LocalVar,2,0,255,0,0,200 ms, DelayHerz1,32,1  ,127,128,1,128,1)

#define Herz_BiRelais_I_V1_AB(LED, InCh, DelayHerz1)                                                                  \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,192,SI_LocalVar,2,0,255,0,0,200 ms,DelayHerz1,16,2  ,127,128,1,128,1)


#define Herz_BiRelais_V1_CD(LED, InCh, DelayHerz1)                                                                    \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,  98,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,4   ,127,128,1,128,1)                       \
            PatternT2(LED+1,97,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,16  ,127,128,1,128,1)

#define Herz_BiRelais_I_V1_CD(LED, InCh, DelayHerz1)                                                                  \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,  98,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,16 ,127,128,1,128,1)                        \
            PatternT2(LED+1,97,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,4  ,127,128,1,128,1)

#define Herz_BiRelais_V1_EF(LED, InCh, DelayHerz1)                                                                    \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,96,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,4  ,127,128,1,128,1)                          \
            PatternT2(LED,98,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,16 ,127,128,1,128,1)

#define Herz_BiRelais_I_V1_EF(LED, InCh, DelayHerz1)                                                                  \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,96,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,16 ,127,128,1,128,1)                          \
            PatternT2(LED,98,SI_LocalVar,1,0,255,0,0,200 ms, DelayHerz1,4  ,127,128,1,128,1)


//Herzstueck 18.05.20:
#define Herz_BiRelais(LED, InCh, Single_Cx, DelayHerz1)                                                               \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,_Cx2StCh(Single_Cx)+192,SI_LocalVar,2,0,255,0,0,200 ms,DelayHerz1,16,2  ,127,128,1,128,1)

#define Herz_BiRelais_I(LED, InCh, Single_Cx, DelayHerz1)                                                             \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,_Cx2StCh(Single_Cx)+192,SI_LocalVar,2,0,255,0,0,200 ms,DelayHerz1,32,1  ,127,128,1,128,1)

#if 1  // Herzstueck with FF
// Saves 2 Bytes RAM and 6 Bytes Flash compared to the Pattern version below, but it uses 7 instead of 6 InCh variables
#define _Herz_MoRelais_RS(LED, InCh, Single_Cx, DelayHerz1, TmpNr, Inv)                                               \
            RS_FlipFlopTimeout(TmpNr+0, InCh, InCh+1, DelayHerz1 + 200 ms)                                            \
            RS_FlipFlopTimeout(TmpNr+1, InCh, InCh+1, DelayHerz1)                                                     \
            RS_FlipFlopTimeout(TmpNr+2, InCh+1, InCh, DelayHerz1 + 200 ms)                                            \
            RS_FlipFlopTimeout(TmpNr+3, InCh+1, InCh, DelayHerz1)                                                     \
            Logic(TmpNr+4, TmpNr+0 AND NOT TmpNr+1)                                                                   \
            Logic(TmpNr+5, TmpNr+2 AND NOT TmpNr+3)                                                                   \
            RS_FlipFlop(TmpNr+6, TmpNr+5-Inv, TmpNr+4+Inv)                                                            \
            Const(LED, Single_Cx, TmpNr+6, 0, 255)

#define Herz_MoRelais(LED, InCh, Single_Cx, DelayHerz1, TmpNr)                                                        \
            _Herz_MoRelais_RS(LED, InCh, Single_Cx, DelayHerz1, TmpNr, 0)

#define Herz_MoRelais_I(LED, InCh, Single_Cx, DelayHerz1, TmpNr)                                                      \
            _Herz_MoRelais_RS(LED, InCh, Single_Cx, DelayHerz1, TmpNr, 1)

#else // Uses the Pattern function to drive the relais (Problems with the EEPron Save version prior 19.05.20)
#define _Herz_MoRelais_PF(LED, InCh, Single_Cx, DelayHerz1, TmpNr, Bits)                                              \
            RS_FlipFlopTimeout(TmpNr+0, InCh, InCh+1, DelayHerz1 + 200 ms)                                            \
            RS_FlipFlopTimeout(TmpNr+1, InCh, InCh+1, DelayHerz1)                                                     \
            RS_FlipFlopTimeout(TmpNr+2, InCh+1, InCh, DelayHerz1 + 200 ms)                                            \
            RS_FlipFlopTimeout(TmpNr+3, InCh+1, InCh, DelayHerz1)                                                     \
            Logic(TmpNr+4, TmpNr+0 AND NOT TmpNr+1)                                                                   \
            Logic(TmpNr+5, TmpNr+2 AND NOT TmpNr+3)                                                                   \
            InCh_to_TmpVar1(TmpNr+4, 2)                                                                               \
            PatternT1(LED,_Cx2StCh(Single_Cx)+160,SI_LocalVar,1,0,255,0,0,200 ms,Bits  ,63,191,191)

#define Herz_MoRelais(LED, InCh, Single_Cx, DelayHerz1, TmpNr)                                                        \
            _Herz_MoRelais_PF(LED, InCh, Single_Cx, DelayHerz1, TmpNr, 2)

#define Herz_MoRelais_I(LED, InCh, Single_Cx, DelayHerz1, TmpNr)                                                      \
            _Herz_MoRelais_PF(LED, InCh, Single_Cx, DelayHerz1, TmpNr, 4)
#endif


#define Herz_2MoRelais(LED, InCh, Single_Cx, DelayHerz2)                                                              \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,_Cx2StCh(Single_Cx)+192,SI_LocalVar,2,0,255,0,0,200 ms,DelayHerz2,16,2  ,127,128,63,128,63)

#define Herz_2MoRelais_I(LED, InCh, Single_Cx, DelayHerz2)                                                            \
            InCh_to_TmpVar1(InCh, 2)                                                                                  \
            PatternT2(LED,_Cx2StCh(Single_Cx)+192,SI_LocalVar,2,0,255,0,0,200 ms,DelayHerz2,32,1  ,127,128,63,128,63)


//--------------------------------------- MP3-TF-16P Sound modul ---------------------------------------------

                                //  ADKey1  ADKey2
#define SOUND_ADKEY10	11		//	Play Mode	14
#define SOUND_ADKEY9	17		//	U/SD/SPI	13
#define SOUND_ADKEY8	22		//	Loop All	12
#define SOUND_ADKEY7	25		//	Pause/Play	11
#define SOUND_ADKEY6	29		//	Prev/Vol-	10
#define SOUND_ADKEY5	37		//	Next/Vol+	9
#define SOUND_ADKEY4    49      //  4           8
#define SOUND_ADKEY3    70      //  3           7
#define SOUND_ADKEY2    134     //  2           6
#define SOUND_ADKEY1    255     //  1           5


// Sound functions could be disable / enabled with the variable SI_Enable_Sound
#define Sound_ADKey(    LED, InCh, ADKey1, ADKey2)         PatternTE2(LED,28,InCh,SI_Enable_Sound,2,0,255,0,PM_SEQUENZ_NO_RESTART,200 ms,10 ms,ADKey1,ADKey2,0,0)

// Volume changing is independent from SI_Enable_Sound
#define Sound_ADKeyTime(LED, InCh, ADKey1, ADKey2, Time)   PatternT2( LED,28,InCh,                  2,0,255,0,PM_SEQUENZ_NO_RESTART,Time,10 ms,ADKey1,ADKey2,0,0)

#define Sound_Prev(      LED, InCh)         Sound_ADKey(LED, InCh, SOUND_ADKEY5,  0)
#define Sound_Next(      LED, InCh)         Sound_ADKey(LED, InCh, SOUND_ADKEY6,  0)
#define Sound_PausePlay( LED, InCh)         Sound_ADKey(LED, InCh, SOUND_ADKEY7,  0)
#define Sound_Loop(      LED, InCh)         Sound_ADKey(LED, InCh, SOUND_ADKEY8,  0)
#define Sound_USDSPI(    LED, InCh)         Sound_ADKey(LED, InCh, SOUND_ADKEY9,  0)  // ? U / SD / SPI
#define Sound_PlayMode(  LED, InCh)         Sound_ADKey(LED, InCh, SOUND_ADKEY10, 0)  // If Loop is active the button toggles between  Sequence, Sequence, Repeat same, Random, Loop off

// Volume
// First Volume change after 1000 ms
// Then volume is changed every 150 ms
// According to the documentation there are 30 stepps
#define Sound_DecVol(    LED, InCh, Steps)  Sound_ADKeyTime(LED, InCh, SOUND_ADKEY5,  0, 850 ms - 75 ms + (Steps-1) * 150 ms)
#define Sound_IncVol(    LED, InCh, Steps)  Sound_ADKeyTime(LED, InCh, SOUND_ADKEY6,  0, 850 ms - 75 ms + (Steps-1) * 150 ms)

#define Sound_Seq1( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_ADKEY1, 0)
#define Sound_Seq2( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_ADKEY2, 0)
#define Sound_Seq3( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_ADKEY3, 0)
#define Sound_Seq4( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_ADKEY4, 0)
#define Sound_Seq5( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY1)
#define Sound_Seq6( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY2)
#define Sound_Seq7( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY3)
#define Sound_Seq8( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY4)
#define Sound_Seq9( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY5)
#define Sound_Seq10(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY6)
#define Sound_Seq11(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY7)
#define Sound_Seq12(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY8)
#define Sound_Seq13(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY9)
#define Sound_Seq14(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_ADKEY10)

#define _LocalVar_Sound(LED)  PatternT1(LED,28,SI_LocalVar,2,0,255,0,0,200 ms,0,0,SOUND_ADKEY1,0,SOUND_ADKEY2,0,SOUND_ADKEY3,0,SOUND_ADKEY4,0,0,SOUND_ADKEY1,0,SOUND_ADKEY2,0,SOUND_ADKEY3,0,SOUND_ADKEY4,0,SOUND_ADKEY5,0,SOUND_ADKEY6,0,SOUND_ADKEY7,0,SOUND_ADKEY8,0,SOUND_ADKEY9,0,SOUND_ADKEY10,0,0  ,1,129,129,129,129,129,129,129,129,129,129,129,129,129,129,127)

#define Sound_PlayRandom(LED, InCh, MaxSoundNr)                                                                      \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_RANDOM | CF_SKIP0,   InCh, SI_1, 0 Sec, MaxSoundNr+1)                    \
              _LocalVar_Sound(LED)

#define Sound_Next_of_N_Reset(LED, InCh, InReset, MaxSoundNr)                                                        \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_SKIP0 | CF_INV_ENABLE,   InCh, InReset, 0 Sec, MaxSoundNr+1) \
              _LocalVar_Sound(LED)

#define Sound_Next_of_N(LED, InCh, MaxSoundNr)                                                                       \
             Sound_Next_of_N_Reset(LED, InCh, SI_0, MaxSoundNr)

//--------------------------------------- MP3-TF-16P Sound modul with 4.7uF capacitor and 2KHz WS2811 ------------------------ 13.10.21:
// The new WS2811 modules (model year >2016) generate a 2 kHz PWM Signal (Old 400 Hz)
// Here the filter capacitor could be reduced 4.7uF (Instead of 22uF) to support also the new
// MP3-TF-16P modul which use the GDB3200B chip. With 22uF the new sound modules didn't work.
// Modules with the old MH2024K chip could also be used this macros if the 4.7uF capacitor is used.

#define SOUND_New_ADKEY10            11  //  Play Mode   14
#define SOUND_New_ADKEY9             18  //  U/SD/SPI    13
#define SOUND_New_ADKEY8             23  //  Loop All    12
#define SOUND_New_ADKEY7             27  //  Pause/Play  11
#define SOUND_New_ADKEY6             31  //  Prev/Vol-   10
#define SOUND_New_ADKEY5             39  //  Next/Vol+   9
#define SOUND_New_ADKEY4             53  //  4   8
#define SOUND_New_ADKEY3             75  //  3   7
#define SOUND_New_ADKEY2             148 //  2   6
#define SOUND_New_ADKEY1             255 //  1   5

#define Sound_New_Prev(      LED, InCh)         Sound_ADKey(LED, InCh, SOUND_New_ADKEY5,  0)
#define Sound_New_Next(      LED, InCh)         Sound_ADKey(LED, InCh, SOUND_New_ADKEY6,  0)
#define Sound_New_PausePlay( LED, InCh)         Sound_ADKey(LED, InCh, SOUND_New_ADKEY7,  0)
#define Sound_New_Loop(      LED, InCh)         Sound_ADKey(LED, InCh, SOUND_New_ADKEY8,  0)
#define Sound_New_USDSPI(    LED, InCh)         Sound_ADKey(LED, InCh, SOUND_New_ADKEY9,  0)  // ? U / SD / SPI
#define Sound_New_PlayMode(  LED, InCh)         Sound_ADKey(LED, InCh, SOUND_New_ADKEY10, 0)  // If Loop is active the button toggles between  Sequence, Sequence, Repeat same, Random, Loop off

// Volume
// First Volume change after 1000 ms
// Then volume is changed every 150 ms
// According to the documentation there are 30 stepps                                         // Incremented the time from 850 to 950 for the new sound modules. Otherwise sometimes the next/prev sound was played.
#define Sound_New_DecVol(    LED, InCh, Steps)  Sound_ADKeyTime(LED, InCh, SOUND_New_ADKEY5,  0, 950 ms - 75 ms + (Steps-1) * 150 ms)
#define Sound_New_IncVol(    LED, InCh, Steps)  Sound_ADKeyTime(LED, InCh, SOUND_New_ADKEY6,  0, 950 ms - 75 ms + (Steps-1) * 150 ms)

#define Sound_New_Seq1( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_New_ADKEY1, 0)
#define Sound_New_Seq2( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_New_ADKEY2, 0)
#define Sound_New_Seq3( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_New_ADKEY3, 0)
#define Sound_New_Seq4( LED, InCh)    Sound_ADKey(LED, InCh, SOUND_New_ADKEY4, 0)
#define Sound_New_Seq5( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY1)
#define Sound_New_Seq6( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY2)
#define Sound_New_Seq7( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY3)
#define Sound_New_Seq8( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY4)
#define Sound_New_Seq9( LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY5)
#define Sound_New_Seq10(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY6)
#define Sound_New_Seq11(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY7)
#define Sound_New_Seq12(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY8)
#define Sound_New_Seq13(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY9)
#define Sound_New_Seq14(LED, InCh)    Sound_ADKey(LED, InCh, 0, SOUND_New_ADKEY10)

#define _LocalVar_New_Sound(LED)  PatternT1(LED,28,SI_LocalVar,2,0,255,0,0,200 ms,0,0,SOUND_New_ADKEY1,0,SOUND_New_ADKEY2,0,SOUND_New_ADKEY3,0,SOUND_New_ADKEY4,0,0,SOUND_New_ADKEY1,0,SOUND_New_ADKEY2,0,SOUND_New_ADKEY3,0,SOUND_New_ADKEY4,0,SOUND_New_ADKEY5,0,SOUND_New_ADKEY6,0,SOUND_New_ADKEY7,0,SOUND_New_ADKEY8,0,SOUND_New_ADKEY9,0,SOUND_New_ADKEY10,0,0  ,1,129,129,129,129,129,129,129,129,129,129,129,129,129,129,127)

#define Sound_New_PlayRandom(LED, InCh, MaxSoundNr)                                                                      \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_RANDOM | CF_SKIP0,   InCh, SI_1, 0 Sec, MaxSoundNr+1)                    \
              _LocalVar_New_Sound(LED)

#define Sound_New_Next_of_N_Reset(LED, InCh, InReset, MaxSoundNr)                                                        \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_SKIP0 | CF_INV_ENABLE,   InCh, InReset, 0 Sec, MaxSoundNr+1) \
              _LocalVar_New_Sound(LED)

#define Sound_New_Next_of_N(LED, InCh, MaxSoundNr)                                                                       \
             Sound_New_Next_of_N_Reset(LED, InCh, SI_0, MaxSoundNr)
// 13.10.21:  End od the new section

//---------------------------- JQ6500 Sound modul ---------------------------------                           // 13.02.19:

// Sound functions could be disable / enabled with the variable SI_Enable_Sound
#define Sound_JQ6500_ADKey(     LED, InCh, ADKey)         PatternTE2(LED,28,InCh,SI_Enable_Sound,1,0,255,0,PM_SEQUENZ_NO_RESTART,400 ms,10 ms,ADKey,0) // Normal WS2811 modul with G+RB outputs anf -O+ inputs

// Volume changing is independent from SI_Enable_Sound
#define Sound_JQ6500_ADKeyTime(     LED, InCh, ADKey, Time)   PatternT2( LED,28,InCh,                1,0,255,0,PM_SEQUENZ_NO_RESTART,Time,10 ms,ADKey,0)

                                        //  ADKey1     400ms 470 Ohm
#define SOUND_JQ6500_ADKEY1_N   29      //  1
#define SOUND_JQ6500_ADKEY2_N   40      //  2
#define SOUND_JQ6500_ADKEY3_N   50      //  3
#define SOUND_JQ6500_ADKEY4_N   60      //  4
#define SOUND_JQ6500_ADKEY5_N   80      //  5
#define SOUND_JQ6500_ADKEY6_N   162     //  Prev/Vol-
#define SOUND_JQ6500_ADKEY7_N   231     //  Next/Vol+

// WS2811 with swapped pins (R+GB/+O-)  //  ADKey1
#define SOUND_JQ6500_ADKEY1_BG	32		//	1
#define SOUND_JQ6500_ADKEY2_BG	44		//	2
#define SOUND_JQ6500_ADKEY3_BG	56		//	3
#define SOUND_JQ6500_ADKEY4_BG	68		//	4
#define SOUND_JQ6500_ADKEY5_BG	90		//	5
#define SOUND_JQ6500_ADKEY6_BG	183		//	Prev/Vol-
#define SOUND_JQ6500_ADKEY7_BG	238		//	Next/Vol+


#define Sound_JQ6500_Prev(  LED, InCh)         Sound_JQ6500_ADKey(     LED, InCh, SOUND_JQ6500_ADKEY6_N)
#define Sound_JQ6500_Next(  LED, InCh)         Sound_JQ6500_ADKey(     LED, InCh, SOUND_JQ6500_ADKEY7_N)

#define Sound_JQ6500_BG_Prev(  LED, InCh)      Sound_JQ6500_ADKey(     LED, InCh, SOUND_JQ6500_ADKEY6_BG)
#define Sound_JQ6500_BG_Next(  LED, InCh)      Sound_JQ6500_ADKey(     LED, InCh, SOUND_JQ6500_ADKEY7_BG)

// Volume
// First Volume change after ~1000 ms   ???
// Then volume is changed every 150 ms  ???
// 30 stepps ??

#define Sound_JQ6500_DecVol(LED, InCh, Steps)  Sound_JQ6500_ADKeyTime(LED, InCh, SOUND_JQ6500_ADKEY6_N,     1000 ms + (Steps-1) * 150 ms)
#define Sound_JQ6500_IncVol(LED, InCh, Steps)  Sound_JQ6500_ADKeyTime(LED, InCh, SOUND_JQ6500_ADKEY7_N,     1000 ms + (Steps-1) * 150 ms)

#define Sound_JQ6500_BG_DecVol(LED, InCh, Steps)  Sound_JQ6500_ADKeyTime(LED, InCh, SOUND_JQ6500_ADKEY6_BG, 1000 ms + (Steps-1) * 150 ms)
#define Sound_JQ6500_BG_IncVol(LED, InCh, Steps)  Sound_JQ6500_ADKeyTime(LED, InCh, SOUND_JQ6500_ADKEY7_BG, 1000 ms + (Steps-1) * 150 ms)


#define Sound_JQ6500_Seq1(  LED, InCh)         Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY1_N)
#define Sound_JQ6500_Seq2(  LED, InCh)         Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY2_N)
#define Sound_JQ6500_Seq3(  LED, InCh)         Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY3_N)
#define Sound_JQ6500_Seq4(  LED, InCh)         Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY4_N)
#define Sound_JQ6500_Seq5(  LED, InCh)         Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY5_N)

#define Sound_JQ6500_BG_Seq1(  LED, InCh)      Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY1_BG)
#define Sound_JQ6500_BG_Seq2(  LED, InCh)      Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY2_BG)
#define Sound_JQ6500_BG_Seq3(  LED, InCh)      Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY3_BG)
#define Sound_JQ6500_BG_Seq4(  LED, InCh)      Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY4_BG)
#define Sound_JQ6500_BG_Seq5(  LED, InCh)      Sound_JQ6500_ADKey(LED, InCh, SOUND_JQ6500_ADKEY5_BG)

#define _LocalVar_Sound_JQ6500(   LED)    PatternT1(LED,28,SI_LocalVar,1,0,255,0,0,400 ms,0,SOUND_JQ6500_ADKEY1_N, SOUND_JQ6500_ADKEY2_N, SOUND_JQ6500_ADKEY3_N, SOUND_JQ6500_ADKEY4_N, SOUND_JQ6500_ADKEY5_N, 0,1,129,129,129,129,129,127)
#define _LocalVar_Sound_JQ6500_BG(LED)    PatternT1(LED,28,SI_LocalVar,1,0,255,0,0,400 ms,0,SOUND_JQ6500_ADKEY1_BG,SOUND_JQ6500_ADKEY2_BG,SOUND_JQ6500_ADKEY3_BG,SOUND_JQ6500_ADKEY4_BG,SOUND_JQ6500_ADKEY5_BG,0,1,129,129,129,129,129,127)


#define Sound_JQ6500_PlayRandom(LED, InCh, MaxSoundNr)                                                               \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_RANDOM | CF_SKIP0,   InCh, SI_1, 0 Sec, MaxSoundNr+1)                    \
              _LocalVar_Sound_JQ6500(LED)

#define Sound_JQ6500_Next_of_N_Reset(LED, InCh, InReset, MaxSoundNr)                                                 \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_SKIP0 | CF_INV_ENABLE,   InCh, InReset, 0 Sec, MaxSoundNr+1) \
              _LocalVar_Sound_JQ6500(LED)

#define Sound_JQ6500_Next_of_N(LED, InCh, MaxSoundNr)                                                                \
             Sound_JQ6500_Next_of_N_Reset(LED, InCh, SI_0, MaxSoundNr)


#define Sound_JQ6500_BG_PlayRandom(LED, InCh, MaxSoundNr)                                                            \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_RANDOM | CF_SKIP0,   InCh, SI_1, 0 Sec, MaxSoundNr+1)                    \
              _LocalVar_Sound_JQ6500_BG(LED)

#define Sound_JQ6500_BG_Next_of_N_Reset(LED, InCh, InReset, MaxSoundNr)                                              \
              New_Local_Var()                                                                                        \
              Counter(CF_ONLY_LOCALVAR | CF_ROTATE | CF_SKIP0 | CF_INV_ENABLE,   InCh, InReset, 0 Sec, MaxSoundNr+1) \
              _LocalVar_Sound_JQ6500_BG(LED)

#define Sound_JQ6500_BG_Next_of_N(LED, InCh, MaxSoundNr)                                                             \
             Sound_JQ6500_BG_Next_of_N_Reset(LED, InCh, SI_0, MaxSoundNr)

/*
#define Sound_Seq( LED, InCh, SeqNr, SoundModul, WS2811_Typ) _Sound##SoundModul##_ADKey(    LED, InCh, SOUND##SoundModul##_ADKEY##SeqNr##WS2811_Typ)
#define Sound_Pre( LED, InCh,        SoundModul, WS2811_Typ) _Sound##SoundModul##_ADKey(    LED, InCh, SOUND##SoundModul##_ADKEY6##WS2811_Typ)
#define Sound_Nxt( LED, InCh,        SoundModul, WS2811_Typ) _Sound##SoundModul##_ADKey(    LED, InCh, SOUND##SoundModul##_ADKEY7##WS2811_Typ)
#define Sound_DecV(LED, InCh, Steps, SoundModul, WS2811_Typ) _Sound##SoundModul##_ADKeyTime(LED, InCh, SOUND##SoundModul##_ADKEY6##WS2811_Typ, 1000 ms + (Steps-1) * 150 ms)
#define Sound_IncV(LED, InCh, Steps, SoundModul, WS2811_Typ) _Sound##SoundModul##_ADKeyTime(LED, InCh, SOUND##SoundModul##_ADKEY7##WS2811_Typ, 1000 ms + (Steps-1) * 150 ms)
*/

#define EndCfg END_T



#define MobaLedLib_Configuration()          const PROGMEM unsigned char Config[] =

#define MobaLedLib_Prepare()             MobaLedLib_C* pMobaLedLib; \
                                         uint8_t Config_RAM[__COUNTER__/2]; /* RAM used for the configuration functions. The size is calculated in the macros which are used in the Config[] table.*/                                         
                                             
#if _USE_STORE_STATUS && _USE_EXT_PROC                                                                                       // 26.09.21: Juergen
#define MobaLedLib_Create(leds)   MobaLedLib_CreateEx(leds, NULL, NULL)
#define MobaLedLib_CreateEx(leds, callback, processor)   uint8_t Config_RAM[__COUNTER__/2]; /* RAM used for the configuration functions. The size is calculated in the macros which are used in the Config[] table.*/ \
                                            MobaLedLib_C MobaLedLib(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM), callback, processor); // MobaLedLib_C class definition
#define MobaLedLibPtr_Create(leds)   MobaLedLib_CreatePtrEx(leds, NULL, NULL)
#define MobaLedLibPtr_CreateEx(leds, callback, processor) \
                                            pMobaLedLib = new MobaLedLib_C(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM), callback, processor); // MobaLedLib_C class definition
#elif _USE_STORE_STATUS 
#define MobaLedLib_Create(leds)   MobaLedLib_CreateEx(leds, NULL)
#define MobaLedLib_CreateEx(leds, callback)   uint8_t Config_RAM[__COUNTER__/2]; /* RAM used for the configuration functions. The size is calculated in the macros which are used in the Config[] table.*/ \
                                            MobaLedLib_C MobaLedLib(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM), callback); // MobaLedLib_C class definition
#define MobaLedLibPtr_Create(leds)   MobaLedLib_CreatePtrEx(leds, NULL)
#define MobaLedLibPtr_CreateEx(leds, callback) \
                                            pMobaLedLib = new MobaLedLib_C(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM), callback); // MobaLedLib_C class definition
#elif _USE_EXT_PROC 
#define MobaLedLib_Create(leds)   MobaLedLib_CreateEx(leds, NULL)
#define MobaLedLib_CreateEx(leds, processor)   uint8_t Config_RAM[__COUNTER__/2]; /* RAM used for the configuration functions. The size is calculated in the macros which are used in the Config[] table.*/ \
                                            MobaLedLib_C MobaLedLib(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM), processor); // MobaLedLib_C class definition
#define MobaLedLibPtr_Create(leds)   MobaLedLib_CreatePtrEx(leds, NULL)
#define MobaLedLibPtr_CreateEx(leds, processor) \
                                            pMobaLedLib = new MobaLedLib_C(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM), processor); // MobaLedLib_C class definition
#else
#define MobaLedLib_Create(leds)             uint8_t Config_RAM[__COUNTER__/2]; /* RAM used for the configuration functions. The size is calculated in the macros which are used in the Config[] table.*/ \
                                            MobaLedLib_C MobaLedLib(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM)); // MobaLedLib_C class definition
#define MobaLedLibPtr_Create(leds)          \
                                            pMobaLedLib = new MobaLedLib_C(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM)); /* MobaLedLib_C class definition */ \
                                            MobaLedLib_C& MobaLedLib = *pMobaLedLib;
#endif

#if _USE_USE_GLOBALVAR
  #define MobaLedLib_Assigne_GlobalVar(GlobalVar)  MobaLedLib.Assigne_GlobalVar(GlobalVar, (uint8_t)(sizeof(GlobalVar)/sizeof(ControlVar_t)))
#endif

#define _ConvChannelNr2Byte(ChannelNr) (ChannelNr%4 == 0 ? (ChannelNr)/4 : 512 + (ChannelNr)/4) // Generates a warning if the channel number can not be divided by 4

// Attention: The function writes  8*ByteCnt input channels !!!
#define MobaLedLib_Copy_to_InpStruct(Src, ByteCnt, ChannelNr)  \
             MobaLedLib.Copy_Bits_to_InpStructArray(Src, ByteCnt, _ConvChannelNr2Byte(ChannelNr)) // Attention: Channel number must be divisible by 4

// Die Cx Konstanten bestehen aus:
// - LedCnt (1..3), StCh (0..2)
#define             _Cx(LedCnt, StCh)   (LedCnt + (StCh<<2))
#define C1          _Cx(1,      0)
#define C2          _Cx(1,      1)
#define C3          _Cx(1,      2)
#define C12         _Cx(2,      0)
#define C23         _Cx(2,      1)
#define C_ALL       _Cx(3,      0)
#define C_RED       C1
#define C_GREEN     C2
#define C_BLUE      C3
#define C_WHITE     C_ALL
#define C_YELLOW    C12
#define C_CYAN      C23

#define _Cx2LedCnt(Cx) ((Cx)&0x03)
#define _Cx2StCh(Cx)   (((Cx)>>2)&0x03)

// FreeBits ist abhaengig vom Muster:
// - Andreaskr.  2 LEDs 0   01101001   Typ:0
// - Const,      1 LED  7   .......1   Typ:1
// - Const,      2 LEDs 6   ......11
// - Const,      3 LEDs 5   .....111
// - Blinker,    1 LED  6   ......01   Typ:2
// - Blinker,    2 LEDs 4   ....0011
// - Blinker,    3 LEDs 2   ..000111
// - DoublFlash, 1 LED  4   ....0101   Typ:4
// - DoublFlash, 2 LEDs 0   00110011
// - DoublFlash, 3 LEDs 4   ....0001 11000111  <= Geht nicht mit dem gleichen Makro -> LeuchtfeuerALL
#define _FreeBits(LedCnt, Typ) ((Typ<=2?(8-(LedCnt)*Typ):((LedCnt)==2?0:4))&0x07)

// NStru contains the Start Channel:0-2 (Bits 0,1), the Bits per Channel-1: 1-8 (Bits 2..4) and the Number of free Bits: 0-7 (Bits 5..7)
// Picture NNNBBBSS
#define _NStru(cx, Typ, BpC) (_Cx2StCh(cx) + ((BpC-1)<<2) + (_FreeBits(_Cx2LedCnt(cx), Typ)<<5))

// The BlueLight and the Leuchtfeuer macros can't be used with C_ALL because in this case
// the two byte pattern B11000111, 0b00000001 is needed. I don't know how to generate this
// with the macro PAT_DBLFL. Therefore the following strange number is used which is printed
// as a warning. Maybe this helps to find this comment ...
#define _C_ALL_not_supported  33877u // Generates the warning "narrowing conversion of '33877u' from 'unsigned int' to ..."


#define _PAT_BLINK(LedCnt) ((LedCnt)==1?0b00000001: ((LedCnt)==2?0b00000011:0b00000111))
#define _PAT_DBLFL(LedCnt) ((LedCnt)==1?0b00000101: ((LedCnt)==2?0b00110011:_C_ALL_not_supported))  // Doesn't work with C_ALL ;-( because in this case we need 2 parameters: B11000111, B00000001

#define _Cx2P_BLINK(Cx)   (_PAT_BLINK(_Cx2LedCnt(Cx)))
#define _Cx2P_DBLFL(Cx)   (_PAT_DBLFL(_Cx2LedCnt(Cx)))


// Constants for the Button period and the times in the pattern function
#define Min         *60000L    // min in lowercase is not possible because of the standard min macro
#define Sec         *1000L
#define Sek         Sec
#define sek         Sec
#define sec         Sec
#define Ms
#define ms

#define PM_NORMAL               0              // Normal mode (Als Platzhalter in Excel)
#define PM_SEQUENZ_W_RESTART    1              // Rising edge-triggered unique sequence. A new edge starts with state 0.
#define PM_SEQUENZ_W_ABORT      2              // Rising edge-triggered unique sequence. Abort the sequence if new edge is detected during run time.
#define PM_SEQUENZ_NO_RESTART   3              // Rising edge-triggered unique sequence. No restart if new edge is detected
#define PM_SEQUENZ_STOP         4              // Rising edge-triggered unique sequence. A new edge starts with state 0. If input is turned off the sequence is stopped immediately.
#define PM_PINGPONG             5              // Change the direction at the end: 118 bytes
#define PM_HSV                  6              // Use HSV values instead of RGB for the channels
#define PM_RES                  7              // Reserved mode
#define PM_MODE_MASK            0x07           // Defines the number of bits used for the modes (currently 3 => Modes 0..7)

#define PM_SEQUENCE_W_RESTART   1              // To be able to use the correct spelling also    07.10.21:
#define PM_SEQUENCE_W_ABORT     2
#define PM_SEQUENCE_NO_RESTART  3
#define PM_SEQUENCE_STOP        4

// Flags for the Pattern function
#define _PF_XFADE               0x08           // Special fade mode which starts from the actual brightness value instead of the value of the previous state
#define PF_NO_SWITCH_OFF        0x10           // Don't switch of the LEDs if the input is turned off. Useful if several effects use the same LEDs alternated by the input switch.
#define PF_EASEINOUT            0x20           // Easing function (Uebergangsfunktion) is used because changes near 0 and 255 are noticed different than in the middle
#define PF_SLOW                 0x40           // Slow timer (divided by 16) to be able to use longer durations
#define PF_INVERT_INP           0x80           // Invert the input switch => Effect is active if the input is 0

// Flags and modes for the Random() and RandMux() function
#define  RM_NORMAL               0b00000000
#define  RF_SLOW                 0b00000001      // Time base is divided by 16 This Flag is set automatically if the time is > 65535 ms
#define  RF_SEQ                  0b00000010      // Switch the outputs of the RandMux() function sequential and not random
#define  RF_STAY_ON              0b00000100      // Flag for the Ranom() function. The Output stays on until the input is turned off. MinOn, MaxOn define how long it stays on.
                                                // 12.01.20:  For some reasons RF_STAY_ON was equal RF_SEQ
#define  RF_NOT_SAME             0b00001000      // Prevent, that the same output is used two times in sequence                                           // 13.01.20:

// Flags for the Counter() function
#define  CM_NORMAL               0              // Normal Counter mode
#define  CF_INV_INPUT            0b00000001      // Invert Input
#define  CF_INV_ENABLE           0b00000010      // Input Enable
#define  CF_BINARY               0b00000100      // Maximal 8 outputs
#define  CF_RESET_LONG           0b00001000      // Taste Lang = Reset
#define  CF_UP_DOWN              0b00010000      // Ein RS-FlipFlop kann mit CM_UP_DOWN ohne CF_ROTATE gemacht werden
#define  CF_ROTATE               0b00100000      // Faengt am Ende wieder von vorne an
#define  CF_PINGPONG             0b01000000      // Wechselt am Ende die Richtung
#define  CF_SKIP0                0b10000000      // Ueberspringt die 0. Die 0 kommt nur bei einem Timeout oder wenn die Taste lange gerueckt wird
#define  CF_RANDOM              (0b00000001<<8)  // Generate random numbers                                                                               // 07.11.18:
#define  CF_LOCAL_VAR           (0b00000010<<8)  // Write the result to a local variable which must be created with New_Local_Var() prior
#define _CF_NO_LEDOUTP          (0b00000100<<8)  // Disable the LED output (the first DestVar contains the maximal counts-1 (counter => 0 .. n-1) )
#define  CF_ONLY_LOCALVAR       (CF_LOCAL_VAR|_CF_NO_LEDOUTP)  // Don't write to the LEDs defined with DestVar. The first DestVar contains the number maximal number of the counter-1 (counter => 0 .. n-1).


#define _CM_RS_FlipFlop1        (CF_BINARY|CF_UP_DOWN)               // RS Flip Flop with one output (Edge triggered)
#define _CM_T_FlipFlop1         (CF_BINARY|CF_INV_ENABLE|CF_ROTATE)  // T Flip Flop  with one output
#define _CM_RS_FlipFlop2        (CF_UP_DOWN)                         // RS Flip Flop with two outputs (Edge triggered)
#define _CM_T_FlipFlopEnable2   (CF_ROTATE)                          // T Flip Flop  with two outputs and enable
#define _CM_T_FlipFlopReset2    (CF_ROTATE | CF_INV_ENABLE)          // T Flip Flop  with two outputs and reset

#define _CF_ROT_SKIP0           (CF_ROTATE   | CF_SKIP0)             // Rotate and Skip 0
#define _CF_P_P_SKIP0           (CF_PINGPONG | CF_SKIP0)



// Constants for the rooms in the HOUSE_T
#define ROOM_DARK          0         // Dark light
#define ROOM_BRIGHT        1         // Bright light
#define ROOM_WARM_W        2         // Warm white light
#define ROOM_RED           3         // Red light
#define ROOM_D_RED         4         // Dark Red light
#define ROOM_COL0          5         // Room with user defined color 0
#define ROOM_COL1          6         // Room with user defined color 1
#define ROOM_COL2          7         // Room with user defined color 2
#define ROOM_COL3          8         // Room with user defined color 3
#define ROOM_COL4          9         // Room with user defined color 4
#define ROOM_COL5          10        // Room with user defined color 5
#define ROOM_COL345        11        // Room with user defined color 3, 4 or 5 which is randomly activated
#define FIRE              (12 +RAM1) // Chimney fire   (RAM is used to store the Heat_p)
#define FIRED             (13 +RAM1) // Dark chimney     "
#define FIREB             (14 +RAM1) // Bright chimney   "
// Reserved don't remove   15
#define ROOM_CHIMNEY      (16 +RAM1) // With chimney fire or Light        (RAM is used to store the Heat_p for the chimney)
#define ROOM_CHIMNEYD     (17 +RAM1) // With dark chimney fire or Light     "
#define ROOM_CHIMNEYB     (18 +RAM1) // With bright chimney fire or Light   "
#define ROOM_TV0           19        // With TV channel 0 or Light
#define ROOM_TV0_CHIMNEY  (20 +RAM1) // With TV channel 0 and fire or Light
#define ROOM_TV0_CHIMNEYD (21 +RAM1) // With TV channel 0 and fire or Light
#define ROOM_TV0_CHIMNEYB (22 +RAM1) // With TV channel 0 and fire or Light
#define ROOM_TV1           23        // With TV channel 1 or Light
#define ROOM_TV1_CHIMNEY  (24 +RAM1) // With TV channel 1 and fire or Light
#define ROOM_TV1_CHIMNEYD (25 +RAM1) // With TV channel 1 and fire or Light
#define ROOM_TV1_CHIMNEYB (26 +RAM1) // With TV channel 1 and fire or Light
#define GAS_LIGHT          27        // Gas light using all channels (RGB)           ATTENTION: First GAS lights are bright  (Don't mix them up)
#define GAS_LIGHT1         28        // Gas light using one channel (R)
#define GAS_LIGHT2         29        // Gas light using one channel (G)
#define GAS_LIGHT3         30        // Gas light using one channel (B)
#define GAS_LIGHTD         31        // Dark gas light using all channels (RGB)      ATTENTION: Following GAS lights are dark (Don't mix them up)
#define GAS_LIGHT1D        32        // Dark gas light using one channel (R)
#define GAS_LIGHT2D        33        // Dark gas light using one channel (G)
#define GAS_LIGHT3D        34        // Dark gas light using one channel (B)
#define NEON_LIGHT         35        // Neon light using all channels
#define NEON_LIGHT1        36        // Neon light using one channel (R)
#define NEON_LIGHT2        37        // Neon light using one channel (G)
#define NEON_LIGHT3        38        // Neon light using one channel (B)
#define NEON_LIGHTD        39        // Dark Neon light using all channels
#define NEON_LIGHT1D       40        // Dark Neon light using one channel (R)
#define NEON_LIGHT2D       41        // Dark Neon light using one channel (G)
#define NEON_LIGHT3D       42        // Dark Neon light using one channel (B)
#define NEON_LIGHTM        43        // Medium Neon light using all channels
#define NEON_LIGHT1M       44        // Medium Neon light using one channel (R)
#define NEON_LIGHT2M       45        // Medium Neon light using one channel (G)
#define NEON_LIGHT3M       46        // Medium Neon light using one channel (B)
#define NEON_LIGHTL        47        // Large room Neon light using all channels. A large room is equipped with several neon lights which start delayed
#define NEON_LIGHT1L       48        // Large room Neon light using one channel (R)
#define NEON_LIGHT2L       49        // Large room Neon light using one channel (G)
#define NEON_LIGHT3L       50        // Large room Neon light using one channel (B)
#define NEON_DEF_D         51        // Decective Neon light using all channels.
#define NEON_DEF1D         52        // Decective Neon light using one channel (R)
#define NEON_DEF2D         53        // Decective Neon light using one channel (G)
#define NEON_DEF3D         54        // Decective Neon light using one channel (B)
#define SINGLE_LED1        55        // Single LED connected with a WS2811 module to channel (R)              // 06.09.19:
#define SINGLE_LED2        56        // Single LED connected with a WS2811 module to channel (G)
#define SINGLE_LED3        57        // Single LED connected with a WS2811 module to channel (B)
#define SINGLE_LED1D       58        // Single dark LED connected with a WS2811 module to channel (R)
#define SINGLE_LED2D       59        // Single dark LED connected with a WS2811 module to channel (G)
#define SINGLE_LED3D       60        // Single dark LED connected with a WS2811 module to channel (B)         // 06.09.19:  End of new block
#define SKIP_ROOM          61        // Room which is not controlled with by the house() function (Usefull for Shops in a house becaue this lights are always on at night)
#define CANDLE            (62+RAM1)  // RGB Candle                                                            // 09.06.20:
#define CANDLE1            63        // Single Candle LED connected with a WS2811 module to channel (R)
#define CANDLE2            64        // Single Candle LED connected with a WS2811 module to channel (G)
#define CANDLE3            65        // Single Candle LED connected with a WS2811 module to channel (B)

#if ((FIRE              & 0x03) != 0) || ((FIRED             & 0x03) != 1) || ((FIREB             & 0x03) != 2) ||   \
    ((ROOM_CHIMNEY      & 0x03) != 0) || ((ROOM_CHIMNEYD     & 0x03) != 1) || ((ROOM_CHIMNEYB     & 0x03) != 2) ||   \
    ((ROOM_TV0_CHIMNEY  & 0x03) != 0) || ((ROOM_TV0_CHIMNEYD & 0x03) != 1) || ((ROOM_TV0_CHIMNEYB & 0x03) != 2) ||   \
    ((ROOM_TV1_CHIMNEY  & 0x03) != 0) || ((ROOM_TV1_CHIMNEYD & 0x03) != 1) || ((ROOM_TV1_CHIMNEYB & 0x03) != 2)
    #warning Wrong number for ROOM_CHIMNEY, ... Last two bits are used in Update_TV_or_Chimney()
#endif



// Spezial Inputs
#define SI_Enable_Sound    253  // Wird auf 1 initialisiert, kann aber vom der Konfigurarion veraendert werden
#define SI_LocalVar        SI_0 // Input fuer Pattern Funktion zum einlesen der Localen Variable. Diese Nummer wird verwendet weil eine Konstante 0 als Eingang keinen Sinn macht
#define SI_0               254  // Immer 0
#define SI_1               255  // Immer 1

/*
 Weitere Special Inputs:   ??
 SI_Status_OK                  // Alles O.K.
 SI_Status_LED_Mis             // Weniger LEDs erkannt als vorgegeben
 SI_Status_Add_LED             // Mehr LEDs erkannt als vorgegeben
 SI_Status_CAN_OK              // CAN DAten vorhanden
 SI_Sunrise                    //
 SI_Sunset
 SI_Day
 SI_Night
*/

// Constants for the Logic() function
#define _OR_NR             SI_0      // Number which can't be used as input channel for the logic function ( A & false is always false)
#define _NOT_NR            SI_1      //  "                "                 "
#define OR                 ,_OR_NR,
#define NOT                _NOT_NR,
#define AND                ,
#define ENABLE             _OR_NR,   // Used to define an enable input. This keyword must be used at the first position in the variable argument list. An OR at this position dosn't make sence => we could use the same numner
#define DISABLE            _OR_NR, NOT

// 'or', 'and', ... is not possible because they are used in c++
// A macro EQ:',' is not possible because then it can't be checked in the macro

// Bit masks for Special SpInp[]
#define _BIT_OLD           0b00000010
#define _BIT_NEW           0b00000001
#define _NEW_AND_OLD       (_BIT_NEW + _BIT_OLD)

#define _ALL_OLD_BITS      0b10101010
#define _ALL_NEW_BITS      0b01010101

// Results generated by Get_Input()
#define INP_OFF            0b00000000
#define INP_ON             0b00000011
#define INP_TURNED_ON      0b00000001
#define INP_TURNED_OFF     0b00000010
#define INP_INIT_OFF       0b00000110



//---------------------------------------------
inline bool Inp_Changed_or_InitOff(uint8_t Inp)
//---------------------------------------------
{
  return Inp == INP_TURNED_ON || Inp == INP_TURNED_OFF || Inp == INP_INIT_OFF;
}
// Optimierungsuntersuchung:
// Inp_Changed_or_InitOff():
//   - Wenn man nur den ersten Vergleich (Inp == INP_TURNED_ON) aktiviert, dann dauert
//     der Update 189us anstelle von 193us bei einer Konfiguration mit 13 Pattern aufrufen
//     und einer Logic() Funktion. => 2% schneller.
//   - Ohne die (Inp == INP_INIT_OFF) Abfrage dauert es 190us => 1.5% schneller.

//------------------------------------
inline uint8_t Invert_Inp(uint8_t Inp)
//------------------------------------
{
  return Inp ^ 0b00000011;  // XOR: 00 => 11, 01 => 10, 10 => 01, 11 => 00
}

//--------------------------------
inline bool Inp_Is_On(uint8_t Inp)
//--------------------------------
{
  return Inp == INP_ON || Inp == INP_TURNED_ON; //Inp & _BIT_NEW;
}

//--------------------------------------------------
inline bool Inp_Is_TurnedOff_or_InitOff(uint8_t Inp)
//--------------------------------------------------
{
  return Inp == INP_TURNED_OFF || Inp == INP_INIT_OFF ;
}

//----------------------------------
inline bool Inp_Changed(uint8_t Inp)
//----------------------------------
{
  return Inp == INP_TURNED_ON || Inp == INP_TURNED_OFF;
}


#define _TV_CHANNELS          2  // If changed adapt also the constants ROOM_TV0, COLOR_TV0_A... and the corrosponding switch case statements



#ifdef _USE_LOCAL_TX
  #define CALCULATE_t4w  uint16_t t4w = (t>>4)  & 0xFFFF; // Time divided by 16
  #define CALCULATE_t4   uint8_t  t4  = (t>>4)  & 0xFF;   // Time divided by 16
  #define CALCULATE_t10  uint8_t  t10 = (t>>10) & 0xFF;   // Time divided by 1024
#else
  #define CALCULATE_t4w
  #define CALCULATE_t4
  #define CALCULATE_t10
#endif

#define ALL_CHANNELS    4   // For RawNr. Is used with "& 0x03" to select channel 0


#define TM_DISABLED     0
#define TM_LED_TEST     1   // Don't change the sequence because its needed for TestName[3][4]
#define TM_INP_TEST     2
#define TM_BUT_TEST     3   // Reserved mode, currently no function (If the line is enabled the mode could be selected with the middle key)


extern uint8_t    Darkness;
extern DayState_E DayState;

extern uint8_t    TestMode;

#define ROOM_COL_CNT    17   // Number of user defined colors for the room lights (Attention: If changed the program has to be adapted at several positions)  06.09.19:  Old 15

#define COLOR0         0
#define COLOR1         1
#define COLOR2         2
#define COLOR3         3
#define COLOR4         4
#define COLOR5         5
#define COLOR_GASL_D   6
#define COLOR_GASL_B   7
#define COLOR_NEON_D   8
#define COLOR_NEON_M   9
#define COLOR_NEON_B   10
#define COLOR_TV0_A    11
#define COLOR_TV0_B    12
#define COLOR_TV1_A    13
#define COLOR_TV1_B    14
#define COLOR_SINGLE   15                                                                                     // 06.09.19:
#define COLOR_SINGLE_D 16                                                                                     //   "

#if _USE_STORE_STATUS                                                                                         // 19.05.20: Juergen
   typedef void(*Callback_t) (uint8_t CallbackType, uint8_t ValueId, uint8_t OldValue, uint8_t* NewValue);
#endif
#if _USE_EXT_PROC                                                                                         // 19.05.20: Juergen
   typedef uint8_t(*ExtProc_t) (uint8_t Type, const uint8_t* progmemAddress, bool process);
#endif

//:::::::::::::::::::
class MobaLedLib_C
//:::::::::::::::::::
{
public:

#if _USE_STORE_STATUS                                                                                         // 19.05.20: Juergen
  #if _USE_EXT_PROC                                                                                         // 19.05.20: Juergen
                    MobaLedLib_C(struct CRGB* _leds, uint16_t Num_Leds, const uint8_t Config[], uint8_t RAM[], uint16_t RamSize, Callback_t Function, ExtProc_t Processor); // Konstruktor mit 2xcallback
  #else                       
                    MobaLedLib_C(struct CRGB* _leds, uint16_t Num_Leds, const uint8_t Config[], uint8_t RAM[], uint16_t RamSize, Callback_t Function); // Konstruktor mit callback
  #endif                        
#else
  #if _USE_EXT_PROC                                                                                         // 19.05.20: Juergen
                    MobaLedLib_C(struct CRGB* _leds, uint16_t Num_Leds, const uint8_t Config[], uint8_t RAM[], uint16_t RamSize, ExtProc_t Processor); // Konstruktor mit callback
  #else                       
                    MobaLedLib_C(struct CRGB* _leds, uint16_t Num_Leds, const uint8_t Config[], uint8_t RAM[], uint16_t RamSize); // Konstruktor OHNE callback
  #endif                        
#endif
#if _USE_USE_GLOBALVAR
 void               Assigne_GlobalVar(ControlVar_t *GlobalVar, uint8_t GlobalVar_Cnt);
#endif
 void               Update();
 void               Set_Input(uint8_t channel, uint8_t On);
 int8_t             Get_Input(uint8_t channel);
 void               Copy_Bits_to_InpStructArray(uint8_t *Src, uint8_t ByteCnt, uint8_t InpStructArrayNr);
 void               Print_Config(); // FLASH usage ~4258 Byte RAM 175 (16.12.18), #define _PRINT_DEBUG_MESSAGES must be enabled in "Lib_Config.h"

public:  // Variables
 uint8_t            Trigger20fps; // <> 0 every 50 ms for one cycle    //  1 Byte

private: // Variables
 const uint8_t     *cp;          // Pointer to the Config[]            //  2 Byte
 CRGB*              leds;                                              //  2 Byte
 uint16_t           Num_Leds;                                          //  2 Byte
 const uint8_t     *Config;                                            //  2 Byte
 bool               Initialize;                                        //  1 Byte
 uint8_t           *RAM;                                               //  2 Byte
 uint8_t            Last_20;     // Last time for the 20fps flag       //  1 Byte
 uint8_t            TriggerCnt;                                        //  1 Byte
 uint8_t           *rp;          // Pointer to the RAM                 //  2 Byte
 uint32_t           t;           // actual time [ms]                   //  4 Byte
#ifndef _USE_LOCAL_TX
 uint16_t           t4w;         // time>>4   = / 16                   //  2 Byte
 uint8_t            t10;         // time>>10  = / 1024                 //  1 Byte
#endif
 TV_Dat_T           TV_Dat[_TV_CHANNELS];                              // 10 Byte
 uint8_t            InpStructArray[_INP_STRUCT_ARRAY_SIZE];            // 64 Byte   Array which contains two bits for each input. One bit for the actual state and a second for the new state. (See INP_TURNED_ON)
 HSV_T             *HSV_p;                                             //  2 Byte
 ControlVar_t      *ActualVar_p;                                       //  2 Byte                                                                  // 07.11.18:
#if _USE_USE_GLOBALVAR
 ControlVar_t      *GlobalVar;                                         //  2 Byte
#endif
 ControlVar_t       TempVar;                                           //  2 Byte
 uint8_t            GlobalVar_Cnt;                                     //  1 Byte
 Sound_Dat_t       *Sound_Dat;                                         //  2 Byte
#if _USE_STORE_STATUS                                                                                         // 01.05.20:
 Callback_t         CallbackFunc;                                      //  4 Byte
 uint8_t            ProcCounterId;                                     //  1 Byte
#endif
#if _USE_EXT_PROC                                                                                             // 26.09.21: Jürgen
 ExtProc_t          CommandProcessorFunc;                              //  4 Byte
#endif
                                                                       // 47 Byte RoomCol[]
                                                                       // -------
                                                                       //154 Byte  + 5 StoreStatus + 4 ExtProc

#if _USE_DEF_NEON                                                                                             // 12.01.20:
 uint8_t            Rand_On_DefNeon; // probability that the neon light starts.             0 = don't start, 1 start seldom,  255 = Start immediately
 uint8_t            RandOff_DefNeon; // probability that the light goes off after a while.  0 = Always on,   1 = shot flash,  255 = very long time active
 uint8_t            Min_DefNeon;     // red glow of the starter.                            1 = minimal value,  5 = maximal value
#endif


 // Private functions
 uint8_t           *Get_LEDPtr(uint8_t &cnt);
 uint8_t            Is_Room_On(CRGB *lp, uint8_t RawNr);
 void               Update_TV_or_Chimney(uint8_t TVNr, CRGB *lp, uint8_t Room_Typ);
 uint8_t            Get_RawNr(uint8_t Room_Typ);
 void               Update_Gas_Light( uint8_t Room_Typ, CRGB *lp);
 void               Update_Neon_Light(uint8_t Room_Typ, CRGB *lp);
 void               Update_Candle(uint8_t Room_Typ, CRGB *lp);                                                // 10.06.20:
 void               Proc_Set_CandleTab();
 void               TurnOnRoom(CRGB* lp, uint8_t Room_Typ);
 void               TurnOffRoom(CRGB* lp, uint8_t RawNr, uint8_t Room_Typ);
 void               Set_Default_TV_Dat_p();                                                                   // 09.01.20:
 void               Update_TV_Data();
 void               Random_Const_Light(CRGB *lp, uint8_t TVNr);
 void               Enable_Light_or_TV(CRGB *lp, uint8_t TVNr);
 void               Enable_Chimney(CRGB *lp);
 void               Enable_Light_or_Chimney(CRGB *lp);
 void               Enable_Light_TV_or_Chimney(CRGB *lp, uint8_t TVNr);
 void               Set_Timer4Random(uint16_t Timer, TimerData16_T *dp, uint8_t P_MinTime);
 void               Set_Dest_RandMux(uint8_t Act, uint8_t Mode);
 void               Inp_Processed();
 void               CalcSwitchVal(Schedule_T *dp);
 uint8_t            Is_Pattern(uint8_t Type);
 void               Print_Comment(uint8_t Type);
 void               Int_Update(uint32_t Time);
 void               Inc_cp(uint8_t Type);
 uint8_t            Find_Next_Priv_InpChannel(uint8_t Channel, int8_t Direction);


 // Test_buttons
 void               Setup_Test_Buttons();
 void               Set_one_Test_Outp(uint8_t Nr, CRGB* &p);
 void               Set_Test_Outp(uint8_t &Ch);
 void               Speed_Test();
 void               Proc_Key_Test(uint8_t key);
 void               Proc_Test_Buttons();

 // Proc functions called in Update
 void               Proc_Const();
 void               Proc_AnalogPattern(uint8_t TimeCnt, bool AnalogMode);
 void               Proc_House();
 void               Proc_Fire();
 #ifdef _NEW_ROOM_COL
  void              Proc_Set_ColTab();
 #endif
 void               Proc_Set_TV_Tab();                                                                        // 09.01.20:
 void               Proc_Set_Def_Neon();                                                                      // 12.01.20:
 void               Proc_Logic();
 void               Proc_New_HSV_Group();
 void               Proc_New_Local_Var();                                                                     // 07.11.18:
 void               Proc_Use_GlobalVar();
#if _USE_INCH_TRIGGER                                                                                         // 02.06.20: New trigger method from Juergen
 void               Proc_InCh_to_X_Var();                                                                     // 07.05.20:  Added Start  // 31.05.20 J:remove start
 void               Proc_Bin_InCh_to_TmpVar();                                                                // 07.05.20:  Added Start  // 31.05.20 J:remove start
#else
 void               Proc_InCh_to_TmpVar(uint8_t Start);                                                       // 07.05.20:  Added Start
 void               Proc_Bin_InCh_to_TmpVar(uint8_t Start);                                                   // 07.05.20:  Added Start
#endif
 void               Proc_Random();
 void               Proc_RandMux();
 void               Proc_Welding(uint8_t withbreaks);
 void               Proc_CopyLED();
 void               Proc_Schedule();
 void               Proc_Counter();

 void               IncCP_House();
 void               IncCP_Const()         { cp += 5 + ADD_WORD_OFFSET; }
 void               IncCP_Fire()          { cp += 4 + ADD_WORD_OFFSET; }
 #ifdef _NEW_ROOM_COL
   void             IncCP_Set_ColTab()    { cp += ADD_WORD_OFFSET + ROOM_COL_CNT*3; }
   bool             Cmp_Room_Col(CRGB *lp, uint8_t ColorNr);
   uint8_t          Get_Room_Col1(uint8_t ColorNr, uint8_t Channel);
   void             Copy_Room_Col(CRGB *Dst, uint8_t ColorNr);
   void             Copy_Single_Room_Col(CRGB *Dst, uint8_t Channel, uint8_t ColorNr);                        // 06.09.19:
 #endif
 #if _USE_CANDLE                                                                                              // 10.06.20:
   void             IncCP_Set_CandleTab()  { cp += sizeof(Candle_Dat_T); }
 #endif
 void               IncCP_Set_TV_Tab();                                                                       // 10.01.20:
 void               IncCP_Set_Def_Neon();                                                                     // 12.01.20:
 void               IncCP_Pattern(uint8_t TimeCnt);
 void               IncCP_Logic();
 void               IncCP_New_HSV_Group() { }
 void               IncCP_New_Local_Var() { }                                                                 // 07.11.18:
 void               IncCP_Use_GlobalVar() { cp++; }
 void               IncCP_InCh_to_X_Var() { cp+=2; }                                                          // 25.11.18:
 void               IncCP_Random()        { cp += EP_RANDOM_INCREMENT;   }
 void               IncCP_RandMux()       { cp += EP_RANDMUX_INCREMENT;  }
 void               IncCP_Welding()       { cp += EP_WELDING_INCREMENT;  }
 void               IncCP_CopyLED()       { cp += EP_COPYLED_INCREMENT;  }
 void               IncCP_Schedule()      { cp += EP_SCHEDULE_INCREMENT; }
 void               IncCP_Counter();

 uint8_t            InpChannel_used(uint8_t Channel);
 bool               Inp_is_Used_in_Logic(uint8_t Channel);

#if _USE_STORE_STATUS                                                                                         // 01.05.20:
 void               Do_Callback(uint8_t CallbackType, uint8_t ValueId, uint8_t OldValue, uint8_t *NewValue);  // 19.05.20: Juergen
#endif

#ifdef _NEW_ROOM_COL
   const uint8_t     *Room_ColP;
#endif

#if _USE_CANDLE                                                                                               // 10.06.20:
   const Candle_Dat_T *Candle_DatP;
#endif

#if _USE_SET_TVTAB                                                                                            // 09.01.20:
   const uint8_t     *TV_Dat_p[_TV_CHANNELS];
#endif

}; // class MobaLedLib_C



#endif // _MOBALEDLIB_H_


