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


 Test_Buttons.cpp
 ~~~~~~~~~~~~~~~~

 Module to test the LEDs with three buttons connected to the Arduino
 Modes:
 - Left button:        Starts the LED test. One or all LEDs could be switched to different colors.
                       - Left:         Privious LED
                       - Middle:       Change the color
                       - Right:        Next LED
                       - Left & Right: Use all LEDs (on/off)

 - Midle button:       Use the three buttons to simulate the inputs 0-2.
                       - Left:         Input 0
                       - Middle:       Input 1
                       - Right:        Input 2
                       - Left long:    Switch between "push button" and "toggle switch"

 - Right button:       Start the input test. Normaly the inputs are read from the CAN bus or other inputs.
                       With this test the inputs could be simulated manualy.
                       - Left:         Privious input channel
                       - Middle:       Toggle the input
                       - Right:        Next input channel
                       - Left & Right: Use all Inputs (on/off)

 - Middle button long: Ends the tests and switch to the normal mode.

*/
#include "MobaLedLib.h"
#include "Read_Buttons.h"

#define KEY_LEFT        1
#define KEY_MIDDLE      2
#define KEY_RIGHT       4
#define KEY_LEFTRIGHT   5
#define KEY_R_LEFT      (KEY_LEFT   | BUTTON_REP_FLAG)
#define KEY_R_RIGHT     (KEY_RIGHT  | BUTTON_REP_FLAG)
#define KEY_M_LEFT      (KEY_LEFT   | BUTTON_MIDDLE_FLAG)
#define KEY_M_MIDDLE    (KEY_MIDDLE | BUTTON_MIDDLE_FLAG)
#define KEY_M_RIGHT     (KEY_RIGHT  | BUTTON_MIDDLE_FLAG)
#define KEY_L_LEFT      (KEY_LEFT   | BUTTON_LONG_FLAG)
#define KEY_L_MIDDLE    (KEY_MIDDLE | BUTTON_LONG_FLAG)
#define KEY_L_RIGHT     (KEY_RIGHT  | BUTTON_LONG_FLAG)

#define KEY_END_TEST    (KEY_MIDDLE | BUTTON_LONG_FLAG)

static uint8_t LED_Channel = 0, Inp_Channel = 0, LED_Test_Mode = 0, Inp_Test_Mode = 0 ;
static bool PushButton_Mode_But = true;
static bool PushButton_Mode_Inp = false;
static uint32_t ButtonPushed = 0;

//--------------------------------------------------------
void MobaLedLib_C::Set_one_Test_Outp(uint8_t Nr, CRGB* &p)
//--------------------------------------------------------
{
  switch (TestMode)
    {
    case TM_LED_TEST: p = &leds[Nr];
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
                          default : LED_Test_Mode = 7;
                                    p->r = p->g = p->b = 255;
                          }
                      break;
    case TM_INP_TEST: Set_Input(Nr, Inp_Test_Mode);
                      break;
    }
}

//-------------------------------------------
void MobaLedLib_C::Set_Test_Outp(uint8_t &Ch)
//-------------------------------------------
{
  CRGB *p = NULL;
  uint16_t MaxChannel;
  switch (TestMode)
    {
    case TM_LED_TEST: MaxChannel = Num_Leds - 1; break;
    case TM_INP_TEST: MaxChannel = 255;          break;
    }

  if (Ch == 255) Ch = MaxChannel;
  else if (Ch >= MaxChannel && Ch != 254) Ch = 0;                                                             // 25.08.18:  Old >= NUM_LEDS

  if (Ch == 254) // All channels
        {
        for (uint8_t Nr = 0; Nr < MaxChannel; Nr++)                                                           // 25.08.18:  Old: < NUM_LEDS
           Set_one_Test_Outp(Nr, p);
        }
   else Set_one_Test_Outp(Ch, p);

   #ifdef _PRINT_DEBUG_MESSAGES
     char NrStr[4];
     if (Ch == 254)
          strcpy(NrStr, "All");
     else sprintf(NrStr, "%i", Ch);
     switch (TestMode)
      {
      case TM_LED_TEST: Dprintf("LED Test %s Mode %i (%i,%i,%i)\n", NrStr, LED_Test_Mode, p->r, p->g, p->b); break;
      case TM_INP_TEST: Dprintf("Inp Test %s Mode %i\n", NrStr, Inp_Test_Mode);                              break;
      }
   #endif
}

//-----------------------------
void MobaLedLib_C::Speed_Test()                                                                               // 26.08.18:
//-----------------------------
// Measure the update speed without writing to the LEDs
// It's started in the input test if the Right button is pressed long
//
// Results: (Configuration with 18 configuration lines (310 bytes) 26.08.18)
// -  421 us all channels disabled  => Reduced to 368
// -    5 us Feuerwehrtransporter (Leerlauf Zeit von 421 ist abgezogen bei allen folgenden Werten)
// -    7 us slow analog Ramp with one channel
// -   12 us 6 analog Fades with one LED
// -  281 us Windrad, AndreaskreuzRGB analog, Andreaskreuz digital, 15 channel Fade
// -  749 us 15 channel Lauflicht             => Die analoge Berechnung bei 200 ms dauert 50 us pro Kanal ;-(
// -    0 us Button
// - 1475 us Sum = All together
// ===> The update time has been reduced by limmiting dt for analog patterns by 20 ms
//      Now the update time for all effects is 596 us (including 370 us "Leerlauf")
//      prior it was 1423 us !
//
// Details
// -   33 us Get_Input()
// -  175 us MobaLedLib_C::Int_Update() without Proc_AnalogPattern() call
// -   52 us random16_add_entropy()  => moved to Trigger20fps calc saved 49 us
// -    4 us Update_TV_Data();
// -    5 us pgm_read_byte_near(cp+const)
{
#ifdef _PRINT_DEBUG_MESSAGES
  do {
     Dprintf("Time for one Int_Update() call: ");
     uint32_t Start = millis();
     for (uint32_t t = 0; t < 3000; t+=3)
       Int_Update(t);
     Dprintf("%ius\n", millis() - Start);
     } while (Key_Pressed());
#endif
}

//----------------------------------------------
void Print_PushButton_Mode(bool PushButton_Mode)
//----------------------------------------------
{
  if (PushButton_Mode)
       Dprintf("Push buttons");
  else Dprintf("Toggle switches");
  Dprintf(" active\n");
}

//-------------------------------------------
void MobaLedLib_C::Proc_Key_Test(uint8_t key)
//-------------------------------------------
{
  switch (TestMode)
    {
    case TM_LED_TEST:  memset(leds, 0, Num_Leds*sizeof(CRGB)); // Yellow LED / Activated with left button
                       switch (key)
                           {
                           case KEY_LEFT :
                           case KEY_R_LEFT:   LED_Channel--;     break;
                           case KEY_RIGHT:
                           case KEY_R_RIGHT:  LED_Channel++;     break;
                           case KEY_M_LEFT:   LED_Channel = 0;   break;
                           case KEY_M_RIGHT:  LED_Channel = 255; break;
                           case KEY_LEFTRIGHT:LED_Channel = 254; break; // All LEDs
                           case KEY_MIDDLE:   LED_Test_Mode++;   break;
                           case KEY_M_MIDDLE: LED_Test_Mode--;   break;
                           //default: Dprintf("TM_LED_TEST undefined key %02X\n", key);
                           }
                       break;

    case TM_BUT_TEST:  if (PushButton_Mode_But == 0) // White LED / Activated with middle button
                          {
                          uint8_t Val;
                          switch (key)
                              {
                              case KEY_LEFT:   Set_Input(0, (Val = !Inp_Is_On(Get_Input(0)))); Dprintf("Sw[0]=%i\n", Val); break;
                              case KEY_MIDDLE: Set_Input(1, (Val = !Inp_Is_On(Get_Input(1)))); Dprintf("Sw[1]=%i\n", Val); break;
                              case KEY_RIGHT:  Set_Input(2, (Val = !Inp_Is_On(Get_Input(2)))); Dprintf("Sw[2]=%i\n", Val); break;
                              }
                          }
                       if (key == KEY_L_LEFT)
                          {
                          PushButton_Mode_But = !PushButton_Mode_But;
                          Print_PushButton_Mode(PushButton_Mode_But);
                          }
                       break;

    case TM_INP_TEST:  switch (key) // Blue LED / Activated with right button
                           {
                           case 0:            Inp_Channel = Find_Next_Priv_InpChannel(0,            0);  break;
                           case KEY_LEFT:
                           case KEY_R_LEFT:   Inp_Channel = Find_Next_Priv_InpChannel(Inp_Channel, -1);  break;
                           case KEY_RIGHT:
                           case KEY_R_RIGHT:  Inp_Channel = Find_Next_Priv_InpChannel(Inp_Channel, +1);  break;
                           case KEY_M_LEFT:   Inp_Channel = Find_Next_Priv_InpChannel(255,         +1);  break;
                           case KEY_M_RIGHT:  Inp_Channel = Find_Next_Priv_InpChannel(0,           -1);  break;
                           case KEY_LEFTRIGHT:Inp_Channel = 254;  break; // All LEDs
                           case KEY_MIDDLE:   Inp_Test_Mode ^= 1;
                                              ButtonPushed = millis();
                                              break;
                           case KEY_M_MIDDLE: PushButton_Mode_Inp = !PushButton_Mode_Inp;
                                              Print_PushButton_Mode(PushButton_Mode_Inp);
                                              break;
                           case KEY_L_RIGHT:  Speed_Test(); return;
                           //default: Dprintf("TM_INP_TEST undefined key %02X\n", key);
                           }
                       break;
    //default: Dprintf("Proc_Key_Test undefined TestMode %i\n", TestMode);
    }

  if (key != KEY_END_TEST)
       {
       if (TestMode == TM_LED_TEST)
            Set_Test_Outp(LED_Channel);
       else Set_Test_Outp(Inp_Channel);
       }
  else {
       #ifdef _PRINT_DEBUG_MESSAGES
         char TestName[3][4] = { "LED", "Inp", "But" };
         Dprintf("End %s Test\n", TestName[TestMode-1]); // key == KEY_END_TEST
       #endif
       }
}

//------------------------------------
void MobaLedLib_C::Proc_Test_Buttons()
//------------------------------------
{
  uint8_t key = Read_Buttons();
  if (key)
     {
     //DEBUG_PRINT_BUTTONS(key); // Debug output  (FLASH: 124, RAM 28)
     if (TestMode != TM_DISABLED)
          {
          Proc_Key_Test(key);
          if (key == KEY_END_TEST)
             {
             TestMode = TM_DISABLED;
             digitalWrite(LED_LEFT_PIN,  LOW);
             digitalWrite(LED_RIGHT_PIN, LOW);
             digitalWrite(LED_MID_PIN,   LOW);
             }
          }
     else {
          switch (key)
             {
             case KEY_LEFT:     TestMode = TM_LED_TEST;
                                digitalWrite(LED_LEFT_PIN,  HIGH);
                                Proc_Key_Test(0);
                                break;
             case KEY_RIGHT:    TestMode = TM_INP_TEST;
                                digitalWrite(LED_RIGHT_PIN, HIGH);
                                Proc_Key_Test(0);
                                break;
             case KEY_MIDDLE:   TestMode = TM_BUT_TEST;
                                Dprintf("%s Test Mode activated\n", PushButton_Mode_But ? "Button":"Switch");
                                digitalWrite(LED_MID_PIN,   HIGH);
                                break;
             }
          }
     }

  switch (TestMode)
     {
     case TM_BUT_TEST: if (PushButton_Mode_But)
                           {
                           for (uint8_t i  = 0; i < sizeof(Buttons); i++)
                               Set_Input(i, !digitalRead(Buttons[i]));
                           }
                       break;
     case TM_INP_TEST: if (PushButton_Mode_Inp && Inp_Test_Mode && millis() - ButtonPushed > 100)
                          {
                          Inp_Test_Mode = 0;
                          Set_Test_Outp(Inp_Channel);
                          }
                       break;
     }
}

//-------------------------------------
void MobaLedLib_C::Setup_Test_Buttons()
//-------------------------------------
{
  Setup_Buttons();
  pinMode(LED_LEFT_PIN,  OUTPUT);
  pinMode(LED_RIGHT_PIN, OUTPUT);
  pinMode(LED_MID_PIN,   OUTPUT);
}


