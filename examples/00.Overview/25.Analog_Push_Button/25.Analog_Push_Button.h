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


 10 Analog Push Buttons                                                                    by Hardi   30.03.19
 ~~~~~~~~~~~~~~~~~~~~~~

 This example demonstrates the usage of the MobaLedLib for several "Push Button" actions.
 A big attraction in the "Miniatur Wunderland" are the "Druck Knopf Aktionen".
 Around the model railway there are buttons which could be pressed by the visitors which start
 a special action on the layout.
 Here is an other example how such actions could be implemented using the MobaLedLib.
 (See also: 22.Burning_House)
 In this example up to 10 push buttons are connected to one single analog input of the Arduino.
 Each of the button has a different resistor which is used to generate a individual voltage which could
 be detected by the processor. This is a very simple way of connecting buttons to the Auduino. More buttons
 could be connected to other analog channel of the arduino. So with 8 analog inputs 80 buttons could be used.
 The disadvantage of this method is that its not possible to press several buttons simultanusly (At the sane
 analog input).

 This example uses 4 buttons to activate 4 different actions:

 Button 1:
 The first button activates 6 gas street lights. The lights start one after the other randomly. It takes
 a while untel the lights reache the full brightness. Sometimes they flicker because of wind or gas drops.
 One lamp has got a problem. It doesen't reach the full brightness.

 Button 2:
 With the second button the lights of a office builduig with neon lights are activated. The workers in
 the different turn on their lights randomly after a while.

 Button 3:
 This button simulates a fire truck with flashing blue lights. The white front light and the turn indicator
 (yellow) is also flashing. This effect looks much better if the LEDs of a model car are used. In this case
 the LEDs are driven be WS2811 modules individualy. See the "07.Macro_Fire_truck" example for more details.

 Button 4:
 If a sound modul is connected to the LED channel 15 a random sound will be played if the button is pressed.
 Instead of a sound modul a LED could also be used. It will flash short if the button is pressed.

 Button 5:
 This is a more complex example. It is used to illuminate a castle on our layout. The button has got two
 states. If the button is pressed the first time the castle is spotlighted with white. When the button
 is pressed a second time the color of the spots change slowly.

 Button LEDs:
 Each push button has got an own LED to indicate the state. The LED could be build-in the button or located
 near the button. In the buid-in LED is driven by a WS2811 modul. If external LEDs are usesd its recomended
 to use a RGB LED to be able to show different colors. The example asumes that RBG LEDs are used.
 The first 4 buttons LEDs change the color from Blue to Green if the button is pressed. For the button LED 5
 a different visualisation has been choosen to be able to show the different states. The LED is flashing
 once if the button is pressed the first time. It flashes two times if the button is pressed a second time.
 A third press to the button wil activate state 1 again. This is shown by flashing once. The action could
 be disabled by holding the button for 3 seconds.


 Other examples:
 ~~~~~~~~~~~~~~~
 This example could be combined with other MobaLedLib examples. Just copy the configuration lines and
 eventual the macros and adapt the first LED to avoid overlapping (First parameter in the configuration line).


 Hardware:
 ~~~~~~~~~
 The example needs at least 5 push buttons which are connected to analog input A6
 (See definition of ANA_BUTTON_PIN)
 The buttons have an individual resistor to be able to distinguish which button is pressed.         .........................
 In addition a LED stripe with 32 LEDs should be connected to pin D6.                               :       Main board
                                                                                                    :    +5V
 Only two wires areneedet to read in 10 buttons.                                                    :     |
 The buttons are connected like this:                                                               :    [R] 22K
                                                                                                    :     |
  .---------o---------o---------o---------o---------o---------o---------o---------o---------o-------+-----o-------A6 Nano
  |         |         |         |         |         |         |         |         |         |       :     |
  *         *         *         *         *         *         *         *         *         *       :     |
 /         /         /         /         /         /         /         /         /         /        :    === 1uF
  |         |         |         |         |         |         |         |         |         |       :     |
 [R]       [R]       [R]       [R]       [R]       [R]       [R]       [R]       [R]       [R]      :     |
  | 0 Ohm   |2k2      |4k7      |8k2      |15k      |22k      |33k      |47k      |68k      |150k   :     |
  '---------o---------o---------o---------o---------o---------o---------o---------o---------o-------+-----o------ GND
                                                                                                    :
                                                                                                    '.......................
 Attention don't forgot the 22K resistor to +5V and the 1uF capacitor on the main board next to the Nano
*/



#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "MobaLedLib.h"  // Use the Moba Led Library

#define NUM_LEDS        32  // Number of LEDs
#define LED_DO_PIN      6   // Pin D6 is connected to the LED stripe
#define ANA_BUTTON_PIN  A6  // Pin which is connected to the analog buttons
                            // Attention: Use an other analog input if a Arduino UNO is used.


#define INCH_GASLIGHT_A 100 // Define names for the input channels to be able to change them easily.
#define INCH_HOUSE_A    101 // In this small example this is not necessary, but it's useful in a
#define INCH_FIRE_TRUCK 102
#define INCH_SOUND      103
#define TMPCH_CASTLE    104 // The castle illumination uses 3 temporary variables for the three states Off, White, Color
//      TMPCH_CASTLE    105
//      TMPCH_CASTLE    106
#define UNUSED6         107
#define UNUSED7         108
#define UNUSED8         109
#define UNUSED9         110
#define UNUSED10        111


#define GASLIGHT_A   0   // Define names for the LED numbers of the consumers.
//      GASLIGHT_A   1   // In a real setup the names could be: "RailwayStation", "Town_Hall", "Pub", ...
//      GASLIGHT_A   2   // Each room get's an own name.
//      GASLIGHT_A   3   // Only the first LED numbers are used in the configuration,
//      GASLIGHT_A   4   // But it's a good practice to list the other rooms to because
//      GASLIGHT_A   5   // then the corrosponding numbers are increasing without gaps.
#define HOUSE_A      6   // This is usefull if a aditional house is inserted.
//      HOUSE_A      7   // In this case th sequence could easyly be checked / updated and
//      HOUSE_A      8   // the aditional lines could be used for documentation
//      HOUSE_A      9
#define FIRE_TRUCK   10  // Vorderlicht blinken Dunkel 15/ Hell 128 (Weis)
//      FIRE_TRUCK   11  // Blaulicht rechts
//      FIRE_TRUCK   12  // Blaulicht Links (Andere Periode damit nicht synchron)
//      FIRE_TRUCK   13  // Blinker hinten (Gelb)
//      FIRE_TRUCK   14  // Ruecklicht     (Rot)
#define SOUND_LED    15  // "LED" Channel for the sound modul.
#define CASTLE_LED   16
//      CASTLE_LED   17
//      CASTLE_LED   18
//      CASTLE_LED   19
//      CASTLE_LED   20
//      CASTLE_LED   21
#define BUTTON_LED1  22
#define BUTTON_LED2  23
#define BUTTON_LED3  24
#define BUTTON_LED4  25
#define BUTTON_LED5  26
#define BUTTON_LED6  27
#define BUTTON_LED7  28
#define BUTTON_LED8  29
#define BUTTON_LED9  30
#define BUTTON_LED10 31


// **** Macro definition: Fire truck for tests with RGB LED stipe if no vehicle is available ****
#define Fire_truck(LED, InCh)                                                                                            \
    Blink2    (LED,   C_ALL, InCh, 500 ms, 500 ms, 15,  128) /* Vorderlicht Blinken Dunkel 15/ Hell 128 (Weis)        */ \
    BlueLight1(LED+1, C3,    InCh)                           /* Blaulicht rechts                                      */ \
    BlueLight2(LED+2, C3,    InCh)                           /* Blaulicht Links (Andere Periode damit nicht synchron) */ \
    Blink2    (LED+3, C12,   InCh, 500 ms, 500 ms,  0,  128) /* Blinker hinten (Gelb)                                 */ \
    Const     (LED+4, C1,    InCh,                  0,   25) /* Ruecklicht     (Rot)                                  */


//------------------------------- Castle Illumination --------------------------------------------------------

#define Castle_Illu_Fade(LED0, InNr)                      /* Maximal time for one time interval 65.535 Sec */           \
    APatternT6(LED0+0,12, InNr,3,0,255,0,PM_HSV,3 Sek,    30 Sek,40 Sek,38 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
    APatternT6(LED0+1,12, InNr,3,0,255,0,PM_HSV,3 Sek+100,30 Sek,40 Sek,40 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
    APatternT6(LED0+2,12, InNr,3,0,255,0,PM_HSV,3 Sek+200,30 Sek,40 Sek,30 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
    APatternT6(LED0+3,12, InNr,3,0,255,0,PM_HSV,3 Sek+300,30 Sek,40 Sek,40 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
    APatternT6(LED0+4,12, InNr,3,0,255,0,PM_HSV,9 Sek+300,31 Sek,40 Sek,45 Sek,2 Sek,500,15,255,255,240,255,255,0,0,0)  \
    APatternT6(LED0+5,12, InNr,3,0,255,0,PM_HSV,8 Sek+300,35 Sek,40 Sek,40 Sek,5 Sek,500,15,255,255,240,255,255,0,0,0)

#define Castle_Off(LED,InNr)   XPatternT1(LED,192,InNr,18,0,255,0,PM_SEQUENZ_STOP,2 Sek,0,0,0)  // Slowly turn off the LIGHTS
#define Castle_On(LED,InNr)    XPatternT1(LED,128,InNr,18,0,255,0,PM_SEQUENZ_STOP,500 ms,7,0,252,0,240,31,192,255,3,255,127,252,255,15)

// Uses: 6 LEDs, 3 Temp variables
#define Castle_Illumination(LED0, B_LED, Cx, InNr, TmpNr, Timeout)  \
    PushButton_w_LED_0_2( B_LED, Cx, InNr, TmpNr, 1, Timeout)       \
    Castle_Off        (LED0, TmpNr+0 )                              \
    Castle_On         (LED0, TmpNr+1 )                              \
    Castle_Illu_Fade  (LED0, TmpNr+2)                               \

//--------------- Push Buttons which toggle a variable and the state of a LED which shows the button status ---------
#define BUTTON_LED_TYP 1

#if BUTTON_LED_TYP == 0   // Flashing LED with one Color or one single LED
  #define PushButton_w_LED_OnOff(B_LED, B_LED_Cx, Unused, InNr, TmpNr, Timeout)        \
      T_FlipFlopResetTimeout(TmpNr, InNr, SI_0, Timeout)                               \
      Blink3(B_LED,B_LED_Cx,TmpNr,500,500,255,0,63)

#elif BUTTON_LED_TYP == 1 // RGB LED with two colors for on/off
  #define PushButton_w_LED_OnOff(B_LED, cx_LED_Off, cx_LED_On, InNr, TmpNr, Timeout)   \
      T_FlipFlopResetTimeout(TmpNr, InNr, SI_0, Timeout)                               \
      Const(B_LED, cx_LED_On,   TmpNr, 0, 255)                                         \
      Const(B_LED, cx_LED_Off,  TmpNr, 63, 0)
#endif



//*******************************************************************
// *** Configuration array which defines the behavior of the sound module which is addressed like a LED ***
MobaLedLib_Configuration()
  {
  // *** Read the buttons, generate a toggling variable and contoll the Buton LEDs ***
  //
  //                              B_LEDNr       Mask_Off Mask_On  InNr OutNr            Timeout)
  PushButton_w_LED_OnOff(         BUTTON_LED1,  C_BLUE,  C_GREEN, 0,   INCH_GASLIGHT_A, 0 Min) // Gas street lights (No timeout)
  PushButton_w_LED_OnOff(         BUTTON_LED2,  C_BLUE,  C_GREEN, 1,   INCH_HOUSE_A,    0 Min) // Office building with neon lights (no timeout)
  PushButton_w_LED_OnOff(         BUTTON_LED3,  C_BLUE,  C_GREEN, 2,   INCH_FIRE_TRUCK, 3 Min) // Fire truck with flashing lights
  PushButton_w_LED_OnOff(         BUTTON_LED4,  C_BLUE,  C_GREEN, 3,   INCH_SOUND,      1 Sek) // Random Sound

  // Special push buton action. First press: White, Second press Colors
  //                  FirstLED    B_LEDNr       Mask_Off          InNr OutNr            Timeout)
  Castle_Illumination(CASTLE_LED, BUTTON_LED5,  C_BLUE,           4,   TMPCH_CASTLE,    5 Min) // The castle iluminsteion has two states

  PushButton_w_LED_OnOff(         BUTTON_LED6,  C_BLUE,  C_RED,   5,   UNUSED6,         5 Min) // Currently unused buttons.
  PushButton_w_LED_OnOff(         BUTTON_LED7,  C_BLUE,  C_RED,   6,   UNUSED7,         5 Min) // They are added to check the
  PushButton_w_LED_OnOff(         BUTTON_LED8,  C_BLUE,  C_RED,   7,   UNUSED8,         5 Min) // reading of the analog signals
  PushButton_w_LED_OnOff(         BUTTON_LED9,  C_BLUE,  C_YELLOW,8,   UNUSED9,         1 Sek) // This two buttons are disabled after
  PushButton_w_LED_OnOff(         BUTTON_LED10, C_BLUE,  C_YELLOW,9,   UNUSED10,        1 Sek) // one second. They could be used for sounds...

  GasLights(GASLIGHT_A, INCH_GASLIGHT_A,    GAS_LIGHT, GAS_LIGHT, GAS_LIGHTD, GAS_LIGHT, GAS_LIGHT, GAS_LIGHT)  // Street lights with gas which take a while to get the full brightness. One Light is broken (Dark)
  House(    HOUSE_A,    INCH_HOUSE_A, 3, 3, NEON_LIGHT,NEON_LIGHTL, NEON_LIGHTL, NEON_LIGHT)                    // House with 4 rooms (Office building with neon lights)

  //          FirstLED TmpNr
  Fire_truck (FIRE_TRUCK,  INCH_FIRE_TRUCK)     // Uses two five RGB LEDs or two WS2811 channels (see above)
  Sound_PlayRandom(SOUND_LED, INCH_SOUND, 14)  // Play a random sound. If no Sound modul is connected the corrosponding LED will flash short

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
  FastLED.addLeds<NEOPIXEL, LED_DO_PIN>(leds, NUM_LEDS);
}


// Table containing the delta from one compare value to the next. We don't store the 16 bit values to save memory.
const PROGMEM uint8_t DeltaTab[10] = { 88, 86, 89, 113, 112, 96, 88, 77, 94, 101};

//:::::::::::::::::::::::
class Analog_Buttons10_C
//:::::::::::::::::::::::
{
public:
  //-------------------------------------
  Analog_Buttons10_C(uint8_t AnalogPinNr) // Constructor
  //-------------------------------------
  {
    APin = AnalogPinNr;
    LastCheck = 0;
    LastKey   = 0xFFFF;
    SameCnt   = 0;
  }

  //------------
  uint16_t Get()
  //------------
  // Must be called periodic in the loop() function
  {
    uint8_t t = millis() % 0xFF;
    if ((uint8_t)(t - LastCheck) > 20) // Check every 20 ms
       {
       LastCheck = t;
       uint16_t Key = Get_Act_Key();
       if (Key == LastKey)
            SameCnt++;
       else SameCnt = 0;
       LastKey = Key;
       }
    if (SameCnt >= 4)    // Wenn 4 mal hintereinander der gleiche Wert gelesen wird, dann wird der Taster als gedrueckt gemeldet
         return LastKey; // Das ist wichtig weil der Analog wert mit einem 1uF Kondensator gefiltert wird
    else return 0;
  }

private:
  //--------------------
  uint16_t Get_Act_Key()
  //--------------------
 {
    const uint8_t *p = DeltaTab; // Table containing the delta from one compare value to the next
    uint16_t Cmp = 0;
    uint16_t Bit = 1;
    uint16_t val = analogRead(APin);
    while (Bit < 513)
        {
        Cmp += pgm_read_byte_near(p);
        if (val < Cmp) return Bit;
        p++;
        Bit <<= 1;
        }
    return 0;
  }

private:
  uint8_t  APin;
  uint8_t  LastCheck;
  uint16_t LastKey;
  uint8_t  SameCnt;
};

Analog_Buttons10_C AButtons(ANA_BUTTON_PIN);

//---------
void loop(){
//---------
// This function contains the main loop which is executed continuously

  uint16_t Button = AButtons.Get();
  MobaLedLib_Copy_to_InpStruct((uint8_t*)&Button, 2, 0);  // Copy the buttons states to the input structure

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
 BUttons   | [ ]A7              INT0/D2[ ] |   D2
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
           | [ ]5v     | A |                9[ ]~|   .   -> push button 2 (B)  --.
           | [ ]GND   -| R |-               8[B] |   B0  -> push button 1 (G)  --o-- GND
           | [ ]GND   -| D |-                    |                               |
           | [ ]Vin   -| U |-               7[A] |   D7  -> push button 0 (R)  --'
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

