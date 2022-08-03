/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de

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


 80 and more switches connected to the Arduino                                             by Hardi   18.11.18:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example demonstrates how more than 80 switches could be connected to the Arduino.

 The trick of reading so much switches with one Arduino is the usage of the counter chip CD4017 which is
 available for 0.31 Eu. With this IC, 10 switches can be read with only three pins. With each additional
 input to the Arduino another 10 switches can be queried.
 => With 10 processor pins 80 switches could be read !!

 But that's not enough. The number of channels could also be expanded by adding an other CD4017.
 If additional IC's are added some channels are used to control the hand over to the next chip.
 The first and the last CD4017 in a chain could drive 9 channels. Counters in the middle of the chain only 8.
 => With 10 processor pins and 3 CD4017 it's possible to read 26*8 = 208 switches !!

 But a configuration with several CD4017 and only one input line is also useful if several push buttons
 around the model railway should be use for "Push Button Actions".
 In this case only 3 processor pins plus 2 power lines = 5 wires have to be wired to read in many buttons.
 If 3 CD4017 IC are used 26 push buttons could be connected to the Arduino.

 For mutual decoupling of the switch, one diode per switch is required in each case.
 For each additional CD4017, an AND gate of the type CD4011 is also required.

 In the documentation there are example circuits and a printed circuit board to read in the switches.

 The configuration of the used Arduino pins, the number of used counter states and the number of input
 channels is configured with the following defines. Attention this #defines must be located in the program
 before the "Keys_4017.h" file is included.

    #define CTR_CHANNELS_1     10                   // Number of used counter channels for keyboard 1. Up to 10 if one 4017 is used, up to 18 if two CD4017 are used, ...
    #define BUTTON_INP_LIST_1  2,7,8,9,10,11,12,15  // Comma separated list of the button input pins (Example use A0-A3: 14, 15, 16, 17)
    #define CLK_PIN            18                   // Pin number used for the CD4017 clock (Example 18 = A4)
    #define RESET_PIN          19                   // Pin number used for the CD4017 reset (Example 19 = A5)

 It uses one CD4017 and 8 button pins => 80 switches could be read in with this keyboard ! It may be used
 in a control desk.

 In some situations two type of keyboards may be wanted. This is also possible with the library.
 A second keyboard could be connected using the same clock and reset pin. The second keyboard only needs
 additional button input pins. The following defines an other keyboard for the Arduino:

    #define CTR_CHANNELS_2     18                   // Number of used counter channels for keyboard 2. Up to 10 if one 4017 is used, up to 18 if two 4017 are used, ...
    #define BUTTON_INP_LIST_2  A0                   // Comma separated list of the button input pins (Example use A0-A3: 14, 15, 16, 17)  Attention A6, A7 are not posible because they can't be used as digital inputs

 The definitions above use two CD4017 and one input channel.
 => 18 switches could be connected. This could be expanded like described above.
 This buttons may be used as "Push Button Action" around the model railway.


 This example program uses 146 macros to visualize the pressed buttons. The first 128 macros are used to
 light a LED if the corresponding button is pressed. The number of 128 was chosen because I use two square
 LED modules with 64 LEDs because then its easier to assignee the LEDs to the buttons.
 The following 18 macros are used to implement a toggle flip Flop which drives two LEDs. One LED is build
 into the switch. It flashes if the "Push Button Action" is active. The second LED is on while the action
 is enabled.


 Interrupt:
 ~~~~~~~~~~
 The keys are read in using the timer interrupt 1. => The Timer1 can't be used for other purpose.
 All keys are scanned within 100 ms.


 Independent module:
 ~~~~~~~~~~~~~~~~~~~
 The module "Keys_4017.h" could be used independent from the rest of the library. The state of the buttons
 is written to the bit array "Keys_Array_1[]".
 The result of the second keyboard is stored in the array Keys_Array_2[].
 To copy the key to the input structure for the LEDs the function MobaLedLib_Copy_to_InpStruct() must be used.


 Bits and Bytes:
 ~~~~~~~~~~~~~~~
 In the example two keyboards are used. The results are stored to:

  Keys_Array_1[10]:                                                                              Keys_Array_2[3]:
  BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB BBBBBBBB      bbbbbbbb bbbbbbbb ~~~~~~bb

 This array uses one bit per scanned switch. The first keyboard used 10 bytes (80/8), the second uses 3 bytes.
 There are 6 unused bits (~) in Keys_Array_2[] because 18 switches are used.

 The function MobaLedLib_Copy_to_InpStruct() is used to copy this bits to the input structure of the MobaLedLib.
 In this structure two bits are used to store the information. One bit contains the old state of the input,
 the other bit the actual state. The InpStructArray[] represents 256 inputs. It uses 64 byte.

  InpStructArray[64]:
  Byte 0   Byte 1   Byte 2   Byte 3   Byte 4   Byte 5   Byte 6   Byte 7   ... Byte 63
  OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA ... OAOAOAOA

 The keyboard bits are copied to the InpStructArray[]. To be able to increase the size of the first keyboard
 the program reserves some space in the InpStructArray[].
 This looks like:

  Byte 0   Byte 1   Byte 2   Byte 3   ... Byte 8   Byte 9   Byte 10  Byte 11  ... Byte 15  Byte 17  Byte 18  Byte 19  Byte 20  Byte 21  Byte 22      Byte 63
  OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA ... OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA ... OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA OAOAOAOA ... OAOAOAOA
   B B B B  B B B B  B B B B  B B B B      B B B B  B B B B                                 b b b b  b b b b      b b  i i i i  i i i i  i i i i

 The InpStructArray[] is also use for intermediate values which are used in the macro for the flip flop.
 In the picture above they are shown with an 'i'.


 Two LED stripes:
 ~~~~~~~~~~~~~~~~
 The example uses the FastLED option to control two independent LED stripes. The second stripe is used
 to drive the LEDs in the push buttons around the model railway which are used to activate special
 "Push Button Actions".
 In the setup there are two "FastLED.addLeds" calls for this purpose. This is one great feature of
 the FastLED library.


 Additional Libraries:
 ~~~~~~~~~~~~~~~~~~~~~
 The TimerOne and the DIO2 library must be installed in addition if you got the error message
   "..fatal error: TimerOne.h (DIO2.h): No such file or directory" the corresponding library is missing.

 The libraries could be installed from the Arduino IDE:

   Arduino IDE:  Sketch / Include library / Manage libraries
                 Type "FastLED" or "DIO2" in the "Filter your search..." field
                 Select the entry and click "Install"

   Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                 "FastLED"  oder "DIO2" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                 Gefundenen Eintrag auswaehlen und "Install" anklicken

 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. "Just" copy the configuration
 lines and eventual the macros and adapt the first LED to avoid overlapping (First parameter
 in the configuration line). I'm curious if you could manage this...

 Hardware:
 ~~~~~~~~~
 The example can be used with an Arduino compatible board (Uno, Nano, Mega, ...)  and one ore two
 WS2812 LED stripes.
 The first stripe should have 148 LEDs. It's connected to pin D6. It shows the state of the switches in
 keyboard 1 and the "Push Button Action" state.
 The second stripe is connected to A2 and should have 6 LED channels. Each color represents the LED in the
 buttons the "Push Button Action".  6 * 3 = 18. This LEDs are normally driven by 6 WS2811 modules.

 It's also possible to use just one stripe and connect it alternating at pin D6 and A2.

 In addition some CD4017, switches, diodes and 47K resistors are required.
 (See the schematics in the documentation).
 Attention: Don't forgot the pull own resistors in the button lines ! Otherwise the LEDs show random
 values. This is also quite nice and it demonstrates the update speed of the library.

 In the "extras" directory of the library there is a file PushButtonAction_4017.zip which contains
 the schematic and printed circuit board for this example.
*/

#define FASTLED_INTERNAL     // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"         // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                             // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                             //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                             //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include <TimerOne.h>        // The TimerOne library must be installed in addition if you got the error message "..fatal error: TimerOne.h: No such file or directory"
#include <DIO2.h>            // The library for fast digital I/O functions must be installed also
                             // Installation see "FastLED" installation above

#include "MobaLedLib.h"      // Use the Moba Led Library

#define NUM_LEDS_1       148 // Number of LEDs for the first stripe
#define NUM_LEDS_2         6 // Number WS2811 modules for the second "stripe"

#if defined (__AVR_ATmega168__) || defined (__AVR_ATmega328P__)
#define LED_DO_PIN_1       6 // Pin D6 is connected to the LED stripe 1
#define LED_DO_PIN_2      A2 // Pin A2 is connected to the LED stripe 2
#else 
#error this example does not support this plattform
#endif

#define CTR_CHANNELS_1    10                   // Number of used counter channels for keyboard 1. Up to 10 if one CD4017 is used, up to 18 if two CD4017 are used, ...
#define BUTTON_INP_LIST_1 2,7,8,9,10,11,12,A1  // Comma separated list of the button input pins (Example use A0-A3: 14, 15, 16, 17)
#define CTR_CHANNELS_2    18                   // Number of used counter channels for keyboard 2. Up to 10 if one 4017 is used, up to 18 if two CD4017 are used, ...
#define BUTTON_INP_LIST_2 A0                   // Comma separated list of the button input pins (Example use A0-A3: 14, 15, 16, 17)   Attention: Not A6, A7 (See blow)
#define CLK_PIN           A4                   // Pin number used for the CD4017 clock (Example 18 = A4)
#define RESET_PIN         A5                   // Pin number used for the CD4017 reset (Example 19 = A5)
                                               // The digital pins D2..D13 could be used also.
                                               // Attention the analog pins A6 & A7 of the Nano can't be used as digital input/output
                                               // => They can't be used for to read the keys

#define NUM_LEDS         NUM_LEDS_1+NUM_LEDS_2 // Necessary for the checking of the LED count in the MobaLedLib

#include "Keys_4017.h"       // Keyboard library which uses the CD4017 counter to save Arduino pins. Attention: The pins (CLK_PIN, ...) must be defined prior.

#define START_SWITCHES_2       128   // Define the start number for the second keyboard. The first 128 inputs are directly mapped to the LEDs in case the keyboard 1 is enlarged
#define START_INTERNAL_INPUTS  (START_SWITCHES_2 + KEYS_ARRAY_BYTE_SIZE_2*8) // The internal variables start after the second switch group

// *** Macros ***
#define myConst(Nr) Const(Nr, C_GREEN, Nr, 0,225) //Simplified Const() function

#define PushButtAct(LEDNr, ChNr, Nr)     T_FlipFlopResetTimeout(Nr+START_INTERNAL_INPUTS, Nr+START_SWITCHES_2, SI_0, 10 sec)   \
                                         Blink3(LEDNr+NUM_LEDS_1, ChNr,    Nr+START_INTERNAL_INPUTS, 500 ms,500 ms, 0,224,100) \
                                         Const( Nr+128,           C_BLUE,  Nr+START_INTERNAL_INPUTS, 0, 225)


//*******************************************************************
// *** Configuration array which defines the behavior of the LEDs ***
MobaLedLib_Configuration()
  {
  myConst(0)    myConst(32)   myConst(64)   myConst(96)
  myConst(1)    myConst(33)   myConst(65)   myConst(97)
  myConst(2)    myConst(34)   myConst(66)   myConst(98)
  myConst(3)    myConst(35)   myConst(67)   myConst(99)
  myConst(4)    myConst(36)   myConst(68)   myConst(100)
  myConst(5)    myConst(37)   myConst(69)   myConst(101)
  myConst(6)    myConst(38)   myConst(70)   myConst(102)
  myConst(7)    myConst(39)   myConst(71)   myConst(103)
  myConst(8)    myConst(40)   myConst(72)   myConst(104)
  myConst(9)    myConst(41)   myConst(73)   myConst(105)
  myConst(10)   myConst(42)   myConst(74)   myConst(106)
  myConst(11)   myConst(43)   myConst(75)   myConst(107)
  myConst(12)   myConst(44)   myConst(76)   myConst(108)
  myConst(13)   myConst(45)   myConst(77)   myConst(109)
  myConst(14)   myConst(46)   myConst(78)   myConst(110)
  myConst(15)   myConst(47)   myConst(79)   myConst(111)
  myConst(16)   myConst(48)   myConst(80)   myConst(112)
  myConst(17)   myConst(49)   myConst(81)   myConst(113)
  myConst(18)   myConst(50)   myConst(82)   myConst(114)
  myConst(19)   myConst(51)   myConst(83)   myConst(115)
  myConst(20)   myConst(52)   myConst(84)   myConst(116)
  myConst(21)   myConst(53)   myConst(85)   myConst(117)
  myConst(22)   myConst(54)   myConst(86)   myConst(118)
  myConst(23)   myConst(55)   myConst(87)   myConst(119)
  myConst(24)   myConst(56)   myConst(88)   myConst(120)
  myConst(25)   myConst(57)   myConst(89)   myConst(121)
  myConst(26)   myConst(58)   myConst(90)   myConst(122)
  myConst(27)   myConst(59)   myConst(91)   myConst(123)
  myConst(28)   myConst(60)   myConst(92)   myConst(124)
  myConst(29)   myConst(61)   myConst(93)   myConst(125)
  myConst(30)   myConst(62)   myConst(94)   myConst(126)
  myConst(31)   myConst(63)   myConst(95)   myConst(127)

  PushButtAct(0,  C1, 0)   // Sw 1
  PushButtAct(0,  C2, 1)   // Sw 2
  PushButtAct(0,  C3, 2)   // Sw 3
  PushButtAct(1,  C1, 3)   // Sw 4
  PushButtAct(1,  C2, 4)   // Sw 5
  PushButtAct(1,  C3, 5)   // Sw 6
  PushButtAct(2,  C1, 6)   // Sw 7
  PushButtAct(2,  C2, 7)   // Sw 8
  PushButtAct(2,  C3, 8)   // Sw 9
  // Second 4017 => First channel can't be used => LEDs are shifted by one
  PushButtAct(3,  C2, 9 )  // Sw 10
  PushButtAct(3,  C3, 10)  // Sw 11
  PushButtAct(4,  C1, 11)  // Sw 12
  PushButtAct(4,  C2, 12)  // Sw 13
  PushButtAct(4,  C3, 13)  // Sw 14
  PushButtAct(5,  C1, 14)  // Sw 15
  PushButtAct(5,  C2, 15)  // Sw 16
  PushButtAct(5,  C3, 16)  // Sw 17
  PushButtAct(3,  C3, 17)  // Sw 18 Last LED is driven from the unused channel of the first WS2811 module

  EndCfg // End of the configuration
  };
//*******************************************************************


CRGB leds[NUM_LEDS];  // Define the array of leds

MobaLedLib_Create(leds); // Define the MobaLedLib instance

LED_Heartbeat_C LED_Heartbeat(LED_BUILTIN); // Use the build in LED as heartbeat

//----------
void setup(){
//----------
// This function is called once to initialize the program
//
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN_1>(leds,            NUM_LEDS_1);
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN_2>(leds+NUM_LEDS_1, NUM_LEDS_2);

  Keys_4017_Setup(); // Initialize the keyboard scanning process
}

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously
//
  MobaLedLib_Copy_to_InpStruct(Keys_Array_1, KEYS_ARRAY_BYTE_SIZE_1, 0);                 // Copy the key states to the input structure
  MobaLedLib_Copy_to_InpStruct(Keys_Array_2, KEYS_ARRAY_BYTE_SIZE_2, START_SWITCHES_2);  //   "            "

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

