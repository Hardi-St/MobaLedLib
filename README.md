# MobaLedLib
## Arduino library for controlling LEDs and other components on a model railway.

**New in release 3.0.0: ESP32 support driving up to 49152 LEDs / DMX512 support / Bootloader Update and "New Bootloader full Mem" / Important bug fixes**

**Excel user interface to configure the LEDs without programming at all improved by the engagement of Pattern Configurator and Program Generator!**

To start the excel program press the Win+r key together and copy the following line
into the "Run" dialog and press enter:
  %USERPROFILE%\Documents\Arduino\libraries\Mobaledlib\extras\Prog_Generator_MobaLedLib.xlsm

**The use of RGB LEDs offers fascinating possibilities for lighting a model railway:**
- **Infinite number of lighting effects** - Brightness and color of each LED can be
  set individually and dynamically
- **Easiest cabling** - The objects (houses, traffic lights, ...) are simply plugged
  into a multiple socket
- **Extremely affordable** - 100 LEDs for the price of three "normal" lights
- Up to **256 RGB LEDs (768 channels)** can be controlled by an Arduino
- Up to **16384 RGB LEDs (49152 channels)** can be controlled by an ESP32
- Controlling other components is just as easy
  - Sound modules
  - Servo and stepper motors
  - Charlieplexing light signals
  - Power devices
- Additional module to read 80 and more switches with 10 signal wires
- Stand-alone operation or networked with other components possible

The lighting and some additional functions are controlled via the WS2812 / WS2811 chips.
For this, only a single signal is required which is forwarded from one chip to the next.
This makes the wiring extremely easy.

A lot of **examples** show the usage of the library. They could be use as startup for own programs.

A huge wiki describes the complete MobaLedLib in detail: https://wiki.mobaledlib.de/doku.php<br/>
This library is used and supported by a big community, see https://www.stummiforum.de/viewtopic.php?f=7&t=165060.
All software and hardware parts are free of license and avaiable for download.

Several videos demonstrate the possibilites of the library. You will find a lot of articles when searching for MobaLedLib in the internet.

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

**Ver.: 0.9.3** 08.12.19:
- Engagement of Pattern_Configurator and Program_Generator => Easy exchange between the tools
- CheckColors function: Live view of the colors and brightens of the LEDs
- Existing lines could be edited in the Prog_Generator

**Ver.: 1.0.0** 16.01.20:
- New Charlieplexing program for the Servo_LED module which could be used to drive Viessmann and other "Multiplexed" light signals
- Configuration upload from the Pattern_Configurator over the LEDs "Bus" to the Charlieplexing module. The module is configured on the railway layout.
- Direct programming support for the Tiny_UniProg module from excel (One click to install the software)
- Flashing of the software for the Charlieplexing Module from excel (One click to install the software)
- Enhanced Color Test program with a lot of new features
- New Black and White TV simulation (configurable)
- Simulation of defective neon lights added
- 1001 of other small changes and improvements

**Ver.: 1.0.1** 17.01.20:
- Corrected the upload in version 1.0.0 because some files have not been update ;-(

**Ver.: 1.0.2** 18.04.20:
- Test of switch and variables

**Ver.: 1.0.3** 01.05.20:
- Test of additional LED channels and EEPROM Storage

**Ver.: 1.0.4** 23.05.20:
- Automatically detecting COM port the Arduino is connected to
- Improved the HV_Reset() in the Tiny_UniProg according to Juergens tipp
- New macros InCh_to_TmpVar1() and Bin_InCh_to_TmpVar1() which start with state 1 instead of 0
- New macros for Servos and Herzstueck polarisation
- New Push Button macros which read DCC and hardware buttons
- Added macros Andreaskreuz with lamp test

**Ver.: 1.0.5** 31.05.20:
- Automatically install all libraries
- Using the Sketchbook path for the working directory

**Ver.: 1.0.6** 06.06.20:
- Added Mishas LED Previev and Mux functions to the Pattern_Configurator
- Using Harolds new pyProgGen_MobaLedLib
- Using the new USB port detection also in the Pattern_Configurator

**Ver.: 1.0.7** 07.06.20:
- Corrected the LED Animation, the "Start LedNr" in combination with "HerzHerz_BiRelais()"

**Ver.: 1.9.4** 14.06.20:
- Added Misha's Multiplexer to the Prog_Generator

**Ver.: 1.9.5** 15.06.20:
- The versions 1.0.2 - 1.9.4 are not released test versions.
- Since there are a huge number of changes since version 1.0.1 all details are described here: https://www.stummiforum.de/viewtopic.php?f=7&t=165060&sd=a&start=2410

**Ver.: 1.9.6** 22.07.20:
- Preview LEDs in the Pattern_Configuarator could be moved on top of a picture or under a transparent picture (by Misha)
- Speedup building and uploading of the Arduino program 10 sec. instead of 23 sec. (by Juergen)

**Ver.: 1.9.6 C** 28.07.20:
- Corrected Servo programming (Flash was erased when setting the Reset pin as output)
- Corrected decimal separator problem when loading pattern examples
- New macros for servo with SMD WS2811 Herz_BiRelais_V1...

**Ver.: 1.9.6 D** 04.08.20:
- Limit the maximal servo value to 210 (Old 220) to avoid promlems with measurement errors at 2kHz
- Additional Delay and check if the old directory has been deleted when updating the Beta version

**Ver.: 1.9.6 E** 07.08.20?:
- Deletted >100000 columns in the DCC sheet which slowed down the loading of .MLL_pfg files
- Added a status display when loading the .MLL_pfg files
- Don't read/save the "Examples sheet from/to .MLL_pfg file

**Ver.: 1.9.6 F** 07.10.20:
- Programming of the fast bootloader added
- Jürgen has added "Update_Start_LedNr" to the end of Read_PGF_from_String_V1_0() because other wise NUM_LEDS is 0
- New function "Mainboard_LED(MB_LED, InCh)" which could be used to use the mainboard LEDs as status LEDs
- Added macro "WeldingCont()" which continuously flickers while the input is active.
- Don't use the Heartbeat LED at PIN A3 if the CAN bus is used AND the SwitchB or SwitchC is used.
- Generate an error message if Mainboard_LED(4..) is used together with SwitchB or SwitchC.
- Created an example file to test the MobaLedLib main board: "Mainboard_Hardware_Test.MLL_pgf"
  This file is stored in the directory "Prog_Generator_Examples" which is copied to the library destination at startup.
- Added DayAndNightTimer which could be used with then Scheduler function

**Ver.: 1.9.6 G** 10.10.20:
- Added additional pins to the Mainboard_LED function. Now nearly every pin could be used as LED pin (New channels 0, 5-16).
- New method "LED_to_Var()" to set variables controlled by the LED values.
- Improved the "Mainboard_Hardware_Test.MLL_pgf". Now the mainboard could be tested without the PushButton4017 board.

**Ver.: 1.9.6 H** 10.10.20:
- Improved the loading of MLL_pgf files

**Ver.: 1.9.6 I** 10.10.20:
-  Possibilitiy added to ignore problems with the baud rate detection

**Ver.: 1.9.6 J** 11.10.20:
- The Mainboard_LED function also acceppts the arduino pin numbers D2-D5, D7-D13 amd A0-A5

**Ver.: 1.9.6 K** 16.10.20:
- Don't gray out the other rooms in the House/Gaslight dialog. Instead the important buttons use bold font
- User interface:
  - Corrected the entering of selextrix data and the position of the USB simulation buttons
  - Corrected the "Dialog" Button. The "Type selection" dialog was called twice in DCC mode.
- Disabled the "ENABLE_CRITICAL_EVENTS_WB" to hopefully get rid of the crash which is generated if lines are deleted in the Pattern_Configurator. By disabling this events the LED numbers are not updated if lines ade hidden or shown again.
- Corrected the NEON_DEF2D entry. Channel 1 was used instead of channel 2. This caused the occupation of a new RGB channel if NEON_DEF1D and NEON_DEF2D was used in a sequence
- Improve the detection probability in "DetectArduino()". Prior the arduino was not always detected at the first trial.
- Corrected the error detection for the ATtiny upload
- Increased the number of Time entries from 30 to 64 and corrected the entries 24-30
- New Charliplexing software which supports the 64 time channels

**Ver.: 2.0.0** 18.10.20:
- Support for the new buffer gate on mainboard version 1.7 added

**Ver.: 2.1.0** 02.11.20:
- 8% additional configuration memory by changing the reserved size of the fast bootloader from 2K to 512 byte
- Corrected the support for 64 time entries in the pattern_Configurator. Unfortunately the prior changes have been made in a wrong worksheet and not in the Main sheet => They have been lost when the release version was build ;-(
- Corrected the importing of data from old Prog_Generator
- Define at least 20 LEDs to be able to test them with the color test program
- Disabling the Event which is called when Enter is pressed when the workbook is closed. Hopefully this solves the problem that the Pattern_Config is opened sometimes unintentionally
- Prevent crash if a wrong formula is entered like "-Test"
- Updating the arduino type in the "Options" dialog if the USB Port detection is started
- Charlie_Buttons and Charlie_Binary control 3 channels (RGB) instead of 2 (GB)
- Corrected the maximal time for the Blinker function by adding PF_SLOW
- Disable the mouse scroll function for Office <= 2007 because here excel generates a crash

**Ver.: 2.1.1** 14.11.20:
- Removed the old Debug functions to simulate DDC commands
  TEST_PUSH_BUTTONS(), TEST_TOGGLE_BUTTONS() and TEST_BUTTONS_INCH()
- Experimental support for ESP32 added

**Ver.: 3.0.0** 21.04.21:
- Release support of ESP32 and up to 49152 single LEDs
- Support controling DMX512 devices (up to 300 per channel)
- Bootloader Update and "New Bootloader full Mem"
- Up to nine independent LED channels
- Search function in macro selection 
- TinyUniProg improvements
- Fixed problem scaling the house dialog for small screens (1366x768)
- Added seven new railway signal macros
- Ability to switch the LED portocol to WS2811 where the Red and Green channel are swapped
- Faster method to download and execute the color test program
- Use only one column for start led number display, may be configured on config page
- Add Macro #define COMMANDS_DEBUG traces DCC messages
- Corrected the "fire" macro
- Fix issue where directory names contain blanks
- Fix DCC offset problem when sending simulated DCC commands
- Speed up ResetTestButtons function
- And many, many more features and bugfixes, ...  Let yourself be surprised!


