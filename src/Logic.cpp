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


 Logic functions:
 ~~~~~~~~~~~~~~~~

 Logic Funktion mit der man mehrere Eingaenge verknuepfen kann.
 Das Ergebnis wird in einer Input Variable gespeichert.
 Die Logischen Ausdruecke koennen mit UND, ODER und NICHT verknuepft werden.
 Dazu wird die Disjunktive Normalform: (A AND B) OR (A AND NOT C) OR D verwendet.
 Implizite Klammerung, die AND koennen durch ein Komma ersetzt werden. OR und NOT
 sind spezielle Nummern welche nicht als Eingang vorkommen koennen (SI_0 / SI_1)
 Beispiel: Logic0(12, 3, NOT, 4, OR, 31, 8)
 Mit der Logic Funktion lassen sich auch Demux Funktionen bauen

 Mehrfachverwendung der Ausgaenge von Loigic Funktionen
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Es ist NICHT moeglich den Ausgang einer Logic Funktion mehrfach zu verwenden weil dadurch
 Zustaende verloren gehen. Die Eingaenge haben 4 verschiedene Zustaende
   INP_OFF            B00000000
   INP_ON             B00000011
   INP_TURNED_ON      B00000001
   INP_TURNED_OFF     B00000010
                              ^
 Die Logic Funktion setzt nur das Letzte Bit.
 Der vorangegabgegangene Zustand wird im Bit mit der Wertigkeit 2 gespeichert.
 Dieses Bit wird am einde einer Updateschleife gesetzt.
 Wenn mehrere Logic Funktionen in die gleiche Variable schreiben, dann wird der Letzte Zustand
 nicht richtig gesetzt => INP_TURNED_ON / _OFF geht verloren ;-(

 => Fuer jede Logic() Funktion muss eine eigene Ausgangsvariable benutzt werden

 Weitere Logic Funktionen
 - Mux/Demux (Kann ueber Logic Funktionen gebildet werden)

 Revision History:
 ~~~~~~~~~~~~~~~~~
 05.09.18:  - Started
 16.09.18:  - Added the ENABLE/DISABLE functionality. If the first argument is ENABLE/DISABLE the
              following number defines an input which enables/disables the output.
                Example: ENABLE  17, 12 AND 13
                         DISABLE 17, 12 AND 13
              DISABLE is equal to: ENABLE NOT
*/

#include "MobaLedLib.h"


//-----------------------------
void MobaLedLib_C::Proc_Logic()
//-----------------------------
{
  uint8_t DstVar = pgm_read_byte_near(cp);
  uint8_t Cnt    = pgm_read_byte_near(cp+1);
  const uint8_t *p = cp + 2;
  bool Invert    = false;
  bool First     = true;
  bool ChkEnable = false;
  uint8_t Val    = _BIT_NEW; // Local result of one AND group
  uint8_t Res    = 0;
  //Dprintf("DstVar %i (%i) ", DstVar, Get_Input(DstVar)); // Test mehrfach verwenung der Logic Ausgaenge
  while (Cnt--)
    {
    uint8_t d = pgm_read_byte_near(p++);
    switch (d)
        {
        case _NOT_NR: Invert = !Invert; break;
        case _OR_NR:  if (First)
                           ChkEnable = true; // _OR_NR at the first position is used to define the ENABLE Input
                      else {
                           Res |= Val;
                           if (Res) { Cnt = 0; break; } // We can abort the loop (More then true is not possible)
                           Val = _BIT_NEW; // start the next AND group
                           }
                      break;
        default:      uint8_t Act;
                      uint8_t Inp = Get_Input(d);
                      //Dprintf("Inp%i:%i ", d, Inp);      // Test mehrfach verwenung der Logic Ausgaenge
                      if (Inp_Is_On(Inp))
                           Act = _BIT_NEW;
                      else Act = 0;
                      if (Invert) Act ^= _BIT_NEW;  // Invert the bit
                      if (ChkEnable)
                           {
                           if (!Act) Val = Cnt = 0; // Not enabled => Stop procesing the other inputs
                           ChkEnable = false;
                           }
                      else Val &= Act;
                      Invert = false;
        }
    First = false;
    }
  Res |= Val;
  Set_Input(DstVar, Res);
  //Dprintf("Res %i %s", Res, pgm_read_byte_near(cp-2) == 1 ? "\n":"   "); // Test mehrfach verwenung der Logic Ausgaenge
}

//------------------------------------------------------
bool MobaLedLib_C::Inp_is_Used_in_Logic(uint8_t Channel)
//------------------------------------------------------
{
  if (Channel == _OR_NR || Channel == _NOT_NR) return false;
  uint8_t Cnt    = pgm_read_byte_near(cp+1);
  const uint8_t *p = cp + 2;
  while (Cnt--)
    {
    uint8_t d = pgm_read_byte_near(p++);
    if (d == Channel) return true;
    }
  return false;
}

//------------------------------
void MobaLedLib_C::IncCP_Logic()
//------------------------------
{
  cp += pgm_read_byte_near(cp+1) + 2;
}
