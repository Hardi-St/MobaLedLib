#ifndef __READ_LDR_H__
#define __READ_LDR_H__
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

 Read LDR:
 ~~~~~~~~~

 Einlesen und Auswerten der Umgebungshelligkeit mit einem Lichtabhaengigen Widerstand (LDR).
 Die Helligkeit wird periodisch per Interrupt ausgelesen und gefiltert

 Der LDR ist zwischen Pin A1 (14 beim Nano) und GND angeschlossen.

 Bei einem Dunkelheitswert von 60 beginnt die Daemmerung
 Bei 160 ist es fast ganz Dunkel.


                  Schalter     ...........
    +----[LDR]-----o           :         :     Automatik  (Schalter Oben)
    |               ----o----->: Arduino :     Nacht      (   "     Mitte)
    +--------------o           :         :     Tag        (   "     Unten)
    |                          ...........
   GND

 Das Modul liest den LDR in einer Interruptroutine ein und brechnet daraus die Globalen Variablen
   - Darkness
   - DayState
 welche in der Schedule() Funktion benutzt werden.


 Revision History:
 ~~~~~~~~~~~~~~~~~
 24.09.18:  - Started
 06.04.20:  - Using the AnalogScanner library instead of an own analog
              interrupt function to be able to use the other analog inputs
              in the program.
*/

// Detection of a switch
#ifndef SWITCH_NIGHT
  #define SWITCH_NIGHT 240
#endif

#ifndef SWITCH_DAY
  #define SWITCH_DAY  5
#endif

#ifndef SWITCH_DAMPING_FACT
  #define SWITCH_DAMPING_FACT  1
#endif


#include "MobaLedLib.h" // variables Darkness / DayState

static AD_Flags_T AD_Flags = { 1, 0 };
static uint32_t LDR_Val  = 0;
static uint16_t AD_Count = 0;
static uint8_t  AD_Min   = 255;
static uint8_t  OldDarkness;
static uint8_t  LDR_damping_Fact;
static uint16_t LDR_Check_Count;

uint8_t Get_Damped_Darkness();
uint8_t Get_Act_Darkness();                    // Actual value of the darkness sensor

// LDR_damping_Fact  Tau     Tau = Zeit bis 63% erreicht ist. Das entspricht dem Wert von 161
// ~~~~~~~~~         ~~~~~
// 1   slow          310s = 5min  Wenn ein Tag/Nacht Schalter benutzt wird
// 5                  62s          "
// 10                 32s
// 20                 16s
// 50                  6s         Wenn das Umgebungslicht per Dimmer gesteuert wird
// 100 fast            4s          "

//----------------------------------------------------------------------------------------------------
void Init_DarknessSensor(uint8_t LDR_Pin, uint8_t Damping_Fact = 50, uint8_t AnalogInp_Scan_Count = 1) // dafault value for Damping_Fact = 50 (Range: 1 = Slow, 100 = fast)
//----------------------------------------------------------------------------------------------------
{
  pinMode(LDR_Pin, INPUT_PULLUP);
  LDR_damping_Fact = Damping_Fact;
  LDR_Check_Count  = 500/AnalogInp_Scan_Count;

}

//-----------------------------------------------------
void Darkness_Detection_Callback(int , int , int value)
//-----------------------------------------------------
// THis function has to be called from the AnalogScanner library
{
#ifdef ESP32
	value = (value * 6)/5;		// ESP32 uses additional voltage divider at the LDR pin, so values have to be converted
	if (value>1023) value=1023;
#endif	
  if (++AD_Count < LDR_Check_Count)
       {
       uint8_t Val8 = value>>2;
       if (Val8 < AD_Min) AD_Min = Val8;
       }
  else {
       if (AD_Flags.Start)
            {
            AD_Flags.Start = false;
            LDR_Val  = 2048L * AD_Min;
            Darkness = OldDarkness = LDR_Val >> 11;
            if (Darkness > SWITCH_NIGHT) { OldDarkness = LDR_Val = 0; AD_Flags.SwitchMode = 1;} // If the switch is set to night at power on the lights are turned on ramdomly
            if (Darkness < SWITCH_DAY)   { OldDarkness = 255;         AD_Flags.SwitchMode = 1;} // If the switch is set to day at power on the lights stay off
            }
       else {
            uint8_t Damping_Fact = AD_Flags.SwitchMode ? SWITCH_DAMPING_FACT : LDR_damping_Fact;
            uint32_t k2 = 2048 - Damping_Fact;
            LDR_Val  = ((k2 * LDR_Val + 1023) >> 11)  +  Damping_Fact * AD_Min;  // +1023 for correct rounding
            Darkness = LDR_Val >> 11;
            int Change = abs(Darkness - OldDarkness);
            if (Change > 3) // SunSet or SunRise ?
               {                                 // Wenn der Unterschied zwischem alten Darkness Wert und dem neuen Darkness Wert > x,dann wird
               if (Darkness > OldDarkness)       // SunSet / SunRise neu gesetzt und der neue Darkness Wert in den alten Darkness geschrieben
                    DayState = SunSet;
               else DayState = SunRise;
               OldDarkness = Darkness;
               }

            if (Change > 3 && (AD_Min < SWITCH_DAY || AD_Min > SWITCH_NIGHT))
               {
               AD_Flags.SwitchMode = 1;
               }
            else if (AD_Flags.SwitchMode && AD_Min > SWITCH_DAY && AD_Min < SWITCH_NIGHT)
                    AD_Flags.SwitchMode = 0;
            }
       AD_Min = value>>2;
       AD_Count = 0;
       }
}

//---------------------------
uint8_t Get_Damped_Darkness()
//---------------------------
{
  return Darkness;   // The variable is defined in MobaLedLib.cpp because it could be written from other functions too
}

//------------------------
uint8_t Get_Act_Darkness()
//------------------------
// Actual value of the darkness sensor
{
  return AD_Min;
}

#endif // __READ_LDR_H__
