@ECHO OFF
REM Arduino parameters see:
REM   https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
REM Revision History:
REM 05.06.20:  - Hiding the Debug messages from the compiler
REM 04.08.20:  - Changed the CPU Clock to 16MHz (Old: 8MHz) clock=8internal => clock=16pll
REM 15.10.20:  - Extracted the compiler call into "Compile_and_Upload_to_ATTiny85_Sub.cmd"
REM              to be able to check the error code. Prior the Find command has overwritten
REM              the result => Errors have not been detected ;-(


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


