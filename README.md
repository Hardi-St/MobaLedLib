# MobaLedLib
## Arduino library for controlling LEDs and other components on a model railway.

**New in release 3.4: support LichtMaschine Pro hardware, Stabilize and speed up FastLED on ESP32, new macros: RGB_Ring**

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
  - DMX 
- Control with DCC, CAN, Selectrix or LNet protocol 
- Additional module to read 80 and more switches with 10 signal wires
- Stand-alone operation or networked with other components possible

The lighting and some additional functions are controlled via the WS2812 / WS2811 chips.
For this, only a single signal is required which is forwarded from one chip to the next.
This makes the wiring extremely easy.

A lot of **examples** show the usage of the library. They could be use as startup for own programs.

A huge wiki describes the complete MobaLedLib in detail: https://wiki.mobaledlib.de/<br/>
This library is used and supported by a big community, see https://forum.mobaledlib.de/ and https://www.stummiforum.de/viewtopic.php?f=7&t=165060.
All software and hardware parts are free of license and available for download.

Several videos demonstrate the possibilities of the library. You will find a lot of articles when searching for MobaLedLib in the internet.

ATTENTION: Always use the Arduino IDE 1.8.19
(Old versions 1.6.x and below or versions 2.x don't support the directory structure used in the library examples)


Installation from the Arduino IDE:
  Sketch / Include library / Manage libraries
  Type "MobaLedLib" in the "Filter your search..." field
  Select the entry and click "Install"


Installation in der deutschen Arduino IDE:
  Sketch / Bibliothek einbinden / Bibliothek verwalten
  "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
  Gefundenen Eintrag auswaehlen und "Install" anklicken

see also https://wiki.mobaledlib.de/anleitungen/quickstart

Questions / suggestions / praise / ...
  MobaLedLib@gmx.de



**Revision History:**

**Ver.: 3.4.0** 24.04.25:

*Features*
- ESP32: Parallel output of LED data on up to 6 channels
- change FastLED to a stable version 3.9.12 coming from MobaLedLib github repository
- change source for library ATTinyCore:avr to MobaLedLib github repo
- support of the new color test
- support of the ESP32 1,3" OLED display
- add MobaLedLib RGB-LED ring macro
- rp2040 support up to 8 parallel LED channels
- improve determination of OneDrive local path
- optional display of MobaLedLib time/LDR values on the display
- improved selection of old ProgGenerator filename when updating to new release/beta
- on "Import from old program" the Arduino type setting is retained
- acceleration of ESP32 start time
- add SI_LocalVar to predefined system variables
- add replacement of "$" in macro arguments         see https://www.stummiforum.de/t165060f7-MobaLedLib-LEDs-Servos-Sound.html#msg2643729
- force a rebuild if ALT key is pressed
- empty compiler cache if last ATMega build failed  see https://www.stummiforum.de/t222466f195-MobaLedLib-Arduino-Upload-geht-nicht.html#msg2649211
- update rp2040 board version to 4.4.0
- improvement of overall PlatformIO build process
- support PlatformIO build for rp2040
- experimental support of ESP32 board version 2.0.17

*Bugfixes*
- ATMega: bug with wrong send buffer size for SEND_INPUTS fixed
- fix detection of first RGB_Heartbeat line while import
- "ESP32 and hieroglyphics for MLL time" see To-Dos#21
- unnecessary memory consumption due to extensions  see To-Dos#20
- error handling didn't work when downloading libraries/boards
- upgrade PlatformIO to 6.9.0 to fix build problem with DMX512
- fix problem with StartLedNumber update on invisible sheets
- improvement of start LED calculation in include sheets
- remove wrong platformIO beta warning
- fix problem with wrong MP3_Command value MP3_STOP which should be MP3_ADVERT_STOP
- fix problem with PatternConfigurator icons

*Changes*
- Selectrix pin definition changed to SX_SIGNAL_PIN 13 and SX_CLOCK_PIN 4 to support both ESP32 Adapter and the new ESP32 main board
- when using PlatformIO don't turn compile window to red in case of build errors to keep colored error output
- changed version string schema to provide platform name


**Ver.: 3.3.2** 19.12.23:

*Features*
- LNet support for Arduino platform (main board >= 1.8.0 mandatory)
- New macros: Set_LEDNr, CopyNLEDs, Include, SingleLedSignal, SingleLedSignalEx
- New icons
- Support of input type "feedback", process CAN messages from ATTiny_CAN_GBM module
- Store_Status: support SwitchB, extend max. InCnt to 63
- Support of DCC/CAN/LNet momentary buttons (GEN_BUTTON_RELASE mode are now setable in config sheet)
- Experimental support of MobaLedLib stored in OneDrive folder
- ATMega328PB support

*Bugfixes*
- fix SwitchC issue with ESP32
- fix issue that AVR build fails caused by vbcr in LEDs_Autoprog.h -> replace by vbcrlf
- add missing macros InCh_to_LocalVar, InCh_to_LocalVar1 and Bin_InCh_to_TmpVar1 to sheet Lib_Macros
- fix #10763: include marco counts LEDNr wrong
- fix #10159: wrong line endings in fastbuild.cmd
- ensure that included sheet uses same protocol as the main sheet

**Ver.: 3.2.1** 09.08.22:

*Features*
- LED simulator 
- Selectrix support for ESP32
- ColorPicker for Const Makro
- Macro RGB_Heartbeat_Color
- MobaLedLib Extensions support
- "Virtual pin" feature
- Retrigger support for patterns using GOTO mode 
- DCC signal state signaled with ESP32 onboard LED

*Bugfixes*
- fix CAN baudrate issue with ESP32 V1 chips
- fix ESP32 build issue with non-default Arduino home directory
- fix issue in case Arduino home directory doesn't exist
- add missing macros InCh_toTmpVar1 and BinCh_toTmpVar1
- fix RandMux bug on ESP32
- fix Set_ColTab bug on ESP32
- fix issue that Analog Pattern flags were ignored in Goto mode 

**Ver.: 3.1.0** 28.11.21:

*Features*
- New TreeView based macro selection dialog with grouping and icons
- Add feature to control sound modules attached to the mainboard
- Add ServoMP3 feature - sound modules are attached to the servo board and controlled via serial line
- Add feature Pin_Alias
- Add #define SWITCH_DAMPING_FACT to the Lib_Macros
- Add possibility to scroll with the mouse in the description box of the TreeView dialog
- UserForm_Other is resizable now

*Enhancements*
- Show please wait screen when loading/updating the tree view
- The macro filter is also activated by typing letters in the list box
- Remove NmraDCC library installation workaround as version 2.0.10 fixes the ESP32, no more need to install from private repository
- Add sheet Platform_Parameters containing all platform dependent settings
- Add Raspberry Pico Mainboard Leds, Mainboard Buttons, PushButtons
- Add experimental Raspberry Pico support
- Disable Autodetect when changing CPU type to ESP32 or Pico
- Change width of Form and new pos for Buttons at Form Other
- Splited the installation of several board packages into separate calls because otherwise the installation fails.
- Update the filter if a row is selected which already contains a macro to show the macro
- Improved the scrolling in the userform others
- Added a scroll bar to description in the TreeView dialog
- As ESP32 is no longer experimental set library U8g2 to mandatory
- Motorola II protocol for interface Arduino
- Allow relative path in ImageBoxAdder
- Reload all Icons when running GenReleaseVersion
- Support of BETA update directly from github
- Add the build date as a tooltip to the version information cell
- Add Excel version check
- Add library external command processing

*Bugfixes*
- Solved problem if the user has no additional board installed. In this case the "packages" directory  has to be created in C:\Users\<Name>\AppData\Local\Arduino15\
- Adapted the cmd files to work with 32 bit windows (Arduino is installed to "Program Files" and not to "Program Files (x86)")
- Corrected start focus and tab index of the Userform_Other (Prior sometimes the 'Abort' button had the focus)
- Added "On Error Resume Next" to prevent crash with Office 365 in EnableDisableAllButtons()
- Replaced ".Add2" by ".Add" in Sort_by_Column because this new function is not supported by Office 365
- (Hopefully) prevent formatting the "Start LEDNr" as date by setting the NumberFormat to "General" when importing files.
- Fixed bug when loadimg the Excel File. The Pattern Configurator icons in the lines have been deleted
- Fix issue on Scroll in UserForm_Other (focus lost)
- Prevent crash when the TreeView is closed with the 'x' button and reopened again
- Fix some typos in start page text
- Fix issue that load of old configuration was not started while a Beta update
- Fix issue #6894 BetaUpdate won't run with UserDir containing blank chars
- Fix LED_to_var - Led_Offs to 31. See #6899
- Fix VB6 FindWindow Issue #6914
- Fix another VB6 Issue in Userform_RunProgram
- Workaround for Excel 2007 isNumericBug
- Add missing EspSoftwareSerial library
- Fix Platform_Parameters: with AM328 SPI Pins are only usable if no CAN module is in use

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

**Ver.: 2.1.1** 14.11.20:
- Removed the old Debug functions to simulate DDC commands
  TEST_PUSH_BUTTONS(), TEST_TOGGLE_BUTTONS() and TEST_BUTTONS_INCH()
- Experimental support for ESP32 added

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

**Ver.: 2.0.0** 18.10.20:
- Support for the new buffer gate on mainboard version 1.7 added

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

**Ver.: 1.9.6 J** 11.10.20:
- The Mainboard_LED function also acceppts the arduino pin numbers D2-D5, D7-D13 amd A0-A5

**Ver.: 1.9.6 I** 10.10.20:
-  Possibilitiy added to ignore problems with the baud rate detection

**Ver.: 1.9.6 H** 10.10.20:
- Improved the loading of MLL_pgf files

**Ver.: 1.9.6 G** 10.10.20:
- Added additional pins to the Mainboard_LED function. Now nearly every pin could be used as LED pin (New channels 0, 5-16).
- New method "LED_to_Var()" to set variables controlled by the LED values.
- Improved the "Mainboard_Hardware_Test.MLL_pgf". Now the mainboard could be tested without the PushButton4017 board.

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

**Ver.: 1.9.6 E** 07.08.20?:
- Deletted >100000 columns in the DCC sheet which slowed down the loading of .MLL_pfg files
- Added a status display when loading the .MLL_pfg files
- Don't read/save the "Examples sheet from/to .MLL_pfg file

**Ver.: 1.9.6 D** 04.08.20:
- Limit the maximal servo value to 210 (Old 220) to avoid promlems with measurement errors at 2kHz
- Additional Delay and check if the old directory has been deleted when updating the Beta version

**Ver.: 1.9.6 C** 28.07.20:
- Corrected Servo programming (Flash was erased when setting the Reset pin as output)
- Corrected decimal separator problem when loading pattern examples
- New macros for servo with SMD WS2811 Herz_BiRelais_V1...

**Ver.: 1.9.6** 22.07.20:
- Preview LEDs in the Pattern_Configuarator could be moved on top of a picture or under a transparent picture (by Misha)
- Speedup building and uploading of the Arduino program 10 sec. instead of 23 sec. (by Juergen)

**Ver.: 1.9.5** 15.06.20:
- The versions 1.0.2 - 1.9.4 are not released test versions.
- Since there are a huge number of changes since version 1.0.1 all details are described here: https://www.stummiforum.de/viewtopic.php?f=7&t=165060&sd=a&start=2410

**Ver.: 1.9.4** 14.06.20:
- Added Misha's Multiplexer to the Prog_Generator

**Ver.: 1.0.7** 07.06.20:
- Corrected the LED Animation, the "Start LedNr" in combination with "HerzHerz_BiRelais()"

**Ver.: 1.0.6** 06.06.20:
- Added Mishas LED Previev and Mux functions to the Pattern_Configurator
- Using Harolds new pyProgGen_MobaLedLib
- Using the new USB port detection also in the Pattern_Configurator

**Ver.: 1.0.5** 31.05.20:
- Automatically install all libraries
- Using the Sketchbook path for the working directory

**Ver.: 1.0.4** 23.05.20:
- Automatically detecting COM port the Arduino is connected to
- Improved the HV_Reset() in the Tiny_UniProg according to Juergens tipp
- New macros InCh_to_TmpVar1() and Bin_InCh_to_TmpVar1() which start with state 1 instead of 0
- New macros for Servos and Herzstueck polarisation
- New Push Button macros which read DCC and hardware buttons
- Added macros Andreaskreuz with lamp test

**Ver.: 1.0.3** 01.05.20:
- Test of additional LED channels and EEPROM Storage

**Ver.: 1.0.2** 18.04.20:
- Test of switch and variables

**Ver.: 1.0.1** 17.01.20:
- Corrected the upload in version 1.0.0 because some files have not been update ;-(

**Ver.: 1.0.0** 16.01.20:
- New Charlieplexing program for the Servo_LED module which could be used to drive Viessmann and other "Multiplexed" light signals
- Configuration upload from the Pattern_Configurator over the LEDs "Bus" to the Charlieplexing module. The module is configured on the railway layout.
- Direct programming support for the Tiny_UniProg module from excel (One click to install the software)
- Flashing of the software for the Charlieplexing Module from excel (One click to install the software)
- Enhanced Color Test program with a lot of new features
- New Black and White TV simulation (configurable)
- Simulation of defective neon lights added
- 1001 of other small changes and improvements

**Ver.: 0.9.3** 08.12.19:
- Engagement of Pattern_Configurator and Program_Generator => Easy exchange between the tools
- CheckColors function: Live view of the colors and brightens of the LEDs
- Existing lines could be edited in the Prog_Generator

**Ver.: 0.9.2** 30.10.19:
- Corrected the XPattern function (used in the Light signals)
- Corrected the Excel Programs

**Ver.: 0.9.1** 06.10.19:
- Corrected the functions Flash(), RandWelding() and PushButton_w_LED_0_? in the Prog_Generator
- Corrected single LEDs in the House() and GasLights() function.

**Ver.: 0.9.0** 27.09.19:
- New Excel User interface to configure the LEDs
- Single LED functions for the House() macro
- Macros for Light signals, Construction lightning, ... added to the library

**Ver.: 0.8.0** 16.07.19:
  - Added new assembly document for the main PCB from Alf and Armin

**Ver.: 0.7.9** 09.07.19:
  Moved the Pattern_Configurator to the "extras" directory
  and updated the excel version.
  - Nice graphical display of the LED brightness and the Goto mode
  - Reduced the size by extracting the examples
  - Added a menu to save/load/delete example sheeds
  - User interface is automatically switched to English (Example descriptions still German)

**Ver.: 0.7.8** 09.04.19:
- Added examples
  - 00.Overview
  - 25.Analog_Push_Button

**Ver.: 0.7.7** 17.02.19:
- Added Support for Sound modul JQ6500
- Corrected the random mode of the Counter() function
- Improved the serial input debug function
- Moved the Heartbeat function in own H-file"
- 12.03.19:
  - added ButtonNOff() macro

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

**Ver.: 0.7.0** 20.12.18:
- First released version
