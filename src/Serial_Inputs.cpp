#include "MobaLedLib.h"

#ifdef  _READ_SERIAL_INPUTS
/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018  Hardi Stengelin: MobaLedLib@gmx.de

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


 Read commands from the PC using the serial port
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

   Eingabe Moeglichkeiten:
   45h    => Hue = 45
   10s    => Saturation = 10
   100v   => Value = 100
   h20    => Hue = 20
   +4h    => Hue+=4
   -h     => Hue--
   h+     => Hue++
   H      => Hue++
   h      => Hue--
   ENTER  => Letzte Eingabe wiederholen (Sinnvoll bei + oder -)
   +      => Letzte Wert vergroessern (Bestaetigen mit mehrfachem ENTER)
   -      => Letzte Wert verkleinern
   L10    => LED Nr
   L+     => Naechste LED

   3e     => Enable  input 3
   3d     => Disable input 3
   3t     => Toggle  input 3
   t      => Toggle last input


   Zur Eingabe kann auch Putty verwendet werden. Hier muss am Ende kein Enter betaetigtigt werden.
   Bei dem Arduino Terminal Fenster muss der Zeilenumbruch (CR) aktiviert werden ("Neue Zeile" geht auch, aber dann wird eine Leerzeile in der Ausgabe generiert)

   Weitere Eingaben:
   x:     Exit

*/

  //----------------------------------------------------------------
  void Update_Val(uint8_t &val, int8_t Dir, char *Line, uint8_t* &p)
  //----------------------------------------------------------------
  {
    if (&val == NULL) return;
    if ((*Line >= '0' && *Line <= '9') ||  (*Line == '+' || *Line == '-'))
         {
         if (*Line == '+' || *Line == '-')
              {
              int d;
              d = atoi(Line);
              if (d==0) d = (*Line == '-') ? -1:1;
              d += val;
              if (d > 255) d = 255;
              if (d < 0)   d = 0;
              val = d;
              }
         else val = atoi(Line);
         }
    else {
         if((Dir > 0 && val < 255) || (Dir < 0 && val > 0)) val += Dir;
         }
    p = &val;
  }

  //----------------------------------------------------------------
  void MobaLedLib_C::Update_Sw(int8_t Mode, char *Line, uint8_t* &p)
  //----------------------------------------------------------------
  {
     Update_Val(SwNr, 0, Line, p);
     if (SwNr/4 < _INP_STRUCT_ARRAY_SIZE)
        {
        switch (Mode)
          {
          case 'e': Set_Input(SwNr, 1); break;
          case 'd': Set_Input(SwNr, 0); break;
          case 't': Set_Input(SwNr, Get_Input(SwNr)>0?0:1); break;
          }
        Dprintf("SwNr(%i)=%i\n", SwNr, Inp_Is_On(Get_Input(SwNr)));
        }
  }

  //--------------------
  void Clear_SerialInp()
  //--------------------
  {
    do {
       delay(10);
       if (Serial.available()) Serial.read();
       } while (Serial.available());
  }

  //------------------------------------
  void MobaLedLib_C::Proc_Serial_Input()
  //------------------------------------
  {
    if (Serial.available() == 0) return ;
    char c = '\0', Line[11] = "";
    uint8_t h = 0, s = 255, v = 128, L = 0, *p = NULL;
    uint8_t Proc = 0, LastProc = 255;
    memset(leds, 0, Num_Leds*sizeof(CRGB));                                                                   // 20.10.18:
    do
      {
      if (Serial.available() > 0)
         {
         c = Serial.read();

         switch (c)
            {
            case  'h': Update_Val(h,         -1, Line, p); Proc = 1; break;
            case  'H': Update_Val(h,         +1, Line, p); Proc = 1; break;
            case  's': Update_Val(s,         -1, Line, p); Proc = 1; break;
            case  'S': Update_Val(s,         +1, Line, p); Proc = 1; break;
            case  'v': Update_Val(v,         -1, Line, p); Proc = 1; break;
            case  'V': Update_Val(v,         +1, Line, p); Proc = 1; break;
            case  'l': Update_Val(L,         -1, Line, p); Proc = 1; break;
            case  'L': Update_Val(L,         +1, Line, p); Proc = 1; break;
            case  'r': Update_Val(leds[L].r, -1, Line, p); Proc = 2; break;
            case  'R': Update_Val(leds[L].r, -1, Line, p); Proc = 2; break;
            case  'g': Update_Val(leds[L].g, -1, Line, p); Proc = 2; break;
            case  'G': Update_Val(leds[L].g, -1, Line, p); Proc = 2; break;
            case  'b': Update_Val(leds[L].b, -1, Line, p); Proc = 2; break;
            case  'B': Update_Val(leds[L].b, -1, Line, p); Proc = 2; break;
            case  'e': // Enable input
            case  'd': // Disable input
            case  't': Update_Sw (    c, Line, p); Clear_SerialInp(); c = 0; break;
            case '\n':
            case '\r': Update_Val(*p,  0, Line, p); Proc = LastProc; break;
            case 0xFFFF: // Is generated if laptop is started (from suspend) ?!?
            case  'x': c = 0;
                       Clear_SerialInp();
                       memset(leds, 0, Num_Leds*sizeof(CRGB));
                       Dprintf("End\n");
                       break;
            case  'a': Clear_SerialInp();                   // Turn on ALL LEDs
                       memset(leds, 255, Num_Leds*sizeof(CRGB));
                       FastLED.show();
                       Proc = 0;
                       break;
            case  'c': memset(leds, 0, Num_Leds*sizeof(CRGB)); // Turn off ALL LEDs                           // 20.10.18:
                       *Line = '\0'; // delete the line
                       FastLED.show();
                       Proc = 0;
                       break;

            default : if (Proc) *Line = '\0'; // delete the line
                      Proc = 0;
            }
         if (strlen(Line) < sizeof(Line) - 1)
            {
            Dprintf("%c", c);
            char *p = &Line[strlen(Line)];
            *(p++) = c;
            *p = '\0';
            }

         if (Proc)
            {
            if (Proc == 1)
               {
               //memset(leds, 0, Num_Leds*sizeof(CRGB));                                                      // 20.10.18:  Disabled
               leds[L].setHSV(h, s, v);
               }
            if (Proc == 2)
               {
               CHSV hsv = rgb2hsv_approximate(leds[L]);
               h = hsv.h;
               s = hsv.s;
               v = hsv.v;
               }
            Dprintf("LED:%3i: h=%3i s=%3i v=%3i (%3i %3i %3i)\n", L, h, s, v, leds[L].r, leds[L].g, leds[L].b);
            FastLED.show();
            LastProc = Proc;
            }
         }
      } while (c);

  }
#endif // _READ_SERIAL_INPUTS


