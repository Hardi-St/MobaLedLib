#include "Arduino.h"
#include "pins_arduino.h"

#include <TimerOne.h>

#include "LEDs.h"


/*
 Charliplexing LEDs
 Pins A2, A3, A4   Port C


 LED A4 A3 A2 Color   Description
 ~~~ ~~ ~~ ~~ ~~~~~~  ~~~~~~~~~~~
  1  +     -  Blue    Reset as IO Pin
  2  -     +  White   Normal Reset Function
  3     +  -  Green   Heart Beat
  4  +  -     Red     Error
  5     -  +  Yellow  Prog.
  6  -  +     Orange

 Achtung: Fehler in Platine Version vom 14.06.19:
 Die LEDs der Relais sind gedreht eingezeichnet. Die "+" Beschriftung auf der Platine  ist aber richtig.
 Ebenso so passt die Mux LED "+" Beschriftung auf der Platine nicht zum Schaltbild.
 => ToDo: Im Schaltplan korrigieren. Platinenlayout bleibt gleich.
*/


#define LED_CNT       6

#define MAX_BRIGHT    (BRIGHT_LEVELS-1)

volatile uint8_t LED_Bright[LED_CNT]        = { 0, 0,   0,   0,   0,   0}; // Full brightness if >= MAX_BRIGHT
         uint8_t Max_LED_Brightnes[LED_CNT] = { 4, 2, 255, 255, 255, 255}; // Correct the brightnes of the LEDs

                           // 76543210
                           // ---XXX--
uint8_t LED_PRT[LED_CNT] = { B00010000,  // 1  Blue    Reset as IO Pin
                             B00000100,  // 2  White   Normal Reset Function
                             B00001000,  // 3  Green   Heart Beat
                             B00010000,  // 4  Red     Error
                             B00000100,  // 5  Yellow  Prog.
                             B00001000   // 6  Orange
                           };
                          // 76543210
                          // ---XXX--
uint8_t LED_DDR[LED_CNT] = { B00010100,  // 1  Blue    Reset as IO Pin
                             B00010100,  // 2  White   Normal Reset Function
                             B00001100,  // 3  Green   Heart Beat
                             B00011000,  // 4  Red     Error
                             B00001100,  // 5  Yellow  Prog.
                             B00011000   // 6  Orange
                           };


#define LED_MASK       B00011100

//-----------------------------
inline void Act_LED(uint8_t Nr)
//-----------------------------
// ~1 us
{
  if (Nr < LED_CNT) // This check is removed by the compiler if it's not used necessary if the function is never called with Nr >= LED_CNT
     {
     PORTC = (PORTC & ~LED_MASK) | LED_PRT[Nr];
     DDRC  = (DDRC  & ~LED_MASK) | LED_DDR[Nr];
     }
}

//------------------------
inline void Disable_LEDs()
//------------------------
{
  DDRC  = (DDRC  & ~LED_MASK);
  PORTC = (PORTC & ~LED_MASK);
}

//--------------------
void Test_Speed_LEDs()
//--------------------
{
 while (1)
    {
    uint32_t Start = millis();
    for (uint16_t t = 0; t < 1000; t++)
        {
        for (uint8_t i = 0; i < LED_CNT; i++)
            {
            Act_LED(i); // ~1 us
            //delay(1);
            }
        }
    Serial.print("Duration ");
    Serial.println(millis()-Start); // => Result 6-7ms =>
    }
}

/*
 Der Timer Interrupt wird alle 100us aufgerufen

 Fuer jede LED wird die Helligkeit in einem Array abgelegt.

 Die LEDs sollten mit mindestens 20ms aktualisiert werden sonst flimmern sie.
 Es sind 32 Helligkeitswerte moeglich.
 32*6*100us = 19.2ms

 Der LED Index wird nach 32 Interrupt aufrufen erhoeht
*/
#include "LED_Polarity.h"

#if LED_POLARITY_NORMAL // Normal LED polarity (+ on the left side) //getestet von Dominik und Okay fuer neue Version von Platine 2020-03-29
   #define LED_RESET_AS_IO  1
   #define LED_NORM_RESETF  0
   #define LED_HEART_BEAT   4
   #define LED_ERROR        5
   #define LED_PROG         2
   #define LED_RES          3
#else // Wrong LED polarity (+ on the right side. Unfortunately the first PCB (14.06.19) have a the + on the wrong side)
   #define LED_RESET_AS_IO  0
   #define LED_NORM_RESETF  1
   #define LED_HEART_BEAT   2
   #define LED_ERROR        3
   #define LED_PROG         4
   #define LED_RES          5
#endif






//----------------------------------------------
void Set_LED(uint8_t LED_Nr, uint8_t brightness)
//----------------------------------------------
{
  if (LED_Nr < LED_CNT)
     LED_Bright[LED_Nr] = brightness;
}


void Set_LED_Reset_as_IO(uint8_t on) { Set_LED(LED_RESET_AS_IO, on?Max_LED_Brightnes[LED_RESET_AS_IO] : 0); }
void Set_LED_Norm_ResetF(uint8_t on) { Set_LED(LED_NORM_RESETF, on?Max_LED_Brightnes[LED_NORM_RESETF] : 0); }
void Set_LED_Heart_Beat (uint8_t on) { Set_LED(LED_HEART_BEAT , on?Max_LED_Brightnes[LED_HEART_BEAT ] : 0); }
void Set_LED_Error      (uint8_t on) { Set_LED(LED_ERROR      , on?Max_LED_Brightnes[LED_ERROR      ] : 0); }
void Set_LED_Prog       (uint8_t on) { Set_LED(LED_PROG       , on?Max_LED_Brightnes[LED_PROG       ] : 0); }
void Set_LED_Read       (uint8_t on) { Set_LED(LED_RES        , on?Max_LED_Brightnes[LED_RES        ] : 0); }

uint8_t Get_LED_Reset_as_IO() { return LED_Bright[LED_RESET_AS_IO]; }
uint8_t Get_LED_Norm_ResetF() { return LED_Bright[LED_NORM_RESETF]; }
uint8_t Get_LED_Heart_Beat () { return LED_Bright[LED_HEART_BEAT ]; }
uint8_t Get_LED_Error      () { return LED_Bright[LED_ERROR      ]; }
uint8_t Get_LED_Prog       () { return LED_Bright[LED_PROG       ]; }
uint8_t Get_LED_Read       () { return LED_Bright[LED_RES        ]; }







//--------------------------------------
void Set_LED_Val_Heart_Beat(int16_t Val)
//--------------------------------------
{ if (Val >= 0)
       Set_LED(LED_HEART_BEAT, Val);
  else Set_LED(LED_HEART_BEAT, 0);
}

//----------------------
void Upd_LED_Heartbeat()
//----------------------
{
  static int16_t hbval = 0;
  static int8_t hbdelta = 1;
  static unsigned long last_time = 0;
  unsigned long now = millis();
  if ((now - last_time) >= 40)
     {
     last_time = now;
     if (hbval >= BRIGHT_LEVELS || hbval < -2) hbdelta = -hbdelta; // Change direction
     hbval += hbdelta;
     Set_LED_Val_Heart_Beat(hbval);
     }
}


volatile uint8_t INT_Cnt = LED_CNT-1;
volatile uint8_t LED_ix = 0;

void (*Add_Int_Proc_Ptr)() = NULL;

//----------------
void LED_IntProc()
//----------------
// Is called every 100 us
{
  if (Add_Int_Proc_Ptr) Add_Int_Proc_Ptr();
  INT_Cnt++;
  if (INT_Cnt >= BRIGHT_LEVELS)
     {
     INT_Cnt = 0;
     LED_ix++;
     if (LED_ix >= LED_CNT)
          {
          LED_ix = 0;
          }
     if (LED_Bright[LED_ix]) Act_LED(LED_ix);
     }
  if (INT_Cnt == LED_Bright[LED_ix])
     Disable_LEDs();
}

//--------------
void Test_LEDs()
//--------------
{
  Timer1.initialize(100);  // [us] Original 500                                                               // 28.06.19:  Old 250
  Timer1.attachInterrupt(LED_IntProc);
  for (uint8_t i = 0; i < LED_CNT; i++)
    Set_LED(i, 255);
  Set_LED(1, 2); // White LED

  while(1) // Pulse the Blue LED
    {
    delay(250);
    LED_Bright[0]++;
    if (LED_Bright[0] > MAX_BRIGHT) LED_Bright[0] = 0;
    }
}

//-----------------------------------------
void Setup_LEDs(void (*Add_Int_Proc)(void))
//-----------------------------------------
{
  Add_Int_Proc_Ptr = Add_Int_Proc;
  Timer1.initialize(100);  // [us] Original 500                                                               // 28.06.19:  Old 250
  Timer1.attachInterrupt(LED_IntProc);
}

//-----------------
void Power_on_Ani()
//-----------------
{
  const uint8_t LedTab[] =                                                                                    // 09.04.20:
    {
    LED_RESET_AS_IO,
    LED_NORM_RESETF,
    LED_HEART_BEAT,
    LED_ERROR,
    LED_PROG,
    LED_RES,
    };

  for (uint8_t i = 0; i < LED_CNT*2-1; i++)
      {
      uint8_t LED_Nr = LedTab[i< LED_CNT?i:2*LED_CNT-2-i];                                                    // 09.04.20:  Added LedTab[]
      Set_LED(LED_Nr, Max_LED_Brightnes[LED_Nr]);
      delay(30);
      Set_LED(LED_Nr, 0);
      }
}



//-------------------------------------------------------
void Setup_LEDs_with_Power_on_Ani(void (*Add_Int_Proc)(void))
//-------------------------------------------------------
{
  Setup_LEDs(Add_Int_Proc);
  Power_on_Ani();
}
