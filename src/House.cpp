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


 Funcions to simulate a lively house.
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 The lights in the house are randomly turned on and off.

 Revision History:
 ~~~~~~~~~~~~~~~~~
 17.09.18:  - Extracted to a separate modul
 06.09.19:  - Added SINGLE_LEDxy
 05.10.19:  - Soved problem with single LEDs (See below)


 Problem:                                                                            05.10.19:
 ~~~~~~~~
 Die Gaslights funktionieren nicht, wenn danach keine weiteren LEDs kommen
  - GasLights(#LED, #InCh, GAS_LIGHT1, GAS_LIGHT1)                             => Nur die rote LED geht an
  + GasLights(#LED, #InCh, GAS_LIGHT1, GAS_LIGHT2)                             => O.K.
    // Reserve LEDs(1)                                                            wenn hier eine zusaetzliche LED reserviert wird
  + GasLights(#LED, #InCh, GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3, GAS_LIGHT1)     => O.K.
  + GasLights(#LED, #InCh, SINGLE_LED1, SINGLE_LED2, SINGLE_LED3, SINGLE_LED1) => O.K.

 Es Funktioniert auch schon, wenn man hier
  CRGB leds[NUM_LEDS];
 +1 einfuegt

 Problem beim Zaehlen der aktiven Raeume erkannt.
 lp wurde in der Schleife erhoeht wenn eine rote einzelne LED erkannt wurde.
 Diese Erkennung wurde aber gemacht bevor die naechste LED mit Get_RawNr(Room_Typ)
 ermittelt wurde. Dadurch wurde lp schon bei der ersten LED erhoeht. lp zeigt dann,
 wenn nur 3 einzelne LEDs verwendet werden auf die naechste LED im leds[] Array.
 Wenn aber keine weiteren LEDs benutzt werden, dann zeigt lp auf die folgende
 Speicherstelle welche undefinierte Werte enthaelt ;-(
 Wenn CRGB leds[NUM_LEDS+1] verwendet wird zeigt lp auf eine unbenutzte LED in dem Array.
 Vermutlich wird diese LED irgend wo auf 0 initialisiert. Ich habe die Stelle aber nicht
 gefunden. Mit Debugausgaben sieht man aber, das On_LEDs gleich zu begin 3 ist, wenn das
 leds[] Array nicht um eins vergroessert wurde.

 Jetzt habe ich die Zeile in der lp erhoeht wird NACH das die ermittlung des naechsten
 Raumtyps verschoben. Damit wird lp erst dann erhoeht wenn die zweite Rote LED oder
 eine RGB LED kommt. Damit Funktioniert das GasLight richtig.

 Aber wirkt sich das auch auf andere House() Konfigurationen aus?

 Bei Karl habe ich einen zweiten Fehler bemerkt: Die SINGLE_LEDs gehen nicht sofort aus
 wenn man den Schalter ausschaltet.


*/

#include "MobaLedLib.h"


//-------------------------------------------------------
uint8_t MobaLedLib_C::Is_Room_On(CRGB *lp, uint8_t RawNr)
//-------------------------------------------------------
{
  if (RawNr == ALL_CHANNELS)
       {
       for (uint8_t i = 0; i < 3; i++)
           if (lp->raw[i]) return 1;
       }
  else if (lp->raw[RawNr]) return 1;
  return 0;
}

/*
 Problem: Bisher wurde der Mode in den Farben r und b abgelegt                                                // 05.08.18:
   r < b => TV
   r > b => Chimney
   r = b => Licht
 Das geht nicht, wenn das Licht beliebige Farben haben darf.
 Bei der Berechnung des TV und Feuer Lichts werden die Farben
 entsprechend angepasst, dass die ersten beiden Bedingungen von
 oben immer stimmen.
 Neue Ueberpruefung: Wenn die Farbe einer der beiden konstanten Farben entspricht,
 dann ist kein TV/Kamin an. Das fuehrt aber auch dazu, das bei entsprechendem
 TV Program / Feuer zu konstantem Licht gewechselt wird. Aber das ist nicht so schlimm.
 Das wird relativ selten passieren. Und wenn dann haben die Preiserlein es sich anders
 ueberlegt.
*/
#ifdef _NEW_ROOM_COL
    //--------------------------------------------------------
    bool MobaLedLib_C::Cmp_Room_Col(CRGB *lp, uint8_t ColorNr)
    //--------------------------------------------------------
    {
      const uint8_t *p = Room_ColP+(ColorNr)*3;
      return !(lp->r == pgm_read_byte_near(p++) &&
               lp->g == pgm_read_byte_near(p++) &&
               lp->b == pgm_read_byte_near(p));
    }

    //-------------------------------------------------------------------
    uint8_t MobaLedLib_C::Get_Room_Col1(uint8_t ColorNr, uint8_t Channel)
    //-------------------------------------------------------------------
    {
      return pgm_read_byte_near(Room_ColP+(ColorNr)*3+(Channel));
    }

    //----------------------------------------------------------
    void MobaLedLib_C::Copy_Room_Col(CRGB *Dst, uint8_t ColorNr)
    //----------------------------------------------------------
    {
      const uint8_t *p = Room_ColP+(ColorNr)*3;
      Dst->r = pgm_read_byte_near(p++);
      Dst->g = pgm_read_byte_near(p++);
      Dst->b = pgm_read_byte_near(p);
    }

    //-----------------------------------------------------------------------------------
    void MobaLedLib_C::Copy_Single_Room_Col(CRGB *Dst, uint8_t Channel, uint8_t ColorNr)                      // 06.09.19:
    //-----------------------------------------------------------------------------------
    {
      const uint8_t *p = Room_ColP+(ColorNr)*3+Channel;
      Dst->raw[Channel] = pgm_read_byte_near(p);
    }


#else
  #define Cmp_Room_Col(lp, ColorNr)        memcmp(lp,  &RoomCol[ColorNr], sizeof(CRGB))
  #define Get_Room_Col1(ColorNr, Channel)  RoomCol[ColorNr][Channel]                                          // 07.12.18:
  #define Copy_Room_Col(Dst, ColorNr)      memcpy(Dst, &RoomCol[ColorNr], sizeof(CRGB))
#endif

//-------------------------------------------------------------------------------
void MobaLedLib_C::Update_TV_or_Chimney(uint8_t TVNr, CRGB *lp, uint8_t Room_Typ)
//-------------------------------------------------------------------------------
{
  if (Is_Room_On(lp, ALL_CHANNELS))                                                                           // 25.10.18:  Old: Get_RawNr(Room_Typ)))
     {
     uint8_t ColNr = COLOR_TV0_A + 2 * TVNr;
     if (Cmp_Room_Col(lp, ColNr) == 0 || Cmp_Room_Col(lp, ColNr+1) == 0) return ;

     if (lp->r < lp->b)  //TV aktiv?
          {
          TV_Dat_T *TV_p = &TV_Dat[TVNr];
          lp->r = TV_p->r;
          lp->g = TV_p->g;
          lp->b = TV_p->b;
          }
     else if (lp->r > lp->b)  //Chimney aktiv?
             {
             if (Trigger20fps)
                {
                uint8_t *Heat_p = (uint8_t*)rp;
                if (*Heat_p > 80) *Heat_p = qsub8(*Heat_p, random8(7)); // Cool down
                if (random8() < 25) // New Spark
                   *Heat_p = qadd8(*Heat_p, random8(25,50));
                *lp = ColorFromPalette(HeatColors_p, scale8(*Heat_p, 170));

                uint8_t Scale;                                                                                // 25.10.18:
                switch (Room_Typ & 0x03)
                    {
                    case 2:  Scale = 70; break; // Bright
                    case 1:  Scale = 12; break; // Dark
                    default: Scale = 35;        // Normal
                    }

                nscale8x3(lp->r, lp->g, lp->b, Scale);// Reduce the brightnes
                if (lp->r <= lp->b) lp->r = qadd8(lp->b,1); // Fire must have r > b
                }
             }
        }
}

//-----------------------------------------------
uint8_t MobaLedLib_C::Get_RawNr(uint8_t Room_Typ)
//-----------------------------------------------
// Some Room_Typ's use only one channel of the WS28xx
// In this case the function returns the the index for the raw[] array (0..2)
// For all other Room_Typ's 4 is returned
{
  switch (Room_Typ)
    {                                                                                            // 06.09.19:  Added SINGLE_LEDxy
    case GAS_LIGHT1:  case GAS_LIGHT1D: case NEON_LIGHT1:  case NEON_LIGHT1D: case NEON_LIGHT1M: case SINGLE_LED1: case SINGLE_LED1D: return 0;
    case GAS_LIGHT2:  case GAS_LIGHT2D: case NEON_LIGHT2:  case NEON_LIGHT2D: case NEON_LIGHT2M: case SINGLE_LED2: case SINGLE_LED2D: return 1;
    case GAS_LIGHT3:  case GAS_LIGHT3D: case NEON_LIGHT3:  case NEON_LIGHT3D: case NEON_LIGHT3M: case SINGLE_LED3: case SINGLE_LED3D: return 2;
    default:                                                                                                                          return ALL_CHANNELS;
    }
}


//-------------------------------------------------------------
void MobaLedLib_C::Update_Gas_Light(uint8_t Room_Typ, CRGB *lp)
//-------------------------------------------------------------
// Gas lights take ~12 seconds to reach full brightness
// and they sometimes flicker.
{
  uint8_t MaxVal, ColorNr;
  uint8_t RawNr = Get_RawNr(Room_Typ); // Return 0, 1, 2 for single LEDs and 4 for all channels
  if (Room_Typ < GAS_LIGHTD)
       { ColorNr = COLOR_GASL_B; MaxVal = Get_Room_Col1(ColorNr, RawNr & 0x03); }  // Helligkeit der normalen Lampen
  else { ColorNr = COLOR_GASL_D; MaxVal = Get_Room_Col1(ColorNr, RawNr & 0x03); }  // Helligkeit der dunklen Lampen
  if (mod8(MaxVal, 2) == 0) MaxVal++;  // Muss ungerade sein weil ungerade Werte heller werden bedeuten, Gerade Werte = dunkler werden

  uint8_t Divide = 512 / MaxVal;
  uint8_t Val = lp->raw[RawNr & 0x03];
  if (Val > 0)
     {
     bool IncBrightness = mod8(Val,2);
     if (!IncBrightness) Divide /= 4; // Turn off faster
     if (Divide < 2 || (mod8(Trigger20fps, Divide))==1)
        {
        if (IncBrightness)  // Increase brightness
             {
             if (Val < MaxVal)
                  {
                  //if (Val == 1)        { Start = millis(); Dprintf("Start %lu\n", Start); } // Debug
                  //if (Val == MaxVal-2) Dprintf("End   %lu %lu\n", millis(), millis()-Start);// Debug
                  Val +=2;
                  //Dprintf("%i\n", Val);

                  if (RawNr == ALL_CHANNELS)
                       for (RawNr = 0; RawNr <= 2; RawNr++) lp->raw[RawNr] = Val; // loop over all channels
                  else lp->raw[RawNr] = Val;                                      // just set one channel
                  }
             else { // Maximal value reached
                  if (mod8(Trigger20fps, 32) == 1 && random8() < 10) // Zufaelliges Flackern etwa alle 80 Sekunden flackert eine Lampe. => Bei 8 Lampen passiert es alle 10 Sekunden
                     {
                     //static uint32_t LastFlackern = 0; Dprintf("Flackern %lu\n", millis() - LastFlackern); LastFlackern = millis(); // Debug
                     Val = MaxVal/3;
                     if (mod8(Val, 2) == 0) Val++;
                     if (RawNr == ALL_CHANNELS)
                          for (RawNr = 0; RawNr <= 2; RawNr++) lp->raw[RawNr] = Val; // loop over all channels
                     else lp->raw[RawNr] = Val;                                      // just set one channel
                     }
                  }
             }
        else { // Decrease brightness
             //if (Val == MaxVal-1)        { Start = millis(); Dprintf("Start %lu\n", Start); } // Debug
             Val = qsub8(Val, 2);
             //if (Val == 0) Dprintf("End   %lu %lu\n", millis(), millis()-Start);// Debug
             //Dprintf("%i\n", Val);

             if (RawNr == ALL_CHANNELS)
                  for (RawNr = 0; RawNr <= 2; RawNr++) lp->raw[RawNr] = Val; // loop over all channels
             else lp->raw[RawNr] = Val;                                      // just set one channel
             }
        }
     }
}

//--------------------------------------------------------------
void MobaLedLib_C::Update_Neon_Light(uint8_t Room_Typ, CRGB *lp)                                              // 02.09.18:
//--------------------------------------------------------------
// Beim einschalten flackert eine Neuonlampe mehrmals zufaellig bis sie an bleibt.
// Die Maximale Anzahl der Versuche wird mit MAX_FLICKER_CNT bestimmt.
// Die Anzahl der Versuche wird in der Helligkeit der LED gespeichert.
// 1 = Erster Versuch, 2 = Zweiter Versuch...
// Im Zuendmoment wird die Versuchsanzahl auch in der Helligkeit gespeichert.
// Hier wird die Versuchsanzahl von der Normalen Helligkeit abgezogen.
// Bei MAX_FLICKER_CNT = 5 ist Helligkeit beim ersten Versuch
//   250 = MaxVal - MAX_FLICKER_CNT.
{
  #define MAX_FLICKER_CNT  5

  if (Trigger20fps > 0)
     {
     uint8_t RawNr = Get_RawNr(Room_Typ); // Return 0, 1, 2 for single LEDs and 4 for all channels
     uint8_t Val = lp->raw[RawNr & 0x03];
     if (Val > 0)  // Enabled ?
        {
        uint8_t MinVal = 1, MaxVal, ColorNr;
        if (Room_Typ < NEON_LIGHTD)
             ColorNr = COLOR_NEON_B; // Helligkeit der normalen Lampen
        else if (Room_Typ < NEON_LIGHTM || Room_Typ >= NEON_LIGHTL)
             ColorNr = COLOR_NEON_D; // Helligkeit der dunklen Lampen
        else ColorNr = COLOR_NEON_M; // Helligkeit der mittleren Lampen
        MaxVal = Get_Room_Col1(ColorNr, RawNr & 0x03);
        if (Val >= MaxVal && Room_Typ == NEON_LIGHTL) // Large room with several neon lights which start delayed
           {
           if (Val == Get_Room_Col1(COLOR_NEON_D, RawNr & 0x03))
              {
              if (random8() < 200) return ; // Pause bevor naechste Neonroehre zuendet amit man sieht, dass es meherer Lampen sind
              //Dprintf("Next Neon Light Val %i\n", Val);
              Val ++;
              }
           if (Val >= Get_Room_Col1(COLOR_NEON_D, RawNr & 0x03))
              {
              MinVal = Get_Room_Col1(COLOR_NEON_D, RawNr & 0x03) + 1;
              MaxVal = Get_Room_Col1(COLOR_NEON_B, RawNr & 0x03);
              ColorNr = COLOR_NEON_B;
              }

//           if (     Val >= Get_Room_Col1(COLOR_NEON_M, RawNr & 0x03)) { MinVal = Get_Room_Col1(COLOR_NEON_M, RawNr & 0x03) + 1; MaxVal = Get_Room_Col1(COLOR_NEON_B, RawNr & 0x03); Dprintf("Set COLOR_NEON_B\n");}
//           else if (Val >= Get_Room_Col1(COLOR_NEON_D, RawNr & 0x03)) { MinVal = Get_Room_Col1(COLOR_NEON_D, RawNr & 0x03) + 1; MaxVal = Get_Room_Col1(COLOR_NEON_M, RawNr & 0x03); Dprintf("Set COLOR_NEON_M\n");}
           }

        if (Val < MaxVal) // Not turned on
             {
             bool Is_On;
             uint8_t Rand = random8();
             if (Val >= MaxVal - MAX_FLICKER_CNT)  // Flicker active    250 >= 255 - 5
                  {
                  if (Rand < 80)  return;  // Mit einer niedrigen Warscheinlichkeit bleibt die LED weitere 20 ms an
                  Val = Val - (MaxVal - MAX_FLICKER_CNT) + 1 + MinVal;  // LED aus, und Zaehler erhoehen  Val = 250 - (255 - 5) + 2 = 2   // 21.10.18:  Old: +2
                  Is_On = false;
                  }
             else { // Flicker off
                  if (Rand > 100) return ;  // Verlaengerung der "Aus"-Zeit
                  if (Val == MinVal) { Val += random8(MAX_FLICKER_CNT-1); // Zufaellige Flackeranzahl                                      // 21.10.18:  Old: Val == 1
                                       //Dprintf("Set FlickerCnt %i Min %i Max %i NewVal %i\n", Val - MinVal, MinVal, MaxVal, MaxVal - MAX_FLICKER_CNT + Val - MinVal);
                                     }
                  Val = MaxVal - MAX_FLICKER_CNT + Val - MinVal;        // Erster Schritt: Val = 255 - 5 + 1 - 1 = 250                    // 21.10.18:  Old -1 => - MinVal
                  Is_On = true;
                  }

             lp->raw[RawNr & 0x03] = Val; // set the first channel
             //Dprintf("Val %i\n", Val);
             if (RawNr == ALL_CHANNELS)
                for (RawNr = 1; RawNr <= 2; RawNr++) lp->raw[RawNr] = (Is_On ? Get_Room_Col1(ColorNr, RawNr) : (MinVal == 1 ? 0 : Get_Room_Col1(COLOR_NEON_D, RawNr))); // loop over all channels   // 21.10.18:  Replaced 0 by (MinVal == 1 ? 0 : Get_Room_Col1(COLOR_NEON_D, RawNr))
             }
        }
     }
}


//-----------------------------------------------------------
void MobaLedLib_C::Random_Const_Light(CRGB *lp, uint8_t TVNr)
//-----------------------------------------------------------
{
  uint8_t RndNr = random8(2); // Random Light
  Copy_Room_Col(lp, COLOR_TV0_A + 2*TVNr + RndNr);
  //Dprintf("ConstLight TV%i %i\n", TVNr, RndNr); // 05.08.18:  Debug
}

#define RAND_TV_OR_CHIM   10  // 05.08.18:  Debug: 90, Normal 10

//-----------------------------------------------------------
void MobaLedLib_C::Enable_Light_or_TV(CRGB *lp, uint8_t TVNr)
//-----------------------------------------------------------
{
  if (random8(100) > RAND_TV_OR_CHIM)
       { lp->r = 0;  lp->b = 10;  } // TV on     (r <  b)
  else Random_Const_Light(lp, TVNr);
}

//-----------------------------------------
void MobaLedLib_C::Enable_Chimney(CRGB *lp)                                                                   // 28.10.18:
//-----------------------------------------
{
  lp->r = 10; lp->b = 0;  // Chimny fire on (r >  b)
}

//--------------------------------------------------
void MobaLedLib_C::Enable_Light_or_Chimney(CRGB *lp)
//--------------------------------------------------
{
  if (random8(100) > RAND_TV_OR_CHIM)
       Enable_Chimney(lp); // Chimny fire on (r >  b)
  else Random_Const_Light(lp, 0);
}

//-------------------------------------------------------------------
void MobaLedLib_C::Enable_Light_TV_or_Chimney(CRGB *lp, uint8_t TVNr)
//-------------------------------------------------------------------
{
  if (random8(100) > RAND_TV_OR_CHIM)
       { if (random8(100) >= 50)
              Enable_Chimney(lp); // Chimny fire on (r >  b)
         else { lp->r = 0;  lp->b = 10;  } // TV on          (r <  b)
       }
  else Random_Const_Light(lp, 0);
}

//-------------------------------------------------------
void MobaLedLib_C::TurnOnRoom(CRGB* lp, uint8_t Room_Typ)
//-------------------------------------------------------
{
  switch(Room_Typ)
    {
    case ROOM_DARK       : lp->r =      lp->g =      lp->b =  50;   break;
    case ROOM_BRIGHT     : lp->r =      lp->g =      lp->b = 255;   break;
    case ROOM_WARM_W     : lp->r = 147; lp->g =  77; lp->b =   8;   break;
    case ROOM_RED        : lp->r = 255; lp->g =      lp->b =   0;   break;
    case ROOM_D_RED      : lp->r =  50; lp->g =      lp->b =   0;   break;
    case ROOM_COL0       :
    case ROOM_COL1       :
    case ROOM_COL2       :
    case ROOM_COL3       :
    case ROOM_COL4       :
    case ROOM_COL5       : Copy_Room_Col(lp, Room_Typ-ROOM_COL0); break;  // Adapt if ROOM_COL_CNT is changed
    case ROOM_COL345     : Copy_Room_Col(lp, random8(3, 6));      break;
    case ROOM_TV0        : Enable_Light_or_TV(lp, 0); break;
    case ROOM_TV1        : Enable_Light_or_TV(lp, 1); break;
    case FIRE            : case FIRED            : case FIREB            :       Enable_Chimney(lp);                break;   // 28.10.18:
    case ROOM_CHIMNEY    : case ROOM_CHIMNEYD    : case ROOM_CHIMNEYB    :       Enable_Light_or_Chimney(lp);       break;
    case ROOM_TV0_CHIMNEY: case ROOM_TV0_CHIMNEYD: case ROOM_TV0_CHIMNEYB:       Enable_Light_TV_or_Chimney(lp, 0); break;
    case ROOM_TV1_CHIMNEY: case ROOM_TV1_CHIMNEYD: case ROOM_TV1_CHIMNEYB:       Enable_Light_TV_or_Chimney(lp, 1); break;
    case GAS_LIGHT:  case GAS_LIGHTD : case NEON_LIGHT:  case NEON_LIGHTD:  case NEON_LIGHTM:  case NEON_LIGHTL:  lp->r = lp->g = lp->b = 1; break;
    case GAS_LIGHT1: case GAS_LIGHT1D: case NEON_LIGHT1: case NEON_LIGHT1D: case NEON_LIGHT1M: case NEON_LIGHT1L: lp->r = 1;                 break;
    case GAS_LIGHT2: case GAS_LIGHT2D: case NEON_LIGHT2: case NEON_LIGHT2D: case NEON_LIGHT2M: case NEON_LIGHT2L: lp->g = 1;                 break;
    case GAS_LIGHT3: case GAS_LIGHT3D: case NEON_LIGHT3: case NEON_LIGHT3D: case NEON_LIGHT3M: case NEON_LIGHT3L: lp->b = 1;                 break;
    case SINGLE_LED1     :                                                                                    // 06.09.19:
    case SINGLE_LED2     :
    case SINGLE_LED3     : Copy_Single_Room_Col(lp, Room_Typ-SINGLE_LED1,  COLOR_SINGLE);   break;
    case SINGLE_LED1D    :
    case SINGLE_LED2D    :
    case SINGLE_LED3D    : Copy_Single_Room_Col(lp, Room_Typ-SINGLE_LED1D, COLOR_SINGLE_D); break;
    }
}

//-----------------------------------------------------------------------
void MobaLedLib_C::TurnOffRoom(CRGB* lp, uint8_t RawNr, uint8_t Room_Typ)
//-----------------------------------------------------------------------
{
  switch (Room_Typ)
    {
    case GAS_LIGHT:   case GAS_LIGHTD :
    case GAS_LIGHT1:  case GAS_LIGHT1D: if (lp->r % 2) lp->r--;    break; // Ungerade Werte bedeuten LED wird heller, Gerade LED wird dunkler
    case GAS_LIGHT2:  case GAS_LIGHT2D: if (lp->g % 2) lp->g--;    break; // => Wert gerade machen damit die LED dunkler wird
    case GAS_LIGHT3:  case GAS_LIGHT3D: if (lp->b % 2) lp->b--;    break;
    default: if (RawNr == ALL_CHANNELS)
                  lp->r = lp->g = lp->b = 0;
             else lp->raw[RawNr] = 0;
    }

}


//-----------------------------
void MobaLedLib_C::Proc_House()
//-----------------------------
// Anzahl der im Mittel angeschalteter LEDs berechnet sich ueber die Normalverteilung.
// Bei einem Wert von 127 sind im Mittel 50% der LEDs an.
// Wenn mehr oder weniger LEDs im Mittel an sein sollen, dann muss der entsprechende
// On_Limit Wert aus mit der Excel Tabelle "Warscheinlichkeit Haus.xlsx" berechnet
// werden.
// Achtung: Der Wert ist abhaengig von der Anzahl der verwendeten LEDs.
// Bei einem Wert von 255 werden alle LEDs nacheinander angeschaltet.
//
// Wenn der Schalter eingeschaltet wird, dann geht sofort eine LED an (Mit der
// sofortige Reaktion auf den Schalter soll die Funktionsweise ueberpruefbar sein
// und das entsprechende Haus lokalisiert werden koennen.). Die weiteren
// Lampen gehen nach und nach an (Die Bewohner haben unterschiedliche Helligkeits-
// beduerfnisse). Erst nach einiger Zeit brennen die ueber On_Limit angegebenen Lampen
// im Durchschnitt.
//
// Ueber On_Min kann angegeben werden wieviele Lampen mindestens an sein sollen.
// Wenn weniger als die angegebene Zahl an LEDs an sind, dann werden keine LEDs
// abgeschaltet bis die gewuenschte Zahl aktiv ist. Aber auch hier dauert es eine
// gewisse Zeit bis die Vorgabe erreicht ist. Wenn On_Min = 0 ist, dann kann es
// vorkommen, dass Zufaellig alle Lichter im Haus aus sind.
//
// Wenn der Schalter ausgeschaltet wird, dann geht sofort eine Lampe aus. Dann
// werden nacheinander Zufaellig alle Lampen ausgeschaltet.
//
// Die Funktion kann auch zur Ansteuerung der GAS Strassenlaternen benutzt werden.
// Hier gibt es Laternen mit Gluehbirnen oder LEDs. Fuer die Gluehbirnen werden 3 Kanaele (RGB)
// parallel geschaltet damit der Strom reicht. Fuer die LED Variante werden die Kanaehle
// einzeln verwendet. Dazu wurde die RawNr eingefuegt welche bestimmt welcher WS281x
// Kanal benutzt wird bzw. ob alle Kanaele benutzt werden.
// Die Funktion ist dadurch ziemlich chaotisch geworden. Evtl. kann man das noch verbessern.
// Einzelne Kanaele muessen nacheinander und in aufsteugender Reihenfolge angegeben werden.
// Beipiel: GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3, GAS_LIGHT, GAS_LIGHT1
// Falsch:  GAS_LIGHT1D, GAS_LIGHTD, GAS_LIGHT2D, GAS_LIGHTD, GAS_LIGHT3D, GAS_LIGHTD,
// Beim 2. Beispiel benutzt die dritte Lampe den gleichen Kanal wie Lampe 2 ;-(
//
{
  TimerData_T *tp = (TimerData_T*)rp; rp += sizeof(TimerData_T);
  uint8_t  Inp    = Get_Input(pgm_read_byte_near(cp+P_HOUSE_INCH));
  uint8_t LED_cnt = pgm_read_byte_near(cp+P_HOUSE_CNT);
  uint8_t Led0    = pgm_read_byte_near(cp+P_HOUSE_LED);
  CRGB *lp;

  #ifdef _HOUSE_STATISTIC
    House_Stat_T *StatP = (House_Stat_T*)rp;
    rp += sizeof(House_Stat_T);
  #endif

  if (Initialize)
     {
     //Dprintf("Init Proc_House InCh:%i cnt:%i Led0:%i\n", pgm_read_byte_near(cp+P_HOUSE_INCH), LED_cnt, Led0); // Debug
     if (Inp_Is_On(Inp))
          Inp = INP_TURNED_ON;
     else Inp = INP_TURNED_OFF;
     }

  CALCULATE_t10; // Calculate the local variable t10 if _USE_LOCAL_TX is defined                              // 22.11.18:
  if (Inp_Changed(Inp) || (uint8_t)(t10 - tp->Last_t) >= tp->dt) // Input changed or next random time
     {
     uint8_t Min_T = pgm_read_byte_near(cp+P_HOUSE_MIN_T);
     uint8_t Max_T = pgm_read_byte_near(cp+P_HOUSE_MAX_T);
     tp->Last_t = t10;
     tp->dt     = random8(Min_T, Max_T+1); // Max_T Maximal 254 because random8(uint8_t, uint8_t)
     uint8_t On_LEDs = 0;
     lp = &leds[Led0];
     const uint8_t *cpr = cp+P_HOUSE_ROOM_0;
     uint8_t Room_Typ = pgm_read_byte_near(cpr);
     uint8_t RawNr = Get_RawNr(Room_Typ);
     uint8_t SkipCnt = 0;
     for (uint8_t r = 0; r < LED_cnt; r++)
         {
         if (Room_Typ != SKIP_ROOM)
              On_LEDs += Is_Room_On(lp, RawNr);
         else SkipCnt++;
//       if ((RawNr & 0x03) == 0) lp++; // 05.10.19:  Old position
         Room_Typ = pgm_read_byte_near(++cpr); // Get the Room_Typ an RawNr of the next LED
         RawNr = Get_RawNr(Room_Typ);
         if ((RawNr & 0x03) == 0) lp++; // If the single channels are used and the next LED is R (0) or not a single channel (4) then the LED pointer should not be incremented 05.10.19:  New position
         }

     #ifdef _PRINT_DEBUG_MESSAGES
       //if (Inp_Changed(Inp) || Initialize) Dprintf("SkipCnt %i On_LEDs %i\n", SkipCnt, On_LEDs);
     #endif

     #ifdef _HOUSE_STATISTIC
       StatP->Cnt++;
       StatP->On += On_LEDs;
       if (++StatP->PrintStat >=10)
          {
          float avg = (float)StatP->On / StatP->Cnt;
          char outstr[15];
          dtostrf(avg,7, 2, outstr);
          Dprintf("LED_cnt %2i  Cnt%8li On%8li %s ", LED_cnt-SkipCnt, StatP->Cnt, StatP->On, outstr);
          dtostrf(100*avg/(LED_cnt - SkipCnt), 7,1, outstr);
          Dprintf("%s %%\n", outstr);
          StatP->PrintStat = 0;
          }
     #endif

     uint8_t On_Min   = pgm_read_byte_near(cp+P_HOUSE_ON_MIN);
     uint8_t On_Max = pgm_read_byte_near(cp+P_HOUSE_ON_MAX);
     bool IsOn = Inp_Is_On(Inp);
   //if (IsOn && (On_LEDs <= On_Min || (On_LEDs < On_Max && random8(2) > 0)))
     if (IsOn && (On_LEDs <  On_Min ||                      random8(2) > 0 ))                                 // 19.10.18:
          { // turn on one LED
          //Serial.print("On_LEDs:"); Serial.println(On_LEDs); // Debug
          if (LED_cnt - SkipCnt > On_LEDs && On_LEDs < On_Max)                                                // 19.10.18:  Added: && On_LEDs < On_Max
               {
               uint8_t Nr = random8(LED_cnt - SkipCnt - On_LEDs);
               lp = &leds[Led0];
               const uint8_t *cpr = cp+P_HOUSE_ROOM_0;
               uint8_t Room_Typ = pgm_read_byte_near(cpr);
               uint8_t RawNr    = Get_RawNr(Room_Typ);
               for (uint8_t r = 0; r < LED_cnt; r++)
                   {
                   //Dprintf("LED %i %i, RawNr %i\n", r, lp - leds+Led0, RawNr); // Debug
                   if (Room_Typ != SKIP_ROOM && !Is_Room_On(lp, RawNr) && Nr-- == 0)
                      {
                      TurnOnRoom(lp, Room_Typ);
                      //Dprintf("House On: %2i\n", On_LEDs+1); // Debug
                      break;
                      }
                   Room_Typ = pgm_read_byte_near(++cpr);  // Get the Room_Typ of the next LED
                   RawNr    = Get_RawNr(Room_Typ);
                   if ((RawNr & 0x03) == 0) lp++; // If the single channels are used and the next LED is R (0) or not a single channel (4) then the LED pointer should not be incremented
                   }
               }
          }
     else { // turn off one LED
          if ((!IsOn && On_LEDs) || (IsOn && On_LEDs > On_Min))                                               // 19.10.18:  Old: if(IsOn)
               {
               uint8_t Nr = random8(On_LEDs);
               lp = &leds[Led0];
               const uint8_t *cpr = cp+P_HOUSE_ROOM_0;
               uint8_t RawNr = Get_RawNr(pgm_read_byte_near(cpr));
               uint8_t Room_Typ = pgm_read_byte_near(cpr);
               for (uint8_t r = 0; r < LED_cnt; r++)
                   {
                   if (Room_Typ != SKIP_ROOM && Is_Room_On(lp, RawNr) && Nr-- == 0)
                      {
                      TurnOffRoom(lp, RawNr, Room_Typ);
                      //Dprintf("House On: %2i\n", On_LEDs-1); // Debug
                      break;
                      }
                   RawNr = Get_RawNr(pgm_read_byte_near(++cpr)); // Read the next RawNr
                   if ((RawNr & 0x03) == 0) lp++; // If the single channels are used and the next LED is R (0) or not a single channel (4) then the LED pointer should not be incremented
                   Room_Typ = pgm_read_byte_near(cpr);  // Get the Room_Typ of the next LED
                   }
               }
          }
     }
  // Update TV, ...
  lp = &leds[Led0];
  const uint8_t *cpr = cp+P_HOUSE_ROOM_0;
  uint8_t Room_Typ = pgm_read_byte_near(cpr);
  for (uint8_t r = 0; r < LED_cnt; r++)
      {
      switch (Room_Typ)
        {
        case ROOM_TV0:         Update_TV_or_Chimney(0, lp, Room_Typ);       break;
        case FIRE:                                                                                            // 28.10.18:
        case FIRED:
        case FIREB:
        case ROOM_CHIMNEY:
        case ROOM_CHIMNEYD:
        case ROOM_CHIMNEYB:
        case ROOM_TV0_CHIMNEY:
        case ROOM_TV0_CHIMNEYD:
        case ROOM_TV0_CHIMNEYB:Update_TV_or_Chimney(0, lp, Room_Typ); rp++; break;
        case ROOM_TV1:         Update_TV_or_Chimney(1, lp, Room_Typ);       break;
        case ROOM_TV1_CHIMNEY:
        case ROOM_TV1_CHIMNEYD:
        case ROOM_TV1_CHIMNEYB:Update_TV_or_Chimney(1, lp, Room_Typ); rp++; break;
        case GAS_LIGHT:
        case GAS_LIGHTD:
        case GAS_LIGHT1:
        case GAS_LIGHT2:
        case GAS_LIGHT3:
        case GAS_LIGHT1D:
        case GAS_LIGHT2D:
        case GAS_LIGHT3D:      Update_Gas_Light(Room_Typ, lp);    break;
        case NEON_LIGHT:
        case NEON_LIGHT1:
        case NEON_LIGHT2:
        case NEON_LIGHT3:
        case NEON_LIGHTD:
        case NEON_LIGHT1D:
        case NEON_LIGHT2D:
        case NEON_LIGHT3D:
        case NEON_LIGHTM:
        case NEON_LIGHT1M:
        case NEON_LIGHT2M:
        case NEON_LIGHT3M:
        case NEON_LIGHTL:
        case NEON_LIGHT1L:
        case NEON_LIGHT2L:
        case NEON_LIGHT3L:     Update_Neon_Light(Room_Typ, lp);    break;
        }
      Room_Typ = pgm_read_byte_near(++cpr);  // Next Room
      uint8_t RawNr = Get_RawNr(Room_Typ);
      if ((RawNr & 0x03) == 0) lp++; // If the single channels are used and the next LED is R (0) or not a single channel (4) then the LED pointer should not be incremented
      }
}

//------------------------------
void MobaLedLib_C::IncCP_House()
//------------------------------
{
  cp += P_HOUSE_ROOM_0 + pgm_read_byte_near(cp+P_HOUSE_CNT);
}


