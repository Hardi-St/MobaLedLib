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


 Flip Flops, Counter and Mono Flops                                                        by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib with Flip Flops, Counters and Mono Flops.

 "Push Button Actions" are started with buttons which are placed around the border of the model railway.
 They are well known from the "Miniatur Wunderland".

 Mono flops are suitable for this type of action. If the button is pressed a action is activated
 for a certain time.
 But there are more possible types of interaction:
 - Stop the action again
 - Select a special scenario
 - ...

 The MobaLedLib contains a lot of functions for this purpose. This example combines some of them.
 Of course they could also be used for other purpose.


 This is the most confusing example of all            ?
                                               _     ?  _
                                                \_('')_/


 Maybe it's better to enable them line by line. Therefore all lines except one in the first block
 of the configuration below should be commented (Not the "Const" lines).
 You could also cover all LEDs except the LEDs used for a special line. This makes it easier to
 understand the behavior of the flops....
 But if you have understood all the examples before this shouldn't be a problem for you...


 All of the functions set one or multiple variables which could be used as an input for other
 functions. In the example several Const() functions are used to show the state of the variables.
 In the real usage this could be other more complex functions which activate the "push button function".

 The examples are all controlled by two push buttons. The first button acts as a trigger, the second
 is used in some lines as reset or to count down (Counter 2).

 Description of the different lines:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 MonoFlop:                 A mono flop is a function which activate the output, if the if the input
                           is triggered, for the given time. A trigger is generated if the button is
                           pressed. The time could be extended by pressing the button again, but not
                           by holding the button.

 MonoFlopLongReset:        This mono flop could be stopped by pressing the button for longer than 1.5 seconds.

 ButtonFunc:               The ButtonFunc is similar to the mono flop. But here the time could be
                           extended by holding the button.

 RS_FlipFlopTimeout:       A RS flip flop is a function which could be activated with one input (button 0)
                           and deactivated with the other input (button 1). This flip flop has the option
                           to deactivate the output automatically after a given time. If the time is
                           set to 0 the timeout is deactivated.

 T_FlipFlopResetTimeout:   A T-flip flop is a function which toggles the output with every trigger
                           on the input. This one could also be deactivated after a given time.

 MonoFlop2:                This mono flop has two outputs which are switched in the opposite way.
                           This could be used to control two functions. In the example two LEDs
                           are toggled (Dark Blue / Cyan).

 MonoFlop2LongReset:       Like MonoFlopLongReset with two outputs.

 RS_FlipFlop2:             A RS flip flop with two outputs.

 RS_FlipFlop2Timeout:      Like above with a timeout.

 T_FlipFlop2Reset:         Like T_FlipFlopResetTimeout (Timeout=0), with two outputs.

 T_FlipFlop2ResetTimeout:  Like T_FlipFlopResetTimeout with two outputs.

 Counter 1:                The Counter function is the basic function for all examples above except
                           the ButtonFunc. It's a very powerful function. The Counter function is
                           controlled by several flags which define the behavior of the counter.
                           (See the documentation for details)
                           Above we have used one or two output variables. Now several variables
                           are used.
                           The first counter is configured to activate 6 outputs (Green LEDs)
                           alternating if the first input is triggered (button 0).
                           The first LED (Dark Green) is only activated at the start or if the
                           reset is activated (button 1). In addition it's activated if the
                           button 0 is not pressed for longer than 8 seconds (timeout).
                           The CF_ROTATE flag is used to start again at the end.
                           Because of the CF_SKIP0 flag the output 0 is skipped.
                           The second input of the counter is an enable input. It's configured
                           as reset with the CF_INV_ENABLE flag.

 Counter 2:                The second counter uses button 0 to activate the next LED and
                           button 1 for the previous LED. This mode is activate with the flag
                           CF_UP_DOWN.
                           The outputs are visualized by blue LEDs.

 Counter 3:                Here the CF_PINGPONG flag is used. The outputs are counted from
                           one side to the other and back (Knight Rider LED bar).
                           The LED 0 (Dark Cyan) is only activated at the beginning or after
                           a timeout.

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line).

 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...),
 a LED Stripe with at least 32 LEDs and two push buttons which are connected to pin 6 an 7.
 The other pin of the switches is connected to ground.
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
#elif defined(ESP32)
#define LED_DO_PIN   27  // Pin 27 is connected to the LED stripe
#ifndef LED_BUILTIN 
#define LED_BUILTIN  2   // Built in LED
#endif
#else 
#error this example does not support this plattform
#endif
#define SWITCH0_PIN  7   // Pin D7 is connected to push button 0
#define SWITCH1_PIN  8   // Pin D8 is connected to push button 1


//*************************************************************************************
// *** Configuration array which demonstartes the different Flip-Flops and Counters ***
MobaLedLib_Configuration()                                                                           // Used LEDs
  {//                     DestVars              InNrs   Timeout  DestVars                        // ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  MonoFlop(               100,                  0,      3.5 Sec)                                 // 0      Blue
  MonoFlopLongReset(      101,                  0,      4.0 Sec)                                 // 1      Blue
  ButtonFunc(             102,                  0,      4.5 Sec)                                 // 2      Blue
  RS_FlipFlopTimeout(     103,                  0, 1,   5.0 Sec)                                 // 3      Red
  T_FlipFlopResetTimeout( 104,                  0, 1,   5.5 Sec)                                 // 4      Yellow
  MonoFlop2(              105, 106,             0,      6.0 Sec)                                 // 5,6    Dark Blue / Cyan
  MonoFlop2LongReset(     107, 108,             0,      6.5 Sec)                                 // 7,8    Dark Blue / Cyan
  RS_FlipFlop2(           109, 110,             0, 1           )                                 // 9,10   Dark Red / Green
  RS_FlipFlop2Timeout(    111, 112,             0, 1,   7.0 Sec)                                 // 11,12  Dark Red / Green
  T_FlipFlop2Reset(       113, 114,             0, 1           )                                 // 13,14  Dark Yellow / White
  T_FlipFlop2ResetTimeout(115, 116,             0, 1,   7.5 Sec)                                 // 15,16  Dark Yellow / White
  Counter(CF_ROTATE  |CF_INV_ENABLE|CF_SKIP0,   0, 1,   8.0 Sec, 117, 118, 119, 120, 121, 122)   // 17-22  Dark Green  / Green / ..
  Counter(CF_ROTATE  |CF_INV_ENABLE|CF_UP_DOWN, 0, 1,   8.5 Sec, 123, 124, 125, 126, 127)        // 23-27  Dark Blue   / Blue  / ..
  Counter(CF_PINGPONG|CF_INV_ENABLE|CF_SKIP0,   0, 1,   9.0 Sec, 128, 129, 130, 131)             // 28-31  Dark Cyan   / Cyan  / ..

  //     LED  Channel  InCh  Val0 Val1
  Const( 0,   C_BLUE,  100,  0,   125)  // MonoFlop                Blue
  Const( 1,   C_BLUE,  101,  0,   125)  // MonoFlopLongReset       Blue
  Const( 2,   C_BLUE,  102,  0,   125)  // ButtonFunc              Blue
  Const( 3,   C_RED,   103,  0,   125)  // RS_FlipFlopTimeout      Red
  Const( 4,   C_YELLOW,104,  0,   125)  // T_FlipFlopResetTimeout  Yellow
  Const( 5,   C_BLUE,  105,  0,    25)  // MonoFlop2               Dark Blue
  Const( 6,   C_CYAN,  106,  0,   125)  //                         Cyan
  Const( 7,   C_BLUE,  107,  0,    25)  // MonoFlop2LongReset      Dark Blue
  Const( 8,   C_CYAN,  108,  0,   125)  //                         Cyan
  Const( 9,   C_RED,   109,  0,    25)  // RS_FlipFlop2            Dark Red
  Const(10,   C_GREEN, 110,  0,   125)  //                         Green
  Const(11,   C_RED,   111,  0,    25)  // RS_FlipFlop2Timeout     Dark Red
  Const(12,   C_GREEN, 112,  0,   125)  //                         Green
  Const(13,   C_YELLOW,113,  0,    25)  // T_FlipFlop2Reset        Dark Yellow
  Const(14,   C_WHITE, 114,  0,   125)  //                         White
  Const(15,   C_YELLOW,115,  0,    25)  // T_FlipFlop2ResetTimeout Dark Yellow
  Const(16,   C_WHITE, 116,  0,   125)  //                         White
  Const(17,   C_GREEN, 117,  0,    25)  // Counter                 Dark Green
  Const(18,   C_GREEN, 118,  0,   225)  //                         Green
  Const(19,   C_GREEN, 119,  0,   225)  //                         Green
  Const(20,   C_GREEN, 120,  0,   225)  //                         Green
  Const(21,   C_GREEN, 121,  0,   225)  //                         Green
  Const(22,   C_GREEN, 122,  0,   225)  //                         Green
  Const(23,   C_BLUE,  123,  0,    25)  // Up Down Counter         Dark Blue
  Const(24,   C_BLUE,  124,  0,   225)  //                         Blue
  Const(25,   C_BLUE,  125,  0,   225)  //                         Blue
  Const(26,   C_BLUE,  126,  0,   225)  //                         Blue
  Const(27,   C_BLUE,  127,  0,   225)  //                         Blue
  Const(28,   C_CYAN,  128,  0,    25)  // Ping Pong Counter       Dark Cyan
  Const(29,   C_CYAN,  129,  0,   225)  //                         Cyan
  Const(30,   C_CYAN,  130,  0,   225)  //                         Cyan
  Const(31,   C_CYAN,  131,  0,   225)  //                         Cyan

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

  pinMode(SWITCH0_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pins
  pinMode(SWITCH1_PIN, INPUT_PULLUP);
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib.Set_Input(0, !digitalRead(SWITCH0_PIN));
  MobaLedLib.Set_Input(1, !digitalRead(SWITCH1_PIN));

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
        C0 | [ ]A0       / N \       D9[ ]~|   B1
        C1 | [ ]A1      /  A  \      D8[ ] |   B0   -> push button 1   --o-- GND
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   -> push button 0   --'
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
           | [ ]GND   -| R |-               8[B] |   B0  -> push button 1   --o-- GND
           | [ ]GND   -| D |-                    |                            |
           | [ ]Vin   -| U |-               7[A] |   D7  -> push button 0   --'
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

