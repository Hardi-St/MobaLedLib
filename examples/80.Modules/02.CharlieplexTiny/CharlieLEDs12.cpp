/*
 Charliplexing LEDs
 ~~~~~~~~~~~~~~~~~~

 Using Port B because this port ia available on the ATTiny and on the Uno/Namo
 ATTiny84  Pin 5, 6, 7, 2
 Arduino   Uno D8 - D11


 Der Timer Interrupt wird alle 18.51us aufgerufen 54 kHz !
 Dadurch erreicht man eine "Bildwiederholrate" von 300 Hz.
 Das flimmert weder bei 50Hz noch bei 60Hz flimmern

 Revision History:
 ~~~~~~~~~~~~~~~~~
 18.12.19:  - Improved the brightness distribution with a table instead of a
              switch / case construct => saved 48 bytes



 ToDo:
 ~~~~~
 Interrupt periode not checked for ATTiny85 / ATTiny84



 Aufteilung in PWM Bereiche zur Erhoehung der Helligkeitsaufloesung
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Bei einer "Bildwiederholrate" von 300 Hz mit 12 LEDs und einer Interrupt Periode von 18.51us
 sind 15 Helligkeitswerte moeglich.´
   15*12*18.5us = 3.33 ms  => 300 Hz
 Wenn man die Abgeschaltet LED dazu zaehlt sind das 16 moegliche Werte. Man hat eine Schrittweite von 6.25%.

 Das reicht normalerweise aus wenn man langsam ueberblenden will.

 Wenn aber nur ein Teil des Bereiches genutzt werden soll weil eine LED zu hell ist, dann reduziert
 sich die verfuegbare Schrittanzahl. Das Problem tritt auf wenn verschiedene LED Typen verwendet werden.
 Eine weisse oder blaue LED leuchtet viel heller als eine andere LED.
 Zur Erhoehung der Aufloesung muesste man aber die Interrupt Periode verkleinern. Mit 18.5us = 54 KHz
 ist die Interrupt Periode aber schon sehr hoch.

 Darum wird die Zeit in der eine LED leuchtet in mehrere Teile unterteilt. Dadurch werden die LEDs
 in einem Zyklus mehrfach angesteuert. Dadurch flimmern die LEDs entsprechend weniger.

 Ohne Aufteilung:
 ~~~~~~~~~~~~~~~~
 LED0 6   ******..........
 LED1 9                   *********.......
 LED3 3                                   *...............

 Aufteiling in zwei Bereiche:                                      (Das Programm verwendet 8 Bereiche)
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 LED0 3+3 ***.....                ***.....
 LED1 5+4         *****...                ****....
 LED2 1+0                 *.......                ........

 An dem Beispiel erkennt man, dass die LEDs doppelt so haeufig an sind.
 Allerdings sind die Zeiten unter Umstaenden nicht gleich lange. Wenn sich die Leuchtdauer durch
 zwei Teilen laesst, wie bei der ersten LED, dann ergibt sich ein symetrisches Bild.
 Wenn bei der Teilung ein Rest uebrig bleibt, dann sind die Zeiten um einen Zeitabschnitt verschieden.
 Das bemerkt man aber zu Glueck nicht.
 Problematisch wird es wenn die Leuchtdauer kleiner als die Anzahl der Schritte ist.
 Bei LED2 sieht man, dass die LED nur noch in der eine Haelfte der Zeit leuchtet. Das fuehrt bei Video
 Aufzeichnungen zu flimmern. Mit der Aufteilung in 8 Teile (PWM_PARTS) tritt dieser Effekt bei
 Helligkeiten im Bereich von 1-3. Hier ist die LED aber so dunkel, dass es nicht auffaelt.

 Mit einer Unterteilung in 8 Bereiche koennen bei gleicher Interruptfrequenz 128 Helligkeitsstufen
 flimmerfrei in Videos dargestellt werden.

*/

#include "Arduino.h"
#include "pins_arduino.h"
#include "Public_Defines.h"

#if !defined __AVR_ATtiny85__
  // Debugging with an Uno or Nano
  #define USE_TIMERONE
  #include "TimerOne_HighRes.h"
  #define INTERRUPT_PERIOD   18.51851 // [us]
  // Attention the interrupt period for the ATTinys must be configured manualy in Activate_Timer_Interrupt()
#endif

#include "Dprintf.h"

#include "CharlieLEDs12.h"


#define LED_CNT            12

#define DDR_X              DDRB
#define PORT_X             PORTB


volatile uint8_t LED_Bright[LED_CNT]        = { 0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0  }; // Full brightness if >= MAX_BRIGHT


// The lines in the following arrays could be sorted by different criteria (Tst / Vis / LED)

                              //                               4  3  2  1 <= Connector pin numbers              L = LED Numbers see Tiny_UniProg_Sch2.pdf
const PROGMEM                 //76543210      Tst Vis   LED   B3 B2 B1 B0 <= Bit number                             https://www.opendcc.de/elektronik/HW2_ledmux/ansteuerung_multiplex.html
uint8_t P_LED_PRT[LED_CNT] = {//----XXXX   // ~~~ ~~~   ~~~   ~~ ~~ ~~ ~~    Enter 1 for each +                 V = Viessmann Multiplex
                               B00000001,  // T00 V08   L01         -  +                                        T = Test Board = Refferenz
                               B00000010,  // T01 V06   L02         +  -
                               B00000001,  // T02 V07   L08      -     +
                               B00000100,  // T03 V03   L07      +     -
                               B00000001,  // T04 V00   L09   -        +
                               B00001000,  // T05 V01   L10   +        -
                               B00000010,  // T06 V02   L12      -  +
                               B00000100,  // T07 V10   L11      +  -
                               B00000010,  // T08 V11   L03   -     +
                               B00001000,  // T09 V04   L04   +     -
                               B00000100,  // T10 V05   L06   -  +
                               B00001000,  // T11 V09   L05   +  -
                               };          //


                              //                               4  3  2  1 <= Connector pin numbers              L = LED Numbers see Tiny_UniProg_Sch2.pdf
const PROGMEM                 //76543210      Tst Vis   LED   B3 B2 B1 B0 <= Bit number                         V = Viessmann Multiplex
uint8_t P_LED_DDR[LED_CNT] = {//----XXXX   // ~~~ ~~~   ~~~   ~~ ~~ ~~ ~~    Enter 1 for + or -                 T = Test Board = Refferenz
                               B00000011,  // T00 V08   L01         -  +
                               B00000011,  // T01 V06   L02         +  -
                               B00000101,  // T02 V07   L08      -     +
                               B00000101,  // T03 V03   L07      +     -
                               B00001001,  // T04 V00   L09   -        +
                               B00001001,  // T05 V01   L10   +        -
                               B00000110,  // T06 V02   L12      -  +
                               B00000110,  // T07 V10   L11      +  -
                               B00001010,  // T08 V11   L03   -     +
                               B00001010,  // T09 V04   L04   +     -
                               B00001100,  // T10 V05   L06   -  +
                               B00001100,  // T11 V09   L05   +  -
                               };

uint8_t LED_PRT[LED_CNT];
uint8_t LED_DDR[LED_CNT];

//-----------------------------
inline void Act_LED(uint8_t Nr)
//-----------------------------
// ~1 us
{
  if (Nr < LED_CNT) // This check is removed by the compiler if it's not used necessary if the function is never called with Nr >= LED_CNT
     {
     #ifdef LED_MASK
       PORT_X = (PORT_X & ~LED_MASK) | LED_PRT[Nr];
       DDR_X  = (DDR_X  & ~LED_MASK) | LED_DDR[Nr];
     #else
       PORT_X = LED_PRT[Nr];
       DDR_X  = LED_DDR[Nr];
     #endif
     }
}

//------------------------
inline void Disable_LEDs()
//------------------------
{
  #ifdef LED_MASK
    DDR_X  = (DDR_X  & ~LED_MASK);
    PORT_X = (PORT_X & ~LED_MASK);
  #else
    DDR_X  = 0;
    PORT_X = 0;
  #endif
}

//----------------------------------------------
void Set_LED(uint8_t LED_Nr, uint8_t brightness)
//----------------------------------------------
{
  if (LED_Nr < LED_CNT)
     LED_Bright[LED_Nr] = brightness;
}

//-----------------------------
uint8_t Get_LED(uint8_t LED_Nr)
//-----------------------------
{
  if (LED_Nr < LED_CNT)
       return LED_Bright[LED_Nr];
  else return 0;
}

void Set_LED_Heart_Beat (uint8_t on) { Set_LED(LED_HEART_BEAT , on ? MAX_BRIGHT: 0); }

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
// Generate values between -2 and BRIGHT_LEVELS+1 to simulate a sin wave
{
  static int16_t hbval = 0;
  static int8_t hbdelta = 1;
  static unsigned long last_time = 0;
  unsigned long now = millis();
  if ((now - last_time) >= (40*16)/BRIGHT_LEVELS)
     {
     last_time = now;
     if (hbval >= BRIGHT_LEVELS || hbval < -2) hbdelta = -hbdelta; // Change direction
     hbval += hbdelta;
     Set_LED_Val_Heart_Beat(hbval);
     }
}

/*
 Verbesserte Interruptroutine:
 - Die einzelnen LEDs werden in mehreren Teilen angezeigt. Dadurch wird die Bildwiederholrate erhoeht
   ohne dass die Interrupt Ferquenz erhoeht werden muss.
 - Dadurch kann man die Interrupt Frequenz verringern oder/und die Aufloesung der Helligkeit erhoehen.
 - Bei 4 Teilen benoetigt man
 - Verteilung der Helligkeiten bei 4 Teilen
    Gesammt
    Helligkeit  Erstverteilung           Rest wird drauf addiert
       8        2   2   2   2   Rest 0
       7        1   1   1   1   Rest 3 =>  2  2  2  1
       6        1   1   1   1   Rest 2 =>  2  1  2  1
       5        1   1   1   1   Rest 1 =>  2  1  1  1
       4        1   1   1   1   Rest 0
       3        0   0   0   0   Rest 3 =>  1  1  1  0
       2        0   0   0   0   Rest 2 =>  1  0  1  0
       1        0   0   0   0   Rest 1 =>  1  0  0  0

   Reihenfolge bei 8 Teilen:
                T0  T1  T2  T3  T4  T5  T6  T7
                a               b
                        c               d
                    e       g       f       h


   Reihenfolge bei 16 Teilen:
                T0  T1  T2  T3  T4  T5  T6  T7
                T0  T1  T2  T3  T4  T5  T6  T7  T8   T9  T10 T11 T12 T13 T14 T15
                 a                               b
                                 c                                d
                         e               g                f               h
                      i       m      k       o        j        n      l       p

   Ich hab keine ahnung wie man das in einen Algorithmus packt, aber das ist auch nicht noetig
   da es nur wenige Varianten gibt.
   => Der Rest wird in folgender Reihenfolge verteilt
      2 Teile: 0
      4 Teile: 0, 2, 1
      8 Teile: 0, 4, 2, 6, 1, 5, 3
     16 Teile: 0, 8, 4, 12, 2, 10, 6, 14, 1, 9, 5, 13, 3, 11, 7

 - Anzeige Reihenfolge
    LED 0,  Part 0
    LED 1,  Part 0
    :
    LED 11, Part 0
    LED 0,  Part 1
    LED 1,  Part 1
    :
    LED 11, Part 1
    :
    LED 11, Part 7

*/

#if PWM_PARTS > 1
  #define BRIGHT_LEVELS_PER_PART  (BRIGHT_LEVELS / PWM_PARTS)  // 16

  // Define which part gets an additional brightness
  #if PWM_PARTS == 2
           static const uint8_t AddBrightnessTab[2] = {//      10
                                                        B00000000,  // Rest = 0
                                                        B00000001,  // Rest = 1
                                                      };
  #elif PWM_PARTS == 4   //  4 Parts: 0, 2, 1
           static const uint8_t AddBrightnessTab[4] = {//    3210
                                                        B00000000,  // Rest = 0
                                                        B00000001,  // Rest = 1
                                                        B00000101,  // Rest = 2
                                                        B00000111,  // Rest = 3
                                                      };
  #elif PWM_PARTS == 8   //  8 Parts: 0, 4, 2, 6, 1, 5, 3
           static const uint8_t AddBrightnessTab[8] = {//76543210
                                                        B00000000,  // Rest = 0
                                                        B00000001,  // Rest = 1
                                                        B00010001,  // Rest = 2
                                                        B00010101,  // Rest = 3
                                                        B01010101,  // Rest = 4
                                                        B01010111,  // Rest = 5
                                                        B01110111,  // Rest = 6
                                                        B01111111,  // Rest = 7
                                                      };
  #else
       #error Unsupported PWM_PARTS size
  #endif

  //----------------
  void LED_IntProc()                         // 18.12.19:  Uses 196 Bytes @Uno, 1  (Checked with avr-nm.exe)
  //----------------
  // Interrupt procedure which
  // is called every 18.5 us  (INTERRUPT_PERIOD)
  {
    static uint8_t Act_Part      = PWM_PARTS;
    static uint8_t LED_ix        = 0;
    static uint8_t INT_Cnt       = 254;
    static uint8_t Act_Brightnes_in_Part = 0;

    INT_Cnt++;
    if (INT_Cnt >= BRIGHT_LEVELS_PER_PART-1)
       {
       INT_Cnt = 0;

       Act_Part++;
       if (Act_Part >= PWM_PARTS*LED_CNT) // All parts have been processed
            { // New Start
            Act_Part = 0;
            LED_ix   = 0;
            }
       else {
            LED_ix++;
            LED_ix = LED_ix % LED_CNT;
            }
       if (LED_Bright[LED_ix])
            {
            Act_Brightnes_in_Part = LED_Bright[LED_ix] / PWM_PARTS;
            uint8_t Rest = LED_Bright[LED_ix] % PWM_PARTS;
            uint8_t PNr  = Act_Part / LED_CNT;
            if (AddBrightnessTab[Rest] & (1<<PNr)) Act_Brightnes_in_Part++;

            if (Act_Brightnes_in_Part) Act_LED(LED_ix); // Turn the LED on
            }
       else Act_Brightnes_in_Part = 0;
       }
    if (INT_Cnt == Act_Brightnes_in_Part)
       Disable_LEDs();
  }

#else // Old function with one PWM part. It is used if PWM_PARTS = 1
      // In this case only 16 brightnes levels are possible without flicker
  //----------------
  void LED_IntProc()
  //----------------
  // Interrupt procedure which
  // is called every 18.5 us  (INTERRUPT_PERIOD)
  {
    static uint8_t INT_Cnt       = 254;
    static uint8_t LED_ix        = 0;
    static uint8_t Act_Brightnes = 0;
    INT_Cnt++;
    if (INT_Cnt >= BRIGHT_LEVELS-1)
       {
       INT_Cnt = 0;
       LED_ix++;
       LED_ix = LED_ix % LED_CNT;
       Act_Brightnes = LED_Bright[LED_ix];
       if (Act_Brightnes) Act_LED(LED_ix);
       }
    if (INT_Cnt == Act_Brightnes)
       Disable_LEDs();
  }
#endif

#ifdef USE_TIMERONE
  //-----------------------------
  void Activate_Timer_Interrupt()
  //-----------------------------
  {
      Timer1.initialize(INTERRUPT_PERIOD);  // [us]
      Timer1.attachInterrupt(LED_IntProc);
  }

#elif defined __AVR_ATtiny85__
  //--------------------
  ISR(TIMER1_COMPA_vect) //Interrupt-Serviceroutine
  //--------------------
  {
    LED_IntProc();
  }

  //-----------------------------
  void Activate_Timer_Interrupt()
  //-----------------------------
  {
    // Setzen der Register fuer 111 us Timerinterrupt   (https://arduino-projekte.webnode.at/meine-projekte/servosteuerung/servotest-attiny/)
    #if F_CPU != 16000000L // for the 16 MHz internal clock
      #error Wrong CPU frequency
    #endif

    // Vergleichswert = OCR1C = (16.000.000 / (2 / 18.51E-6)) - 1 = 147.5
    cli();      // Loesche globales Interruptflag
    TCNT1 = 0;   // Loesche Timer Counter 1
    TCCR1 = 0;   // Loesche Timer Counter Controll Register
    OCR1C = 147; // Setze Output Compare Register C
    TCCR1 |= (1 << CS11); // Setze CS11 - Clock Select Bit 10 (Prescaler 2) (Table 12-5 in atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf)
    TCCR1 |= (1 << CTC1); // CTC-Mode ein (Clear Timer and Compare)
    //Timer/Counter Interrupt Mask Register
    TIMSK |= (1 << OCIE1A); //Output Compare A Match Interrupt Enable
    sei(); //Setze globales Interruptflag
  }
#else
    #error unsupported CPU
#endif // USE_TIMERONE

//----------------------------------------------------
void Set_one_LED_Assignement(uint8_t Dst, uint8_t Src)
//----------------------------------------------------
{
  //Dprintf("LED %i <- %i\n", Dst, Src);
  LED_PRT[Dst] = pgm_read_byte_near(P_LED_PRT+Src);
  LED_DDR[Dst] = pgm_read_byte_near(P_LED_DDR+Src);
}

//---------------
void Setup_LEDs()
//---------------
{
  for (uint8_t i = 0; i < sizeof(LED_PRT); i++)
    Set_one_LED_Assignement(i, i);

  Activate_Timer_Interrupt();
}

//-----------------
void Power_on_Ani()
//-----------------
{
  for (uint8_t i = 0; i < LED_CNT*2-1; i++)
      {
      uint8_t LED_Nr = i< LED_CNT?i:2*LED_CNT-2-i;
      Set_LED(LED_Nr, MAX_BRIGHT);
      delay(100);
      Set_LED(LED_Nr, 0);
      }
}
