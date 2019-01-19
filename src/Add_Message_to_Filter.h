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

 Add_Message_to_Filter.h
 ~~~~~~~~~~~~~~~~~~~~~~~

 This module is used to set the filters in the MCP2515 CAN controller to pass only
 the wanted messages to the arduinio in order to reduce the CPU load.

 Attention: The can bus must be initialized with MCP_STDEXT and not MCP_ANY !!!!!
*/

#ifndef _ADD_MESSAGE_TO_FILTER_H_
#define _ADD_MESSAGE_TO_FILTER_H_

#include <mcp_can.h>     // https://github.com/coryjfowler/MCP_CAN_lib
                         // Download the ZIP file from https://github.com/coryjfowler/MCP_CAN_lib
                         // Arduino IDE: Sketch / Include library / Add .ZIP library...                 Deutsche IDE: Sketch / Bibliothek einbinden / .ZIP Bibliothek hinzufuegen...
                         //              Navigate to the download directory and select the file                       Zum Download Verzeichnis werchseln und die Datei auswaehlen
                         //
                         // Attention: Disable the debug in mcp_can_dfs.h by setting the following define to 0:
                         //              #define DEBUG_MODE 0
                         //            This is important because the debug messages use a lot of RAM.
                         //            To prevent this use the F() macro in print() functions like used here.
                         //            If your program uses to much memory you get the following warning:
                         //              "Low memory available, stability problems may occur."

//------------------------------------------------------------------
void Add_Message_to_Filter(MCP_CAN &CAN, uint32_t MsgId, uint8_t Nr)                                          // 15.06.18:  Copied from _16LED_CAN.ino
//------------------------------------------------------------------
// Up to 6 messages could be filtered exactly. If more messages are used
// the filter may pass some unwanted messages.
// Attention: The can bus must be initialized with MCP_STDEXT and not MCP_ANY !!!!!
{
  static uint32_t Mask0;                    // CAN mask register 0
  static uint32_t Filt0;                    // CAN filter 0
  uint32_t Addr, Mask;
  uint8_t  Ext;
  if (MsgId & 0x80000000)  // One channel can be used for normal or extended messages. A mix is not possible
       {
       Addr = MsgId;
       Ext = 1;
       Mask = 0x1FFFFFFF;
       }
  else {
       Addr = MsgId << 16; // In Normal mode the first two bytes are used as filter
       Ext = 0;
       Mask = 0x1FFF0000;  // The lower two bytes could be used to filter the data
       }

  if (Nr == 0)
     {
     Mask0 = Mask; // Store the Mask and filter in case more than 6 messages are used
     Filt0 = Addr;
     CAN.init_Mask(1, Ext, Mask);     // Init the mask 1 also because otherwise all messages are passed by this channel if only one message is used
     for (uint8_t i = 1; i < 6; i++)
         CAN.init_Filt(i, Ext, Addr); // Init the filters also
     }

  if (Nr < 6)
       {
       uint8_t FNr;
       switch (Nr)                // The first message uses channel 0, the second channel 1. The third channel 0. All other channel 1
         {
         case 1:  FNr = 2; break;
         case 2:  FNr = 1; break;
         default: FNr = Nr;       // 0, 3, 4, 5
         }
       CAN.init_Mask(FNr < 2 ? 0 : 1, Ext, Mask);  // Init the mask
       CAN.init_Filt(FNr, Ext, Addr);              // Init the filter
       }
  else { // More than 6 messages used => Wrong messages may pass the filter
       uint32_t DifBits = Filt0 ^ Addr; // Get the different bits
       Mask0 &= ~DifBits;               // Clear all bits which are different
       CAN.init_Mask(0, Ext, Mask0);    // Init the mask
       }
}

#endif // _ADD_MESSAGE_TO_FILTER_H_
