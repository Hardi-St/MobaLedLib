@ECHO OFF
REM Arduino parameters see:
REM   https://github.com/arduino/Arduino/blob/master/build/shared/manpage.adoc
REM Revision History:
REM 15.10.20:  - Extracted from Compile_and_Upload_to_ATTiny85.cmd

REM Parameter:
REM   %1: COM Port

"C:\Program Files (x86)\Arduino\arduino_debug.exe" "Any_detected_ino.ino" ^
   --upload ^
   --port %1 ^
   --pref programmer=arduino:arduinoasisp ^
   --board ATTinyCore:avr:attinyx5:LTO=disable,TimerClockSource=default,chip=85,clock=16pll,eesave=aenable,bod=2v7,millis=enabled

IF ERRORLEVEL 1 ECHO Start_Arduino_Result: %ERRORLEVEL% > "Compile_and_Upload_to_ATTiny85_Result.txt"

