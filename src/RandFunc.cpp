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


 Random functions:
 ~~~~~~~~~~~~~~~~~

 Zufallsfunktionen mit denen Eingangskanaele zufaellig geschaltet werden. Diese Kanaele koennen von anderen
 Funktionen eingelesen werden.

 - Random(DstVar, Inp, MinTime, MaxTime, MinOn, MaxOn)
   Schaltet einen Eingangskanal nach einer zufaelligen Zeit ein und wieder aus.

 - RandMux(DstVar1, DstVarN, Inp, Mode, MinTime, MaxTime)
   Schaltet zufaellig zwischen verschiedenen Eingangskanaelen um. Wenn Inp aus ist, dann wird der erste
   Eingangskanal aktiviert.


 Revision History:
 ~~~~~~~~~~~~~~~~~
 12.09.18:  - Started
 13.09.18:  - Output is set directly when turned on
            - RandMux tested with MinTime=3 Min,  MaxTime=2 Min
 15.09.18:  - RF_STAY_ON added


 Anwendungen fuer die Zufallsfunktion:
   - Blitzlicht Fotograf
   - Bewegungsablaeufe triggern
   - Umschaltung Kirmes Effekte / Disco
   - Schweisslicht (Evtl. hat die Schweisslicht Funktion eingebaute zufallsfunktionen fuer Einschalthaeufigkeit und Dauer)


 Gedanken zu den Random Funktionen                                                                            // 13.09.18:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 - 3 Bytes fuer Zeit
   + von 1 ms bis zu 4.6 Stunden (Sonst 1 ms bis 17 Min allerdings
     reduziert sich die Aufloesung auf 16 ms wenn die Zeiten groesser als 65 Sek sind)
   + Keine Umschaltung per Makro noetig
   - Mehr Speicher pro Eintrag
   => Eigentlich reichen die 17 Minuten

 - DestVar als Variable Argumente
   + Man sieht welche Var benutzt werden
   + Bestimmte Ausgaenge koennen eine hoeherer Wahrscheinlichkeit bekommen
     Das kann man auch ueber eine nachgeschaltete Logik machen
   - Mehr Speicher
   - Unterschiedlicher Parameterreihenfolge. Bei allen anderen Funktionen
     kommt zuerst das Ziel, danach die Eingaenge und Parameter.
   => Don't change a running ...

 - Braucht man den Slow Mode
   => Ja, weil sonst die Laenge kurzer Blitze stark variiert
*/

#include "MobaLedLib.h"

//---------------------------------------------------------------------------------------
void MobaLedLib_C::Set_Timer4Random(uint16_t Timer, TimerData16_T *dp, uint8_t P_MinTime)
//---------------------------------------------------------------------------------------
{
  uint16_t MinTime = pgm_read_word_near(cp+P_MinTime);
  uint16_t MaxTime = pgm_read_word_near(cp+P_MinTime+2);

  dp->Last_t = Timer;
  dp->dt = MinTime + random16(MaxTime - MinTime);
  //Dprintf("dt = %i\n", dp->dt); // Debug
}

//------------------------------
void MobaLedLib_C::Proc_Random()
//------------------------------
{
  TimerData16_T *dp = (TimerData16_T*)rp; // Set the time pointer to the reserved RAM
  rp += sizeof(TimerData16_T);

  uint8_t Inp  = Get_Input(pgm_read_byte_near(cp+P_RANDOM_INP));
  uint8_t Mode = pgm_read_byte_near(cp+P_RANDOM_MODE);
  CALCULATE_t4w; // Calculate the local variable t4w if _USE_LOCAL_TX is defined                              // 22.11.18:
  uint16_t Timer = Mode & RF_SLOW ? t4w : t;

  if (Initialize)
     {
     //Dprintf("Init Proc_Random Mode=%i\n", Mode);// Debug
     if (Inp_Is_On(Inp))
          Inp =  INP_TURNED_ON;
     }

  if (Inp == INP_TURNED_ON)
       {
       Set_Timer4Random(Timer, dp, P_RANDOM_MINTIME_L);
       //Dprintf("%8lu Inp turned on. Next Change in %lu ms\n", millis(), (Mode & RF_SLOW)?16L*dp->dt:dp->dt); // Debug
       }
  else if (Inp == INP_TURNED_OFF) // Input is turned off
       {
       //Dprintf("Inp turned off\n");// Debug
       if (Mode & RF_STAY_ON)
            {
            Set_Timer4Random(Timer, dp, P_RANDOM_MINON_L);   // Set time to turn off
            //Dprintf("%8lu Inp turned off. Next Change in %lu ms\n", millis(), (Mode & RF_SLOW)?16L*dp->dt:dp->dt); // Debug
            }
       else Set_Input(pgm_read_byte_near(cp+P_RANDOM_DSTVAR), 0);
       }
  else {
       if ((uint16_t)(Timer - dp->Last_t) >= dp->dt)
          {
          if (Mode & RF_STAY_ON) // DstVar is turned on delayed and turned off after the input is turnd off with the times MinOn, MaxOn
               {
               uint8_t DstVar = Get_Input(pgm_read_byte_near(cp+P_RANDOM_DSTVAR));
               bool Is_On = Inp_Is_On(DstVar);
               if (Inp == INP_ON)
                    { if (!Is_On) Set_Input(pgm_read_byte_near(cp+P_RANDOM_DSTVAR), true);  }
               else { if ( Is_On) Set_Input(pgm_read_byte_near(cp+P_RANDOM_DSTVAR), false); }
               }
          else { // Normal mode: While the input is on DstVar is periodic turned on and off
               if (Inp == INP_ON)
                  {
                  uint8_t DstVar = Get_Input(pgm_read_byte_near(cp+P_RANDOM_DSTVAR));
                  bool Is_On = Inp_Is_On(DstVar);
                  if (Is_On)
                       Set_Timer4Random(Timer, dp, P_RANDOM_MINTIME_L); // Set time to turn on again
                  else Set_Timer4Random(Timer, dp, P_RANDOM_MINON_L);   // Set time to turn off
                  //Dprintf("%8lu Turn %s", millis(), Is_On ? "on":"off"); Dprintf(" in %lu ms\n", (Mode & RF_SLOW)?16L*dp->dt:dp->dt); // Debug
                  Set_Input(pgm_read_byte_near(cp+P_RANDOM_DSTVAR), !Is_On);
                  }
               }
          }
       }
}

//------------------------------------------------------------
void MobaLedLib_C::Set_Dest_RandMux(uint8_t Act, uint8_t Mode)
//------------------------------------------------------------
{
  uint8_t DstVar1 = pgm_read_byte_near(cp+P_RANDMUX_DSTVAR1);
  uint8_t DstVarN = pgm_read_byte_near(cp+P_RANDMUX_DSTVARN);
  if (Act == 255)
     {
     if (Mode & RF_SEQ)
          {
          for (uint8_t Nr = DstVar1, i = 0; Nr <= DstVarN; Nr++, i++)
              {
              if (Inp_Is_On(Get_Input(Nr)))
                 {
                 Act = i+1;
                 break;
                 }
              }
          if (Act > DstVarN - DstVar1) Act = 1;
          }
     else Act = 1 + random8(DstVarN - DstVar1);
     }

  //Dprintf("RandMux set %i DstVar1=%i DstVarN=%i\n", Act, DstVar1, DstVarN);// Debug
  for (uint8_t Nr = DstVar1, i = 0; Nr <= DstVarN; Nr++, i++)
      Set_Input(Nr, Act == i);
}

//-------------------------------
void MobaLedLib_C::Proc_RandMux()
//-------------------------------
{
  TimerData16_T *dp = (TimerData16_T*)rp; // Set the time pointer to the reserved RAM
  rp += sizeof(TimerData16_T);

  uint8_t  Inp   = Get_Input(pgm_read_byte_near(cp+P_RANDMUX_INP));
  uint8_t  Mode  = pgm_read_byte_near(cp+P_RANDMUX_MODE);
  CALCULATE_t4w; // Calculate the local variable t4w if _USE_LOCAL_TX is defined                              // 22.11.18:
  uint16_t Timer = Mode & RF_SLOW ? t4w : t;

  if (Initialize)
     {
     //Dprintf("Init Proc_RandMux Mode %i\n", Mode);// Debug
     if (Inp_Is_On(Inp))
          Inp = INP_TURNED_ON;
     else Inp = INP_TURNED_OFF;
     }

  if (Inp == INP_TURNED_ON)
       {
       Set_Timer4Random(Timer, dp, P_RANDMUX_MINTIME_L);
       //Dprintf("%8lu Inp turned on. Set Dest in %lu ms\n",millis(), (Mode & RF_SLOW)?16L*dp->dt:dp->dt); // Debug
       Set_Dest_RandMux(255, Mode);
       }
  else if (Inp == INP_TURNED_OFF) // Input is turned off
       {
       //Dprintf("Inp turned off\n");// Debug
       Set_Dest_RandMux(0, Mode);
       }
  else if (Inp == INP_ON && (uint16_t)(Timer - dp->Last_t) >= dp->dt)
       {
       Set_Timer4Random(Timer, dp, P_RANDMUX_MINTIME_L);
       //Dprintf("%8lu Next Change in %lu ms\n", millis(), (Mode & RF_SLOW)?16L*dp->dt:dp->dt); // Debug
       Set_Dest_RandMux(255, Mode);
       }
}

