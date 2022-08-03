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


 LED Ansteuerung fuer die Eisenbahn
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Die Beleuchtungen und einige Zusatzfunktionen werden ueber die WS2812 / WS2811 Chips angesteuert.
 Dazu wird nur ein einziges Signal benoetigt welche von einem Chip and den naechsten weitergeleitet wird.

 Gesteuert werden die Funktionen von einem Arduino UNO/Nano. Da dieser beschraenkte Resorcen hat
 (2 KB RAM, 32 KB FLASH, 1 KB EEPROM) muss das Programm optimal entworfen sein.
 Wenn die Resourcen nicht reichen, dann kann auf einen Arduino Mega 2560 (8 KB RAM, 256 KB FLASH, 4 K EEPROM)
 umgestiegen werden. Zur Not kann auch ein ESP 32 verwendet werden. Hier wird dann ein 3.3V -> 5V Wandler und
 evtl. ein externer EEPROM benoetigt.
 Alternativ kann die Ansteuerung auch auf mehrere UNOs oder NANOs verteilt werden.
 Momentan (28.09.18) werden aber mit allen Debug Funktionen nur 65% des FLASHs benoetigt => Ein Uno/Nano reicht
 voellig.

 Die einzelnen LEDs werden ueber eine Tabelle konfiguriert. Diese ist zunaechst im FLASH abgelegt. Evtl. kann sie
 spaeter im EEPROM gespeichert werden. Pro Ausgang wird eine Variable Anzahl von Bytes benutzt. Das erste Byte
 definiert den Typ des Ausgangs.

 Periodischer Start:
 - Wenn der Zeitpunkt des letzten Aufrufs abgelegt wird, dann kann mit
     t - t1 >= Duration
   der Trigger fuer den naechsten Aufruf generiert werden. Dabei spielt der Ueberlauf des Zaehlers keine Rolle.
   Allerdings braucht man dann eine weitere Variable Duration wenn diese variabel sein soll.
 - Wenn man den naechsten Triggerzeitpunkt ablegt, dann wird die Funktion zu frueh aufgerufen wenn es zu
   einem Ueberlauf der Triggerzeitpunkts kommt.
 => Die erste Methode ist besser und wird verwendet.
 - Der erste Startzeitpunkt kann mit t1 = - Delay definiert werden. Die Funktion wird dann zum Zeitpunkt
   Periode - Delay zum ersten mal gestartet.

 Die Ausgabefunktionen werden bei jedem Schleifendurchlauf aufgerufen. In den Funktionen wird der Zustand des
 Eingangs geprueft und ueberprueft ob eine Zeitgetrigerte Aktion durchgefuehrt werden muss.

 House:
 - Beim Wechsel 0->1 geht sofort ein Licht an, 1->0 geht ein Licht aus, damit man erkennt welcher Schalter fuer
   welches Haus zustaendig ist.
 - Effekte welche nur in bestimmten Zimmern auftreten:
   - TV
   - Offener Kamin
   - Nachtlicht (Rot / Blau)
   - Disco
 - Helligkeit / Lichtfarbe fuer jedes Zimmer
   - Fuer jedes Zimmer (LED) wird ein Byte benutzt. Dieses bestimmt den Effekt, die Farbe, die Helligkeit, ...
     => Es gibt 256 verschiedene Zimmer Typen
     => Die Funktion hat eine variable Anzahl von Parametern
 - Wenn ein Haus nur eine LED hat soll diese dann immer an/ausgeschaltet werden ?
 - Soll immer mindestens eine LED an sein ?
   => Parametrierbar: On_Min
 - TV:
   - Mehrere Programme
   - Die Preiserleins schauen nicht immer TV (Auch wenn das in der realitaet nicht so ist)
     - TurnOnRoom() akiviert zufaellig TV oder Licht oder Kaminfeuer
       Was aktiv ist kann aus den LED Farben ermittelt werden:
       - Licht:      R = B
       - TV:         R < B
       - Kaminfeuer: R > B
         Hier wird ein zusaetzliches byte im RAM benoetigt (Siehe Definition von ROOM_CHIMNEY  (5 +RAM1))
 - Gas Laternen: http://www.gaswerk-augsburg.de/fernzuendung.html
   Werden auch ueber die Haus-Funktion gesteuert. Die Gas Laternen werden Zeitlich nacheinander gezuendet.
   In der Reallitaet ist das auch so. Das liegt daran, das die Uhren oder die Helligkeitssensoren zum
   Zuenden der Lampen nicht gleich eingestellt sind. Ich hab das letztens auf dem nachhauseweg beobachtet.
   Die Gas Lampen gehen aber nicht wie bei Zimmerlichtern zufaellig aus. Sie werden ueber das Makro GasLights()
   in dem Konfigurationsarray definiert.
   Der Typ "GAS_LIGHT.." kann auch als Zimmerlicht benutzt werden


 Blinker Auto:          1Hz = 1000ms
 Blaulicht:             2Hz =  500ms, 30% An  83ms  (Verschiedene Varianten: https://www.digital-bahn.de/bau_pkw/pkw.htm)
 Blaulicht Doppelblitz: 500ms, ~25ms An, ~75ms Aus, ~25ms An (Aus Diagramm oben abgelesen)
 Windrad:               1s An, 0.5s Aus, 1s An, 1.5s Aus: https://www.windparkwaldhausen.de/contentbeitrag-170-84-kennzeichnung_befeuerung_von_windkraftanlagen_.html

 Feuer Simulation:
 ~~~~~~~~~~~~~~~~~
 Bei der Feuer Simulation von https://learn.adafruit.com/led-campfire/introduction
 wird pro LED eine byte RAM fuer das "heat[]" array benoetigt.
 => Eigene Funktion fuer das Feuer. RAM wird ueber die Anzahl der LED's reserviert

 Monoflop
 ~~~~~~~~
 Mit dem Monoflop koennen Aktionen gestartet werden welche nur eine bestimmte Zeit lang aktiv sein sollen.
 Beispiele:
 - Rauchrenerator fuer Haeuser (1-3 Minuten?)
 - Sound Modul (0.5-3 Sekunden)
 - Schaukel
 Maximale Dauer 8 Minuten, Aufloesung 8 ms, 2 Byte
 Die Funktionen sollen nur auf eine positiven Flanke (0->1) reagieren.
 Wenn der Schalter dauerhaft an ist soll nichts passieren. Auch nicht beim Programmstart.
 Verschedene Modis
 - Taster mit Ein- und Ausschaltfunktion. (1. Druck schaltet ein, 2. Druck innerhalb der Monoflopzeit schaltet wieder aus)
 - Kippschalter. An wenn 0->1. Aus wenn 1->0 (evtl. spaeter Implementieren)

 Revision History:
 ~~~~~~~~~~~~~~~~~
 19.03.18:  - Started
 20.03.18:  - Disabled the Version message in C:\Users\Hardi\Documents\Arduino\libraries\FastLED-3.1.8\FastLED.h
            - Individual RAM size for each output function using the __COUNTER__ macro
            - Improved the DoubleFlash() function.
              - Added Val0, Val1
              - Could be used as Blink function
            - Replaced the old Blink function with DoubleFlash()
            - Same FLASH memory usage as before (only 18 additional bytes)
 21.03.18:  - Get_Input() function added
 16.04.18:  - Fire and Chimney finished
               Flash: 11716 Bytes (36%)  mit  _PRINT_DEBUG_MESSAGES
                      11274 Bytes (34%)  ohne _PRINT_DEBUG_MESSAGES  (Saved 442 Bytes)
               RAM:     650 Bytes (31%)
 15.06.18:  - Const and DoubleFlash function support outputs with more than one LED
 16.06.18:  - Reading the switch commands from the CAN bus
 03.08.18:  - Started with the keyboard test functions (Enabled with _TEST_BUTTONS)
 04.08.18:  - keyboard test functions are working
            - Corrected the DoubleFlash function (Following LED was influenced by mistake)
 05.08.18:  - The Proc_DblFlash function now uses an extra state variable instead of using the
              LED value because prior the LED wasn't totally dark in the break between the two flashes
 08.08.18:  - Separate variables for LED and Inp Test
 17.08.18:  - Incremented the RAM usage of the Blink functions also to 3 because the also use the
              DoubleFlash() function which uses an extra state variable since 05.08.18.
 20.08.18:  - Pattern funktion finished. It replaces the Const(), the DoubleFlash() and the Button() function
              and is able to generate any cind of other patterns.
              The FLASH memory consumption with the Pattern() function is even 82 bytes less then before.
              Tested with a configuration containing 1x Const(), 5x DoubleFlash() and 1x Button().
 21.08.18:  - Using fast math routines from FastLED: mod8()
 26.08.18:  - AnalogPattern is working. It has also a digital mode => The old Pattern function is
              no longer needed. It's keept for tests at the moment.
 31.08.18:  - Variable number of bits started
 01.09.18:  - AnalogPattern() is finished
            - Deleting digital Pattern function and some others (Old version in "LEDs_Eisenbahn vor Loeschen digital Pattern.zip")
 02.09.18:  - Added Neon lights
 04.09.18:  - Added r, g, b input to the serial input function
            - Color of Neon lights could be defined
            - Brightness of gas lights could be defined
            - Gas lights turn off slowly
 06.09.18:  - Started the Logic() function
 07.09.18:  - Logic function finished
 08.09.18:  - PF_NO_SWITCH_OFF implemented
 09.09.18:  - One array for inputs and Special inputs. Logic() function writes to the normal inputs.
 10.09.18:  - PM_HSV finished
 12.09.18:  - Randomfunctions started
 15.09.18:  - RF_STAY_ON added
 16.09.18:  - Welding() function finished
            - Logic:
              Added the ENABLE/DISABLE functionality. If the first argument is ENABLE/DISABLE the
              following number defines an input which enables/disables the output.
                Example: ENABLE  17, 12 AND 13
                         DISABLE 17, 12 AND 13
              DISABLE is equal to: ENABLE NOT
 17.09.18:  - ButtonFunc() = MonoFlop implemented with the Random() function
 24.09.18:  - Read_LDR.cpp finished
 28.09.18:  - Schedule finished
            - Updated from FastLED Ver.3.1.6 to 3.2.0
            - Moved the Serial input test functions into an own module
 10.10.18:  - Created an own heartbeat class
 12.10.18:  - New Pattern mode _PF_XFADE added. This mode uses the real last LED value instead
              of the value readout from the prior state. This is used in configurations where several
              functions update the same LEDs alternating.
                Example: LichtSignal3Begriff()
              This flag is set with a special function call like the APattern function
              Therefore it's started with a "_". The User doesn't have to set it because this is
              done in the XPattern macro
 14.10.18:  - Added switch _HOUSE_STATISTIC to generate statistics of the number of active lights
              in the houses
            - New paramters for the House() function. Om_Max is now used instead of the strange
              On_Limit parameter. This simplifies the configuration and its much easier to
              understand.
            - Started initializing the random numbers with the CPU temperature and AD0
              => Aber es geht noch gar nicht ;-((
 15.10.18:  - Initialize the random numbers with the "uninitialized RAM"
            - New macro HouseT() where the times could be set
 19.10.18:  - Added SKIP_ROOM
            - If On_Min is equal On_Max the given number of lights stay on util the
              input is turne off.
 21.10.18:  - Set better default values for RoomCol[]
            - New Room types:
              - NEON_LIGHTM: Medium brightnes
              - NEON_LIGHTL: Large rooms with several neon lights is simumated with one LED
                             => At first a dark light starts,
                                then a bright light starts in addition.
 21.10.18:  - Create_Lib.bat: Converting the German umlauts in the generated library
            - Print_Config() could be used also if _PRINT_DEBUG_MESSAGES is disabled.
              It uses no memory if it's not used.
 22.10.18:  - Corrected the turn off behavior in the HSV mode. Now the given Off value is written
              as HSV components to the LED. Prior it was written as color => LED was not turned off.
              Problem was identified with the RGB_Heartbeat.
            - Example Contr.Warn.Light_Pattern_Func finished
 24.10.18:  - Increased the COUNT_VARARGS() macro to 2000 arguments
 25.10.18:  - 6 new ROOM_*CHIMNEY types which end with "D" = ark and "B" = Bright
 27.10.18:  - New pattern flag PM_SEQUENZ_STOP which is used fot the signals.
              It stops updating the LEDs imidiately if the input is turned off.
            - 10.RailwaySignal_Pattern_Func finished
 27.10.18:  - Updated FastLED Library 3.2.0 -> 3.2.1
 28.10.18:  - New function Counter() implemented
            - Button test mode could be activated with the middle key.
              Then the three buttons control the inputs 0-2
            - Adapted the brackets after setup() and loop() to the strange "Visual Micro" debugger: https://www.visualmicro.com/
              -> New: setup(){  loop(){
            - Finished the 13.FlipFlop_Counter example
            - New room types FIRE, FIRED and FIREB which are like the chimney types, but here
              the light is not ranomly activated
 06.11.18:  - Added Enable Input to the Pattern functions (Additional macros have an added "E" in the names.)
              (Example: PatternTE1)
            - New FlipFlops with inverse controlled DstVar. DstVar is active at the start and after a timeout.
 07.11.18:  - New function New_Local_Var()
            - Counter() function uses 16 Bit flags. New flags:
              - CF_LOCAL_VAR       Write the result to a local variable which must be created with New_Local_Var() prior
              - CF_ONLY_LOCALVAR   Don't write to the LEDs defined with DestVar. The first DestVar contains
                                   the number maximal number of the counter-1. (counter => 0 .. n-1)
              Additional FLASH usage: 98 Bytes + 1 Byte per Counter() line
 09.11.18:  - Started the "Goto" functions for the pattern function (See: "Goto Funktion" in AnalogPattern.cpp
 13.11.18:  - Sound_PlayRandom() is working using the "Goto" function
 14.11.18:  - If the start position is not found nothing is executed (Goto End).
 18.11.18:  - Key_4017 is working
            - Improved the memory consumption of the Const() function and enabled _USE_SEP_CONST
            - Example "14.Switches_80_and_more.ino" finished
 22.11.18:  - Disabled the Sprintf and Nprintf functions because the called Serial.print() uses 157 bytes flash
 23.11.18:  - Example with two keyboards is working
 24.11.18:  - It doesn't matter which CTR_CHANNELS is greater
 25.11.18:  - Added _CHKL() to all locations where LED is used to generate a warning if the LED number
              is invalid (>=NUM_LEDS). The following warnung  is generated:
                "warning: narrowing conversion of '512' from 'int' to 'const unsigned char' inside { } [-Wnarrowing]"
 26.11.18:  - New function InCh_to_TmpVar() which combines several InpStructArray[] channels to one temporary
              variable which could be used be used in combination with the Pattern function when the special
              input SI_LocalVar is used.
            - Updated the example "11.CAN_Bus_MS2_RailwaySignal" to use the new function
            - New Example "12.CAN_Bus_MS2_Switch_Lights"
 27.11.18:  - New Example "16.Illumination_Pattern_Func"
 08.12.18:  - Replacing the Set_Color() function by the Set_ColTab() function to save 43 bytes RAM.
            - Adedd CAN_ok variable to prevent crash if no MCP2515 is connected after a while
            - Tested with the CAN library: https://github.com/coryjfowler/MCP_CAN_lib
 20.12.18:  - Uploaded to
              - https://github.com/Hardi-St/MobaLedLib
              - https://www.stummiforum.de/viewtopic.php?f=7&t=165060
 05.01.19:  - Corrected the binary mode of the counter (CF_BINARY)
 13.01.19:  - Increased the number of possible allocated RAM for a macro from 50 to 100
 18.01.19:  - Added function Bin_InCh_to_TmpVar() and RGB_Heartbeat2()
            - Corrected the initialization of the Pattern function if the Goto mode is used.
 19.01.19:  - Using Pattern_Configurator.xlsb instead of .xlsm in the library
            => Version 0.7.5
 15.02.19:  - Corrected the random mode of the counter CF_RANDOM). The ActualVar_p->Changed
              flag was not set if the same random number was generated than before. This caused problems
              with the Sound_PlayRandom macro because in this case no sound was played.
 16.02.19:  - Changed the serial input debug function
              - Moved to H-file to be able to enable it without change in the Lin_Config.h
              - Using only lower case letters (Inputs are converted to lowercase)
            - New example: 24.DCC_and_Sound.ino
 17.02.19:  - Moved the heartbeat function into a separate H-file. This file could be used
              independent from the MobaLedLib by including "Serial_Inputs.h" only.
 10.03.19:  - Corrected the Status_Button_0_x() macros to be able to use the PushButton_w_LED_0_x()
              macros with all Button LEDs. Prior only C1 was working
 11.03.19:  - ToDo liste moved to private file
 01.06.19:  - Corrected the value limmitation of the pattern function in Calculate_V()
              The values have been limmited to 0/255 and not to the given limmits Val0/Val1.
              This caused wrong values if the gradient of the signal was to high. The problame came up
              in the Servo_Ballet which uses the analog pattern function zo switch from 30 to 180 in
              20 ms. The error genrerates intemideate values: 30, 30, 240, 240, 240, 240, 180, 180, 180, 180,
              and also when switched back from 180 to 30: 180, 180, 180, 0, 0, 0, 0, 30, 30, 30,
 16.07.19:  => Released Ver. 0.8.0
 06.09.19:  - Added SINGLE_LEDxy
            - Added two new colors to Default_Room_Col_Tab
 24.09.19:  - Added binary signals
 02.10.19:  - New: Excel User interface to configure the LEDs without programming at all!
            => Released Ver. 0.9.0
 05.10.19:  - Added +1 to the leds[] array because otherwise the House() function doesn't work with less than 3 single LEDs
              if no other LED is following.
                - GasLights(#LED, #InCh, GAS_LIGHT1, GAS_LIGHT1)                             => Onla the RED LED is turned on
                + GasLights(#LED, #InCh, GAS_LIGHT1, GAS_LIGHT2)                             => O.K.
                  // Reserve LEDs(1)
                + GasLights(#LED, #InCh, GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3, GAS_LIGHT1)     => O.K.
                + GasLights(#LED, #InCh, SINGLE_LED1, SINGLE_LED2, SINGLE_LED3, SINGLE_LED1) => O.K.
 06.10.19:  => Released Ver. 0.9.1
 20.10.19:  - Corrected problems in the pattern function
            - XPattern:
              Added missing initialization in two for() loops which cause random errors in the XPattern mode
              Problem was detected in the "KellerLicht()". Here the start value was wrong when the function
              was turned on. It starts fading from a different color than expected ;-(
            - HSV Mode:
              Corrected the initialization of the HSV mode.
              Problem was detected in the "Cave_Illumination()". Here the LEDs started with random color
              at power on. The Cave_Illumination is not active at power on.
 24.10.19:  - Corrected XPattern function
 29.10.19:  - Corrected number of input chanels in the DepSignal4() and DepSignal4_RGB() macros. (Old 7: => 4)
              This error created strange effects. In the example from Thomas the prior called Flash()
              function stopped the fading in the XPattern() function. This was caused because the
              InCh_to_TmpVar() macro used the temporary input channel by mistake ;-(
 30.10.19:  => Released Ver. 0.9.2
 08.12.19:  - Engagement of Pattern_Configurator and Program_Generator finished
            => Released Ver. 0.9.3
 09.01.20:  - Started the black and white TV
            - New compiler switch TEST_PUSH_BUTTONS in "LEDs_AutoProg.ino" to use the buttons as push button
 10.01.20:  - Finished the B&W TV (Additional menory: FLASH: 176 byte, RAM: 4 byte)
              New macros: Set_TV_COL1(), Set_TV_COL2(), Set_TV_BW1(), Set_TV_BW2()
              It's possible to change the "program" by using several Set_TV... commands and enable them
              with a switch (DCC, ...)
              Football: Set_TV_COL1(#InCh, 500/16, 2500/16, 40,65,   2,2, 200,255, 3,10)
            - Prevent disabling the TV in case the color is equal to one of the room colors
 12.01.20:  - "LEDs_AutoProg.ino": Improved the Check_Mainboard_Buttons() function
            - For some reasons RF_STAY_ON was equal RF_SEQ
            - New function Set_Def_Neon(SI_1, 1, 255, 1) which defines the propabilitys for the defective neon light
            - New Room types: NEON_DEF_D, NEON_DEF1D, NEON_DEF2D, NEON_DEF3D
 13.01.20:  - RandMux(): RF_NOT_SAME added
            - House(): Using the upper bit (0x80) of ON_MIN to invert the input
            - Added: House_Inv() , HouseT_Inv(), GasLights_Inv(), RandCntMux()
 06.04.20:  - Using the AnalogScanner library instead of an own analog interrupt function for the darkness
              detection to be able to use the other analog inputs in the program.
              This is necessary to read the analog push buttons.
 30.04.20:  - Changed the parameter Skip0 to Use0 in the PushButton functions because it's easyer to understand
              Attention: Existing macros have do be adapted !
            - Corrected pointer error in Update_TV_Data (Thanks Juergen)
 01.05.20:  - Juergen add new feature to enable application to store the last status of most functions
 07.05.20:  - Added new macros InCh_to_TmpVar1 and Bin_InCh_to_TmpVar1 which could be used togeter with the
              pattern function if the first Goto Number should be 1. This us useful because the power on
              Goto number is always 0. The new macros are used for the Servo function wirgout stop input
 19.05.20:  - Juergen corrected the EEPROM saving. Problems occoured with the _Herz_MoRelais_PF() function.
              When restoring the states the wrong state was shown for a short time
            - Finished the Herz_..Relais. macros.
 20.05.20:  - Changed the SI_1 behavior. Now it's initialized after the Inp_Processed() call in the
              constructor to trigger also the INP_TURNED_ON event. This was changed to initialize the state
              for the Bin_InCh_to_TmpVar() macro which is used in the "AndreaskrLT..." macro
              => ToDo: Check if this change influences other macros
            - Changed the abort criteria in Proc_InCh_to_TmpVar and Proc_Bin_InCh_to_TmpVar to be able to
              use SI_1 = 255 as an input to.
              => ToDo: Check if this change influences other macros
 02.06.20:  - Added Juergens changes which add the possibility to detect trigger events in the inputs in
              Proc_InCh_to_TmpVar() and Proc_Bin_InCh_to_TmpVar() and forward them to the temp variable.
              This is used for instance with light signals which should not dim down and dim up the brightness
			  if the same DCC command is send several times.
              The trigger is used with the new command Trig_to_TmpVar and Trig_to_TmpVar1.
 05.06.20:  - Corrected problem with the new "_USE_INCH_TRIGGER" in "Bin_InCh_to_TmpVar()" which came up
              if SI_1 is used as input. If the "AndreaskrLT3_RGB()" is used without an DCC input the special
              input SI_1 = 255 ist used. This generatesd an endless loop in Proc_Bin_InCh_to_TmpVar()
 08.06.20:  - Change the Trigger behavior of InCh_to_TmpVar again because the old method didn't work since
              InCh_to_TmpVar uses one global TmpVar for all calls => This variable can't be used to store
              the last state. It's always overwritten by the next line.
              Instead the new macros InCh_to_LocalVar, InCh_to_LocalVar1 habe been implemented which use a
              LocalVar to store the last state. This method uses 1 bytes of RAM (Old 2, see below) for each call.
              There are 6 macros to read inputs into a 8 Bit variable:
              - InCh_to_LocalVar<1>   Used for DCC, "Change" flag is set with every pressed button
              - InCh_to_TmpVar<1>     Used for DCC, "Change" flag is set if an other button than before is pressed
              - Bin_InCh_to_TmpVar<1> Used for SX,  "Change" flag is set if the binary input is changed
              Each of the macros has a version with a tailing 1. They generate a result which starts with
              1 instead of 0. This is usefull for goto functions which use a default value which should
              be different from the normal values.
 09.06.20:  - Using only one byte to store the last state according to Juergens tipp
 10.06.20:  - New CANDLE room type and new Set_CandleTab() macro based on Roberts sources (322 additional Bytes)
 29.09.20:  - Corrected the case in the Pattern*24 macros: t24 => T24
            - Incremented the number of Pattern functions from 30 to 32
 01.10.20:  - New function "Mainboard_LED(MB_LED, InCh)" which could be used to use the mainboard LEDs as status LEDs
            - Added macro "WeldingCont()" which continuously flickers while the input is active.
 14.10.20:  - Corrected definition of XPatternTE21.. The "_PF_XFADE" Flag was missing here ;-(
            - Moved the Pattern Macro definitions to "Macros.h"
 17.10.20:  - Added Juergens changes:
              - Added "__attribute__ ((packed))" to all tyypedefs to be able to compile the lib on a 32 bit plattform
              - Changed the deprecated binary constants B... to 0B...
              - Disabled the random number generator initialisation by random noise in the RAM if RAMEND
                is not defined (ESP)
                => On this platforms the random numbers are always the same
                ==> ToDo: An othe method has to be implemted to get true random numbers.
              - Moved the initialisation of Update_TV_Data() down after Room_ColP is initialized
 31.10.20:  - Charlie_Buttons and Charlie_Binary control 3 channels (RGB) instead of 2 (GB)
 22.02.21:  - Corrected the "fire" macro
 04.03.21:	- Jürgen adds ESP32 Support for Analog and 16384 Leds
 29.09.21:  - Jürgen add new feature to enable processing of extra commands outside the core library
 13.10.21:  - Hardi added new Sound_New_... functions for new and old MP3-TF-16P Sound modules (use 4.7uF instead of 22uF and 2 new 2 KHz WS2811)

 RAM Bedarf (NUM_LEDS 32 = 96):             http://jheyman.github.io/blog/pages/ArduinoTipsAndTricks/#figuring-out-where-memory-went
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Heartbeat      590 Byte => 393 Bytes !!  PROBLEM: Ein einziges Serial.println() "kostet" 173 Bytes RAM ;-(
 Blink            9 Byte
 FastLED Blink  100 Byte
 LEDs_Eisenbahn 690 Byte

 RoomCol[]          45 Byte
 InpStructArray[]   64 Byte
 TV_Dat[]           10 Byte


 Mit Dprintf_proc:
 Der Sketch verwendet 16312 Bytes (53%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
 Globale Variablen verwenden 690 Bytes (33%) des dynamischen Speichers, 1358 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

 Ohne Dprintf_proc:
 Der Sketch verwendet 15500 Bytes (50%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
 Globale Variablen verwenden 517 Bytes (25%) des dynamischen Speichers, 1531 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.
                        812  173

 Dump mit: (Siehe: https://arduino.stackexchange.com/questions/31190/detailed-analyse-of-memory-usage)

   cd C:\Program Files (x86)\Arduino\hardware\tools\avr\bin
   avr-nm.exe -Crtd --size-sort C:\Users\Hardi\AppData\Local\Temp\arduino_build_61988\01.Heartbeat.ino.elf             | C:\Users\Hardi\AppData\Local\atom\app-1.28.0\resources\app.asar.unpacked\node_modules\dugite\git\usr\bin\grep.exe -i ' [dbv] '
   avr-nm.exe -Crtd --size-sort C:\Users\Hardi\AppData\Local\Temp\arduino_build_491073\14.Switches_80_and_more.ino.elf | C:\Users\Hardi\AppData\Local\atom\app-1.28.0\resources\app.asar.unpacked\node_modules\dugite\git\usr\bin\grep.exe -i ' [dbv] '

   00000157 b Serial              !!                       22.11.18:
   00000152 b MobaLedLib
   00000096 b leds
   00000034 b Config_RAM
   00000019 d CSWTCH.65
   00000018 d vtable for NEOPIXEL<(unsigned char)6>
   00000018 d vtable for HardwareSerial
   00000017 b CLEDController& CFastLED::addLeds<NEOPIXEL, (unsigned char)6>(CRGB*, int, int)::c
   00000016 d vtable for CLEDController
   00000013 b FastLED
   00000008 b guard variable for CLEDController& CFastLED::addLeds<NEOPIXEL, (unsigned char)6>(CRGB*, int, int)::c
   00000006 b LED_Heartbeat
   00000004 B timer0_overflow_count
   00000004 B timer0_millis
   00000004 B random_seed
   00000004 d next
   00000004 b CFastLED::countFPS(int)::lastframe
   00000004 b lastshow
   00000002 D rand16seed
   00000002 b CFastLED::countFPS(int)::br
   00000002 b CLEDController::m_pTail
   00000002 b CLEDController::m_pHead
   00000001 b timer0_fract
   00000001 b PixelController<(EOrder)66, 1, 4294967295ul>::init_binary_dithering()::R
   00000001 b gTimeErrorAccum256ths




 Untersuchung welche Updateraten man sieht:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Dazu habe ich mehrere RGB LEDs mit verschiedenen Blink und Blitzmustern angesteuert
 - Bei einem Blinklicht mit einer Periodendauer 30ms = 33 Hz (15ms an, 15ms) aus sieht man flacken.
   Wenn die Periode kleiner oder gleich 20ms = 50 sieht man kein Flackern mehr.
 - Bei dem Blitzlicht ist die Helligkeit eines 5 ms Blitzes dunkler als ein 15 ms Blitz.
   Aber die Unterschiede sind kaum zu erkennen.

   PatternT1(10,64,1,3,0,255,0,0,1*5 ms,7)
   PatternT1(11,64,2,3,0,255,0,0,2*5 ms,7)
   PatternT1(12,64,3,3,0,255,0,0,3*5 ms,7)
   PatternT1(13,64,4,3,0,255,0,0,4*5 ms,7)
   PatternT1(14,64,5,3,0,255,0,0,5*5 ms,7)
   PatternT1(15,64,6,3,0,255,0,0,6*5 ms,7)
   PatternT1(16,64,7,3,0,255,0,0,7*5 ms,7)                        // 35 ms

   PatternT2(17,64,1+20,3,0,255,0,0,1 * 5 ms,1 Sek - 1 * 5 ms,7)
   PatternT2(18,64,2+20,3,0,255,0,0,2 * 5 ms,1 Sek - 2 * 5 ms,7)
   PatternT2(19,64,3+20,3,0,255,0,0,3 * 5 ms,1 Sek - 3 * 5 ms,7)
   PatternT2(20,64,4+20,3,0,255,0,0,4 * 5 ms,1 Sek - 4 * 5 ms,7)
   PatternT2(21,64,5+20,3,0,255,0,0,5 * 5 ms,1 Sek - 5 * 5 ms,7)
   PatternT2(22,64,6+20,3,0,255,0,0,6 * 5 ms,1 Sek - 6 * 5 ms,7)
   PatternT2(23,64,7+20,3,0,255,0,0,7 * 5 ms,1 Sek - 7 * 5 ms,7)  // 35 ms


 Anstelle eines Konfigurationsarrays koennen C++ Funktionen in der loop() Funktion benutzt werden.
 + Bessere Syntaxpruefung durch den Compiler moeglich
 + Einfacher zu verstehen ?
 + Einfacher zu implementieren (Parameter koennen ohne pgm_read_byte() verwendet werden)
 + Unbenutzte Funktionen belegen keinen Speicher
 - Braucht vermutlich mehr Speicher (Call 2 Byte, Push, Pop, return) => Sollte man mal untersuchen
 - Dazu muesste viel geaendert werden
 - Kann nicht aus dem EEPROM gelesen werden. (Attiny hat 1 K EEPROM, Momentan sin ~12K Flash frei)

 CAN Bootloader:
 http://www.kreatives-chaos.com/artikel/can-bootloader
 https://www.mikrocontroller.net/topic/381008


 Arduino Nano:          +-----+  Used in examples
           +------------| USB |------------+
 CAN       |            +-----+            |
 CLK    B5 | [ ]D13/SCK        MISO/D12[ ] |   B4   SO CAN          Attention: The buildin LED can't be used if the
           | [ ]3.3V           MOSI/D11[ ]~|   B3   SI CAN                     SPI bus is used for the CAN
           | [ ]V.ref     ___    SS/D10[ ]~|   B2   CS CAN
 |--LDR C0 | [ ]A0       / N \       D9[ ]~|   B1   Taster Rechts
 Res.   C1 | [ ]A1      /  A  \      D8[ ] |   B0   Taster Mitte
        C2 | [ ]A2      \  N  /      D7[ ] |   D7   Taster Links
 HB.LED C3 | [ ]A3       \_0_/       D6[ ]~|   D6   -> WS281x LED Strang
        C4 | [ ]A4/SDA               D5[ ]~|   D5   LED Blau Rechts
        C5 | [ ]A5/SCL               D4[ ] |   D4   LED Weis Mitte
           | [ ]A6              INT1/D3[ ]~|   D3   LED Gelb Links
           | [ ]A7              INT0/D2[ ] |   D2   <- Rueckmeldung WS281x (Momentan nicht benutzt)
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
           |                              GND[ ] |                        CAN Modul
           | [ ]N/C                    SCK/13[A] |   B5                   SCK
           | [ ]v.ref                 MISO/12[A] |   .                    SO   Achtung SO und SI sind vertauscht am CAN Modul
           | [ ]RST                   MOSI/11[A]~|   .                    SI
           | [ ]3V3    +---+               10[ ]~|   .                    CS
           | [ ]5v     | A |                9[ ]~|   .    SDA soft
           | [ ]GND   -| R |-               8[B] |   B0   SCL soft
           | [ ]GND   -| D |-                    |
           | [ ]Vin   -| U |-               7[A] |   D7
           |          -| I |-               6[A]~|   .
           | [ ]A0    -| N |-               5[C]~|   .
           | [ ]A1    -| O |-               4[A] |   .
           | [ ]A2     +---+           INT1/3[A]~|   .
           | [ ]A3                     INT0/2[ ] |   .
      SDA  | [ ]A4      RST SCK MISO     TX>1[ ] |   .
      SCL  | [ ]A5      [ ] [ ] [ ]      RX<0[ ] |   D0
           |            [ ] [ ] [ ]              |
           |  UNO_R3    GND MOSI 5V  ____________/
            \_______________________/


Arduino Pro Mini:
                           D0   D1   RST
            GND  GND  VCC  RX   TX   /DTR
           +--------------------------------+
           |  [ ]  [ ]  [ ]  [ ]  [ ]  [ ]  |
           |              FTDI              |
       D1  | [ ]1/TX                 RAW[ ] |
       D0  | [ ]0/RX                 GND[ ] |
           | [ ]RST        SCL/A5[ ] RST[ ] |   C6
           | [ ]GND        SDA/A4[ ] VCC[ ] |
       D2  | [ ]2/INT0    ___         A3[ ] |   C3
       D3  |~[ ]3/INT1   /   \        A2[ ] |   C2
       D4  | [ ]4       /PRO  \       A1[ ] |   C1
       D5  |~[ ]5       \ MINI/       A0[ ] |   C0
       D6  |~[ ]6        \___/    SCK/13[ ] |   B5
       D7  | [ ]7          A7[ ] MISO/12[ ] |   B4
       B0  | [ ]8          A6[ ] MOSI/11[ ]~|   B3
       B1  |~[ ]9                  SS/10[ ]~|   B2
           |           [RST-BTN]            |
           +--------------------------------+
*/

#include "MobaLedLib.h"

// Some variables are only used if some #defines are activated. But it's not necessary to disable
// them by a #ifdef statement because the compiler optimizes the generated code in a way that
// unused or never changed variables don't use RAM or FLASH even if they are used in the program

// Global variables for Schedule function
uint8_t    Darkness = 0;
DayState_E DayState = Unknown;

uint8_t TestMode = 0;    // If this variable is set the normal update function of the LEDs is disabled and the LEDs could be set by a debug

/*
 Problem mit dem Initialisieren des Zufallszahlengenerators                                                   // 15.10.18:
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 Zum initialisieren des Zufallszahlengenerators wird randomSeed() und random16_set_seed()
 mit einer Zufaelligen Zahl initialisiert. Normalerweise nimmt man dazu die Spannung an einem
 offenen Analogeingang. Dummerweise funktioniert die analogRead() Funktion nicht im Konstruktor
 eine Globalen Klasse weil dieser aufgerufen wird bevor die Hardware initialisiert ist ;-(
   Siehe: https://forum.arduino.cc/index.php?topic=111463.0
 Leider geht auch das lesen der Internen Temperatur nicht im Konstruktor ;-(

 Eine schoenere Moeglichkeit habe ich hier gefunden:
   https://arduino.stackexchange.com/questions/50671/getting-a-truly-random-number-in-arduino

 Die Funktion random_seed() wird, wenn ich das richtig verstanden hab, automatisch vor dem eigentlichen
 Programm gestartet da sie in der section ".init3" steht.
   (https://www.nongnu.org/avr-libc/user-manual/mem_sections.html)
 Sie berechnet eine XOR Summe ueber den ganzen RAM. Einige Speicherstellen im RAM aendern ihren Wert
 beim einschalten zufaellig. (https://hackaday.com/2015/06/29/true-random-number-generator-for-a-true-hacker/)
 Es muessen so viele Speicherzellen wie moeglich berachtet werden weil die meisten Speicherzellen nach
 jedem einschalten den gleichen Wert haben. Nur ganz symetrisch aufgebaute Zellen haben zufaellige Werte.

 Das Funktioniert aber nur bei einem Kaltstart (Versorgungsspannung einschalten). Bei einem Warmstart
 bleibt der Speicher erhalten.

 Mit der Funktion Get_random_seed() bekommt man nach jedem Warmstart die gleiche Zahl. Wenn dazwischen
 ein Kaltstart liegt, dann bekommt man eine andere Zahl.

 Zur Generierung einer anderen seed Zahl bei einem Warmstart wird random_seed in Int_Update() periodisch
 mit einem Zufaelligen Wert gesetzt. Damit ist der Startwert des Zufallszahlengenerators jedes
 mal ein anderer Wert :-))
*/

#ifdef RAMEND																								 // 17.10.20: Jürgen - not all platforms provide a static RAMEND define
uint32_t __attribute__((section(".noinit"))) random_seed;  // Variable located in the RAM section which is not initialized => It keeps the value after a warm start

//------------------------------------------------------------
void __attribute__((naked, section(".init3"))) seed_from_ram()
//------------------------------------------------------------
{
    extern const uint32_t __data_start;
    const uint32_t * const ramend = (uint32_t *) RAMEND;
    for (const uint32_t *p = &__data_start; p <= ramend; p++)
        random_seed ^= *p;
}
#endif

#ifdef _NEW_ROOM_COL
const PROGMEM uint8_t Default_Room_Col_Tab[ROOM_COL_CNT*3] = // 51 Byte FLASH
                       {
                          15,  13,   3, // 0  ROOM_COL0 (very dark warm White)
                          22,  44,  27, // 1  ROOM_COL1 (cold dark White)
                         155,  73,   5, // 2  ROOM_COL2 (warm Yellow)
                          39,  18,   1, // 3  ROOM_COL345 (Dark Yellow)  randomly color 3, 4 or 5 is used
                          30,   0,   0, // 4  ROOM_COL345 (Dark Red)
                          79,  39,   7, // 5  ROOM_COL345 (Dark warm White)
                          50,  50,  50, // 6  Gas light  dark    Bei einzeln adressierten Gas LEDs wird der individuelle Helligkeitswert verwendet (GAS_LIGHT1, GAS_LIGHT2, GAS_LIGHT3)
                         255, 255, 255, // 7  Gas light  bright  Wenn 3 Kanaele zusammen verwendet werden dan bestimmt der erste Wert die Helligkeit das ist wichtig damit alle Ausgaenge gleich belastet werden (GAS_LIGHT und GAS_LIGHT)
                          20,  20,  27, // 8  Neon light dark  (Achtung: Muss groesser als 2*MAX_FLICKER_CNT sein)
                          70,  70,  80, // 9  Neon light medium
                         245, 245, 255, // 10 Neon light bright
                          50,  50,  20, // 11 TV0 and chimney color A randomly color A or B is used
                          70,  70,  30, // 12 TV0 and chimney color B
                          50,  50,   8, // 13 TV1 and chimney color A
                          50,  50,   8, // 14 TV2 and chimney color B
                         255, 255, 255, // 15 Single LED Room:      Fuer einzeln adressierte LEDs wird der individuelle Helligkeitswert verwendet (SINGLE_LED1,  SINGLE_LED2,  SINGLE_LED3)  // 06.09.19:
                          50,  50,  50, // 16 Single dark LED Room: Fuer einzeln adressierte LEDs wird der individuelle Helligkeitswert verwendet (SINGLE_LED1D, SINGLE_LED2D, SINGLE_LED3D)
                       };
#endif // _NEW_ROOM_COL

#if _USE_CANDLE
   const PROGMEM Candle_Dat_T Default_Candle_Dat =
       {
       25,     // Candle_Min_Hue            Farbe der Kerze (25 = Orange)
       25,     // Candle_Max_Hue              "
       80,     // Candle_Min_BrightnessD    Dunkelphase
       100,    // Candle_Max_BrightnessD
       120,    // Candle_Min_Brightness     Standard-Helligkeit
       145,    // Candle_Max_Brightness
       64,     // Candle_Change_Probability Wahrscheinlichkeitsfaktor einer Helligkeitsaenderung (0..255)
       50,     // Candle_Chg_Hue            Wahrscheinlichkeitsfaktor eines Farbwechsels
       5       // Candle_Time_Dark          Wahrscheinlichkeitsfaktor eins Helligkeitswechsel in den dunklen Bereich
       };
#endif


#if _USE_SET_TVTAB                                                                                           // 09.01.20:
  const PROGMEM uint8_t Default_TV_Dat[] =
                     {
                     500/16, 2500/16, // Update_t_Min, Update_t_Max,
                     40, 65,          // Min_Brightness, Max_Brightness,
                     0,  85,          // R_Min, R_Max,
                     70, 210,         // G_Min, G_Max,
                     60, 150          // B_Min, B_Max
                     };
#endif // 09.01.20:  _USE_SET_TVTAB



//------------------------------------------------------------------------------------------------------------------------------
MobaLedLib_C::MobaLedLib_C(
    struct CRGB* _leds,
    uint16_t Num_Leds,
    const unsigned char Config[],
    uint8_t RAM[],
    uint16_t RamSize
#if _USE_STORE_STATUS                                                                                         // 19.05.20: Juergen
    , Callback_t Function
#endif
#if _USE_EXT_PROC                                                                                             // 26.09.21: Juergen
    , ExtProc_t Processor
#endif
    ) // Constructor
//------------------------------------------------------------------------------------------------------------------------------
 : leds(_leds),
   Num_Leds(Num_Leds),
   Config(Config),
   Initialize(true),
   RAM(RAM),
   Last_20(50),
   HSV_p(NULL)
 #if _USE_USE_GLOBALVAR
   ,GlobalVar_Cnt(0)
 #endif
{
  // Serial printing in the constructor could generate problems.
  // - Initialisation of the serial port in the constructor could hang up the program for some reasons ;-(
  // - Some characters may get lost
  // - If the baudrate is equal to the Flash Tool it seams to be better
   #ifdef _PRINT_DEBUG_MESSAGES
     Serial.begin(115200); //   Attention: The serial monitor in the Arduino IDE must use the same baudrate
   #endif
   Dprintf("MobaLedLib_C Constructor\n");

  //memset(_leds, 3, sizeof(CRGB)*Num_Leds); // Debug line to find initialization problems
  memset(TV_Dat, 0, sizeof(TV_Dat));
  memset(RAM,    0, RamSize);

  memset(InpStructArray, 0x00, _INP_STRUCT_ARRAY_SIZE);
#if _USE_STORE_STATUS                                                                                         // 19.05.20: Juergen
  CallbackFunc = Function;
#endif
#if _USE_EXT_PROC                                                                                             // 21.06.21: Juergen
  CommandProcessorFunc = Processor;
#endif
  
  Set_Input(SI_Enable_Sound, 1); // Could be changed by the configuration

  Set_Default_TV_Dat_p();                                                                                     // 09.01.20:

  Inp_Processed();               // Clear the Old_Inp[] array

  Set_Input(SI_1, 1);            // Special input which is always 1                                           // 20.05.20:  New Location

  // Initialize the random numbers with the "uninitialized RAM"
#ifdef RAMEND                                                                                                 // 17.10.20: Jürgen - not all platforms provide a static RAMEND define
  srandom(random_seed);
  random16_set_seed(random());
#endif
  
#ifndef _NEW_INITIALIZE  
  // 18.12.2021 remove initial update to be able to set initial values after constructor and before first update run
  Int_Update(millis());   // Must be called once before the inputs are read. (Attention: srandom() must be called before to get unpredictable random numbers)
#endif
  
  #ifdef _TEST_BUTTONS
    Setup_Test_Buttons();
  #endif

  TriggerCnt = 0;

  #ifdef _CHECK_UPD_RATE
    Upd_Cnt = 0;
    Next_Upd_Check = 5000;
  #endif
}

#if _USE_USE_GLOBALVAR
//----------------------------------------------------------------------------------
void MobaLedLib_C::Assigne_GlobalVar(ControlVar_t *GlobalVar, uint8_t GlobalVar_Cnt)                          // 15.11.18:
//----------------------------------------------------------------------------------
{
  this->GlobalVar_Cnt = GlobalVar_Cnt;
  this->GlobalVar     = GlobalVar;
  Dprintf("GlobalVar_Cnt %i\n", GlobalVar_Cnt);
}
#endif

//----------------------------------------------------
inline uint8_t* MobaLedLib_C::Get_LEDPtr(uint8_t &cnt)
//----------------------------------------------------
{
  cnt = 1;
  ledNr_t LEDNr = pgm_read_led_nr(cp+P_LEDNR);
  CRGB *LED_p = &leds[LEDNr];
  uint8_t ChanelMsk = pgm_read_byte_near(cp+P_CHANELMSK);
  //Dprintf("%i %i\n", LEDNr, ChanelMsk);
  switch (ChanelMsk)
    {
    case C_ALL:   cnt++;
    case C12:     cnt++;
    case C_RED:   return &LED_p->red;
    case C23:     cnt++;
    case C_GREEN: return &LED_p->green;
    default:      return &LED_p->blue;
    }
}

//-----------------------------
void MobaLedLib_C::Proc_Const()
//-----------------------------
{
  uint8_t Inp = Get_Input(pgm_read_byte_near(cp+P_INPCHANEL));
  if (Inp == INP_TURNED_ON || Inp == INP_TURNED_OFF || Initialize)
     {
     const uint8_t pos = Inp_Is_On(Inp) ? P_CONST_VAL1 : P_CONST_VAL0;
     uint8_t Val = pgm_read_byte_near(cp+pos);
     uint8_t cnt;
     uint8_t *LEDPtr = Get_LEDPtr(cnt);
     memset(LEDPtr, Val, cnt);
     }
}

/*
Der Sketch verwendet 24216 Bytes (78%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
Globale Variablen verwenden 657 Bytes (32%) des dynamischen Speichers, 1391 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.


Der Sketch verwendet 24162 Bytes (78%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
Globale Variablen verwenden 657 Bytes (32%) des dynamischen Speichers, 1391 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.

Der Sketch verwendet 24026 Bytes (78%) des Programmspeicherplatzes. Das Maximum sind 30720 Bytes.
Globale Variablen verwenden 657 Bytes (32%) des dynamischen Speichers, 1391 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 2048 Bytes.
                     136

*/

// Fire simulation:
// ~~~~~~~~~~~~~~~~
// Copied from:
// https://learn.adafruit.com/led-campfire/introduction
// Sieht erst im Modellhaus reallistisch aus, nicht wenn die LEDs frei da liegen.

// There are two main parameters you can play with to control the look and
// feel of your fire: COOLING (used in step 1 above), and SPARKING (used
// in step 3 above).
//
// COOLING: How much does the air cool as it rises?
// Less cooling = taller flames.  More cooling = shorter flames.
// Default 55, suggested range 20-100
#define COOLING  25

// SPARKING: What chance (out of 255) is there that a new spark will be lit?
// Higher chance = more roaring fire.  Lower chance = more flickery fire.
// Default 120, suggested range 50-200.
#define SPARKING 120

//----------------------------
void MobaLedLib_C::Proc_Fire()
//----------------------------
// Simulate a fire with at least 3 RGB LEDs
{
  uint8_t *Last_t = (uint8_t*)rp;
  rp++;

  if (Initialize)
     {
     Dprintf("Init Proc_Fire()\n");
     }

  uint8_t LED_cnt  = pgm_read_byte_near(cp+P_FIRE_LEDCNT);

  if ((uint8_t)((t & 0xFF) - *Last_t) >= 50) // Update everey 50 ms => 20 Frames/sec
     {
     *Last_t = t & 0xFF;
     uint8_t Inp    = Get_Input(pgm_read_byte_near(cp+P_FIRE_INCH));
     ledNr_t Led0   = pgm_read_byte_near(cp+P_FIRE_LED0);
     uint8_t Bright = pgm_read_byte_near(cp+P_FIRE_BRIGHT);

     // Array of temperature readings at each simulation cell
     uint8_t *heat = rp;

     if (Inp_Is_On(Inp))
          {
          // Step 1.  Cool down every cell a little
          for(uint8_t i = 0; i < LED_cnt; i++)
            heat[i] = qsub8( heat[i],  random8(0, ((COOLING * 10) / LED_cnt) + 2));

          // Step 2.  Heat from each cell drifts 'up' and diffuses a little
          for(uint8_t k= LED_cnt - 3; k > 0; k--)
            //heat[k] = (heat[k + 1] + heat[k + 2] + heat[k + 2] ) / 3;   // 15.04.18:  Old - 1, -2  Video P1120336.MP4
            heat[k] = (heat[k - 1] + heat[k - 2] + heat[k - 2] ) / 3; // Original Code               Video P1120335.MP4   13.01.19:  Enabled again because the modified line doesn't work with more than 7 LEDs

          // Step 3.  Randomly ignite new 'sparks' of heat near the bottom
          if (random8() < SPARKING )
             {
             uint8_t y = random8(min((uint8_t)7,LED_cnt));                                           // Adapt the 7 for // 17.10.20 Jürgen - cast uint8_t for >8bit platforms
             heat[y] = qadd8(heat[y], random8(160,255));
             }
          }
     else { // Turned off => Cool down
          for(uint8_t i = 0; i < LED_cnt; i++)
            heat[i] = qsub8( heat[i],  random8(0, 10));
          }
     // Step 4.  Map from heat cells to LED colors
     CRGB *lp = &leds[Led0];
     for(uint8_t j = 0; j < LED_cnt; j++, lp++)
       {
       // Scale the heat value from 0-255 down to 0-240
       // for best results with color palettes.
       uint8_t colorindex = scale8( heat[j], 240);
       *lp = ColorFromPalette(HeatColors_p, colorindex);
       nscale8x3 (lp->r, lp->g, lp->b, Bright);
       }
     }

  rp += LED_cnt;
}


#ifdef _NEW_ROOM_COL
//----------------------------------
void MobaLedLib_C::Proc_Set_ColTab()                                                                          // 07.12.18:
//----------------------------------
{
  Room_ColP = cp;
}
#endif

#if _USE_CANDLE                                                                                               // 10.06.20:
//-------------------------------------
void MobaLedLib_C::Proc_Set_CandleTab()
//-------------------------------------
{
  Candle_DatP = (const Candle_Dat_T*)cp;
}
#endif

#if _USE_SET_TVTAB                                                                                            // 09.01.20:
//----------------------------------
void MobaLedLib_C::Proc_Set_TV_Tab()
//----------------------------------
{
  uint8_t Inp = Get_Input(pgm_read_byte_near(cp));
  if (Inp)
     {
     uint8_t Channel = pgm_read_byte_near(cp+1);
     TV_Dat_p[Channel] = cp+2; // Skip InCh and Channel
     }
}

//-----------------------------------
void MobaLedLib_C::IncCP_Set_TV_Tab()
//-----------------------------------
{
  cp += 12;
}
#endif

#if _USE_DEF_NEON                                                                                             // 12.01.20:
//------------------------------------
void MobaLedLib_C::Proc_Set_Def_Neon()
//------------------------------------
{
  uint8_t Inp = Get_Input(pgm_read_byte_near(cp));
  if (Inp)
     {
     Rand_On_DefNeon = pgm_read_byte_near(cp+1); // (Def=10)  probability that the neon light starts.             0 = don't start, 1 start seldom, 255 = Start immediately
     RandOff_DefNeon = pgm_read_byte_near(cp+2); // (Def=200) probability that the light goes off after a while.  0 = Always on,   1 = shot flash, 255 = very long time active
     Min_DefNeon     = pgm_read_byte_near(cp+3); // (Def=1)   red glow of the starter.                            1 = minimal value,  5 = maximal value
     }
}

//-------------------------------------
void MobaLedLib_C::IncCP_Set_Def_Neon()
//-------------------------------------
{
  cp += 4;
}

#endif
//-------------------------------------
void MobaLedLib_C::Proc_New_HSV_Group()                                                                       // 13.10.18:
//-------------------------------------
{
  HSV_p = (HSV_T*)rp;
  if (Initialize)
     {
     HSV_p->Hue = HSV_p->Sat = HSV_p->Val = 255; // memset() uses 4 bytes more FLASH
     //Dprintf("Init New_HSV %i\n", HSV_p->Sat);
     }
  rp += 3;
}

//-------------------------------------
void MobaLedLib_C::Proc_New_Local_Var()                                                                       // 07.11.18:
//-------------------------------------
{
  ActualVar_p = (ControlVar_t*)rp;
  ActualVar_p->Changed = 0;
  rp += sizeof(ControlVar_t);
}

#if _USE_USE_GLOBALVAR
//-------------------------------------
void MobaLedLib_C::Proc_Use_GlobalVar()                                                                       // 15.11.18:
//-------------------------------------
{
  uint8_t Nr = pgm_read_byte_near(cp);
  if (Nr >= GlobalVar_Cnt)
     {
     if (!Initialize) Dprintf("Error Use_GlobalVar(%i) > GlobalVar_Cnt:%i\n", Nr, GlobalVar_Cnt);
     return ;
     }
  if (Initialize) Dprintf("Proc_Use_GlobalVar %i\n", Nr);

  ActualVar_p = &GlobalVar[Nr];
  ActualVar_p->Changed = ActualVar_p->ExtUpdated; // Wird in der loop() funktion gesetzt und in Changed uebernommen
  ActualVar_p->ExtUpdated = 0;                    // Und dann zurueckgesetzt
}
#endif


#if _USE_INCH_TRIGGER                                                                                         // 02.06.20: New trigger method from Juergen
//---------------------------------------------------
void MobaLedLib_C::Proc_InCh_to_X_Var()
//---------------------------------------------------
// ActualVar_p is set to the number +Start of the first input variable which is changed to 1
//  If InCh 0 is turned to 1 the ActualVar_p is 0 +Start
//  If InCh 1 is turned to 1 the ActualVar_p is 1 +Start
//  ...
// If no input variable is changed the ActualVar_p->Changed = 0
//
// The new function supports 4 different modes which are controlled by the second argument.
// - InCh_to_TempVar     Using the global TempVar to store the result (ActualVar_p) for the folloring function
// - InCh_to_TempVar1    Like above, but the result starts with 1 (Start=1)
// - InCh_to_LocalVar    Using a individual local variable to store the last state. The Changed flag in the result is only set if number is changed
// - InCh_to_LocalVar1   Like above, but the result starts with 1 (Start=1)
//
// The new "LocalVar" Mode is used for light signals with blanking (LEDs are turned of when the signal aspect changes)
// Here the blanking should be prevented when the same DCC command is send again.
// These modes use 1 (Old 2) additional bytes RAM die store the last state and the Changed flag.
//
// The modes are coded into the upper two bitts of the argument:
// args bits:
// 0..5         number of channels to read  (currently max 6 bits, allows up to 64 gotos)
// 6            start offset                                                                   (I2X_USE_START1)
// 7            Use local var: if set to 1 then changed flag is only set if the state changes  (I2X_USE_LOCALVAR)
{
  uint16_t InCh = pgm_read_byte_near(cp);                                        // 05.06.20:  Changed to 16 Bit in case InCh = 255
  uint8_t   arg = pgm_read_byte_near(cp+1);
  uint8_t  Nr      = (arg & I2X_USE_START1) ? 1:0;  // Does ">> 6" instead of "?1:0" save memory? No, it uses exact the same amount of FLASH
  uint16_t EndInCh = InCh + (arg & (I2X_USE_START1-1));                                                       // 05.06.20:  Changed to 16 Bit in case InCh = 255
  /*
  uint8_t p = 0;                                                                 // Debug
  static uint32_t Next_t = 0;                                                    // Debug
  if (t > Next_t)                                                                // Debug
     {                                                                           // Debug
     if (Next_t == 0) Dprintf("InCh=%i EndInCh=%i\n", InCh, EndInCh);            // Debug
     Next_t +=5000;                                                              // Debug
     p = true;                                                                   // Debug
     }                                                                           // Debug
  for (uint8_t i = InCh; i <= EndInCh; i++) if(Inp_Changed(Get_Input(i))) p = 1; // Debug
  */
  bool Use_LocalVar = arg & I2X_USE_LOCALVAR;                                                                 // 09.06.20: New method from Juergen which uses only one byte RAM
  ActualVar_p = &TempVar;
  if (Use_LocalVar)
      {
      ActualVar_p->Val = *rp;
      rp++;
      }
    else
      {
      ActualVar_p->Val = 0;
      }
  ActualVar_p->Changed = 0;                                                                                   // 18.12.21: Set default to "nothing has changed"
  for (; InCh <= EndInCh; InCh++, Nr++)                                                                       // 31.05.20:  J: "<=" instead of "<" because EndInCh is now 0..63 instead of 1..64
    {
    uint8_t Inp = Get_Input(InCh);
    //Dprintf("Get_Input(%i)=%i ", InCh, Inp); // Debug
    if (Inp == INP_TURNED_ON)
       {
       if (Use_LocalVar == false || ActualVar_p->Val != Nr|| Initialize)                                     // 08.06.20:
          {
          if (!Initialize || !(arg & I2X_USE_START1))                                                        // 18.12.21: goto Mode pattern with a "off" state will initilize to "off"
            ActualVar_p->Changed = 1;
          
          ActualVar_p->Val = Nr;
          if (Use_LocalVar) *(rp-1) = Nr;                                                                     // 09.06.20:
          //Dprintf(" ActualVar=%i Trig\n", ActualVar_p->Val); // Debug
          }
       return ;
       }
     }
  //Dprintf(" ActualVar=%i\n", ActualVar_p->Val); // Debug
}

//-------------------------------------------------------
void MobaLedLib_C::Proc_Bin_InCh_to_TmpVar()
//-------------------------------------------------------
// New function which treats the input variables as binary number
{
  uint16_t InCh = pgm_read_byte_near(cp);                                        // 05.06.20:  Changed to 16 Bit in case InCh = 255

  // args bits:
  // 0..2         number of channels to read  (currently max 3 bits, see code below (Mask is 8 bit)
  // 3...5        reserverd
  // 6            start offset
  // 7            retrigger bit: if set to 1 also set Changed flag if state didn't change
  uint8_t  arg     = pgm_read_byte_near(cp+1);
  uint16_t EndInCh = InCh + (arg & 0x07);                                        // 31.05.20:  J: max 3 bits, see code below (Mask is 8 bit)  // 05.06.20:  Changed to 16 Bit in case InCh = 255
  uint8_t  Start   = (arg & 0x40) >> 6;                                          // 31.05.20:  J: start offset taken from arg

  ActualVar_p = &TempVar;
  ActualVar_p->Changed = 0;
  uint8_t Nr = 0;
  for (uint8_t Mask = 1; InCh <= EndInCh; InCh++, Mask <<= 1)                    // 31.05.20:  J: "<=" instead of "<" because EndInCh is now 0..7 instead of 1..8
      {
      uint8_t Inp = Get_Input(InCh);
      if (Inp_Changed(Inp)) ActualVar_p->Changed = 1;
      if (Inp_Is_On(Inp)) Nr |= Mask;
      }
  if (ActualVar_p->Changed)
      {
      ActualVar_p->Val = Nr + Start;                                                                             // 07.05.20:  Added + Start
      if (Initialize && (arg & I2X_USE_START1))                                                                  // 18.12.21: goto Mode pattern with a "off" state will initilize to "off"
        ActualVar_p->Changed = 0;                                                                                // only set Val, but don't trigger Pattern

      Dprintf("ActualVar=%d Changed=%d\n", Nr + Start, ActualVar_p->Changed);                                    // 31.05.20:  J: Start also added to log output
      }
}

#else // 02.06.20: Old
//---------------------------------------------------
void MobaLedLib_C::Proc_InCh_to_TmpVar(uint8_t Start)                                                         // 07.05.20:  Added Start
//---------------------------------------------------
// ActualVar_p is set to the number +Start of the first input variable which is changed to 1
//  If InCh 0 is turned to 1 the ActualVar_p is 0 +Start
//  If InCh 1 is turned to 1 the ActualVar_p is 1 +Start
//  ...
// If no input variable is changed the ActualVar_p->Changed = 0
{
  uint8_t InCh    = pgm_read_byte_near(cp);
  uint8_t EndInCh = InCh + pgm_read_byte_near(cp+1);

  /*
  static uint32_t Next_t = 0;                                                    // Debug
  uint8_t p = 0;                                                                 // Debug
  if (t > Next_t)                                                                // Debug
     {                                                                           // Debug
     if (Next_t == 0) Dprintf("InCh=%i EndInCh=%i\n", InCh, EndInCh);            // Debug
     Next_t +=5000;                                                              // Debug
     p = true;                                                                   // Debug
     }                                                                           // Debug
  for (uint8_t i = InCh; i < EndInCh; i++) if (Inp_Changed(Get_Input(i))) p = 1; // Debug
  */

  ActualVar_p = &TempVar;
  for (uint8_t Nr = 0; InCh != EndInCh; InCh++, Nr++)                                                         // 20.05.20:  Old InCh < EndInCh causes Problem with SP_1
    {
    uint8_t Inp = Get_Input(InCh);
    //if (p) Dprintf("Get_Input(%i)=%i ", InCh, Inp); // Debug
    if (Inp == INP_TURNED_ON)
       {
       ActualVar_p->Changed = 1;
       ActualVar_p->Val = Nr+Start;                                                                           // 07.05.20:  Added + Start
       //if (p) Dprintf(" ActualVar=%i\n", ActualVar_p->Val); // Debug
       return ;
       }
    }
  //if (p) Dprintf(" ActualVar=%i\n", ActualVar_p->Val); // Debug
  ActualVar_p->Changed = 0; // If nothing has changed
}

//-------------------------------------------------------
void MobaLedLib_C::Proc_Bin_InCh_to_TmpVar(uint8_t Start)                                                     // 07.05.20:  Added Start
//-------------------------------------------------------
// New function which treats the input variables as binary number
{
  uint8_t InCh    = pgm_read_byte_near(cp);
  uint8_t EndInCh = InCh + pgm_read_byte_near(cp+1);
  ActualVar_p = &TempVar;
  ActualVar_p->Changed = 0;
  uint8_t Nr  = 0;
  for (uint8_t Mask = 1; InCh != EndInCh; InCh++, Mask<<=1)                                                   // 20.05.20:  Old InCh < EndInCh causes Problem with SP_1
    {
    uint8_t Inp = Get_Input(InCh);
    if (Inp_Changed(Inp)) ActualVar_p->Changed = 1;
    if (Inp_Is_On(Inp)) Nr |= Mask;
    }
  if (ActualVar_p->Changed)
     {
     ActualVar_p->Val = Nr+Start;                                                                             // 07.05.20:  Added + Start
     Dprintf("ActualVar=%i\n", Nr);
     }
}
#endif // _USE_INCH_TRIGGER


//-------------------------------
void MobaLedLib_C::Proc_CopyLED()
//-------------------------------
{
  uint8_t Inp = Get_Input(pgm_read_byte_near(cp+P_COPYLED_INP));
  CRGB *lp = &leds[pgm_read_led_nr(cp+P_COPYLED_LED)];

  if (Inp_Is_On(Inp))
       {
       *lp = leds[pgm_read_led_nr(cp+P_COPYLED_SRCLED)];
       }
  else lp->r = lp->g = lp->b = 0;
}

//---------------------------------------
void MobaLedLib_C::Set_Default_TV_Dat_p()                                                                     // 09.01.20:
//---------------------------------------
{
  #if _USE_SET_TVTAB
    for (uint8_t i = 0; i < _TV_CHANNELS; i++)
       TV_Dat_p[i] = Default_TV_Dat;
  #endif
}

#if 0
uint8_t Debug_read_byte = 10;                                                                                 // 09.01.20:
//------------------------------------------------
uint8_t pgm_read_byte_near_Debug(const uint8_t *p)
//------------------------------------------------
{
  uint8_t b = pgm_read_byte_near(p);
  if (Debug_read_byte) { Serial.print(b); Serial.print(", "); }
  return b;
}
#else
  #define pgm_read_byte_near_Debug pgm_read_byte_near
#endif

//---------------------------------
void MobaLedLib_C::Update_TV_Data()
//---------------------------------
// Algorithm from: https://www.stummiforum.de/viewtopic.php?f=7&t=154443
{
  CALCULATE_t4; // Calculate the local variable t4 if _USE_LOCAL_TX is defined                                // 22.11.18:

  for (TV_Dat_T *p = TV_Dat, *e = p + _TV_CHANNELS; p < e; p++)
    {
    if ((uint8_t)(t4 - p->Last_t) >= p->dt)
       {
       p->Last_t = t4;
       // CRGB *lp;                                                                                           // 30.04.20:  Old: uninitialized pointer ;-(
       do
         {
         #if _USE_SET_TVTAB                                                                                   // 10.01.20:
            uint8_t TVNr = p - TV_Dat;
            //if (Debug_read_byte) { Serial.print("Update_TV_Data "); Serial.print(TVNr); Serial.print(": ");} // Debug
            const uint8_t *tp = TV_Dat_p[TVNr];
            uint8_t r1 = pgm_read_byte_near_Debug(tp++); // Use temp variables to make shure that the sequence is correct
            uint8_t r2 = pgm_read_byte_near_Debug(tp++);
            p->dt = random8(r1, r2);              // Naechsten update Zeitpunkt bestimmen
            //if (p==TV_Dat) Dprintf("%i\n", p->dt); // Debug
            r1 = pgm_read_byte_near_Debug(tp++);
            r2 = pgm_read_byte_near_Debug(tp++);
            uint8_t brightnes = random8(r1, r2);  // sorgt fuer das schwanken der Helligkeit
            //if (p==TV_Dat) Dprintf("%i\n", brightnes); // Debug
            for (uint8_t i = 0; i < 3; i++)
                {
                r1 = pgm_read_byte_near_Debug(tp++);
                r2 = pgm_read_byte_near_Debug(tp++);
                p->raw[i] = ((int)brightnes * random8(r1, r2)) / 256;                                         // 24.05.20:  Old: 265
                }
            //if (p==TV_Dat) Dprintf("%i: %i %i %i\n", brightnes, p->r, p->g, p->b); // Debug
            //if (Debug_read_byte) { Debug_read_byte--; Serial.println(""); } // Debug
         #else
            p->dt = random8(500/16, 2500/16);           // Naechsten update Zeitpunkt bestimmen
            uint8_t brightnes = random8(40, 65);        // sorgt fuer das schwanken der Helligkeit      Old: 40, 65
            p->r  = brightnes * random8(0, 85)   / 256; // Diese Einstellung erzeugt weitgehend
            p->g  = brightnes * random8(70, 210) / 256; // ein Weisses Licht mit Blau und Gruen Stich   Old: 70, 210
            p->b  = brightnes * random8(60, 150) / 256; // Rot tritt eher selten auf                    Old: 60, 150
         #endif
         if (p->r >= p->b)      // r < b indicates TV activ. => Make sure that r < b (Fire uses r > b)
            {                   // If the TV color matches with the room color the TV is disabled and the room is constant lightned ;-(
            if (p->r == 255)                                                                                    // 10.01.20:
                 p->r--;
            else p->r = p->b++;
            }
           // Prevent that the TV color is equal to one of the const room           // 10.01.20:      // 30.04.20:  Old: lp->r = p->r; lp_>g = p->g; lp->b = p->b;
           // colors because in this case the TV would be disabled in all houses.                     //    "       Old: while (Cmp_Room_Col(lp, 0) == 0 || Cmp_Room_Col(lp, 1) == 0);
         } while (Cmp_Room_Col((CRGB*)&(p->raw), 0) == 0 || Cmp_Room_Col((CRGB*)&(p->raw), 1) == 0);  //    "       New line from Jueregn whic saves 24 bytes in addition
       //Dprintf("TV%i %ims  %i %i %i\n", p - TV_Dat, p->dt*16, p->r, p->g, p->b);
       }
    }
  Set_Default_TV_Dat_p(); // Reset the pointer for the next loop in case they are not set in the loop (SET_TV_... is disabled)  // 10.01.20:
}


//------------------------------------------
void MobaLedLib_C::Int_Update(uint32_t Time)
//------------------------------------------
// Internal function to update all LEDs. It's called from Update()
{
  ActualVar_p = NULL;                                                                                         // 07.11.18:
  bool End = false;
  t   = Time;
  #ifndef _USE_LOCAL_TX                                                                                       // 22.11.18:
    t4w = (t>>4)  & 0xFFFF; // Time divided by 16
    t10 = (t>>10) & 0xFF;   // Time divided by 1024
  #endif
  if ((uint8_t)((t & 0xFF) - Last_20) >= 50)
       { // is called every 50 ms = 20 Hz
#ifdef RAMEND																								  // 17.10.20: Jürgen - not all platforms provide a static RAMEND define
       random16_add_entropy(random_seed = random()); // Add entropy to random number generator                // 27.08.18:  Old position outside the if
#endif
       Trigger20fps = TriggerCnt++;
       if (TriggerCnt == 0) TriggerCnt++;                                                                     // 09.06.20:
       Last_20 = t & 0xFF;
       }
  else Trigger20fps = 0;

  #ifdef _NEW_ROOM_COL
    Room_ColP = Default_Room_Col_Tab;
  #endif

  Update_TV_Data();																							  // 17.10.20: must be called AFTER Room_ColP is initialized - Jürgen

  #if _USE_CANDLE                                                                                             // 10.06.20:
    Candle_DatP = &Default_Candle_Dat;
  #endif

  #if _USE_DEF_NEON                                                                                           // 12.01.20:
    Rand_On_DefNeon = 10;
    RandOff_DefNeon = 200;
    Min_DefNeon     = 1;
  #endif
  #if _USE_STORE_STATUS                                                                                       // 01.05.20:
    ProcCounterId = 0;
  #endif
  for (cp = Config, rp = RAM; !End; )
    {
    uint8_t Type = pgm_read_byte_near(cp++);
    switch (Type)
      {                                                                                                            // #ifdef's to disable some features for memory consumption checks
#                                                                                                                  ifdef _USE_SEP_CONST
      case CONST_T              : Proc_Const();              IncCP_Const();         break;
#                                                                                                                  endif
#                                                                                                                  if _USE_HOUSE
      case HOUSE_T              : Proc_House();              IncCP_House();         break;
#                                                                                                                  endif
#                                                                                                                  if _USE_FIRE
      case FIRE_T               : Proc_Fire();               IncCP_Fire();          break;
#                                                                                                                  endif
#                                                                                                                  if _USE_SET_COLTAB
      case SET_COLTAB_T         : Proc_Set_ColTab();         IncCP_Set_ColTab();    break;
#                                                                                                                  endif
#                                                                                                                  if _USE_CANDLE           // 10.06.20:
      case SET_CANDLETAB_T     : Proc_Set_CandleTab();      IncCP_Set_CandleTab(); break;
#                                                                                                                  endif
#                                                                                                                  if _USE_SET_TVTAB        // 10.01.20:
      case SET_TV_TAB_T         : Proc_Set_TV_Tab();         IncCP_Set_TV_Tab();    break;
#                                                                                                                  endif
#                                                                                                                  if _USE_DEF_NEON         // 12.01.20:
      case SET_DEF_NEON_T       : Proc_Set_Def_Neon();       IncCP_Set_Def_Neon();  break;
#                                                                                                                  endif
#                                                                                                                  if _USE_HSV_GROUP
      case NEW_HSV_GROUP_T      : Proc_New_HSV_Group();      IncCP_New_HSV_Group(); break;
#                                                                                                                  endif
#                                                                                                                  if _USE_LOCAL_VAR
      case NEW_LOCAL_VAR_T      : Proc_New_Local_Var();      IncCP_New_Local_Var(); break;                         // 07.11.18:
#                                                                                                                  endif

#                                                                                                                  if _USE_USE_GLOBALVAR
      case USE_GLOBALVAR_T      : Proc_Use_GlobalVar();      IncCP_Use_GlobalVar(); break;
#                                                                                                                  endif
#                                                                                                                  if _USE_INCH_TO_VAR
#if _USE_INCH_TRIGGER                                                                                              // 02.06.20:
      case INCH_TO_X_VAR_T      : Proc_InCh_to_X_Var();      IncCP_InCh_to_X_Var(); break;
      case BIN_INCH_TO_TMPVAR_T : Proc_Bin_InCh_to_TmpVar(); IncCP_InCh_to_X_Var(); break;
#else
      case INCH_TO_TMPVAR_T     : Proc_InCh_to_TmpVar(0);    IncCP_InCh_to_X_Var(); break;
      case INCH_TO_TMPVAR1_T    : Proc_InCh_to_TmpVar(1);    IncCP_InCh_to_X_Var(); break;                         // 07.05.20:
      case BIN_INCH_TO_TMPVAR_T : Proc_Bin_InCh_to_TmpVar(0);IncCP_InCh_to_X_Var(); break;
      case BIN_INCH_TO_TMPVAR1_T: Proc_Bin_InCh_to_TmpVar(1);IncCP_InCh_to_X_Var(); break;                         // 07.05.20:
#endif
#                                                                                                                  endif
#                                                                                                                  if _USE_LOGIC
      case LOGIC_T              : Proc_Logic();              IncCP_Logic();         break;
#                                                                                                                  endif
#                                                                                                                  if _USE_RANDOM
      case RANDOM_T             : Proc_Random();             IncCP_Random();        break;
#                                                                                                                  endif
#                                                                                                                  if _USE_RANMUX
      case RANDMUX_T            : Proc_RandMux();            IncCP_RandMux();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_WELDING
      case WELDING_CONT_T       : Proc_Welding(0);           IncCP_Welding();       break;                    // 01.10.20:
      case WELDING_T            : Proc_Welding(1);           IncCP_Welding();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_COPYLED
      case COPYLED_T            : Proc_CopyLED();            IncCP_CopyLED();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_SCHEDULE
      case SCHEDULE_T           : Proc_Schedule();           IncCP_Schedule();      break;
#                                                                                                                  endif
#                                                                                                                  if _USE_COUNTER
      case COUNTER_T            : Proc_Counter();            IncCP_Counter();       break;
#                                                                                                                  endif
      case END_T                : End = true;                                       break;
      default:
#                                                                                                                  if _USE_PATTERN
                                  uint8_t pt = Is_Pattern(Type);
                                  if (pt)
                                       {
                                       uint8_t TimeCnt = Type - pt + 1;
                                       Proc_AnalogPattern(TimeCnt, pt != PATTERNT1_T);
                                       IncCP_Pattern(TimeCnt); // Increment the configuration pointer to point to the next block
                                       break;
                                       }
#                                                                                                                 endif
#                                                                                                                 if _USE_EXT_PROC  // 26.09.21: Juergen
                                  if (CommandProcessorFunc!=NULL) 
                                      { 
                                        uint8_t size = CommandProcessorFunc(Type, cp, true);
                                        if (size) // if size >0 then command could by handled by command processor
                                          {
                                            cp += size;
                                            break;
                                          }
                                      }
#                                                                                                                 endif
                                  Dprintf("Unknown Typ %i\n", Type);
      }
    }

  if (Initialize != false)
  {
#if defined(_DUMP_CONFIG) && defined(_PRINT_DEBUG_MESSAGES)
			const uint8_t* end2 = cp;
			cp = Config;
			int cntX = 0;
			Dprintf("\nDumping configuration Len=%d\n", end2-cp);
			if (end2-cp<128) end2=cp+128;
			for (;cp<end2;cp++)
			{	
			  uint8_t cell = pgm_read_byte_near(cp);				
				Dprintf("%02X ", (uint8_t)cell);
				if (cntX>0 && (cntX&0x07)==0)
				{
					Dprintf("- ");
				}
				if (++cntX==32)
				{
					Dprintf("\n");
					cntX=0;					
				}
#ifndef ESP32				
				Serial.flush();																																												 // 14.03.21 Juergen: Nano needs a flush, otherwise startup blocks
#endif				
			}
			Dprintf("\r\nconfiguration end\n");
#endif			
      Dprintf("reset initialize\n");                                                                          // 01.05.20:
      Initialize = false;
  }
  Inp_Processed();
}

//--------------------------------------------
uint8_t MobaLedLib_C::Is_Pattern(uint8_t Type)
//--------------------------------------------
// return  0             if it's no pattern
//         PATTERNT1_T   if it's a digital pattern
//        APATTERNT1_T   for analog pattern or XPatternT
{
  if      (Type >= PATTERNT1_T  && Type <= LAST_PATTERN_T)  return PATTERNT1_T;
  else if (Type >= APATTERNT1_T && Type <= LAST_APATTERN_T) return APATTERNT1_T; // 24.08.18:
  return 0;
}

//-------------------------------------
void MobaLedLib_C::Inc_cp(uint8_t Type)
//-------------------------------------
{
  switch (Type)
     {                                                                                                             // #ifdef's to disable some features for memory consumption checks
#                                                                                                                  ifdef _USE_SEP_CONST
     case CONST_T               : IncCP_Const();         break;
#                                                                                                                  endif
#                                                                                                                  if _USE_HOUSE
     case HOUSE_T               : IncCP_House();         break;
#                                                                                                                  endif
#                                                                                                                  if _USE_SET_TVTAB        // 10.01.20:
     case SET_TV_TAB_T          : IncCP_Set_TV_Tab();    break;
#                                                                                                                  endif
#                                                                                                                  if _USE_DEF_NEON         // 12.01.20:
     case SET_DEF_NEON_T        : IncCP_Set_Def_Neon();  break;
#                                                                                                                  endif
#                                                                                                                  if _USE_FIRE
     case FIRE_T                : IncCP_Fire();          break;
#                                                                                                                  endif
#                                                                                                                  if _USE_SET_COLTAB
     case SET_COLTAB_T          : IncCP_Set_ColTab();    break;
#                                                                                                                  endif
#                                                                                                                  if _USE_CANDLE           // 10.06.20:
     case SET_CANDLETAB_T       : IncCP_Set_CandleTab();    break;
#                                                                                                                  endif
#                                                                                                                  if _USE_HSV_GROUP
     case NEW_HSV_GROUP_T       : IncCP_New_HSV_Group(); break;
#                                                                                                                  endif
#                                                                                                                  if _USE_LOCAL_VAR
     case NEW_LOCAL_VAR_T       : IncCP_New_Local_Var(); break;                                                    // 07.11.18:
#                                                                                                                  endif
#                                                                                                                  if _USE_USE_GLOBALVAR
     case USE_GLOBALVAR_T       : IncCP_Use_GlobalVar(); break;
#                                                                                                                  endif
#                                                                                                                  if _USE_INCH_TO_VAR
#if _USE_INCH_TRIGGER                                                                                              // 02.06.20:
     case INCH_TO_X_VAR_T       :
     case BIN_INCH_TO_TMPVAR_T  :IncCP_InCh_to_X_Var();  break;
#else
      case INCH_TO_TMPVAR_T     :
      case INCH_TO_TMPVAR1_T    :
      case BIN_INCH_TO_TMPVAR_T :
      case BIN_INCH_TO_TMPVAR1_T:IncCP_InCh_to_X_Var();  break;
#endif
#                                                                                                                  endif
#                                                                                                                  if _USE_LOGIC
     case LOGIC_T               : IncCP_Logic();         break;
#                                                                                                                  endif
#                                                                                                                  if _USE_RANDOM
     case RANDOM_T              : IncCP_Random();        break;
#                                                                                                                  endif
#                                                                                                                  if _USE_RANMUX
     case RANDMUX_T             : IncCP_RandMux();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_WELDING
     case WELDING_T             : IncCP_Welding();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_COPYLED
     case COPYLED_T             : IncCP_CopyLED();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_SCHEDULE
     case SCHEDULE_T            : IncCP_Schedule();      break;
#                                                                                                                  endif
#                                                                                                                  if _USE_COUNTER
     case COUNTER_T             : IncCP_Counter();       break;
#                                                                                                                  endif
#                                                                                                                  if _USE_PATTERN
     default:                     uint8_t pt = Is_Pattern(Type);
                                  if (pt) IncCP_Pattern(Type - pt + 1);
#                                                                                                                  endif
#                                                                                                                  if _USE_EXT_PROC  // 26.09.21: Juergen
                                  if (CommandProcessorFunc!=NULL) cp += CommandProcessorFunc(Type, cp, false);
#                                                                                                                  endif

     }
}

//----------------------------------------------------
uint8_t MobaLedLib_C::InpChannel_used(uint8_t Channel)
//----------------------------------------------------
// Check if the given input channel is used in the configuration
{
  for (cp = Config; ;)
        {
        uint8_t Type = pgm_read_byte_near(cp++);
        if (Type == END_T) return 0;
#if _USE_LOGIC
        if (Type == LOGIC_T)
             {
             if (Inp_is_Used_in_Logic(Channel)) return 1;
             }
#endif
#if _USE_COUNTER
        if (Type == COUNTER_T)
             {
             if (Channel == pgm_read_byte_near(cp+P_COUNT_INP   ) ||
                 Channel == pgm_read_byte_near(cp+P_COUNT_ENABLE) )  return 1;
             }
#endif
        if (Type < WITHOUT_INP_CH)
             {
             uint8_t InpCh_Offs = Type >= WITHOUT_CHANNEL_MASK ? P_INPCHANEL_NO_CM : P_INPCHANEL_W_CM;
             uint8_t ActCh = pgm_read_byte_near(cp+InpCh_Offs);
             if (ActCh == Channel) return 1;
             }
        Inc_cp(Type);
        }
}


//--------------------------------------------------------------------------------
uint8_t MobaLedLib_C::Find_Next_Priv_InpChannel(uint8_t Channel, int8_t Direction)
//--------------------------------------------------------------------------------
// Find the next or prior used InpChannel in the configuration starting with "Channel".
// If "Direction" is +1 the next channel is returned.
// If "Direction" is -1 the privios channel is returned.
// If "Direction" is  0 the current channel is returned if it's used in the configuration
// otherwiste the next used channel is returned
{
  if (Direction == 0)
     {
     if (InpChannel_used(Channel)) return Channel;
     Direction = +1;
     }
  int cnt = 0;
  while (!InpChannel_used((Channel += Direction)) && cnt++ < 256) ; // Find the next / privios Channel
  return Channel;
}

// Table to convert the bits from the to the bit structure in the InpStructArray. The table contains the values for one nibble (4 bits)
 //                                        Input:   00000000   00000001   00000010   00000011   00000100   00000101   00000110   00000111   00001000   00001001   00001010   00001011   00001100   00001101   00001110   00001111
const PROGMEM uint8_t Bits_to_InpStruct_Tab[16] = { 0b00000000, 0b00000001, 0b00000100, 0b00000101, 0b00010000, 0b00010001, 0b00010100, 0b00010101, 0b01000000, 0b01000001, 0b01000100, 0b01000101, 0b01010000, 0b01010001, 0b01010100, 0b01010101 };

//-----------------------------------------------------------------------------------------------------
void MobaLedLib_C::Copy_Bits_to_InpStructArray(uint8_t *Src, uint8_t ByteCnt, uint8_t InpStructArrayNr)
//-----------------------------------------------------------------------------------------------------
// InpStructArrayNr 0 => Channels 0 .. 3
//                  1 =>          4 .. 7
//                  n =>          n*8 .. n*8+3
//
// Attention: The function writes  8*ByteCnt input channels !!!
{
  uint8_t *Dest = &InpStructArray[InpStructArrayNr];
  while (ByteCnt--)
      {
      *Dest &= _ALL_OLD_BITS; // Clear the new bits (Keep only the old)
      *Dest |= pgm_read_byte_near(&Bits_to_InpStruct_Tab[*Src & 0x0F]);
      Dest++;
      *Dest &= _ALL_OLD_BITS; // Clear the new bits (Keep only the old)
      *Dest |= pgm_read_byte_near(&Bits_to_InpStruct_Tab[(*Src)>>4]);
      Dest++;
      Src++;
      }
}

/*
 - Als Ziel soll die Kanalnummer und nicht die Byte Nummer angegeben werden
 - Die Anzahl der kopierten Bits soll nicht als vielfaches von 8 uebergeben werden
 - Soll auch eine Bitgenaue Start position moeglich sein ? => Nein
*/

//----------------------------------------------------
inline int8_t MobaLedLib_C::Get_Input(uint8_t channel)
//----------------------------------------------------
// Return the state of one input channel
//  INP_OFF
//  INP_ON
//  INP_TURNED_ON
//  INP_TURNED_OFF
{
  uint8_t ByteNr = channel>>2;       // channel / 4;
  uint8_t Shift  = (channel % 4)<<1;
  return (InpStructArray[ByteNr]>>Shift) & 0b00000011;
}
// Optimierungen:
//   - Wenn die ByteNr und Shift Berechnung durch Konstanten ersetzt wird, dann dauert es
//     156us anstelle von 193us. => 20% schneller
//   - Die Verwendung vom mod8() macht es deutlich langsamer ! 228us anstelle von 193us!
//   - ByteNr berechnung in die [] => Kein Unterschied
//   => Keine weitere Idee zur Verbesserung




//-------------------------------------------------------
void MobaLedLib_C::Set_Input(uint8_t channel, uint8_t On)
//-------------------------------------------------------
{
  //if (channel>2) Dprintf("Set_Input %i=%i\n", channel, On?1:0);
  uint8_t ByteNr  = channel>>2;  // / 4;
  uint8_t BitMask = 1 << ((channel % 4)<<1);  // 2^((channel%4)*2)
#if _USE_STORE_STATUS                                                                                         // 01.05.20:
  uint8_t oldValue = InpStructArray[ByteNr]>>((channel % 4)<<1) & 0x03;
  //uint8_t oldValue2 = InpStructArray[ByteNr];
#endif
  if (On)
       InpStructArray[ByteNr] |=  BitMask;
  else InpStructArray[ByteNr] &= ~BitMask;
#if _USE_STORE_STATUS                                                                                         // 19.05.20: Juergen
  //Dprintf("Set_Input Inp[%d] changed from %d to %d (On=%d/%d)\n", channel, oldValue2, (InpStructArray[ByteNr]), On, BitMask);
  Do_Callback(CT_CHANNEL_CHANGED, channel, oldValue, &On);
#endif
}

//--------------------------------
void MobaLedLib_C::Inp_Processed()
//--------------------------------
// Must be called after the Update loop to store the old input values
{
  for (uint8_t *p = InpStructArray, *e = p + _INP_STRUCT_ARRAY_SIZE; p < e; p++)
    {
    uint8_t Act = *p & _ALL_NEW_BITS;
    *p &= _ALL_NEW_BITS;
    *p |= Act << 1;
    }
}

//-------------------------
void MobaLedLib_C::Update()
//-------------------------
// This function has to be called periodicly in the loop() function
// to update all LEDs
{
  #ifdef _TEST_BUTTONS
    Proc_Test_Buttons(); // Buttons to test the LEDs
  #endif


  if (TestMode != TM_LED_TEST) // Call the internal update function which updates the LED's if the LED test is not active.
     {                         // The TM_LED_TEST is activated with the left button. It's used to set the LED's manually.
     Int_Update(millis());
     }
}

#if _USE_STORE_STATUS                                                                                         // 19.05.20: Juergen
void MobaLedLib_C::Do_Callback(uint8_t CallbackType, uint8_t ValueId, uint8_t OldValue, uint8_t *NewValue)
{
    //Dprintf("Do_Callback CallbackType %d ValueId %i OldValue %i NewValue %i\n", CallbackType, ValueId, OldValue, *NewValue);
    if (CallbackFunc!=NULL) CallbackFunc(CallbackType, ValueId, OldValue, NewValue);
}
#endif


