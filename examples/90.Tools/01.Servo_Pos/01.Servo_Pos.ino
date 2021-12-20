/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de

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

 Servo_Pos                                                                                 by Hardi   20.06.19
 ~~~~~~~~~
 This Program is used to define the end positions and the speed of the servos
 connected to the ATTiny 85 "Servo_LED" modul.

 For more details see "Servo Positionierungstool.pdf"


 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and a WS2812 LED stripe with at least 32 LEDs (Adapt NUM_LEDS if an other stripe is used).
 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).

 All examples could also be used with the other LED stripe types which are supported
 by the FastLED library (See FastLED Blink example how to adapt the "FastLED.addLeds"
 line.).
*/

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#define BAUDRATE    19200
#include "Printf.h"

#define NUM_LEDS     32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#elif defined(ESP32)
#define LED_DO_PIN   27  // Pin 27 is connected to the LED stripe
#ifndef LED_BUILTIN 
#define LED_BUILTIN  2   // Built in LED
#endif
#else 
#error this example does not support this plattform
#endif

#define PWM_BUTTON1   205
#define PWM_SEL_LED0  225
#define PWM_SEL_LED1  230
#define PWM_SEL_LED2  235
#define PWM_MIN_MAX   240
#define PWM_SPEED     245
#define PWM_SPECIAL   250
#define PWM_FINISH    254


#define WAIT_TIME_CMD 200   // [ms] time to wait if a PWM command was send

#define PWM_WIGG_MIN  110   // Minimal position in Wiggle mode (should not generate a sound if send to a sound module by mistake)
#define PWM_WIGG_MAX  120   // Maximal     "                           "                        "
#define WIGG_TIME     500   // Time for one wiggle movement

typedef struct
    {
    uint8_t  LEDNr;
    uint8_t  ChNr;
    } Channel_T;


typedef struct
    {
    uint32_t Next_t;
    uint32_t Timeout;
    uint8_t  State;
    } Wigg_T;

Channel_T  Ch;
Wigg_T     Wig;

CRGB leds[NUM_LEDS];     // Define the array of leds

LED_Heartbeat_C LED_Heartbeat(LED_BUILTIN); // Use the build in LED as heartbeat

#define SERIAL_INP_WITHOUT_MOBALEDLIB
#include "Serial_Inputs.h" // Use the serial monitor to test LED colors and brightness values


void Setup_Buttons();    // Forward definition (Defined in "Read_Buttons.h" )
uint8_t Read_Buttons();  //    "        "


//--------------
void Cont_Proc()
//--------------
{
  Proc_Serial_Input(0);
  LED_Heartbeat.Update();
}

//----------
void setup()
//----------
// This function is called once to initialize the program
//
{
  Serial.begin(BAUDRATE);
  printf("Servo_Pos\n");

  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  pinMode(LED_LEFT_PIN,  OUTPUT);
  pinMode(LED_MID_PIN,   OUTPUT);
  pinMode(LED_RIGHT_PIN, OUTPUT);

  Setup_Buttons();

  memset(&Ch, 0, sizeof(Ch));
}


//----------------------------
void Wiggle_Servo(Wigg_T *Wig)
//----------------------------
{
  uint32_t t = millis();
  if (t >= Wig->Next_t)
     {
     Wig->Next_t = t + WAIT_TIME_CMD;
     switch (Wig->State++)
        {
        case 0: memset(leds, 0, sizeof(leds));                                             break; // clear all RGB LEDs
        case 1: leds[Ch.LEDNr].r = PWM_SEL_LED0 + Ch.ChNr * (PWM_SEL_LED1 - PWM_SEL_LED0); break; // Select the servo
        case 2: if (t < Wig->Timeout) leds[Ch.LEDNr].r = PWM_WIGG_MIN; // move to the min wiggle position
                digitalWrite(LED_LEFT_PIN , 1);
                digitalWrite(LED_RIGHT_PIN, 0);
                Wig->Next_t = t + WIGG_TIME;
                break;
        case 3: if (t < Wig->Timeout) leds[Ch.LEDNr].r = PWM_WIGG_MAX; // move to the max wiggle position
                digitalWrite(LED_LEFT_PIN , 0);
                digitalWrite(LED_RIGHT_PIN, 1);
                Wig->Next_t = t + WIGG_TIME;
                Wig->State = 2;
                break;
        }
     }
}

//--------------------
void Button_LEDs_Off()
//--------------------
{
  digitalWrite(LED_LEFT_PIN,  0);
  digitalWrite(LED_MID_PIN,   0);
  digitalWrite(LED_RIGHT_PIN, 0);
}

/*
 LED flash modes
           -----       -----
 -1       |     |     |     |               Unchanged
     -----       -----       -----
               -           -
  0           | |         | |
     ---------   ---------   -----
       ---------   ---------   ---
  1   |         | |         | |
     -           -           -
  t 0 1   2   3 0 1   2   3 0

 Off t0
 On  t1 if mode  1
 On  t2 if mode -1
 On  t3 if mode  0
*/

int8_t  ServoPositions[3]; // -1: Unchanged, 0 = Minimal position, 1 = Maximal position

const uint8_t Button_LED_Pin[3] = { LED_LEFT_PIN, LED_MID_PIN, LED_RIGHT_PIN };

//----------------------
void Upd_PostTest_LEDs()
//----------------------
{
  static uint32_t NextT = 0;
  static uint8_t LED_t  = 0;
  if (millis() >= NextT)
     {
     int8_t Check;
     switch (LED_t)
       {
       case 1:  Check =  1; break;
       case 2:  Check = -1; break;
       case 3:  Check =  0; break;
       default: Check =  0; LED_t = 0;
       }
     for (uint8_t i = 0; i < 3; i++)
         if (LED_t == 0 || ServoPositions[i] == Check) digitalWrite(Button_LED_Pin[i], LED_t);
     if (Check)
          NextT = millis() + 490;
     else NextT = millis() + 10;
     LED_t++;
     }
}

//------------------------
void Calc_Tenary_LED_Val()
//------------------------
{
  uint8_t Sum = 35;
  for (uint8_t i = 0; i < 3; i++)
    {
    uint8_t val = 5 * (ServoPositions[i] + 1);
    if (i == 1) val *= 3;
    if (i == 2) val *= 9;
    Sum += val;
    }
  //printf("Tenary[%i]=%i\n", Ch.LEDNr, Sum); // Debug
  leds[Ch.LEDNr].r = Sum;
  FastLED.show();
}

//-----------------
void Calc_LED_Val()
//-----------------
{
  for (uint8_t i = 0; i < 3; i++)
    {
    uint8_t Val;
    switch (ServoPositions[i])
        {
        case -1: Val = 0;   break;
        case  0: Val = 10;  break;
        case  1: Val = 210; break;
        }
    leds[Ch.LEDNr][i] = Val;
    }
  FastLED.show();
}

//--------------------
void Print_Positions()
//--------------------
{
  printf("Positions:");
  for (uint8_t i = 0; i < 3; i++)
    {
    char c;
    switch (ServoPositions[i])
        {
        case -1: c = 's'; break;
        case  0: c = '0'; break;
        case  1: c = '1'; break;
        }
    printf(" %c", c);
    }
  printf("\n");
}

//----------------------------------
void Test_Positions(uint8_t Ternary)
//----------------------------------
{
  printf("Test servo positions ");
  if (Ternary)
       {
       printf("ternary"); // One LED channel is used to control 3 servos
       leds[Ch.LEDNr].r = 0;
       FastLED.show();

       delay(100);
       leds[Ch.LEDNr].r = 250;
       }
  else {
       printf("normal\n"); // Three LED channel control 3 servos
       leds[Ch.LEDNr] = CRGB::Black;
       }
  FastLED.show();

  memset(ServoPositions, -1, sizeof(ServoPositions));
  uint8_t End = 0;
  while (!End)
    {
    uint8_t key = Read_Buttons();
    //if (key) printf("Key: %02X\n", key); // Debug
    int8_t Ch = 0;
    switch (key)
       {
       case 0x04: case 0x44: Ch++;   // Right  key
       case 0x02: case 0x42: Ch++;   // Middle key
       case 0x01: case 0x41:         // Left   key
                  if (key & 0x40)
                       ServoPositions[Ch] = -1;
                  else ServoPositions[Ch] = ServoPositions[Ch] ? 0:1;
                  digitalWrite(Button_LED_Pin[Ch],  ServoPositions[Ch]);
                  if (Ternary)
                       Calc_Tenary_LED_Val();
                  else Calc_LED_Val();
                  Print_Positions();
                  break;
       case 0x05: case 0x45: // Left and Right key
       case 0x82: // Middle key long
                  End++;
                  break;
       }
    Upd_PostTest_LEDs();
    }
  leds[Ch.LEDNr] = CRGB::Black;
  FastLED.show();

  printf("End position test\n");
  digitalWrite(LED_MID_PIN, 0);
}

//--------------------------------------------
void Set_one_Test_Outp(uint8_t &LED_Test_Mode)
//--------------------------------------------
{
  CRGB* p = &leds[Ch.LEDNr];
  *p = CRGB::Black;
  switch (LED_Test_Mode)
      {
      case   8: LED_Test_Mode = 0; // no break
      case   0: p->r = p->g = p->b = 127; break;
      case   1: p->r =  63; break;
      case   2: p->g =  63; break;
      case   3: p->b =  63; break;
      case   4: p->r = 255; break;
      case   5: p->g = 255; break;
      case   6: p->b = 255; break;
      default : *p = CRGB::White;
                LED_Test_Mode = 7;
      }
}

/*
 ToDo LED_Test:
 ~~~~~~~~~~~~~~
 - Linke+Rechte Taste zum aktivieren aller LEDs
 - Linke LED schnell Blinken lassen oder Auf und Abblenden
 - Anzeige der RGB Werte
*/

//-------------
void LED_Test()
//-------------
{
  printf("LED test\n");
  uint8_t End = 0;
  uint8_t LED_Test_Mode = 0;
  uint8_t OldNr = Ch.LEDNr;
  while (!End)
    {
    uint8_t key = Read_Buttons();
    //if (key) printf("Key: %02X\n", key); // Debug
    switch (key)
       {
       case 0x01: // Left key
                  if (Ch.LEDNr > 0) Ch.LEDNr--;
                  break;
       case 0x04: // Right key
                  if (Ch.LEDNr < NUM_LEDS-1) Ch.LEDNr++;
                  break;
       case 0x42: // Middle key 1 sec => Mode back
                  LED_Test_Mode-=2;
       case 0x02: // Middle key
                  LED_Test_Mode++;
                  Set_one_Test_Outp(LED_Test_Mode);
                  FastLED.show();
                  break;
       case 0x81: // Left key long
       case 0x82: // Middle key long
                  End++;
                  break;
       }
    if (OldNr != Ch.LEDNr)
       {
       printf("LED Nr %i\n", Ch.LEDNr);
       Set_one_Test_Outp(LED_Test_Mode);
       leds[OldNr] = CRGB::Black;
       OldNr = Ch.LEDNr;
       FastLED.show();

       }
    }
  printf("End LED test\n");
}

//-----------------
void Select_Servo()
//-----------------
{
  memset(&Wig, 0, sizeof(Wig));
  printf("Select Servo:\n");
  uint8_t End = 0;
  //uint32_t t;
  uint8_t PrintActServo = 1;
  while (!End)
    {
    uint8_t key = Read_Buttons();
    uint8_t Ternary = 1;
    //if (key) printf("Key: %02X\n", key); // Debug
    switch (key)
       {
       case 0x01:// Left key
                 PrintActServo++;
                 if (Ch.ChNr > 0)
                      Ch.ChNr--;
                 else if (Ch.LEDNr > 0)
                         {
                         Ch.LEDNr--;
                         Ch.ChNr = 2;
                         }
                 break;
       case 0x04:// Right key
                 PrintActServo++;
                 if (Ch.ChNr < 2)
                      Ch.ChNr++;
                 else if (Ch.LEDNr < NUM_LEDS-1)
                         {
                         Ch.LEDNr++;
                         Ch.ChNr = 0;
                         }
                 break;
       case 0x02:// Middle key
                 End++;
                 break;
       case 0x42:// Middle key 1 sec => Reactivate the wiggle after timeout
                 PrintActServo++;
                 break;
       case 0x05:// Left and right key
       case 0x45:// Left and right key 1 sec
                 Ternary = 0; // Normal mode
       case 0x82:// Middle key long
                 Test_Positions(Ternary);
                 PrintActServo++;
                 break;
       case 0x81:// Left key long => LED Test
                 LED_Test();
                 PrintActServo++;
                 break;
       }
    if (PrintActServo)
         {
         PrintActServo = 0;
         printf("Adjust servo %i (LEDNr %-3i ChNr %i)\n", Ch.LEDNr*3 + Ch.ChNr, Ch.LEDNr, Ch.ChNr);
         Wig.State = 0;
         Wig.Timeout = millis() + 30000;
         }
    Wiggle_Servo(&Wig);
    FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)
    Cont_Proc();
    }
  Button_LEDs_Off();
}

//------------------------
uint8_t Read_Pressed_Key()
//------------------------
{
  for (uint8_t i = 0, Mask = 1; i < sizeof(Buttons); i++, Mask<<=1)
      if (digitalRead(Buttons[i]) == 0) return Mask;
  return 0;
}

//-------------------------------------------
void Msg_Adjust(uint8_t Mode, uint8_t MinMax)
//-------------------------------------------
{
  printf("Servo %i adjust ", Ch.LEDNr*3+Ch.ChNr);
  if (Mode == PWM_MIN_MAX)
       {
       if (MinMax)
            printf("max");
       else printf("min");
       printf("imal position");
       }
  else printf("speed");
  printf(":\n");
}

//--------------------
void Adjust_MinMax_or_Speed(uint8_t Mode)
//--------------------
{
  Msg_Adjust(Mode, 0);
  leds[Ch.LEDNr].r = Mode; // Activate the Min Max Mode or the Speed
  FastLED.show();
  delay(WAIT_TIME_CMD);
  leds[Ch.LEDNr].r = PWM_SPECIAL; // Activate the Min Max Button Mode
  FastLED.show();
  delay(WAIT_TIME_CMD);

  uint8_t State = 0;
  uint32_t t, Start_t = 0, Speed, Next_Flash = 0;
  while (State < 2)
    {
    uint8_t key = Read_Pressed_Key();
    if (key)
       {
       //printf("Key: %02X\n", key); // Debug
       t = millis();
       if (Start_t == 0) Start_t = t;
       Speed = (t - Start_t) / 50;
       if (Speed > 70) Speed = 70;
       }
    switch (key)
       {
       case 1:  // Left key
                leds[Ch.LEDNr].r =  90 - Speed;
                break;
       case 4:  // Right key
                leds[Ch.LEDNr].r = 110 +  Speed;
                break;
       case 2:  // Middle key
                leds[Ch.LEDNr].r = PWM_BUTTON1;
                FastLED.show();
                while(Read_Pressed_Key() || millis() - t < WAIT_TIME_CMD) delay(50);
                Button_LEDs_Off();
                if (Mode == PWM_MIN_MAX)
                     {
                     if (++State == 1) Msg_Adjust(Mode, 1);
                     }
                else {
                     State = 99; // End
                     leds[Ch.LEDNr].r = PWM_FINISH; // Save the speed (if changed)
                     FastLED.show();
                     delay(WAIT_TIME_CMD);
                     }
                Next_Flash = 0;
                break;
       default: leds[Ch.LEDNr].r = 100;
                Start_t = 0;
       }

    if (State < 2 && millis() > Next_Flash) // LED flash
       {
       if (Mode == PWM_MIN_MAX)
            {
            switch (State)
              {
              case 0: digitalWrite(LED_LEFT_PIN,  !digitalRead(LED_LEFT_PIN )); break;
              case 1: digitalWrite(LED_RIGHT_PIN, !digitalRead(LED_RIGHT_PIN)); break;
              }
            }
       else digitalWrite(LED_MID_PIN, !digitalRead(LED_MID_PIN));
       Next_Flash = millis() + 500;
       }

    FastLED.show();
    Cont_Proc();
    }
  Button_LEDs_Off();
}


//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//                                                                    ----
  Select_Servo();

  Adjust_MinMax_or_Speed(PWM_MIN_MAX);

  Adjust_MinMax_or_Speed(PWM_SPEED);

  leds[Ch.LEDNr].r = 0;
  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)
  delay(WAIT_TIME_CMD);


  //MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)
  Cont_Proc();
}


/*
 Arduino Nano:          +-----+
           +------------| USB |------------+
           |            +-----+            |
        B5 | [ ]D13/SCK        MISO/D12[ ] |   B4
           | [ ]3.3V           MOSI/D11[ ]~|   B3
           | [ ]V.ref     ___    SS/D10[ ]~|   B2
        C0 | [ ]A0       / N \       D9[ ]~|   B1
        C1 | [ ]A1      /  A  \      D8[ ] |   B0
        C2 | [ ]A2      \  N  /      D7[ ] |   D7
        C3 | [ ]A3       \_0_/       D6[ ]~|   D6   -> WS281x LED stripe pin DIN
        C4 | [ ]A4/SDA               D5[ ]~|   D5
        C5 | [ ]A5/SCL               D4[ ] |   D4
           | [ ]A6              INT1/D3[ ]~|   D3
           | [ ]A7              INT0/D2[ ] |   D2
           | [ ]5V                  GND[ ] |
        C6 | [ ]RST                 RST[ ] |   C6
           | [ ]GND   5V MOSI GND   TX1[ ] |   D0
           | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |   D1
           |          [ ] [ ] [ ]          |
           |          MISO SCK RST         |
           | NANO-V3                       |
           +-------------------------------+

 Arduino Uno:                           +-----+
           +----[PWR]-------------------| USB |--+
           |                            +-----+  |
           |           GND/RST2  [ ] [ ]         |
           |         MOSI2/SCK2  [ ] [ ]  SCL[ ] |   C5
           |            5V/MISO2 [ ] [ ]  SDA[ ] |   C4
           |                             AREF[ ] |
           |                              GND[ ] |
           | [ ]N/C                    SCK/13[A] |   B5
           | [ ]v.ref                 MISO/12[A] |   .
           | [ ]RST                   MOSI/11[A]~|   .
           | [ ]3V3    +---+               10[ ]~|   .
           | [ ]5v     | A |                9[ ]~|   .
           | [ ]GND   -| R |-               8[B] |   B0
           | [ ]GND   -| D |-                    |
           | [ ]Vin   -| U |-               7[A] |   D7
           |          -| I |-               6[A]~|   .   -> WS281x LED stripe pin DIN
           | [ ]A0    -| N |-               5[C]~|   .
           | [ ]A1    -| O |-               4[A] |   .
           | [ ]A2     +---+           INT1/3[A]~|   .
           | [ ]A3                     INT0/2[ ] |   .
      SDA  | [ ]A4      RST SCK MISO     TX>1[ ] |   .
      SCL  | [ ]A5      [ ] [ ] [ ]      RX<0[ ] |   D0
           |            [ ] [ ] [ ]              |
           |  UNO_R3    GND MOSI 5V  ____________/
            \_______________________/
*/










