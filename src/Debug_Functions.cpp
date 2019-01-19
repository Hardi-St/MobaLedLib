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


 Debug_Functions.h
 ~~~~~~~~~~~~~~~~~
 This file contains functions for debugging the library


*/

#include "MobaLedLib.h"

#ifdef _PRINT_DEBUG_MESSAGES
//---------------------------------------------------------
static void Print_Pattern_Comment(uint8_t Type, uint8_t pt)
//---------------------------------------------------------
// Typ  LED  NStr InNr LEDs Val0 Val1 Off  Mode Time 0    Time 1    argc argv
// 11   0    194  23   1    0    255  0    0    244  1    244  1    1    1
{
  uint8_t TimeCnt = Type - pt + 1;
  Dprintf("%cPatternT%-2i", pt == PATTERNT1_T?' ':'A', TimeCnt);
  Dprintf("LED  NStr InNr LEDs Val0 Val1 Off  Mode ");
  for (uint8_t i = 0; i < TimeCnt; i++)
      Dprintf("Time_%-3i  ", i);
  Dprintf("PCnt Pattern...");
}
#endif

//--------------------------------------------
void MobaLedLib_C::Print_Comment(uint8_t Type)
//--------------------------------------------
{
#ifdef _PRINT_DEBUG_MESSAGES
  uint8_t pt = Is_Pattern(Type);
  Dprintf("Typ ");
  switch (Type)
    {
  #ifdef _USE_SEP_CONST
    case CONST_T:             Dprintf("Const");             break;
  #endif
    case END_T:               Dprintf("End");               break;
    case HOUSE_T:             Dprintf("House");             break;
    case FIRE_T:              Dprintf("Fire");              break;
  #ifdef _NEW_ROOM_COL
    case SET_COLTAB_T:        Dprintf("Set_ColTab");        break;
  #endif
    case LOGIC_T:             Dprintf("Logic");             break;
    case NEW_HSV_GROUP_T:     Dprintf("New_HSV_Group");     break;
    case NEW_LOCAL_VAR_T:     Dprintf("New_Local_Var");     break;                                            // 07.11.18:
    case USE_GLOBALVAR_T:     Dprintf("Use_GlobalVar");     break;                                            // 15.11.18:
    case INCH_TO_TMPVAR_T:    Dprintf("InCh_to_TmpVar");    break;
    case BIN_INCH_TO_TMPVAR_T:Dprintf("Bin_InCh_to_TmpVar");break;                                            // 18.01.19:
    case RANDOM_T:            Dprintf("Random");            break;
    case RANDMUX_T:           Dprintf("RandMux");           break;
    case WELDING_T:           Dprintf("Welding");           break;
    case COPYLED_T:           Dprintf("CopyLED");           break;
    case SCHEDULE_T:          Dprintf("Schedule");          break;
    case COUNTER_T:           Dprintf("Counter");           break;
    default: if (pt)
                  Print_Pattern_Comment(Type, pt);
             else Dprintf("Unknown");
    }
  Dprintf("\n");                                                                                              // 07.11.18:  Remove the "\n" from all lines above and added this line
#endif
}

//-------------------------------
void MobaLedLib_C::Print_Config()
//-------------------------------
// Typ  LED  NStr InNr LEDs Val0 Val1 Off  Mode Time_0    Time_1    Argc Argv
// 11   0    194  23   1    0    255  0    0    244  1    244  1    1    1
{
#ifdef _PRINT_DEBUG_MESSAGES
  Dprintf("Config:");
  const uint8_t *sp = Config;
  for (const uint8_t *p = Config; ; p++)
    {
    if (p == sp) // Start of a block
       {
       Dprintf("\n");
       cp = p;
       uint8_t Type = pgm_read_byte_near(p);
       Print_Comment(Type);
       if (Type == END_T) break;
       cp++;
       Inc_cp(Type);
       sp = cp; // Set the next start pointer
       Dprintf("          ");
       }
    Dprintf("%-5i", pgm_read_byte_near(p));
    }
  Dprintf("\n");
#endif
}


