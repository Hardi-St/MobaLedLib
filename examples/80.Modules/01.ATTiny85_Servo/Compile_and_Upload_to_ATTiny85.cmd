@ECHO OFF
REM Arduino parameters see:
REM   https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc


REM Used additional resources:
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~
REM - None

REM Black on Gray
COLOR 70

ECHO Programmierung des ATTiny85 fuer das Servo Modul
ECHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ECHO Mit diesem Programm wird das Servo Programm auf den ATTiny85 gespielt.
ECHO Der ATTiny85 muss dazu in die Tiny_UniProg Platine gesteckt sein.
ECHO Dieser Vorgang muss nur ein mal gemacht werden.
ECHO.

SET DefaultPort=7

SET ComPort=%1
IF NOT "%ComPort%" == "" Goto PortIsSet
   SET /P PortNr=COM port Nummer an den der Tiny_UniProg angeschlossen ist [%DefaultPort%]:
   IF "%PortNr%" == "" SET PortNr=%DefaultPort%
   SET ComPort=\\.\COM%PortNr%
:PortIsSet

iF EXIST "Compile_and_Upload_to_ATTiny85_Result.txt" DEL "Compile_and_Upload_to_ATTiny85_Result.txt" > NUL


ECHO.
ECHO.
ECHO **********************************
ECHO * Compile and uplaod the program *
ECHO **********************************
ECHO.
"C:\Program Files (x86)\Arduino\arduino_debug.exe" "01.ATTiny85_Servo.ino" ^
   --upload ^
   --port %ComPort% ^
   --pref programmer=arduino:arduinoasisp ^
   --board ATTinyCore:avr:attinyx5:LTO=disable,TimerClockSource=default,chip=85,clock=8internal,eesave=aenable,bod=2v7,millis=enabled

IF ERRORLEVEL 1 (
   REM White on RED
   COLOR 4F
   ECHO Start_Arduino_Result: %ERRORLEVEL% > "Compile_and_Upload_to_ATTiny85_Result.txt"
   ECHO   **********************************
   ECHO   * Da ist was schief gegangen ;-( *            ERRORLEVEL %ERRORLEVEL%
   ECHO   **********************************
   Pause
   )
