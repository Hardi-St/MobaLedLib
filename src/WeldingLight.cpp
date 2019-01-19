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


 WeldingLight.cpp
 ~~~~~~~~~~~~~~~~
 Abgewandelter Schweisslichtsketch vom Marco.
 (siehe https://stummiforum.de/viewtopic.php?f=21&t=127899#p1446544)

 Revision History:
 ~~~~~~~~~~~~~~~~~
 15.09.18:  - Started
 16.09.18:  - It's working (Flash Usage 442 Byte)

*/

#include "MobaLedLib.h"


// Definition des Ausgabewertes fuer das Schweisslicht
#define ledflickermin       100
#define ledflickermax       255

// Definition der Flackerzeit
#define leddelaymin         15
#define leddelaymax         125

// Definition wie viele Flacker vor Pause
#define flickertimesmin     50
#define flickertimesmax     150

// Definition Laenge der Pause (in Millisekunden)
#define pausemin            1000
#define pausemax            4000


// Definition Startwert Gluehen und Delay
#define maxglow             12       // Maximal 63
#define glowdelay           50


//-------------------------------
void MobaLedLib_C::Proc_Welding()
//-------------------------------
{
  WeldingData_T *dp = (WeldingData_T*)rp; // Set the data pointer to the reserved RAM
  rp += sizeof(WeldingData_T);

  if ((uint16_t)(t - dp->Last_t) >= dp->dt)
     { // waehrend der Timer laeuft, wird keiner der folgenden if / else if Bloecke ausgefuehrt:
     CRGB *lp = &leds[pgm_read_byte_near(cp+P_WELDING_LED)];
     uint8_t Inp = Get_Input(pgm_read_byte_near(cp+P_WELDING_INP));
     if (dp->flickertimes > 0)
          { // So lange die Anzahl der Flacker mehr als 0 ist Led ansteuern (Blaeulichs Weiss)
          byte flicker = random8(ledflickermin, ledflickermax); // Waehlt zufaelligen Flackerwert
          lp->r = lp->g = qsub8(flicker, 50);
          lp->b = flicker;

          if (--(dp->flickertimes) == 0 || !Inp_Is_On(Inp))
               {
               // Reduziert Flackerzahl um den Wert 1, wenn 0, Nachgluehen start
               lp->r = maxglow << 2; // Gelb
               lp->g = maxglow;
               lp->b = 0;
               dp->dt = glowdelay;
               dp->flickertimes = 0;
               }
          else dp->dt = random8(leddelaymin, leddelaymax); // waehlt zufaelliges Delay
          }
     else if (lp->g > 0)  // Gluehen langsam ausgehen lassen
          {
          if ( --(lp->g) == 0 )
             { // Reduziert Gluehwert um 1, wenn 0 Pause starten
             dp->dt = random16(pausemin, pausemax); // Selektiert zufaelliges Delay fuer Pause zwischen den Schweissvorgaengen
             }
          lp->r = lp->g << 2; // Gelb: r = 4 * g
          }
     else { // Pause zwischen den Flackerern zu Ende
          if (Inp_Is_On(Inp))
               dp->flickertimes = random8(flickertimesmin, flickertimesmax); // Waehlt ein zufallswert fuer die Anzahl der Flacker
          else dp->dt = 100; // Next check in 100ms
          }
     dp->Last_t = t;
     }
}

