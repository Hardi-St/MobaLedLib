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


 Read commands from the PC using the serial port
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This file could be used to test different colors and brightness values of the LEDs.
 To use it this file must be included with
    #include "Serial_Inputs.h"
 after the "MobaLedLib_Create(leds);" line and the funchtion "Proc_Serial_Input()"
 must be called in the "loop()".



   Input possibilities (Case insensitive):
   ---------------------------------------
   L10    => LED Nr
   L+     => Next LED
   L-     => Previous LED

   ENTER  => Repeat last input (Useful with +, -, T or impulse)

   X      => Exit test mode

   h45    => Hue = 45
   v100   => Value = 100
   H20    => Hue = 20
   H+4    => Hue+=4
   H-     => Hue--
   H+     => Hue++

   R30    => Red LED value
   G+     => Increment Green value
   B-3    => Decrement green value by 3

   P50    => ImPulse with a value of 50 (Sound module tests)
   i100   => Impulse length [2ms]  i100 = 200 ms
   M2     => Impulse mask 1 = Red, 2 = Green, 4 = Blue, 7 = white

   A      => All LEDs on (255)
   C      => All LEDs off

   e3     => Enable  input 3
   d3     => Disable input 3
   t3     => Toggle  input 3

   Sevaral commands could be written in one line:
     r12g76b12

   Bei dem Arduino Terminal Fenster muss der Zeilenumbruch (CR) aktiviert werden ("Neue Zeile" geht auch)

*/
#ifndef __SERIAL_INPUTS_H__
#define __SERIAL_INPUTS_H__

  #include <stdarg.h>
  #include <Wstring.h>


  #ifndef _PRINT_DEBUG_MESSAGES
    // F() makro saves dynamic memory
    #undef  Dprintf
    #define Dprintf(Format, ...) Dprintf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

    //-------------------------------------------------------
    void Dprintf_proc(const __FlashStringHelper *format, ...)  // uses 812 byte FLASH and 173 Bytes RAM if enabled ;-(
    //-------------------------------------------------------
    {
      char buf[50];
      va_list ap;
      va_start(ap, format);
      #ifdef __AVR__
         vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
      #else
         vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
      #endif

      va_end(ap);
      Serial.print(buf);
    }
  #endif // _PRINT_DEBUG_MESSAGES

  // Neue verarbeitung der seriellen Daten
  // - Daten werden erst ausgewertet wenn ENTER kommt
  //
  // - Unterschiedlich Einstellungen des Terminal Programms
  //    - Kein Zeilenende        => Nichts
  //    - Neue Zeile             => \n
  //    - CR                     => \r
  //    - Sowohl NL als auch CR  => \r\n
  //

  #ifndef SERIAL_INPUT_BUFFER_SIZE
    #define SERIAL_INPUT_BUFFER_SIZE 15
  #endif

  char Line[SERIAL_INPUT_BUFFER_SIZE] = "";
  char LastChar = 255;

//::::::::::::::::::
class Serial_Input_C
//::::::::::::::::::
{
  private:
       uint8_t  h           = 0;
       uint8_t  s           = 255;
       uint8_t  v           = 128;
       uint8_t  L           = 0;
       uint8_t  ImpLen      = 100; // Impulslength [2ms]
       uint8_t  ImpVal      = 255; // Impuls value
       uint8_t  ImpMsk      = 7;   // Impuls mask 1 = Red, 2 = Green, 4 = Blue, 7 = White
       uint8_t  EndLoop     = 0;
       uint8_t  SwNr        = 0;
       uint8_t* Var_Ptr     = NULL;
       char *LinePtr;

  //--------------------
  void Clear_SerialInp()
  //--------------------
  {
    do {
       delay(10);
       if (Serial.available()) Serial.read();
       } while (Serial.available());
  }

  //---------------------------
  void Update_Val(uint8_t &val)
  //---------------------------
  {
    if (&val == NULL) return;
    //Dprintf("Old %i ", val); // Debug
    char *NextChar_p = LinePtr+1;
    if ((*NextChar_p >= '0' && *NextChar_p <= '9') ||  (*NextChar_p == '+' || *NextChar_p == '-'))
         {
         if (*NextChar_p == '+' || *NextChar_p == '-')
              {
              int d;
              d = atoi(NextChar_p);
              if (d==0) d = (*NextChar_p == '-') ? -1:1;
              d += val;
              if (d > 255) d = 255;
              if (d < 0)   d = 0;
              val = d;
              }
         else val = atoi(NextChar_p);
         }
   //Dprintf(" => %i\n", val); // Debug
  }

#ifndef SERIAL_INP_WITHOUT_MOBALEDLIB                                                                         // 22.06.19:
  //-------------------------
  void Update_Sw(int8_t Mode)
  //-------------------------
  {
     Update_Val(SwNr);
     if (SwNr/4 < _INP_STRUCT_ARRAY_SIZE)
        {
        switch (Mode)
          {
          case 'e': MobaLedLib.Set_Input(SwNr, 1); break;
          case 'd': MobaLedLib.Set_Input(SwNr, 0); break;
          case 't': MobaLedLib.Set_Input(SwNr, MobaLedLib.Get_Input(SwNr)>0?0:1); break;
          }
        Dprintf("SwNr(%i)=%i\n", SwNr, Inp_Is_On(MobaLedLib.Get_Input(SwNr)));
        }
  }
#endif // SERIAL_INP_WITHOUT_MOBALEDLIB

  //--------------
  void Proc_Line()
  //--------------
  {
    // Dprintf("Proc_Line '%s'\n", Line); // Debug
    uint8_t ProcTyp = 0;

    for (LinePtr = Line; *LinePtr != '\0'; LinePtr++)
        {
        switch (tolower(*LinePtr))
            {
            case 'h': Update_Val(h);         ProcTyp = 1; break;
            case 's': Update_Val(s);         ProcTyp = 1; break;
            case 'v': Update_Val(v);         ProcTyp = 1; break;
            case 'l': Update_Val(L);         ProcTyp = 1; break;
            case 'r': Update_Val(leds[L].r); ProcTyp = 2; break;
            case 'g': Update_Val(leds[L].g); ProcTyp = 2; break;
            case 'b': Update_Val(leds[L].b); ProcTyp = 2; break;
            case 'i': Update_Val(ImpLen);    ProcTyp = 3; break;
            case 'p': Update_Val(ImpVal);    ProcTyp = 3; break;
            case 'm': Update_Val(ImpMsk);    ProcTyp = 3; break;
            case 'e': // Enable input
            case 'd': // Disable input
         #ifndef SERIAL_INP_WITHOUT_MOBALEDLIB                                                                // 22.06.19:
            case 't': EndLoop = 1; Update_Sw(*LinePtr); Clear_SerialInp(); break;
         #endif
            case 'x': EndLoop = 1;                        break;
            case 'a': memset(leds, 255, NUM_LEDS*sizeof(CRGB)); // Turn on ALL LEDs
                      FastLED.show();
                      ProcTyp = 0;
                      break;
            case 'c': memset(leds, 0, NUM_LEDS*sizeof(CRGB)); // Turn off ALL LEDs
                      FastLED.show();
                      ProcTyp = 0;
                      break;
            }
        }

    if (ProcTyp)
       {
       switch (ProcTyp)
           {
           case 1: leds[L].setHSV(h, s, v); break;
           case 2: {
                   CHSV hsv = rgb2hsv_approximate(leds[L]);
                   h = hsv.h;
                   s = hsv.s;
                   v = hsv.v;
                   } break;
           case 3: if (ImpMsk & 1) leds[L].r = ImpVal;
                   if (ImpMsk & 2) leds[L].g = ImpVal;
                   if (ImpMsk & 4) leds[L].b = ImpVal;
                   FastLED.show();
                   delay(2*ImpLen);
                   leds[L].r = leds[L].g = leds[L].b = 0;
                   Dprintf("imPuls=%i i=%i ms Msk=%i ", ImpVal, 2*ImpLen, ImpMsk & 0x07);
                   break;
           }
       Dprintf("LED:%3i: h=%3i s=%3i v=%3i (%3i %3i %3i)\n", L, h, s, v, leds[L].r, leds[L].g, leds[L].b);
       FastLED.show();
       }
  }

  //----------------------
  public: Serial_Input_C()  // Constructor
  //----------------------
  {
  }

  //-------------------------
  public: uint8_t Proc_Char()
  //-------------------------
  {
    Dprintf("Started Proc_Serial_Input %i LEDs\n", NUM_LEDS); // Debug
    memset(leds, 0, NUM_LEDS*sizeof(CRGB));                                                                   // 20.10.18:
    do {
       if (Serial.available() > 0)
          {
          char c = Serial.read();
          // Alle Zeichen werden in Line[] gespeichert bis \n oder \r kommt
          // Dann wird die Zeile bearbeitet
          switch (c)
              {
              case '\n': if (LastChar == '\r') break; // Wenn "Sowohl NL als auch CR" eingestellt ist, dann kommt "\r\n" => Das '\n' wird ignoriert
                         // No break if the condition is false
              case '\r': Proc_Line();
                         break;
              default:   if (LastChar == '\n' || LastChar == '\r') *Line = '\0'; // Clear the line
                         if (strlen(Line) < sizeof(Line) - 1)
                            {
                            char *p = &Line[strlen(Line)]; // Store the input line
                            *(p++) = c;
                            *p = '\0';
                            }
              }
          LastChar = c;
          }
       } while (!EndLoop);

    Dprintf("Ende Proc_Serial_Input\n");  // Debug
    Clear_SerialInp();
    memset(leds, 0, NUM_LEDS*sizeof(CRGB));
    return 1;
  }

}; // End of class Serial_Input_C

//-------------------------------
uint8_t Proc_Serial_Input(char c)
//-------------------------------
{
  if (c == 0 && Serial.available() == 0)
       return 0;
  else {
       Serial_Input_C Serial_Input;
       return Serial_Input.Proc_Char();
       }
}


#endif // __SERIAL_INPUTS_H__
