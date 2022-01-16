#include "MobaLedLib.h"
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


 AnalogPattern.h
 ~~~~~~~~~~~~~~~
 This file contains the Pattern function which which could be used to generate
 animated pictures.



 Tests:
 - Maximale Zeit eines Abschnitts 65535         O.K.

 ToDo:
 - Untersuchen ob es wenige Speicher braucht wenn EaseInOut nicht in Update_LED verarbeitet wird sondern ausserhalb

 Speicher Untersuchungen:
 - static Funktionen brauchen mehr FLASH
 - Wenn Calc_m() in Calc_dV() integriert wird, dann werden 36 Bytes mehr FLASH benoetigt


 Versuch der Umstellung der AnalogPattern Funktion als eigene Klasse                                          // 01.09.18:
 ===================================================================
 Ich dachte mir, dass die Funktion AnalogPattern uebersichtlicher wird
 wenn man sie und alle Unterfunktionen in eine Eigen Klasse packt.
 Aber das war nix ;-(
 Es ist nicht wirklich uebersichtlicher wenn man Klassenvariablen verwendet.
 Die Funktionaufrufe werden dadurch zwar kuerzer aber man sieht auch nicht
 auf einen Blick wovon die Funktion abhaengig ist.
 Ausserdem kann die Funktion AnalogPattern() dann icht mehr auf die Variablen
 der Klasse MobaLedLib_C zugreifen => Die Variable muessen zusaetzlich uebergeben
 werden.
 Und dann braucht das ganze auch noch deutlich mehr Speicher (174 Bytes FLASH)
 => Das geht gar nicht.
 Ausserdem hab ich anscheinend irgend einen Bug eingebaut so das beim Performance
 Test der Aktive Eingang abgeschaltet wird.
 Der Misslungene Versuch ist in Versuch_mit_Klasse_fuer_AnalogPattern.zip

 Mit Klasse:
 Der Sketch verwendet 18248 Bytes (59%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
 Globale Variablen verwenden 737 Bytes (35%) des dynamischen Speichers, 1311 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

 Ohne Klasse:
 Der Sketch verwendet 18074 Bytes (58%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
 Globale Variablen verwenden 737 Bytes (35%) des dynamischen Speichers, 1311 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.


 Ueberlegung zu neuer Berechnung der Analogen Werte                                                            // 11.10.18:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Fuer das Anzeigen von Signalbildern waehre es praktisch, wenn die Ueberblendfunktionalitaet
 mit dem aktuellen Helligkeitswert der LED beginnt. Dann koennten die Signalbilder als einzelne
 APattern() Funktionen implementiert werden welche entsprechend aktiviert werden. Diese
 Funktionen wurden mit dem Flag PM_SEQUENZ_NO_RESTART und PF_NO_SWITCH_OFF benutzt.
 Pruefen ob ein zusaetzliches Flag eingefuehrt werden muss mit dem der Zustand sofort beendet wird
 wenn der Eingang aus ist. Es sieht auf die schnelle so aus als waehre as nicht noetig.

 Momentan wird der neue Helligkeitswert ueber eine Geradengleichung berechnet. Diese Gerade wird
 bei jedem Durchlauf neu berechnet. Dazu wird der Endwert des vorangegangenen Zustands v0 und der
 Endwert des aktuellen Zustands und die Dauer des Zustands benutzt.

 Man koennte die Berechnung so umstellen, dass der aktuelle Helligkeitswert der LED und der aktelle
 Zeitpunkt benutzt wird. Ih befuerchte aber, das das zu grossen Rundungsfehlern fuehrt ;-(

 Wenn man den Anfangswert jeder LED im RAM ablegt, dann sollte die Berechnung das gleiche
 Ergebniss liefern. Das wuerde aber zusaetzlichen RAM belegen ;-(
 => Das wurde implementiert

 Erweiterung der HSV Steuerung                                                                                // 13.10.18:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Die h, s, v Werte sollen mit verschiedenen asynchronen Perioden angesteuert werden.
 Dazu muessen die h, s, v Werte Global abgelegt werden. Damit mehrere verschiedene
 HSV Funktionen moeglich sind wird eine Funktion zum aktivieren eines neuen Bereichs eingefuehrt.
 - Es sollen Beliebig viele HSV Gruppen moeglich sein. Diese sollen sich nicht beeinflussen
 - Mit dem Befehl New_HSV_Group() wird Speicher fuer den HSV_T angelegt und ein Pointer darauf gesetzt.
   Die folgenden Pattern Funktionen  verwenden diesen Speicher.
 - Wenn in einer Zeile alle drei Kanaele berechnet werden, dann ist auch keine Beeinflussung moeglich
   => Bei drei Kanaelen koennen lokale Variablen verwendet werden wie bisher
      Der von New_HSV_Group gesetzte Pointer wird nicht benutzt.
 - Neuer Typ: HSV_T

 Goto Funktion                                                                                                // 12.11.18:
 ~~~~~~~~~~~~~
 Mit der Goto Funktionalitaet kann man mehrere Startpositionen in der Pattern Funktion definieren.
 Diese werden angsprungen abhaengig von einer lokale 8 Bit Variable welche von einer vorangegangenen
 Funktion geschrieben wird. Momentan schreibt nur die Counter() Funktion diese Variable. Angelegt wird
 die Variable mit der New_Local_Var() Funktion. Sie reserviert im RAM[] Array zwei Bytes. Eins fuer
 den Wert, das zweite als Flag mit dem Aenderungen Gespeichert werden. Dieses Changed Flag wird
 in der Pattern Funktion als Trigger benutzt.
 Wenn die Variable beim Trigger eine 0 enthaelt, dann wird an den Anfang des Musters gesprungen. Wenn
 die Variable eine 1 enthaelt, dann wird zur ersten Markierung gesprungen. Bei einer 2 zur Zweiten...
 Die Markierungen werden in einer zusaetzlichen Zeile in der Excel Tabelle eingegeben. Dazu wird
 in der entsprechenden Spalte ein "S" eingetragen. Auf iese Weise werden alle Startspalten markiert.
 Wenn die Variable einen Wert enthaelt der groesser ist als die Anzahl der vorhandenen "S" Eintraege,
 dann wird zum Ende gesprungen. Dabei wird nichts am Zustand der Ausgaenge veraendert.
 Neben den Startmarkierungen werden in der Tabelle auch Goto Anweisungen und die von den Goto Eintraegen
 angesprungenen Positionen Markiert.
 Eine Goto Anweisung besteht aus einem "G" gefolgt von der Nummer der Position.
   Beispiel: G 5
 Ein "G 0" ist hier nicht moeglich. Wenn zur Ersten Spalte gesprungen werden soll, dann muss dort ein "P"
 eingetragen werden.
 Ein Sprung zur 0 ist nicht moeglich weil die Informationen "S", "P" und Goto in einem
 Byte gespeichert werden. "S" = B10000000, "P" = B01000000 und fuer die Goto nummer werden die unteren 6
 Bits genutzt. Eine 0 an dieser Stelle bedeutet das hier kein Sprung stattfindet.
 Es gibt noch ein Weiteres Kommando: "E". Es ist eine Abkuerzung fuer "G 63". Damit wird die Abarbeitung
 des Musters beendet. Steht ein E in einer Spalte, dann wird Anschliessend keine weitere Spalte angezeigt.

 In einer Spalte koennen alle vier Kommandos enthalten sein. ( G und E aber nicht gleichzeitig)
 Beispiel:  | SP |    | E  | G 1 |  S  |

 Aktiviert wird die Goto Funktionalitaet in dem als Eingangskanal (InCh) SI_LocalVar verwendet wird.
 Dieser Spezielle Eingang hat ie gleiche nummer wie SI_0. Diese Nummer wurde gewaehlt weil es
 Unlogisch ist einen Eingang zu verwenden der immer 0 ist. Im Excel Sheet wird das automatich gemacht
 wenn im Feld "Goto Mode" eine 1 eingetragen wird.

 Alle Flags der Pattern Funktion koennen auch zusammen mit dem Goto Mode benutzt werden. Das Gilt auch
 fuer den PM_PINGPONG Mode. Achtung: as kann konfuse werden.

 Der Erste Anwendungsfall fuer die Goto Funktion ist die Zufaellige Sound Generierung.
*/

// Flags in the Goto table
#define START_BIT 128
#define POS_M_BIT  64
#define GOTO_MASK  (POS_M_BIT-1)


//------------------------------------
int32_t Calc_m(int16_t dV, uint16_t t)
//------------------------------------
// Calculate the factor m as a fixpoint number
// which is scaled by 2^SHIFT_FF = FIXFACT
//
// Value range:
// dV: -255 .. +255              9 Bit
// t:   0   .. 65535            16 Bit
// => Intermidiate values       25 Bit
// Maximal result at t = 1:     25 Bit
//
// Calculation:
//     FIXFACT * dV                         FIXFACT * dV + (dV>0 ? t:-t) / 2
// m = -----------    Correct rounding: m = ----------------------------------
//         t                                             t
// Without Round = With Round = 0,001257 ms ??
{
  // Dprintf("Calc_m %i %i\n", dV, t);
  if (t == 0)
       { // Senkrecht
       if (dV > 0)
            return MAX_M;
       else return MIN_M;
       }
  else
       #if 0 // With round (52 additional FLASH Bytes)
         return (((int32_t)dV<<SHIFT_FF) + ((dV>0 ? t:-t)>>1) ) / t;
       #else
         return (((int32_t)dV<<SHIFT_FF)                      ) / t;
       #endif
}

//------------------------------------
int16_t Calc_dV(uint16_t t, int32_t m)
//------------------------------------
// Calculate the new value for the given time
{
  if (m == MAX_M)      return  255;
  else if (m == MIN_M) return -255;
  else                 return (t*m)>>SHIFT_FF;
}

//------------------------------------------------------------------------------------------------------------------
uint8_t Calculate_V(uint8_t Val0, uint8_t Val1, uint16_t Duration, uint16_t Start_t, uint16_t Timer, bool EaseInOut)
//------------------------------------------------------------------------------------------------------------------
{
  int16_t v = Val0 + Calc_dV(Timer - Start_t, Calc_m((int32_t)Val1 - Val0, Duration));

  if (Val0 < Val1)                                                                                            // 01.06.19:
       {
       if      (v > Val1) v = Val1;
       else if (v < Val0) v = Val0;
       }
  else {
       if      (v > Val0) v = Val0;
       else if (v < Val1) v = Val1;
       }
//  if (v > 255)    v = 255;                                                                                  // 01.06.19:  Old
//  else if (v < 0) v = 0;

  if (EaseInOut)
       return ease8InOutApprox(v);   // easing function (Uebergangsfunktion) is used because changes near 0 and 255 are noticed different than in the middle
  else return v;
}
/*
//---------------------------------------------------------------------------------------------------------------------------
void Update_LED(uint8_t *lp, uint8_t Val0, uint8_t Val1, uint16_t Duration, uint16_t Start_t, uint16_t Timer, bool EaseInOut)
//---------------------------------------------------------------------------------------------------------------------------
{
  int16_t v = Val0 + Calc_dV(Timer - Start_t, Calc_m((int32_t)Val1 - Val0, Duration));

  if (v > 255)    v = 255;
  else if (v < 0) v = 0;
  if (EaseInOut)
       *lp = ease8InOutApprox(v);   // easing function (Uebergangsfunktion) is used because changes neare 0 and 255 are noticed different than in the middle
  else *lp = v;
  //Dprintf("v %i %i %i\n", v, *lp, ease8InOutCubic(v));
}
*/
//-------------------------------------------------------------------------------------------------------------------------------
void Update_LED_HSV(uint8_t *lp, uint8_t Val0, uint8_t Val1, uint16_t Duration, uint16_t Start_t, uint16_t Timer, bool EaseInOut)
//-------------------------------------------------------------------------------------------------------------------------------
{
  int16_t v = Val0 + Calc_dV(Timer - Start_t, Calc_m((int32_t)Val1 - Val0, Duration));

  if (v > 255)    v = 255;
  else if (v < 0) v = 0;
  if (EaseInOut)
       *lp = ease8InOutApprox(v);   // easing function (Uebergangsfunktion) is used because changes neare 0 and 255 are noticed different than in the middle
  else *lp = v;
  //Dprintf("v %i %i %i\n", v, *lp, ease8InOutCubic(v));
}

typedef struct
   {
   uint8_t LEDs;
   uint8_t Val0;
   uint8_t Val1;
   uint8_t Off;
   } Par_t;

//--------------------------------------------------------------------------------------
uint8_t ScaleValue(uint8_t BitVal, uint8_t BitsPerLed, uint8_t Val_Min, uint8_t Val_Max)
//--------------------------------------------------------------------------------------
{ // Example data:  BitVal = 2, BitsPerLed = 2, Val_Min=7, Val_Max = 141
  if (BitVal == 0) return Val_Min;                //                              no calculation needed
  if (BitsPerLed == 8) return BitVal;             //                              Ignore Val_Min and Val_Max
  uint8_t Range = Val_Max - Val_Min;              // 141 - 7 = 134
  uint8_t  MaxBitVal = (1<<BitsPerLed)-1;         // (1<<2)-1 = 3
  if (BitVal >= MaxBitVal) return Val_Max;        //                              no calculation needed
  uint8_t  OneStep = Range / MaxBitVal;           // 134 / 3 = 44     (44.666)    <= Rundungsfehler
  uint8_t Val      = Val_Min + BitVal * OneStep;  // 7 + 2 * 44 = 95  (0:0, 1:51, 2:95, 3:139)
  return Val & 0xFF;
}

//---------------------------------------------------------------------------
uint8_t GetLEDVal(uint16_t LEDNr, uint8_t BitsPerLed, const uint8_t *BytePos)
//---------------------------------------------------------------------------
// Get one value from an array of bytes started at position BytePos in PROGMEM.
// LEDNr defines the number of the LED and BitsPerLed the number of used bits.
// Example: 3 BitsPerLed, LedNr 2  => Pos = 6
//
//  Pos: 76543210 54321098 32109876
//       00000000 11111100 22221111
//       ~~~~~~~~ ~~~~~~~~ ~~~~~~~~
//       22111000 54443332 77766655
//
//  BitsPerLed => Mask
//   00000111  =  (1<<3) - 1 = 8-1
{
  uint16_t Pos = LEDNr * BitsPerLed;                                    // 2 * 3 = 6
  BytePos        += Pos / 8;                                            // 6 / 8 = 0
  uint8_t BitPos  = Pos % 8;                                            // 6 % 8 = 6
  uint16_t Mask = (1 << BitsPerLed) - 1;                                // 1 << 3 = 8    8-1 = 00000111
  Mask  <<= BitPos;                                                     // 00000001 11000000
  uint8_t Val = (pgm_read_byte_near(BytePos) & Mask) >> BitPos;         //          00000022
  if (Mask & 0xFF00) // Remaining bits in the upper byte
     { //                                     00000001        2
     Val |= (pgm_read_byte_near(++BytePos) & (Mask>>8)) << (8-BitPos);  // 00000222
     }

  return Val;
}

/*
 Komisch, wenn man ScaleValue() in GetLEDVal() aufruft, dann dauert ein Aufruf 16 us.
 Wenn man diesen Aufruf: ScaleValue(GetLEDVal(i, 3, Test3Bit),3,7,160) verwendet nur 10 us !?!
*/

//----------------------------------------------------------------------
uint8_t Calc_Prior_State(uint8_t State, uint8_t LastState, bool Reverse)
//----------------------------------------------------------------------
{
    if (Reverse)
         return State == LastState ? 0 : State + 1;
    else return State == 0 ? LastState : State - 1;
}

//--------------------------------------------------------------------------------------------------------
void Set_Next_State(uint8_t &State, bool &Reverse, uint8_t LastState, bool Run2endAndWait, bool PingPong)
//--------------------------------------------------------------------------------------------------------
{
  if (State != PT_INACTIVE)
     { // *** Set next period and state ***
     if (PingPong)
          {
          if (Reverse) // Reverse
               {
               if (State == 0)
                    { State = 1; Reverse = false; } // Change the direction
               else   State = State-1;
               }
          else { // Forward
               State++;
               if (State > LastState) { State = (LastState - 1); Reverse = true; } // Change the direction
               }
          }
     else { // Normal part which is called if ping pong is not activ
          State++;
          if (State > LastState) // End reached ?
             {
             State = Run2endAndWait ? PT_INACTIVE : 0; // Set state inactive if Run2endAndWait is active otherwise restart
             }
          }
     if (LastState == 0) State = PT_INACTIVE; // If we have only one state we go to sleep
     }
}
/*
 Beim HSV Mode muessen die h, s, v Werte gespeichert werden und anschliessend am Stueck geschrieben werden
*/

//-----------------------------------------------------------------------------------------------
uint8_t Find_GotoState(uint8_t Nr, uint8_t Search, const uint8_t *GotoTable_p, uint8_t LastState)             // 10.11.18:
//-----------------------------------------------------------------------------------------------
// Return the State where Search is found Nr times
// Nr: 0..n
// If State is not found LastState+1 is returned
{
  const uint8_t *tp = GotoTable_p;
  uint8_t State = 0;
  // Dprintf("Find_GotoState(%i) ", Nr); // Debug
  if (Nr > 0)
     {
     for (State = 0; State <= LastState; State++)
         {
         // Dprintf(" %i", pgm_read_byte_near(tp)); // Debug
         if (pgm_read_byte_near(tp) & Search)
            if (--Nr == 0) break;
         tp++;
         }
     }
  if (State > LastState)
       {
       State = PT_INACTIVE;
       // Dprintf("\nGotoEnd\n"); // Debug
       }
  else {
       // Dprintf("\nGoto Pos %i\n", State); // Deug
       }
  return State;
}

//---------------------------------------------------------------------
void MobaLedLib_C::Proc_AnalogPattern(uint8_t TimeCnt, bool AnalogMode)                                       // 24.08.18:
//---------------------------------------------------------------------
{
  AnalogPatternData_T *dp = (AnalogPatternData_T*)rp; // Set the time pointer to the reserved RAM
  if (AnalogMode)
       rp += sizeof(AnalogPatternData_T);
  else rp += 5;
  uint8_t ModeAFlag = pgm_read_byte_near(cp+P_PATERN_MODE);
  CALCULATE_t4w; // Calculate the local variable t4w if _USE_LOCAL_TX is defined                              // 22.11.18:
  uint16_t Timer    = ModeAFlag & PF_SLOW ? t4w : t; // Slow timer (divided by 16) to be able to use longer durations
  uint8_t InCh      = pgm_read_byte_near(cp+P_PATERN_INCH);                                                   // 09.11.18:
  uint8_t Inp;
  bool GotoMode;
  if ((GotoMode = (InCh == SI_LocalVar)))                                                                     // 09.11.18:
       Inp = INP_ON;   // Don't use the normal input if "Goto Mode" is active
  else {
       Inp = Get_Input(InCh);
       if (ModeAFlag & PF_INVERT_INP) Inp = Invert_Inp(Inp); // Invert the input.
       }
  uint8_t Enable = Get_Input(pgm_read_byte_near(cp+P_PATERN_ENABLE));                                         // 05.11.18:
  switch (Enable)
    {
    case INP_OFF:        return ;
    case INP_TURNED_OFF: Inp = INP_TURNED_OFF;
                         // Dprintf("Enable turned off\n"); // debug
                         break;
    case INP_TURNED_ON:  if (Inp_Is_On(Inp))
                            {
                            Inp = INP_TURNED_ON;
                            // Dprintf("Enable turned on\n");
                            }
                         break;
    }

  #ifdef USE_XFADE // Special fade mode which starts fading from the actual value instead of the last value in the configuration. Used for configirations where different lines control one LED
    union { Par_t p; uint32_t u32; }; // unnamed union                                                        // 11.10.18:  Moved up
    u32  = pgm_read_dword_near(cp+P_PATERN_LEDS); // Read LEDs, Val0, Val1, Off at once. Attention: Don't change the sequence of the parameters
    uint8_t XFade = false;
  
    #pragma GCC diagnostic push                                                           // 19.12.21:  Disable warning "LEDRamP uninitialized..." with ESP32 build
    #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
    uint8_t *LEDRamStart, *LEDRamP;
    #pragma GCC diagnostic pop
    
    if (ModeAFlag & _PF_XFADE) { XFade = true; LEDRamStart = rp; rp += p.LEDs; }
  #endif

  if (Initialize)
     {
     Dprintf("Init Proc_AnalogPattern ModeAFlag %i, LEDs %i\n", ModeAFlag, pgm_read_byte_near(cp+P_PATERN_LEDS)); // Debug
     Dprintf("Initialize0: Inp=%i\n", Inp); // Debug
     if (Inp_Is_On(Inp))
          Inp =  INP_TURNED_ON;
     else Inp =  INP_INIT_OFF; // To initialize the LEDs to "Off" value
     dp->State = PT_INACTIVE;
     Dprintf("Initialize1: Inp=%i\n", Inp); // Debug
     if (GotoMode) Dprintf("Initialize Goto: %i Changed %i\n", ActualVar_p->Val, ActualVar_p->Changed); // Debug
     }

  if (GotoMode)
     {
     if (ActualVar_p && ActualVar_p->Changed)
           {
           Inp = INP_TURNED_ON; // to trigger the update below
           //Dprintf("Pattern LocalVar = %i\n", ActualVar_p->Val); // Debug
           }
     }

  // *** Check if the input has changed or it it's time for an update ***
  if (Inp_Changed_or_InitOff(Inp) || (dp->State != PT_INACTIVE && (uint16_t)(Timer - dp->Last_t) >= dp->dt)) // switch changed or next update time reached.
     {
     //Dprintf("Init %i Inp %i %i\n", Initialize, Inp, Inp_Is_TurnedOff_or_InitOff(Inp)); // Debug
     ledNr_t Led0 = pgm_read_led_nr(cp+P_PATERN_LED);

     #ifndef USE_XFADE
       union { Par_t p; uint32_t u32; }; // unnamed union                                                     // 11.10.18:  Old position
       u32  = pgm_read_dword_near(cp+P_PATERN_LEDS); // Read LEDs, Val0, Val1, Off at once. Attention: Don't change the sequence of the parameters
     #endif

     if (p.Val0 == p.Val1) p.Val1 = ~p.Val0; // Prevent division by zero in the dt calculation and make sure that something is shown    // 26.08.18:

     uint8_t NStru = pgm_read_byte_near(cp+P_PATERN_NSTRU);
     uint8_t *lp   = (uint8_t*)&leds[Led0] + (NStru & 0x03);  // Set LED pointer to the first LED (& 0x03 because the lower two bits are used for the start channel)
     #ifdef USE_XFADE
       uint8_t *lp0 = lp;
     #endif
     const uint8_t *BitMaskCntP = cp+P_PATERN_T1_L + 2*TimeCnt; // Calculate the position where the count of pattern bytes is stored

     uint16_t BitMaskCnt        = pgm_read_word_near(BitMaskCntP);                                            // 23.10.18:  Old: uint8_t
     uint8_t  BitsPerChannel    = ((NStru>>2)&0x07)+1;                                                        // 23.10.18:  Old: ((NStru>>2)&0x03)+1;
     uint8_t  FreeBits          = NStru>>5;
     uint16_t BitsPerState      = p.LEDs * BitsPerChannel;                                                    // 20.01.21:  Juergen: Old uint8_t
     if (GotoMode) BitsPerState += 8; // One additional byte for the "Goto table"                             // 09.11.18:
     const uint8_t *PatternP    = BitMaskCntP + 2;                                                            // 23.10.18:  Old: BitMaskCntP + 1;
     uint8_t LastState          = (BitMaskCnt * 8 - FreeBits) / BitsPerState - 1 ;
     //if (Initialize) {Dprintf("BitMaskCnt=%i BpC=%i ", BitMaskCnt, BitsPerChannel); Dprintf("FreeBits=%i LastState=%i\n", FreeBits, LastState); }

     const uint8_t *GotoTable_p;
     if (GotoMode) GotoTable_p = cp + P_PATERN_T1_L + 2*TimeCnt + BitMaskCnt + 2 - (LastState+1);  // 10.11.18:
     // 18.12.21: GOTO mode Find_GotoState is moved down into "case  INP_TURNED_ON" handling, because previously Retrigger_Stop and Retrigger were not supported in GOTO mode

     // *** Mode flags ***
     bool Run2endAndWait = false; // Run to the end state and wait. No reaction if the switch is turned off.
     bool Retrigger_Stop = false;
     bool Retrigger      = true;
     bool PingPong       = false;
     bool Reverse        = false;
     bool EaseInOut      = ModeAFlag & PF_EASEINOUT;
     bool HSV_mode       = false;
     HSV_T L_HSV, *L_HSV_p;

     switch (ModeAFlag & PM_MODE_MASK)
        {
        case PM_SEQUENZ_W_ABORT:    Retrigger_Stop = true; // fall through, no break // Flankengetriggerter einmalige Sequenz. Abbruch bei neuer Flanke waehrend der Laufzeit
        case PM_SEQUENZ_STOP:                              // fall through, no break // Flankengetriggerter einmalige Sequenz. Bei neuer Flanke wird mit State 0 begonnen. Beim auschalten wird sofort abgebrochen  26.10.18:
        case PM_SEQUENZ_W_RESTART:                     Run2endAndWait = true; break; // Flankengetriggerter einmalige Sequenz. Bei neuer Flanke wird mit State 0 begonnen
        case PM_SEQUENZ_NO_RESTART: Retrigger = false; Run2endAndWait = true; break; // Flankengetriggerter einmalige Sequenz. Kein Neustart waehrend der Laufzeit
        case PM_PINGPONG:           PingPong = true;
                                    if (dp->State != PT_INACTIVE)
                                       {
                                       Reverse = dp->State & 0x80;
                                       dp->State &= 0x7F;
                                       }
                                    break;
        case PM_HSV:                HSV_mode = true;
                                    L_HSV.Hue = L_HSV.Sat = L_HSV.Val = 255; // Uses 4 bytes more FLASH: memset(&L_HSV, 255, 3);
                                    break;
         }

     // *** Process the Input ***
     switch (Inp)
        {
        case  INP_TURNED_ON: // Switch is turned on
                             if (Retrigger_Stop && dp->State != PT_INACTIVE)
                                  dp->State = LastState; // Stop the state machine if the last state is not reached or restarte it if the end is reached
                             else {
                                  if (Retrigger || dp->State == PT_INACTIVE)  // Normaly Retrigger is active
                                     {
	   		                             if (GotoMode)
  				                              {

				                                if (ActualVar_p)
				                                   {
				                                   if (ActualVar_p->Changed || Initialize)                                                          // 18.12.21: Goto1 Patters will initialite to Goto State 0
                                                                                                                                            // 18.01.19:  Added: "|| Initialize" otherwise State is set to PT_INACTIVE
				                                      {                                                                                             //            which causes wrong startup values because "L1 = p.LEDs * dp->State;"
                                                                                                                              
					                                    uint8_t val = (!ActualVar_p->Changed && Initialize) ? 0 : ActualVar_p->Val;                   // 18.12.21 goto pattern init to 0 by default. But also Changed may be set to true
                                                                                                                                            // while Initialize in case the last state is set by EEPROM stored status
				                                      dp->State = Find_GotoState(val, START_BIT, GotoTable_p, LastState);                           
				                                      //if (dp->State > LastState) { dp->State = LastState; Dprintf("Goto state not found => Goto LastState\n");}
				                                      }
				                                   }
				                                else 
                                           { if (!Initialize) Dprintf("Error: New_Local_Var() not called before the GotoMode is used in pattern function\n"); 
                                           }
  				                              }
   				                           else
 				                                {
                                          dp->State = 0;
                                        }
                                     if (AnalogMode) dp->Start_t = Timer;
                                     Dprintf("Start State=%i\n", dp->State); // Debug
                                     }
                                  else 
                                     {
                                     Dprintf("Don't retrigger\n"); // Debug
                                     return;  // Don't update the LEDs and the state if Retrigger is disabled and StateP != PT_INACTIVE
                                     }
                                  }
                             #ifdef USE_XFADE
                               if (XFade)                                                                     // 11.10.18:
                                  {
                                  LEDRamP = LEDRamStart;
                                  for (uint8_t led = 0; led < p.LEDs; led++)                                  // 20.10.19:  Initialisation led = 0 was missing ;-(
                                    *(LEDRamP++) = lp[led];
                                  }
                             #endif
                             break;
        case INP_TURNED_OFF: // Switch is turned off
                             if ((ModeAFlag & PM_MODE_MASK) == PM_SEQUENZ_STOP)                               // 26.10.18:
                                {
                                dp->State = PT_INACTIVE;  // Stop
                                return ;
                                }
                             if (Run2endAndWait && dp->State != PT_INACTIVE)
                                   return ;     // Don't update the LEDs and the state if Run2endAndWait is active and StateP != PT_INACTIVE
                             else dp->State = PT_INACTIVE;  // Stop
                             if (ModeAFlag & PF_NO_SWITCH_OFF) return ;                                       // 08.09.18:
                             // Dprintf("Turned Off\n");
                             break;
        }

     // ****** Loop to update the LEDs ******
     uint16_t Duration = pgm_read_word_near(cp+P_PATERN_T1_L+2*mod8(dp->State, TimeCnt));
     uint8_t v0, v1;
     uint16_t L0, L1;
     if (p.LEDs >= 3 || HSV_p == NULL)
          L_HSV_p = &L_HSV;
     else L_HSV_p = HSV_p;

     if (AnalogMode)
         {
         #ifdef USE_XFADE
           if (XFade) LEDRamP = LEDRamStart;                                                                  // 11.10.18:
           else
         #endif
                L0 = p.LEDs * Calc_Prior_State(dp->State, LastState, Reverse);
         }
     //Dprintf("State %i\n", dp->State);
     L1 = p.LEDs * dp->State;
     for (uint8_t L = 0; L < p.LEDs; L++, lp++) // Loop over all LEDs
         {
         if (Inp_Is_TurnedOff_or_InitOff(Inp))
              v1 = p.Off; // Turn the LEDs off
         else {
              if (AnalogMode)
                 {
                 #ifdef USE_XFADE
                   if (XFade)
                        {                                                                           // 11.10.18:
                        #pragma GCC diagnostic push                                                           // 17.11.20:  Disable warning "LEDRamP uninitialized..."
                        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
                        v0 = *(LEDRamP++);
                        #pragma GCC diagnostic pop
                        }
                   else
                 #endif
                        {
                        #pragma GCC diagnostic push                                                           // 17.11.20:  Disable warning "L0 uninitialized..."
                        #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
                        v0 = ScaleValue(GetLEDVal(L0++, BitsPerChannel, PatternP),BitsPerChannel,p.Val0,p.Val1);
                        #pragma GCC diagnostic pop
                        }
                 }
              v1           = ScaleValue(GetLEDVal(L1++, BitsPerChannel, PatternP),BitsPerChannel,p.Val0,p.Val1);
              //if (!Initialize && AnalogMode) Dprintf("%i %i", (int)v0, (int)v1); // Debug 24.10.19:
              }
         // Update one LED
         if (AnalogMode && (Inp_Is_On(Inp) || (HSV_mode && !Initialize))) // 09.09.18:  Hier war Inp > INP_TURNED_OFF oder so  // 22.10.18:  Added: HSV_mode to correctly turn off the LEDs
              {                                                                                                                // 20.10.19:  Added: !Initialize because otherwise the Cave_Illumination shows random colors at startup
              #pragma GCC diagnostic push                                                                     // 17.11.20:  Disable warning "v0 uninitialized..."
              #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
              v1 = Calculate_V(v0, v1, Duration, dp->Start_t, Timer, EaseInOut); // Calculate the analog value and write it to lp
              #pragma GCC diagnostic pop
              //if (!Initialize && AnalogMode) Dprintf(" %i\n", v1); // Debug 24.10.19:
              if (HSV_mode)
                   {
                   uint8_t LEDCh = (L + (NStru & 0x03)) % 3;
                   //Dprintf("%i\n", LEDCh);
                   switch (LEDCh)
                      {
                      case 0:  L_HSV_p->Hue = v1; break;
                      case 1:  L_HSV_p->Sat = v1; break;
                      default: L_HSV_p->Val = v1;
                      }
                   if (LEDCh == 2 || L == p.LEDs-1)
                      {
                      CRGB *rgbp = (CRGB*)(lp - LEDCh);
                      rgbp->setHSV(L_HSV_p->Hue, L_HSV_p->Sat, L_HSV_p->Val);
                      //Dprintf("%i %i %i\n", L_HSV_p->Sat, L_HSV_p->Val); // Debug
                      }
                   }
              else *lp = v1;
              }
         else *lp = v1; // Write the value direct to the LED
         }

     // *** Prepare the next State ***
     if (!AnalogMode || (uint16_t)(Timer - dp->Start_t) >= Duration) // in AnalogMode the LEDs are updated in several sub states. The next state is activated after the time has been elapsed
        {
        if (AnalogMode) dp->Start_t = Timer;

        bool Jump = false;                                                                                    // 10.11.18:
        if (GotoMode)
             {
             #pragma GCC diagnostic push                                                                      // 17.11.20:  Disable warning "GotoTable_p uninitialized..."
             #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
             uint8_t GotoTabVal = pgm_read_byte_near(GotoTable_p+dp->State);
             #pragma GCC diagnostic pop
             //Dprintf("GotoTabVal[%i] = %i\n", dp->State, GotoTabVal); // debug
             uint8_t GotoNr = (GotoTabVal & GOTO_MASK);
             if (GotoNr)
                {
                // Dprintf("GotoNr %i\n", GotoNr); // Debug
                dp->State = Find_GotoState(GotoNr, POS_M_BIT, GotoTable_p, LastState);
                Jump = true;
                }
             }
        if (!Jump) Set_Next_State(dp->State, Reverse, LastState, Run2endAndWait, PingPong);

        // Debug
        //uint16_t ps = Calc_Prior_State(dp->State, LastState, Reverse); Dprintf("Prior State %i State:%i %i->%i\n", ps+1, dp->State+1, GetLEDVal(ps, BitsPerChannel, PatternP), GetLEDVal(dp->State, BitsPerChannel, PatternP)); // Debug
        }
     if (Reverse) dp->State |= 0x80;

     // *** Set Last_t and dt to define the next time when the LEDs have to be updated ***
     dp->Last_t = Timer; // Store the time when the function was called the last time for the calculation of the next update time
     if (AnalogMode)
          {
          if (dp->Start_t == Timer)
             {
             dp->dt = max(20, Duration>>8); // Eigentlich haengt dt von der Anzahl der Schritte ab (abs(Val1-Val0)) aber das macht sich erst ab einer Dauer > 5.4 Sekunden bemerkbar
             //Dprintf("dt=%i State %i\n", dp->dt, dp->State+1); // Debug
//             }  // 24.10.19:  Old Position
             #ifdef USE_XFADE
               if (XFade)
                  {
                  LEDRamP = LEDRamStart;
                  lp = lp0;
                  for (uint8_t led = 0; led < p.LEDs; led++)                                                  // 20.10.19:  Initialisation led = 0 was missing ;-(
                    *(LEDRamP++) = *(lp++);
                  }
             #endif
             } // 24.10.19:  New position
          }
     else dp->dt = Duration;
     }
}

/*
 Fehler in X-Mode:                                                                                            24.10.19:
 ~~~~~~~~~~~~~~~~~
 Die Ueberblendkurve hat nicht gestimmt. Das Ueberblenden ging viel schneller als im A-Mode.
 Problem: Der Startwert LEDRamP wurde berits beim ueberblenden neu gesetzt. Eigentlich haette er erst
 am Ende der Ueberblendphase auf den endwert gesetzt werden sollen.
 Dadurch war die Ueberblendfunktion keine Gerade sondern eine abgerundete Rechteck Funktion.
 Der Fehler war eine falsch gesetzte Klammer welche wegen der falschen Einrueckung nicht aufgefallen ist.
 Achtung: Davon sind alle XPattern Effekte betroffen.

 Fehlerhaftes ueberblenden         So haette es sein sollen
          _-------                             ---------
         '                                    /
        |                                    /
        |                                   /
        |                                  /
       |                                  /
 ____-'                             _____/

*/


// Reducing the updatetime by limmiting the minimal dt:        But not faster than 20 ms = 50 Hz
// max(30,... 447 Lauflicht, 555 All  (15 channels, 200ms)
// max(20,... 479 Lauflicht, 596 All
// max(10,... 559 Lauflicht, 700 All
// max( 0,...1119 Lauflicht,1423 All
// Leerlauf   370

//-----------------------------------------------
void MobaLedLib_C::IncCP_Pattern(uint8_t TimeCnt)                                                             // 16.08.18:
//-----------------------------------------------
{
  uint16_t BitMaskCnt = pgm_read_word_near(cp+P_PATERN_T1_L + 2*TimeCnt);                                    // 23.10.18:
  cp += P_PATERN_T1_L + 2*TimeCnt + BitMaskCnt + 2;
}

