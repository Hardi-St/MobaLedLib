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


 Schedule
 ~~~~~~~~
 Zeit- oder Helligkeitsgesteuertes Ein- und Ausschalten von Variablen.

  - Schedule(DstVar1, DstVarN, EnableCh, Start, End)

 Mit den globalen Variablen Darkness und DayState werden die Configurations Variablen gesteuert.

 Abends (DayState == SunSet):
 Wenn der Darkness Wert goesser oder gleich dem Schaltwert ist, dann wird eine noch nicht active Variable eingeschaltet.
 Anschliessend wird der naechste Schaltwert zufaellig bestimmt.

 Morgens (DayState == SunRise):
 Wenn der Darkness Wert kleiner als der Schaltwert ist, dann wird eine Variable ausgeschaltet und ebenso ein neuer
 Schaltwert bestimmt.


 Revision History:
 ~~~~~~~~~~~~~~~~~
 25.09.18:  - Started
 28.09.18:  - Finished (Memory usage 604 bytes FLASH)

 ToDo:
 ~~~~~

*/

#include "MobaLedLib.h"


//------------------------------------------------------------------------------
uint8_t RandNr(uint8_t lowerbound, uint8_t upperbound, uint8_t cnt, bool SunSet)
//------------------------------------------------------------------------------
// Gleichverteilte zufaellige Werte werden dadurch erreicht, das die Random
// Funktion mehrfach aufgerufen wird und der mimimal-/maximalwert gebildet wird.
// Problem:
// Normalerweise wuerden zu beginn N zufaellige Werte berechnet. Diese sind
// gleichmaessig ueber den Berech verteilt. Dazu wird aber Speicher fuer N Werte
// benoetigt.
// Wenn nur ein Speicher verwendet werden soll, dann muessen die zufaelligen
// Werte nacheinander berechnet werden. Spaeter berechnete Werte koennen aber
// nicht mehr den gesammten Bereich abdecken, da der Anfangsbereich durch
// die vorangegangenen Werte schon abgedeckt ist. Das fuehrt zu einer Haeufung
// der Werte im hinteren/vorderen Bereich.
// Abhilfe schafft das mehrfache berechnen des Zufallswerts mit einer Minimal
// Bildung. Geprueft wurde das mit dem Excel Makro TestRandom.xlsm.
{
  uint8_t Nr, RetNr;
  if (SunSet)
       RetNr = upperbound;
  else RetNr = lowerbound;
  for (uint8_t i = 0; i < cnt; i++)
      {
      Nr = random8(lowerbound, upperbound+1);
      if (SunSet)
           { if (Nr < RetNr) RetNr = Nr;}
      else { if (Nr > RetNr) RetNr = Nr;}
      }
  //Dprintf("RandNr(%i, %i, %i, %i)=%i\n", lowerbound, upperbound, cnt, SunSet, RetNr); // Debug
  return RetNr;
}

//----------------------------------------------
void MobaLedLib_C::CalcSwitchVal(Schedule_T *dp)
//----------------------------------------------
{
  if (DayState == SunSet)
       dp->SwitchVal = RandNr(max(dp->SwitchVal, pgm_read_byte_near(cp+P_SCHEDULE_START)), pgm_read_byte_near(cp+P_SCHEDULE_END), dp->ToDoCnt,  true);
  else dp->SwitchVal = RandNr(pgm_read_byte_near(cp+P_SCHEDULE_START), min(dp->SwitchVal, pgm_read_byte_near(cp+P_SCHEDULE_END)), dp->ToDoCnt, false);
}

//--------------------------------
void MobaLedLib_C::Proc_Schedule()
//--------------------------------
{
  Schedule_T *dp = (Schedule_T*)rp; // Set the time pointer to the reserved RAM
  rp += sizeof(Schedule_T);

  uint8_t Inp = Get_Input(pgm_read_byte_near(cp+P_SCHEDULE_INCH));

  if (Inp_Is_On(Inp))
     {
     if (dp->OldDayState != DayState || Inp_Changed(Inp))
        {
        uint8_t DstVar1 = pgm_read_byte_near(cp+P_SCHEDULE_DSTVAR1);
        uint8_t DstVarN = pgm_read_byte_near(cp+P_SCHEDULE_DSTVARN);
        uint8_t Cnt = 0;
        for (uint8_t DstVar = DstVar1; DstVar <= DstVarN; DstVar++)
           if (Inp_Is_On(Get_Input(DstVar))) Cnt++;

        //Dprintf("%8lu: StCh: ", millis()); // Debug
        if (DayState == SunSet) // if() uses 28 bytes less memory than switch/case
             dp->ToDoCnt = DstVarN + 1 - DstVar1 - Cnt;
        else dp->ToDoCnt = Cnt;
        CalcSwitchVal(dp);

        dp->OldDayState = DayState;
        }

     if (dp->ToDoCnt > 0 && (
         (DayState == SunSet  &&  Darkness >= dp->SwitchVal) ||    // Abend  und noch nicht alle Variablen an  und Schwelle erreicht
         (DayState == SunRise &&  Darkness <  dp->SwitchVal)   ))  // Morgen und noch nicht alle Variablen aus und Schwelle erreicht
        {
        uint8_t DstVar1 = pgm_read_byte_near(cp+P_SCHEDULE_DSTVAR1);
        uint8_t DstVarN = pgm_read_byte_near(cp+P_SCHEDULE_DSTVARN);
        uint8_t Nr      = random8(dp->ToDoCnt); // Eine Zufaellige Nummer aus den zu aendernden Variablen

        for (uint8_t DstVar = DstVar1; DstVar <= DstVarN; DstVar++)
           {
           if (Inp_Is_On(Get_Input(DstVar)) != (DayState == SunSet)) // SunSet ist das Gegenteil vom Sollwert. Abends werden die ausgeschalteten Variablen betrachtet, Morgens die eingeschalteten Variablen
              {
              if (Nr-- == 0)
                 {
                 Set_Input(DstVar, DayState == SunSet);
                 //Dprintf("Set_Input(%i, %i)\n", DstVar, (DayState == SunSet)); // Debug
                 break;
                 }
              }
           }
        dp->ToDoCnt--;
        //Dprintf("%8lu: if(): ", millis()); // Debug
        CalcSwitchVal(dp);
        }
     }
}

