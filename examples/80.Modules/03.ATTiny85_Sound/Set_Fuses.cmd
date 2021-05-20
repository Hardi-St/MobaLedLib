@ECHO OFF
REM This file was generated by 'Pattern_Configurator.xlsm'  15:06:24

@ECHO OFF
REM Setting the fuses for the ATTiny
REM ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
REM
REM   Arduino Configuration:                            Comments
REM   ~~~~~~~~~~~~~~~~~~~~~~
REM   Board:              'ATtiny25/45/85'
REM   Clock:              '16 MHz (PLL)'
REM   Chip:               'ATTiny85'
REM   B.O.D.Level:        'B.O.D. Enabled (2.7V)'
REM   Save EEPROM:        'EEPROM retained'
REM   Timer 1 Clock:      'CPU'
REM

REM Schwarz auf Ocker
COLOR 60

ECHO Programmierung der Fuses fuer den ATTiny
ECHO ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
ECHO Mit diesem Programm werden die Fuses des ATTiny85 gesetzt.
ECHO Die Fuses bestimmen die Taktfrequenz ('16 MHz (PLL)') und die
ECHO Unterspannungserkennung (B.O.D. Level 'B.O.D. Enabled (2.7V)')
ECHO Der ATTiny85 muss dazu in die Tiny_UniProg Platine gesteckt sein.
ECHO Dieser Vorgang muss nur ein mal gemacht werden. Danach wird nur noch die geaenderte
ECHO Konfiguration vom Pattern_Configuartor aus zum ATTiny geschickt.
ECHO.

SET DefaultPort=5
SET ComPort=%1
IF NOT "%ComPort%" == "" Goto PortIsSet
   SET /P PortNr=COM port Nummer an den der Tiny_UniProg angeschlossen ist [%DefaultPort%]:
   IF "%PortNr%" == "" SET PortNr=%DefaultPort%
   SET ComPort=\\.\COM%PortNr%
:PortIsSet


"%USERPROFILE%\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino18\bin\avrdude.exe" ^
    "-C%USERPROFILE%\AppData\Local\Arduino15\packages\ATTinyCore\hardware\avr\1.5.2\avrdude.conf"  ^
    -v -pattiny85 -cstk500v1 -P%ComPort% -b19200 ^
    -Uefuse:w:0xFF:m -Uhfuse:w:0x55:m -Ulfuse:w:0xF1:m



IF ERRORLEVEL 1 (
    ECHO Error settung the fuses ;-(
    ECHO Starting second try
    timeout /T 3 /nobreak
    "%USERPROFILE%\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino18\bin\avrdude.exe" ^
        "-C%USERPROFILE%\AppData\Local\Arduino15\packages\ATTinyCore\hardware\avr\1.5.2\avrdude.conf"  ^
        -v -pattiny85 -cstk500v1 -P%ComPort% -b19200 ^
        -Uefuse:w:0xFF:m -Uhfuse:w:0x55:m -Ulfuse:w:0xF1:m

   )

IF ERRORLEVEL 1 (
    ECHO Error settung the fuses ;-(((
    ECHO Starting third try
    timeout /T 3 /nobreak
    "%USERPROFILE%\AppData\Local\Arduino15\packages\arduino\tools\avrdude\6.3.0-arduino18\bin\avrdude.exe" ^
        "-C%USERPROFILE%\AppData\Local\Arduino15\packages\ATTinyCore\hardware\avr\1.5.2\avrdude.conf"  ^
        -v -pattiny85 -cstk500v1 -P%ComPort% -b19200 ^
        -Uefuse:w:0xFF:m -Uhfuse:w:0x55:m -Ulfuse:w:0xF1:m

   )

IF ERRORLEVEL 1 (
   REM White on RED
   COLOR 4F
   ECHO Set_Fuses Result: %ERRORLEVEL% > "Set_Fuses_Result.txt"
   ECHO   *********************************************
   ECHO  * Da ist was schief gegangen ;-(            *              ERRORLEVEL %ERRORLEVEL%
   ECHO   *********************************************
   Pause
   )