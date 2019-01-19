#ifndef __READ_LDR_H__
#define __READ_LDR_H__
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

 ToDo:
 - Andere Analogwerte zur verfuegung stellen
 - Erkennen wenn ein Schalter angeschlossen ist. Dann soll die Daempfung sehr gross sein


 Memory usage: Flash: 586, RAM 13 Byte (Incl. Dprintf()                                                       // 24.09.18:
 USE_LDR (792 FLASH)

 Wenn Init_LDR() nicht aufgerufen wird, dann spart man nur 68 Bytes FLASH. Die Interrupt benoetigt
 704 Bytes FLASH.
 Wenn man die Funktionalitaet des Interrupts in eine eigene Funktion auslagert und diese ueber eine
 boolsche Variable geschaltet im Interrupt aufruft, dann kann man 434 Bytes sparen. Aber nicht
 alle 704 Bytes ;-(
 => Der compiler Schalter kann nicht entfallen ;-(
 ==> Das Einlesen des LDR kommt in eine H-Datei. Dann braucht es nur Speicher wenn die h-Datei eingebunden ist.


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

typedef struct
      {
      uint8_t Start:1;
      uint8_t SwitchMode:1;
      } AD_Flags_T;

static AD_Flags_T AD_Flags = { 1, 0 };
static uint32_t LDR_Val  = 0;
static uint16_t AD_Count = 0;
static uint8_t  AD_Min   = 255;
static uint8_t  OldDarkness;
static uint8_t  LDR_damping_Fact;

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

//-------------------------------------------------------------
void Init_DarknessSensor(uint8_t LDR_Pin, uint8_t Damping_Fact = 50) // dafault value for Damping_Fact = 50 (Range: 1 = Slow, 100 = fast)
//-------------------------------------------------------------
// http://yaab-arduino.blogspot.com/2015/02/fast-sampling-from-analog-input.html
//
// Attention: The analogRead() function can't be used together with the darkness sensor
//            because this function reads the analog values in an interrupt routine.
{
  pinMode(LDR_Pin, INPUT_PULLUP);
  LDR_damping_Fact = Damping_Fact;

  ADCSRA = 0;                       // clear ADCSRA register
  ADCSRB = 0;                       // clear ADCSRB register
  ADMUX |= ((LDR_Pin - A0) & 0x07); // set analog input pin   (Tested with Auduino Nano)
  ADMUX |= (1 << REFS0);            // set reference voltage
  ADMUX |= (1 << ADLAR);            // left align ADC value to 8 bits from ADCH register

  // sampling rate is [ADC clock] / [prescaler] / [conversion clock cycles]
  // for Arduino Uno ADC clock is 16 MHz and a conversion takes 13 clock cycles
  ADCSRA |= (1 << ADPS2) | (1 << ADPS1) | (1 << ADPS0);    // 128 prescaler for   9.6 KHz
//ADCSRA |= (1 << ADPS2) |                (1 << ADPS0);    // 32  prescaler for  38.5 KHz
//ADCSRA |= (1 << ADPS2);                                  // 16  prescaler for  76.9 KHz
//ADCSRA |=                (1 << ADPS1) | (1 << ADPS0);    // 8   prescaler for 153.8 KHz

  ADCSRA |= (1 << ADATE); // enable auto trigger
  ADCSRA |= (1 << ADIE);  // enable interrupts when measurement complete
  ADCSRA |= (1 << ADEN);  // enable ADC
  ADCSRA |= (1 << ADSC);  // start ADC measurements
}


//-----------
ISR(ADC_vect)
//-----------
// Using a low pass filter in addition with a min calculation.
// The 50 Hz flicker of the Lights is "filtered" with a min
// calculation because this is very fast. In addition
// a low pass is used.
{
   if (++AD_Count < 500)
       {
       uint8_t Val = ADCH;
       if (Val < AD_Min) AD_Min = Val;
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
       AD_Min = ADCH;
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
