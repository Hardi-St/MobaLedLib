# MobaLedLib
###Arduino library for controlling LEDs and other components on a model railway.


**New: Excel user interface to configure the LEDs without programming at all improved by the engagement of Pattern_Configurator and Program_Generator!**

To start the excel program press the Win+r key together and copy the following line
into the "Run" dialog and press enter:
  %USERPROFILE%\Documents\Arduino\libraries\Mobaledlib\extras\LEDs_AutoProg\Prog_Generator_MobaLedLib.xlsm

**The use of RGB LEDs offers fascinating possibilities for lighting a model railway:**
- **Infinite number of lighting effects** - Brightness and color of each LED can be
  set individually and dynamically
- **Easiest cabling** - The objects (houses, traffic lights, ...) are simply plugged
  into a multiple socket
- **Extremely affordable** - 100 LEDs for the price of three "normal" lights
- Up to **256 RGB LEDs (768 channels)** can be controlled by an Arduino
- Controlling other components is just as easy
  - Sound modules
  - Servo and stepper motors
  - Power devices
- Additional module to read 80 and more switches with 10 signal wires
- Stand-alone operation or networked with other components possible

The lighting and some additional functions are controlled via the WS2812 / WS2811 chips.
For this, only a single signal is required which is forwarded from one chip to the next.
This makes the wiring extremely easy.

A lot of **examples** show the usage of the library. They could be use as startup for own programs.


This library is described in detail here:
https://www.stummiforum.de/viewtopic.php?f=7&t=165060&sid=e3b7f6122766c330d04cecf18b7b4878
Several videos which demonstrate the library are available in the following pages in the forum
You could also search for MobaLedLib in the internet.

ATTENTION: Always use the actual Arduino IDE
(Old versions 1.6.x and below don't support the directory structure used in the library examples)


Installation from the Arduino IDE:
  Sketch / Include library / Manage libraries
  Type "MobaLedLib" in the "Filter your search..." field
  Select the entry and click "Install"


Installation in der deutschen Arduino IDE:
  Sketch / Bibliothek einbinden / Bibliothek verwalten
  "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
  Gefundenen Eintrag auswaehlen und "Install" anklicken


Questions / suggestions / praise / ...
  MobaLedLib@gmx.de



**Revision History:**

**Ver.: 0.7.0** 20.12.18:
- First released version

**Ver.: 0.7.5**  19.01.19:
- Added examples:
  - Burning house (Push button action with fire, smoke, sound and fire truck). Shown in the video: https://vimeo.com/311006857
  - DCC (Digital Command Control) decoder example with two Arduinos. Shown in the video: https://vimeo.com/311996452
- Added function Bin_InCh_to_TmpVar() and RGB_Heartbeat2()
- Added zip file with the RGB LED distribution PCBs
- Improved the fire algorithm
- Corrected the binary mode of the counter (CF_BINARY)
- Corrected the initialization of the Pattern function if the Goto mode is used.
- Updated the English documentation to the same state like the German.
- Using the correct version of the Pattern_Configurator.xlsb

**Ver.: 0.7.7** 17.02.19:
- Added Support for Sound modul JQ6500
- Corrected the random mode of the Counter() function
- Improved the serial input debug function
- Moved the Heartbeat function in own H-file"
- 12.03.19:
  - added ButtonNOff() macro

**Ver.: 0.7.8** 09.04.19:
- Added examples
  - 00.Overview
  - 25.Analog_Push_Button

**Ver.: 0.7.9** 09.07.19:
  Moved the Pattern_Configurator to the "extras" directory
  and updated the excel version.
  - Nice graphical display of the LED brightness and the Goto mode
  - Reduced the size by extracting the examples
  - Added a menu to save/load/delete example sheeds
  - User interface is automatically switched to English (Example descriptions still German)

**Ver.: 0.8.0** 16.07.19:
  - Added new assembly document for the main PCB from Alf and Armin

**Ver.: 0.9.0** 27.09.19:
- New Excel User interface to configure the LEDs
- Single LED functions for the House() macro
- Macros for Light signals, Construction lightning, ... added to the library

**Ver.: 0.9.1** 06.10.19:
- Corrected the functions Flash(), RandWelding() and PushButton_w_LED_0_? in the Prog_Generator
- Corrected single LEDs in the House() and GasLights() function.

**Ver.: 0.9.2** 30.10.19:
- Corrected the XPattern function (used in the Light signals)
- Corrected the Excel Programs

**Ver.: 0.9.2** 08.12.19:
- Engagement of Pattern_Configurator and Program_Generator => Easy exchange between the tools
- CheckColors function: Live view of the colors and brightens of the LEDs
- Existing lines could be edited in the Prog_Generator
