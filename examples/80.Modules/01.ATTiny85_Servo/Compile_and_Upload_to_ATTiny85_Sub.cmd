@ECHO OFF
REM Arduino parameters see:
REM   https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
REM Revision History:
REM 15.10.20:  - Extracted from Compile_and_Upload_to_ATTiny85.cmd
REM 29.10.21:  - Adapted to 32 bit windows

REM Parameter:
REM   %1: COM Port

REM 29.10.21: Adapt to 32-Bit windows.
IF EXIST "%PROGRAMFILES(X86)%" (set "ProgDir=%PROGRAMFILES(X86)%") ELSE (set "ProgDir=%PROGRAMFILES%")
ECHO ProgDir: %ProgDir%

"%ProgDir%\Arduino\arduino_debug.exe" "Any_detected_ino.ino" ^
   --upload ^
   --port %1 ^
   --pref programmer=arduino:arduinoasisp ^
   --board ATTinyCore:avr:attinyx5:LTO=disable,TimerClockSource=default,chip=85,clock=16pll,eesave=aenable,bod=2v7,millis=enabled

IF ERRORLEVEL 1 ECHO Start_Arduino_Result: %ERRORLEVEL% > "Compile_and_Upload_to_ATTiny85_Result.txt"

