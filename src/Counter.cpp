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


 Counter Function
 ~~~~~~~~~~~~~~~~

 Parameter:
   Counter(InNr, Enable, TimeOut, Mode, Outputs...)

 RAM:
   Timeout  2 Byte
   Zaehler   1 Byte

 Entwurf:
 - Trigger mit Taster
 - Steuert mehrere Ausgaenge
 - Binaer oder SingleBit Ausgang
 - Ausgaenge als Variable Argumente. Wenn nur ein Ausgang angegeben wird, dann wird ein zweiter virtueller Ausgang generiert
 - Enable Eingang
 - Timeout (0 = Kein Timeout (bzw. <15 ms))   [16ms]
 - Flags
   - CF_INV_INPUT       Invert Input
   - CF_INV_ENABLE      Input Enable
   - CF_BINARY
   - CF_RESET_LONG      Taste Lang = Reset
   - CF_UP_DOWN         Ein RS-FlipFlop kann mit CM_UP_DOWN ohne CF_ROTATE gemacht werden
   - CF_ROTATE
   - CF_PINGPONG
   - CF_SKIP0
   -
 - Nur Flags implementieren Modes koennen als Makros aus dem Flags zusammen gebastelt werden

 - Memory usage > 608 Bytes Flash (2%)
*/

#include "MobaLedLib.h"

#define NEW_MODES                                                                                             // 07.11.18:

//-------------------------------
void MobaLedLib_C::Proc_Counter()
//-------------------------------
{
#if _USE_STORE_STATUS                                                                                         // 01.05.20:
    ProcCounterId++;
#endif
  CALCULATE_t4w; // Calculate the local variable t4w if _USE_LOCAL_TX is defined                              // 22.11.18:
  Counter_T* dp = (Counter_T*)rp; // Set the time pointer to the reserved RAM
  rp += sizeof(Counter_T);
  uint16_t ModeL = pgm_read_byte_near(cp+P_COUNT_MODE_L);
#ifdef NEW_MODES
  uint16_t ModeH = pgm_read_byte_near(cp+P_COUNT_MODE_H);
#else
  #define ModeH ModeL
#endif
  uint8_t OldCounter = dp->Counter;
  uint8_t DestCnt;
#ifdef NEW_MODES
  if (ModeH & (_CF_NO_LEDOUTP>>8))                                                                                  // 07.11.18:
       DestCnt = pgm_read_byte_near(cp+P_COUNT_FIRST_DEST);  // If the LED output is disabled the first DestVar contains the destination count (Counter => 0 .. n-1)
  else
#endif
       DestCnt = pgm_read_byte_near(cp+P_COUNT_DEST_COUNT);
  uint8_t MaxCnt;
  if (ModeL & CF_BINARY)             // DestCnt   1<<DestCnt)      Result
       MaxCnt = (1<<DestCnt) - 1;    // 0         2^0 = 1          0
  else MaxCnt = DestCnt - 1;         // 1         2^1 = 2          1
                                     // 2         2^2 = 4          3

  uint8_t Inp     = Get_Input(pgm_read_byte_near(cp+P_COUNT_INP));
  uint8_t Enable  = Get_Input(pgm_read_byte_near(cp+P_COUNT_ENABLE));
  if (ModeL & CF_INV_INPUT)  Inp    = Invert_Inp(Inp);    // Invert the input
  if (ModeL & CF_INV_ENABLE) Enable = Invert_Inp(Enable); // Invert the enable

  if (Initialize)
     {
     OldCounter = 255; // Force initialisation
     //Dprintf("Mode:    %02X %02X\n", ModeH, ModeL);                          // Achtung: Wenn zu viele Dprintf im Constructor verwendet werden, dann haengt sich der Arduino evtl. auf
     //Dprintf("MaxCnt:  %i\n",   MaxCnt);
     //Dprintf("InpCh:   %i\n",   pgm_read_byte_near(cp+P_COUNT_INP));
     //Dprintf("EnabCh:  %i\n",   pgm_read_byte_near(cp+P_COUNT_ENABLE));
     //Dprintf("Timeout: %i\n",   pgm_read_word_near(cp+P_COUNT_TIMEOUT_L));
     //Dprintf("DestCnt: %i\n",   DestCnt);
     //Dprintf("Counter: %i\n",   dp->Counter);
     if ((ModeL & CF_UP_DOWN) == 0 && !Inp_Is_On(Enable)) Dprintf("Achtung: Enable = 0\n");
#if _USE_STORE_STATUS                                                                                         // 01.05.20:
     Do_Callback(CT_COUNTER_INITIAL, ProcCounterId, 0, &dp->Counter);
     //Dprintf("Counter: %i %i %i\n", ProcCounterId, Inp, dp->Counter);
#endif
  }

  if ((ModeL & CF_UP_DOWN) == 0 && !Inp_Is_On(Enable)) // Enabled ?
       dp->Counter = 0;
  else { // *** Process the Input ***
       switch (Inp)
         {
         case INP_TURNED_ON: // Button pressed
                             if (ModeL & CF_PINGPONG)
                                  { // Ping Pong Mode
                                  if      (dp->Counter == MaxCnt)                            dp->Counter = (MaxCnt - 1) | 0x80; // Switch to reverse
                                  else if (((ModeL & CF_SKIP0) == 0) && dp->Counter == 0x80) dp->Counter = 1;                   // Skip0 disabled => 0->1
                                  else if (((ModeL & CF_SKIP0) != 0) && dp->Counter == 0x81) dp->Counter = 2;                   // Skip0 enabled  => 1->2
                                  else if (dp->Counter &  0x80)                              dp->Counter--;                     // Reverse direction, but not the lower end
                                  else                                                       dp->Counter++;                     // Normal direction, but not upper end
                                  }
                             else { // Not Ping Pong
#ifdef NEW_MODES
                                  if (ModeH & (CF_RANDOM>>8))                                                 // 07.11.18:
                                       {
                                       if (ModeL & CF_SKIP0)
                                            dp->Counter = random8(1, MaxCnt+1);
                                       else dp->Counter = random8(0, MaxCnt+1);
                                       OldCounter = dp->Counter+1; // To trigger the update below             // 15.02.19:
                                       }
                                  else
#endif
                                       { // Normal
                                       if (dp->Counter == MaxCnt)
                                            {
                                            if      ((ModeL & _CF_ROT_SKIP0) == _CF_ROT_SKIP0)    dp->Counter = 1; // Rotate and start with 1
                                            else if ((ModeL & _CF_ROT_SKIP0) ==  CF_ROTATE)       dp->Counter = 0; // Rotate and start with 0
                                            }
                                       else dp->Counter++;
                                       }
                                  }
                             dp->Last_t = t4w; // Reset timeout
                             break;
         case INP_ON:        // Button hold
                             if ((ModeL & CF_RESET_LONG) && t4w - dp->Last_t >= 1500/16)
                                dp->Counter = 0;
                             break;
         }
       if (ModeL & CF_UP_DOWN) // Up Down Counter
          {
          if (Enable == INP_TURNED_ON) // Enable Button pressed => Count Down
             {
             //Dprintf("Count Down %i => ", dp->Counter);
             if   ( (((ModeL & _CF_ROT_SKIP0) == CF_ROTATE)    && dp->Counter == 0) ||
                    (((ModeL & _CF_ROT_SKIP0) ==_CF_ROT_SKIP0) && dp->Counter <= 1))
                  dp->Counter = MaxCnt;
             else if (dp->Counter >  0) dp->Counter--;
             dp->Last_t = t4w; // Reset timeout
             //Dprintf("%i\n", dp->Counter);
             }
          }

       // Check Timeout
       uint16_t Timeout = pgm_read_word_near(cp+P_COUNT_TIMEOUT_L);
       if (Timeout && dp->Counter != 0 && t4w - dp->Last_t >= Timeout) dp->Counter = 0;
       }

  if (OldCounter != dp->Counter)
     {
     uint8_t Clean_Counter = dp->Counter;            // Counter without reverse flag
     if (ModeL & CF_PINGPONG) Clean_Counter &= 0x7F; // Remove the reverse flag
#if _USE_STORE_STATUS                                                                                         // 01.05.20:
     Do_Callback(CT_COUNTER_CHANGED, ProcCounterId, OldCounter, &dp->Counter);
     //Dprintf("Proc_Counter: %i => %i: ", OldCounter, Clean_Counter);
#endif

#ifdef NEW_MODES
     if (ModeH & (CF_LOCAL_VAR>>8))                                                                           // 07.11.18:
          {
          if (ActualVar_p)
             {
             ActualVar_p->Val     = Clean_Counter;
             ActualVar_p->Changed = 1;
             //Dprintf("LocalVar = %i\n", ActualVar_p->Val); // Debug
             }
          }
     if ((ModeH & (_CF_NO_LEDOUTP>>8)) == 0)                                                                  // 07.11.18:  New flag
#endif
          {
          const uint8_t  *ip = cp+P_COUNT_FIRST_DEST;

          uint8_t e = pgm_read_byte_near(cp+P_COUNT_DEST_COUNT);
          uint32_t Mask;
          if (ModeL & CF_BINARY) Mask = 1<<(DestCnt-1);                                                       // 05.01.19: Old: = 1;
          ip += e-1;
          for (uint8_t i = e-1; i != 255; i--, ip--)
               {
               uint8_t InpNr = pgm_read_byte_near(ip);
               //Dprintf("Counter Inp[%i]=%i\n", InpNr, Get_Input(InpNr));
               #pragma GCC diagnostic push                                                                    // 17.11.20:  Disable compiler warning
               #pragma GCC diagnostic ignored "-Wmaybe-uninitialized"
               if (ModeL & CF_BINARY)
                    { Set_Input(InpNr, Clean_Counter & Mask); Mask>>=1;}  // Enable binary pattern
               else Set_Input(InpNr, i == Clean_Counter);                 // Enable only one
               #pragma GCC diagnostic pop
               //Dprintf("=>%i (M:%i)", Get_Input(InpNr), Mask);
               }
          //Dprintf("\n");
          }
     }
}

//--------------------------------
void MobaLedLib_C::IncCP_Counter()
//--------------------------------
{
  cp += P_COUNT_FIRST_DEST + pgm_read_byte_near(cp+P_COUNT_DEST_COUNT);
}

