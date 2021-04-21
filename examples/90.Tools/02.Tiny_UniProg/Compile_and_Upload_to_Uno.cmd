@ECHO OFF
REM Arduino parameters see:
REM   https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
REM
REM Revision History:
REM 05.06.20:  - Hiding the Debug messages from the compiler
REM 29.12.20:  - Added error message if Arduino debugger is missing

REM Used additional resources:
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~
REM - TimerOne.h        O.K.
REM - SoftwareSerial.h: Standard
REM - SPI.h:            Standard


REM Black on Cyan
COLOR 30

ECHO Programmierung des Arduino Uno als ISP fr die Tiny_UniProg Platine
ECHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ECHO Mit diesem Programm wird das Programm zum Tiny_UniProg Programmiergeraet geschickt.
ECHO Dieser Vorgang muss nur einmal durchgefuehrt werden.
ECHO.

SET DefaultPort=7

SET ComPort=%1
IF NOT "%ComPort%" == "" Goto PortIsSet
   SET /P PortNr=COM port Nummer an den der Tiny_UniProg angeschlossen ist [%DefaultPort%]:
   IF "%PortNr%" == "" SET PortNr=%DefaultPort%
   SET ComPort=\\.\COM%PortNr%
:PortIsSet

iF EXIST "Compile_and_Upload_to_Uno_Result.txt" DEL "Compile_and_Upload_to_Uno_Result.txt" > NUL


IF NOT EXIST "%USERPROFILE%\Documents\Arduino\libraries\TimerOne\" (
   ECHO **********************************
   ECHO * Installing TimerOne library... *
   ECHO **********************************
   ECHO.
   "C:\Program Files (x86)\Arduino\arduino_debug.exe" --install-library "TimerOne"
   )

ECHO.
ECHO.
ECHO **********************************
ECHO * Compile and uplaod the program *
ECHO **********************************
ECHO.
CHCP 65001 >NUL

if not exist "C:\Program Files (x86)\Arduino\arduino_debug.exe" (
  ECHO.
  ECHO ************************************
  ECHO Error: Arduino program not installed
  ECHO ************************************
  ECHO .
  Goto ErrorMsg
  )

"C:\Program Files (x86)\Arduino\arduino_debug.exe" "02.Tiny_UniProg.ino" ^
   --upload ^
   --port %ComPort% ^
   --board arduino:avr:uno --pref programmer=arduino:arduinoisp ^
   2>&1 | find /v "Set log4j store directory" | find /v " StatusLogger " | find /v "serial.SerialDiscovery"

IF ERRORLEVEL 1 (
:ErrorMsg
   REM White on RED
   COLOR 4F
   ECHO Start_Arduino_Result: %ERRORLEVEL% > "Compile_and_Upload_to_Uno_Result.txt"
   ECHO   **********************************
   ECHO   * Da ist was schief gegangen ;-( *            ERRORLEVEL %ERRORLEVEL%
   ECHO   **********************************
   Pause
   )
