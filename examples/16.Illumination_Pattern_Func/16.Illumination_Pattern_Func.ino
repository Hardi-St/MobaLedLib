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


 Illumination Pattern Function                                                             by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with the pattern function to generate
 fancy illuminations. The example also shows some complex macros. It uses the Logic() and the
 Counter() function.
 On our model railway this example is used to illuminate a cave. It could also be used to
 illuminate a special building or a nice bridge.

 The illumination is turned on with a push button which is locate near the cave. It's one of
 the "Push Button Actions"  around the border of our railway. The button has got a RGB light
 which shines blue if the "Action" is available, and flashes green while it's running.
 Three different illumination modes for the cave could be selected with the start button
  1. Fire only
  2. Fire and some floating lights (dark)
  3. Fire and a all lights activated with floating colors (bright)

 1. There are two fireplaces in the cave. They are activated randomly. They are implemented with
    the FIRE type of the House() function. The second fire uses the dark version (FIRED)

 2. The second effect uses a great feature of the FastLED library; the HSV colors.
    With the FastLED library the colors of the LEDs could also be changed using HSV colors.
    The HSV color space describes colors by hue, saturation, and value.
    The big advantage of the HSV colors is the easy changing of the color or the brightness
    of a LED. If the hue is slowly changed from 0 to 255 all colors in a rainbow are shown.
    This method could be used in the Pattern() function with the flag "PM_HSV".
    If the flag is active the first channel if a LED represents the hue, the second the saturation
    and the third the value. With the analog blending feature of the pattern function one channel
    could be changed slowly from one value to an other.
    This is used in the ilumination to show different colors. The animation uses different periods
    for the changing of the color in the range from 25 seconds to 70 seconds. As a result, seemingly
    random color changes are produced. Here is the macro for this:

       #define Changing_Hue( LED, InNr, Period)                                          \
                    APatternT2(LED,192,InNr,1,0,255,0,PM_HSV|PF_SLOW,Period/16, 0 ms,1)

    The pattern function is generated with the excel file "Pattern_Configurator.xlsm" in the sheet
    "Illumination Hue".

    As a second component, the brightness is continuously changed. This can be generated with the
    "val" channel. The corresponding pattern function is a ramp that rises slowly, then lingers for
    a certain amount of time and then falls down again. This is followed by a dark phase until
    it starts all over again.
    That's implemented with the following macro:

       #define Pulsating_Val(LED, InNr, Delay, RampTime, HoldTime, Pause)                                            \
                    APatternT5(LED, 98,InNr,1,0,255,0,PM_HSV|PF_EASEINOUT,Delay,RampTime, HoldTime,RampTime,Pause,6)

    The pattern function is generated in the sheet "Illumination Val".

    Those two macros are combined to an other macro because they have to be grouped together with the
    "New_HSV_Group()" function. This is important that the following two functions can use a common
    memory to convert the HSV values to RGB.

       #define Illumination(LED, InNr, HuePeriod, Delay, RampTime, HoldTime, Pause) \
                    New_HSV_Group()                                                 \
                    Changing_Hue( LED, InNr, HuePeriod)                             \
                    Pulsating_Val(LED, InNr, Delay, HoldTime, RampTime, Pause)

    Finally, several "illumination" macros are put together to form a sort of table. Of course, this
    is also done via a macro. In this table, the different numerical values create a leg of infinite
    sequence of different color and brightness patterns.
    At the end we have one macro "Illumination1(LED0, InNr)" which is used in the configuration.

 3. The effect number 3 uses a similar method. Here hue and val are modified within one pattern line.
    This does not produce such a wide variety of color combinations. But that is not necessary here
    either. The resulting illumination is much brighter and creates beautiful lighting effects.

 Switching between the effects
 -----------------------------
 This illumination is activated by a button placed at the edge of the system. This switch should also
 toggle between the different effects. Therefor the counter function is used. Every time the button is
 pressed the next counter state is activated. At the end the counter starts from the beginning because
 the flag CF_ROTATE is used. The number 0 is skipped due to the flag CF_SKIP0. If the button is pressed
 for a long time the effect it turned of (CF_RESET_LONG). It's also deactivated after a "Timeout".
 The counter function is also used in the examples "RailwaySignal_Pattern_Func" and "FlipFlop_Counter".
 There are three white LEDs which show the counter number for debugging.

 Logic() function
 ----------------
 The logic function is used in this example to invert an input signal:
   Logic(TmpNr+1, NOT TmpNr)
 It is used to illuminate the switch either with a constant blue light or to make it flash green.




 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Video:
 ~~~~~~
 This video demonstrates the example:
   https://vimeo.com/309065937

 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)
 and a WS2812 LED stripe.
 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).
 One push button is used to activate the ilumination and toggle between the modes. The button
 is connected to D7 and ground.

*/

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#define NUM_LEDS     32  // Number of LEDs with some spare channels (Maximal 256 RGB LEDs could be used)
#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN   6   // Pin D6 is connected to the LED stripe
#define BUTTON_PIN   7   // Pin D7 is connected to the push button
#elif defined(ESP32)
#define LED_DO_PIN   27  // Pin 27 is connected to the LED stripe
#define BUTTON_PIN   12  // Pin 12 is connected to the push button
#ifndef LED_BUILTIN 
#define LED_BUILTIN  2   // Built in LED
#endif
#else 
#error this example does not support this plattform
#endif

// *** Macros ***

#define Changing_Hue( LED, InNr, Period)                                          \
             APatternT2(LED,192,InNr,1,0,255,0,PM_HSV|PF_SLOW,Period/16, 0 ms,1)

#define Pulsating_Val(LED, InNr, Delay, RampTime, HoldTime, Pause)                                            \
             APatternT5(LED, 98,InNr,1,0,255,0,PM_HSV|PF_EASEINOUT,Delay,RampTime, HoldTime,RampTime,Pause,6)

#define Illumination(LED, InNr, HuePeriod, Delay, RampTime, HoldTime, Pause) \
             New_HSV_Group()                                                 \
             Changing_Hue( LED, InNr, HuePeriod)                             \
             Pulsating_Val(LED, InNr, Delay, HoldTime, RampTime, Pause)

#define Illumination1(LED0, InNr)  /* 480 Byte FLASH, 209 Byte RAM */                   \
             /*          LED      InNr  HuePeriod Delay   RampTime  HoldTime Pause*/    \
             Illumination(LED0+0,  InNr, 70 Sec,    0 Sek,  5 Sek,   2 Sek,   10 Sek)   \
             Illumination(LED0+1,  InNr, 30 Sec,    6 Sek,  5 Sek,   2 Sek,   19 Sek)   \
             Illumination(LED0+2,  InNr, 40 Sec,   11 Sek,  5 Sek,   4 Sek,   23 Sek)   \
             Illumination(LED0+3,  InNr, 65 Sec,   24 Sek,  8 Sek,   2 Sek,    1 Sek)   \
             Illumination(LED0+4,  InNr, 60 Sec,   30 Sek,  5 Sek,   2 Sek,    0 Sek)   \
             Illumination(LED0+5,  InNr, 50 Sec,   33 Sek,  5 Sek,   3 Sek,    2 Sek)   \
             Illumination(LED0+6,  InNr, 75 Sec,   28 Sek, 10 Sek,   2 Sek,    1 Sek)   \
             Illumination(LED0+7,  InNr, 25 Sec,    4 Sek,  5 Sek,   5 Sek,    4 Sek)   \
             Illumination(LED0+8,  InNr, 45 Sec,   13 Sek, 15 Sek,   9 Sek,    7 Sek)   \
             Illumination(LED0+9,  InNr, 55 Sec,   21 Sek,  5 Sek,   3 Sek,   21 Sek)   \
             Illumination(LED0+10, InNr, 65 Sec,   38 Sek,  5 Sek,   2 Sek,    2 Sek)   \
             Illumination(LED0+11, InNr, 50 Sec,    1 Sek,  9 Sek,   2 Sek,   32 Sek)

#define Illumination2(LED0, InNr)                                                                                               \
             APatternT6(LED0+0,12, InNr,3,0,255,0,PM_HSV,3 Sek,3 Sek,10 Sek,10 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)      \
             APatternT6(LED0+1,12, InNr,3,0,255,0,PM_HSV,3 Sek+100,3 Sek,10 Sek,10 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
             APatternT6(LED0+2,12, InNr,3,0,255,0,PM_HSV,3 Sek+200,3 Sek,10 Sek,10 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
             APatternT6(LED0+3,12, InNr,3,0,255,0,PM_HSV,3 Sek+300,3 Sek,10 Sek,10 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
             APatternT6(LED0+4,12, InNr,3,0,255,0,PM_HSV,9 Sek+300,4 Sek,10 Sek,10 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
             APatternT6(LED0+5,12, InNr,3,0,255,0,PM_HSV,8 Sek+300,6 Sek,10 Sek,10 Sek,5 Sek,500,15,255,255,240,255,255,0,0,0)  \
             APatternT6(LED0+6,12, InNr,3,0,255,0,PM_HSV,7 Sek+300,7 Sek,10 Sek,10 Sek,7 Sek,500,15,255,255,240,255,255,0,0,0)  \
             APatternT6(LED0+7,12, InNr,3,0,255,0,PM_HSV,300,8 Sek,10 Sek,10 Sek,8 Sek,500,15,255,255,240,255,255,0,0,0)        \
             APatternT6(LED0+8,12, InNr,3,0,255,0,PM_HSV,300,8 Sek,11 Sek,11 Sek,9 Sek,500,15,255,255,240,255,255,0,0,0)        \
             APatternT6(LED0+9,12, InNr,3,0,255,0,PM_HSV,300,8 Sek,12 Sek,12 Sek,10 Sek,500,15,255,255,240,255,255,0,0,0)       \
             APatternT6(LED0+10,12,InNr,3,0,255,0,PM_HSV,300,8 Sek,13 Sek,13 Sek,11 Sek,500,15,255,255,240,255,255,0,0,0)       \
             APatternT6(LED0+11,12,InNr,3,0,255,0,PM_HSV,300,8 Sek,14 Sek,14 Sek,12 Sek,500,15,255,255,240,255,255,0,0,0)


#define Cave_Illumination(LED0, StatusLEDs, InNr, TmpNr,Timeout)                                                          \
            Counter(CF_ROTATE|CF_SKIP0|CF_RESET_LONG, InNr, SI_1, Timeout, TmpNr, TmpNr+2, TmpNr+3, TmpNr+4)              \
            Logic         (TmpNr+1, NOT TmpNr)                                                                            \
            Const         (StatusLEDs+0,C_BLUE,  TmpNr+0, 0, 10)                 /* Blue LED:           Ready         */  \
            Blink2        (StatusLEDs+0,C_GREEN, TmpNr+1, 500 ms, 500 ms, 0, 50) /* Green Flashing LED: Active        */  \
            Const         (StatusLEDs+1,C_ALL,   TmpNr+2, 0, 10)                 /* White LED:          Ilu 1 Active  */  \
            Const         (StatusLEDs+2,C_ALL,   TmpNr+3, 0, 10)                 /* White LED:          Ilu 2 Active  */  \
            Const         (StatusLEDs+3,C_ALL,   TmpNr+4, 0, 10)                 /* White LED:          Ilu 2 Active  */  \
            House         (LED0+12,              TmpNr+1, 1, 2, FIRE, FIRED)     /* At least one fire is always on    */  \
            Illumination1 (LED0,                 TmpNr+3)                                                                 \
            Illumination2 (LED0,                 TmpNr+4)


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  //               LED0  StatusLEDs InNr TmpNr Timeout
  Cave_Illumination(0,    27,        0,   100,  5 Min)  // Maximal 17 Minutes

  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];     // Define the array of leds

MobaLedLib_Create(leds); // Define the MobaLedLib instance

LED_Heartbeat_C LED_Heartbeat(LED_BUILTIN); // Use the build in LED as heartbeat

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS); // Initialize the FastLED library

  pinMode(BUTTON_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
  MobaLedLib.Set_Input(0, !digitalRead(BUTTON_PIN));

  MobaLedLib.Update();    // Update the LEDs in the configuration

  FastLED.show();         // Show the LEDs (send the leds[] array to the LED stripe)

  LED_Heartbeat.Update(); // Update the heartbeat LED. This must be called periodically in the loop() function.
}

/*
 Arduino Nano:          +-----+
           +------------| USB |------------+
           |            +-----+            |
        B5 | [ ]D13/SCK        MISO/D12[ ] |   B4
           | [ ]3.3V           MOSI/D11[ ]~|   B3
           | [ ]V.ref     ___    SS/D10[ ]~|   B2
        C0 | [ ]A0       / N \       D9[ ]~|   B1   -> toggle switch 2  --.
        C1 | [ ]A1      /  A  \      D8[ ] |   B0   -> toggle switch 1  --o-- GND
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   -> toggle switch 0  --'
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
           | [ ]5v     | A |                9[ ]~|   .   -> toggle switch 2  --.
           | [ ]GND   -| R |-               8[B] |   B0  -> toggle switch 1  --o-- GND
           | [ ]GND   -| D |-                    |                             |
           | [ ]Vin   -| U |-               7[A] |   D7  -> toggle switch 0  --'
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

