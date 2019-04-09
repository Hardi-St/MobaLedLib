#ifndef _KEYS_4017_H_
#define _KEYS_4017_H_
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


 Einlesen von Tasten ueber wenige Prozessor Pins
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Zur reduzierung der benoetigten Prozessorpins werden ein oder mehrere Zaehler ICs vom Typ 4017 eingesetzt.
 Dieser Zaehler schaltet sequenziel immer einen Ausgang gegen 5V. An diesen Ausgaengen haengen ueber Dioden
 entkoppelt die Schalter. Die Dioden sind notwendig damit mehrere Schalter gleichzeitig betaetigt werden
 koennen. Die Zaehler koennen ueber ein UND Gatter kaskadiert werden. Dabei geht allerdings ein Ausgang verloren.
 Eingelesen werden die Schalter ueber eine oder mehrere Eingaenge.

 Einlesen der Schalter im Hauptprogramm:
 Wenn die Funktion zum einlesen der Schalter im Hauptprogramm aufgeufen wird dann darf die Hauptschleife
 Nicht zu lange dauern.
 Damit man ein direktes Feedback der betaetigten Schalter erkennt sollten diese mit einer Abtastperiode
 von 100 ms gelesen werden.
 Bei einer Hauptschleifendauer von 20ms koennen auf diese Weise aber nur 5 Schaltergruppen eingelesen werden.
 => Die Schalter muessen per Interrupt gelesen werden.


 Dieses Modul besteht nur aus einer H-Datei dadurch kann man mit #define die Parameter bestimmen.
 Damit wird der Speicherverbrauch minimiert. Ausserdem ist das Programm auf diese Weise schneller.

 Die H-Datei kann unabhaengig vom Rest der MobaLedLib benutzt werden.


 Revision History:
 ~~~~~~~~~~~~~~~~~
 17.11.18:  - Started
 24.11.18:  - It doesn't matter which CTR_CHANNELS is greater

 ToDo:
 ~~~~~
 - Testen wieviel FLASH das Modul braucht
*/

#include <TimerOne.h> // The TimerOne library must be installed in addition if you got the error message "..fatal error: TimerOne.h: No such file or directory"
#include <DIO2.h>     // The library for I/O 2 functions must be installed also
                      // "Blink" without delay 400 kHz (Normal digitalWrite: 145 kHz => 2.8 times faster


// The following #defines could be changed before this modul module is included in the main moudule
#ifndef CTR_CHANNELS_1
  #define  CTR_CHANNELS_1    10       // Number of used counter channels for keyboard 1. Up to 10 if one CD4017 is used, Up to 19 if two 4017 are used, ...
#endif

#ifndef BUTTON_INP_LIST_1
  #define BUTTON_INP_LIST_1  14       // Comma separeted list of the button input pins (Example use A0-A3: 14, 15, 16, 17)
#endif

#ifndef CLK_PIN
  #define CLK_PIN            A4       // Pin number used for the 4017 clock (Example 18 = A4)
#endif

#ifndef RESET_PIN
  #define RESET_PIN          A5       // Pin number used for the 4017 reset (Example 19 = A5)
#endif


const PROGMEM uint8_t Button_Pins[] = { BUTTON_INP_LIST_1 };        // Array of pins which read the pressed buttons
#define BUTTON_INP_CNT sizeof(Button_Pins)

#define KEYS_COUNT_1           (CTR_CHANNELS_1*BUTTON_INP_CNT)
#define KEYS_ARRAY_BYTE_SIZE_1 ((KEYS_COUNT_1+7)/8)
uint8_t Keys_Array_1[KEYS_ARRAY_BYTE_SIZE_1];                       // Array which contains the result of the keyboard scan. Active keys are repersented by a 1

#ifndef CTR_CHANNELS_2
  #define CTR_CHANNELS_2 0
#endif

#define _MAX_CTR_CHANNELS (CTR_CHANNELS_1 > CTR_CHANNELS_2 ? CTR_CHANNELS_1 : CTR_CHANNELS_2)

//*** Second key matrix ***
#if CTR_CHANNELS_2 > 0 && defined BUTTON_INP_LIST_2
  #define  KEYMATRIX_2

  const PROGMEM uint8_t Button_Pins_2[] = { BUTTON_INP_LIST_2 };    // Array of pins which read the pressed buttons
  #define BUTTON_INP_CNT_2 sizeof(Button_Pins_2)

  #define KEYS_COUNT_2           (CTR_CHANNELS_2*BUTTON_INP_CNT_2)
  #define KEYS_ARRAY_BYTE_SIZE_2 ((KEYS_COUNT_2+7)/8)
  uint8_t Keys_Array_2[KEYS_ARRAY_BYTE_SIZE_2];                     // Array which contains the result of the keyboard scan. Active keys are repersented by a 1
#endif


uint8_t Counter = 0;

//------------------------------
void WriteData_to_Keys_Array_1()
//------------------------------
// Write the actual button states into the Keys_Array_1[]
{
  uint8_t Nr = Counter*BUTTON_INP_CNT;
  uint8_t Byte = Nr / 8;
  uint8_t Bit  = Nr % 8;
  for (uint8_t i = 0; ; )
    {
    uint8_t Mask = 1 << Bit;
    if (digitalRead2(pgm_read_byte_near(&Button_Pins[i])))
          Keys_Array_1[Byte] |=  Mask;
    else  Keys_Array_1[Byte] &= ~Mask;
    if (++i < BUTTON_INP_CNT)
         {
         if (Bit == 7)
              {
              Byte++;
              Bit = 0;
              }
         else Bit++;
         }
    else break; // End of the for loop
    }
}

#ifdef KEYMATRIX_2
//------------------------------
void WriteData_to_Keys_Array_2()
//------------------------------
// Write the actual button states into the Keys_Array_2[]
{
  uint8_t Nr = Counter*BUTTON_INP_CNT_2;
  uint8_t Byte = Nr / 8;
  uint8_t Bit  = Nr % 8;
  for (uint8_t i = 0; ; )
    {
    uint8_t Mask = 1 << Bit;
    if (digitalRead2(pgm_read_byte_near(&Button_Pins_2[i])))
          Keys_Array_2[Byte] |=  Mask;
    else  Keys_Array_2[Byte] &= ~Mask;
    if (++i < BUTTON_INP_CNT_2)
         {
         if (Bit == 7)
              {
              Byte++;
              Bit = 0;
              }
         else Bit++;
         }
    else break; // End of the for loop
    }
}
#endif

//--------------
void TimerInt1()      // ~6 us per button input line
//--------------
// Am Anfang der Interrupt Routine wird der CLK_PIN auf 0 gesetzt.
// Am Ende wird er wieder auf 1 gesetzt. Damit Zaehlt der 4017 weiter.
// Die Signale der Taster haben Zeit zum einschwingen bis zum naechsten Interrupt.
//
// Wenn alle Kanaele abgefragt sind, dann wird RESET_PIN und nicht der CLK_PIN
// auf 1 gesetzt.
//
// Zu Beginn des Programms steht Counter auf 0 und der Reset Pin wird aktiviert.
{
  if (Counter == 0)
       digitalWrite2(RESET_PIN, 0); // End of the reset condition
  else digitalWrite2(CLK_PIN, 0);

  if (Counter < CTR_CHANNELS_1) WriteData_to_Keys_Array_1();

  #ifdef KEYMATRIX_2
    if (Counter < CTR_CHANNELS_2) WriteData_to_Keys_Array_2();
  #endif

  if (++(Counter) >= _MAX_CTR_CHANNELS)
       {
       Counter = 0;
       digitalWrite2(RESET_PIN, 1);
       }
  else digitalWrite2(CLK_PIN, 1);
}


//--------------------
void Keys_4017_Setup()
//--------------------
{
  pinMode(CLK_PIN,    OUTPUT);
  pinMode(RESET_PIN,  OUTPUT);
  digitalWrite2(RESET_PIN, 1); // Reset
  Timer1.initialize(100000/(_MAX_CTR_CHANNELS)); // [us]   Kann aus irgend einem Grund nicht im Konstruktor aufgerufen werden. Dann stimmt Periode gar nicht ;-(
  Timer1.attachInterrupt(TimerInt1);             //        Darum ist es jetzt eine Normale Funktion und keine Klasse.
}

#endif // _KEYS_4017_H_
