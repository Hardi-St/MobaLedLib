@ECHO OFF
REM Arduino parameters see:
REM   https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
REM Revision History:
REM 05.06.20:  - Hiding the Debug messages from the compiler
REM 15.10.20:  - Extracted the compiler call into "Compile_and_Upload_to_ATTiny85_Sub.cmd"
REM              to be able to check the error code. Prior the Find command has overwritten
REM              the result => Errors have not been detected ;-(

REM Used additional resources:
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~
REM - ATTiny board package: "ATTinyCore by Spencer Konde" (Ver. 1.3.3)
REM   http://drazzy.com/package_drazzy.com_index.json
REM - EwmaT.h: "Exponentially Weighted Moving Average filter" by Arsen Torbarins (Ver. 1.0.2)
REM - FastLED.h:
REM - For debugging:
REM   - ATtinySerialOut.h: "ATtinySerialOut by Joachimsmeyer" (Ver. 1.0.4)
REM     The library is not installed by this program


REM Black on Gray
COLOR 70

ECHO Programmierung des ATTiny85 fuer das Charlieplexing Modul
ECHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ECHO Mit diesem Programm wird das Charlieplexing Programm auf den ATTiny85 gespielt.
ECHO Der ATTiny85 muss dazu in die Tiny_UniProg Platine gesteckt sein.
ECHO Dieser Vorgang muss nur ein mal gemacht werden. Danach wird nur noch die geaenderte
ECHO Konfiguration vom Pattern_Configuartor aus zum ATTiny geschickt.
ECHO.

SET DefaultPort=7

SET ComPort=%1
IF NOT "%ComPort%" == "" Goto PortIsSet
   SET /P PortNr=COM port Nummer an den der Tiny_UniProg angeschlossen ist [%DefaultPort%]:
   IF "%PortNr%" == "" SET PortNr=%DefaultPort%
   SET ComPort=\\.\COM%PortNr%
:PortIsSet

IF EXIST "Compile_and_Upload_to_ATTiny85_Result.txt" DEL "Compile_and_Upload_to_ATTiny85_Result.txt" > NUL

IF NOT EXIST "%USERPROFILE%\AppData\Local\Arduino15\packages\ATTinyCore\hardware\avr\" (
   REM Attention: If the directory exists, but it's empty the installation aborts with the following message:
   REM  Fehler: ATTinyCore: Unbekanntes Paket
   REM
   ECHO ***************************************
   ECHO * Installing ATTiny boards package... *
   ECHO ***************************************
   ECHO.
   REM Is installed to: C:\Users\Hardi\AppData\Local\Arduino15\packages\ATTinyCore\hardware\avr\1.3.3
   REM If the ...\hardware\avr directory is empty it's not installed
   REM For test the 1.3.3 directory could be deleted

   REM Sometimes a CRC error is generated. In this case the installation has to be restarted => errorlevel=255
   REM Message:
   REM    java.lang.Exception: CRC stimmt nicht überein, Datei ist beschädigt. Es kann ein temporäres Problem sein, bitte versuchen Sie es später erneut.
   REM Englisch:
   REM    java.lang.Exception: CRC doesn't match. File is corrupted.
   REM Error doesn't occoure if "C:\Users\Hardi\AppData\Local\Arduino15\staging\packages\ATTinyCore-1.3.3.tar.gz"
   REM is deleted before
   IF EXIST "%USERPROFILE%\AppData\Local\Arduino15\staging\packages\ATTinyCore*.tar.gz" DEL "%USERPROFILE%\AppData\Local\Arduino15\staging\packages\ATTinyCore*.tar.gz"

   REM The additional url is just added temporary
   "C:\Program Files (x86)\Arduino\arduino_debug.exe" ^
     --pref "boardsmanager.additional.urls=http://drazzy.com/package_drazzy.com_index.json" ^
     --install-boards "ATTinyCore:avr"
   ECHO Errorlevl=%errorlevel%

   REM errorlevel=255 if the board is already installed
   REM errorlevel=0 if it was installed sucessfully
   )

IF NOT EXIST "%USERPROFILE%\Documents\Arduino\libraries\EWMA\" (
   ECHO *************************************
   ECHO * Installing EWMA filter package... *
   ECHO *************************************
   ECHO.
   "C:\Program Files (x86)\Arduino\arduino_debug.exe" --install-library "EWMA"
   )

IF NOT EXIST "%USERPROFILE%\Documents\Arduino\libraries\FastLED\" (
   ECHO *********************************
   ECHO * Installing FastLED Library... *
   ECHO *********************************
   ECHO.
   REM                                                                        14.03.20: Old "EWMA" => "FastLED"
   "C:\Program Files (x86)\Arduino\arduino_debug.exe" --install-library "FastLED"
   )

ECHO.
ECHO.
ECHO **********************************
ECHO * Compile and uplaod the program *
ECHO **********************************
ECHO.
CHCP 65001 >NUL

Call Compile_and_Upload_to_ATTiny85_Sub.cmd %ComPort% ^
         2>&1 | find /v "Set log4j store directory" | find /v " StatusLogger " | find /v "serial.SerialDiscovery"

if exist "Compile_and_Upload_to_ATTiny85_Result.txt" (
   REM White on RED
   COLOR 4F
   ECHO   **********************************
   ECHO   * Da ist was schief gegangen ;-( *
   ECHO   **********************************
   Pause
)

