@echo off
goto :start
/*

 Fast Build Script for building ESP32 version of LEDs_AutoProg

 Copyright (C) 2020  Jürgen Winkler: MobaLedLib@gmx.at

 This script is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This script is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this script; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------------------------------------

 Revision History :
 ~~~~~~~~~~~~~~~~~
 10.11.20:  - Versions 1.0 (Juergen)
 11.11.20:  - Added the old path without "\src\" to FastLED and NmraDcc to the gppOpts
              to be able to compile it without changing the libraries
*/

:start
SETLOCAL EnableDelayedExpansion

set home=%~d0%~p0

call :short espbase "%USERPROFILE%\AppData\Local\Arduino15\packages\esp32\"
call :short aTemp "%USERPROFILE%\AppData\Local\Temp\MobaLedLib_build\ESP32"
call :short aCache "%USERPROFILE%\AppData\Local\Temp\MobaLedLib_cache\ESP32"
if not exist "%aTemp%"  md "%aTemp%"
if not exist "%aCache%" md "%aCache%"
set tools=%espbase%\tools
set hardware=%espbase%\hardware\esp32\1.0.4
set include=%espbase%\hardware\esp32\1.0.4\tools\sdk\include
call :short userlibs "%USERPROFILE%\Documents\Arduino\libraries"
set buildDir=%aTemp%

set buildTools=%tools%\xtensa-esp32-elf-gcc\1.22.0-80-g6c4433a-5.2.0/bin

call :write "%buildTools%/xtensa-esp32-elf-g++"  >"%aTemp%\compile.cmd"

REM Added the old path without "\src\" to FastLED and NmraDcc                 11.11.20:
call :write -DESP_PLATFORM "-DMBEDTLS_CONFIG_FILE=\"mbedtls/esp_config.h\"" -DHAVE_CONFIG_H -DGCC_NOT_5_2_0=0 -DWITH_POSIX ^
    -iprefix %include%\  ^
    -iwithprefix config            -iwithprefix app_trace       -iwithprefix app_update -iwithprefix asio            -iwithprefix bootloader_support ^
    -iwithprefix bt                -iwithprefix coap            -iwithprefix console    -iwithprefix driver          -iwithprefix esp-tls ^
    -iwithprefix esp32             -iwithprefix esp_adc_cal     -iwithprefix esp_event  -iwithprefix esp_http_client -iwithprefix esp_http_server ^
    -iwithprefix esp_https_ota     -iwithprefix esp_ringbuf     -iwithprefix ethernet   -iwithprefix expat           -iwithprefix fatfs ^
    -iwithprefix freemodbus        -iwithprefix freertos        -iwithprefix heap       -iwithprefix idf_test        -iwithprefix jsmn ^
    -iwithprefix json              -iwithprefix libsodium       -iwithprefix log        -iwithprefix lwip            -iwithprefix mbedtls ^
    -iwithprefix mdns              -iwithprefix micro-ecc       -iwithprefix mqtt       -iwithprefix newlib          -iwithprefix nghttp ^
    -iwithprefix nvs_flash         -iwithprefix openssl         -iwithprefix protobuf-c -iwithprefix protocomm       -iwithprefix pthread ^
    -iwithprefix sdmmc             -iwithprefix smartconfig_ack -iwithprefix soc        -iwithprefix spi_flash       -iwithprefix spiffs ^
    -iwithprefix tcp_transport     -iwithprefix tcpip_adapter   -iwithprefix ulp        -iwithprefix vfs             -iwithprefix wear_levelling ^
    -iwithprefix wifi_provisioning -iwithprefix wpa_supplicant  -iwithprefix xtensa-debug-module -iwithprefix esp-face -iwithprefix esp32-camera ^
    -iwithprefix esp-face          -iwithprefix fb_gfx ^
    -std=gnu++11 -Os -g3 -Wpointer-arith -fexceptions -fstack-protector -ffunction-sections -fdata-sections -fstrict-volatile-bitfields ^
    -mlongcalls -nostdlib -Wno-error=maybe-uninitialized -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable ^
    -Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-unused-but-set-parameter -Wno-missing-field-initializers -Wno-sign-compare ^
    -fno-rtti -MMD -c -DF_CPU=240000000L -DARDUINO=10813 -DARDUINO_ESP32_DEV -DARDUINO_ARCH_ESP32 ^
    "-DARDUINO_BOARD=\"ESP32_DEV\"" "-DARDUINO_VARIANT=\"esp32\"" -DESP32 -DCORE_DEBUG_LEVEL=0 >>"%aTemp%\compile.cmd"


rem add additional compiler defines, take it from build.options.json
if exist "%atemp%\build.options.json" (
	for /f "delims=: tokens=1,*" %%i in ("%atemp%\build.options.json") do (
		for /f tokens^=1^,2^,3^ delims^=^" %%a in ("%%i") do (
				set token=%%b
			)
			set items=
			if "!token!"=="customBuildProperties" if not "%%j"=="" (
				set items=%%j
			)
			:repeat
			if not "!items!"=="" (
				for /f "tokens=1* delims=," %%a IN ("!items!") DO (
					set line=%%a
					if "!line:~0,14!"=="build.defines=" (
						set opts=!line:~14!
						if -!opts:~-1!-==-^"- set opts=!opts:~0,-1!
						call :write !opts! >>"%aTemp%\compile.cmd"
					) else (
						set items=%%b
						goto :repeat
					)
				)
			)
		)
	)
)

rem now add all include directories previously harvested by ArduinoBuilder
if not exist "%atemp%\includes.cache" (
	echo can't find file includes.cache, aborting build
	exit /b 4
)

for /f "delims=: tokens=1,*" %%i in (%atemp%\includes.cache) do (
	for /f tokens^=1^,2^,3^ delims^=^" %%a in ("%%i") do (
			set token=%%b
		)
		if "!token!"=="Includepath" if not "%%j"=="" (
			if not "%%j"==" null" (
				set dir=%%j
				set dir=!dir:\\=\!
				set dir=!dir:"=!
				set dir=!dir:~1!
				call ::write "-I!dir!" >>"%aTemp%\compile.cmd"
			)
		)
	)
)
echo off

call :write %%%%* >>"%atemp%\compile.cmd"

set gccOpts=-nostdlib "-L%hardware%/tools/sdk/lib" "-L%hardware%/tools/sdk/ld" -T esp32_out.ld -T esp32.common.ld -T esp32.rom.ld -T esp32.peripherals.ld -T esp32.rom.libgcc.ld -T esp32.rom.spiram_incompatible_fns.ld -u ld_include_panic_highint_hdl -u call_user_start_cpu0 -Wl,--gc-sections -Wl,-static -Wl,--undefined=uxTopUsedPriority -u __cxa_guard_dummy -u __cxx_fatal_exception -Wl,--start-group  -lgcc -lesp32 -lphy -lesp_http_client -lmbedtls -lrtc -lesp_http_server -lbtdm_app -lspiffs -lbootloader_support -lmdns -lnvs_flash -lfatfs -lpp -lnet80211 -ljsmn -lface_detection -llibsodium -lvfs -ldl_lib -llog -lfreertos -lcxx -lsmartconfig_ack -lxtensa-debug-module -lheap -ltcpip_adapter -lmqtt -lulp -lfd -lfb_gfx -lnghttp -lprotocomm -lsmartconfig -lm -lethernet -limage_util -lc_nano -lsoc -ltcp_transport -lc -lmicro-ecc -lface_recognition -ljson -lwpa_supplicant -lmesh -lesp_https_ota -lwpa2 -lexpat -llwip -lwear_levelling -lapp_update -ldriver -lbt -lespnow -lcoap -lasio -lnewlib -lconsole -lapp_trace -lesp32-camera -lhal -lprotobuf-c -lsdmmc -lcore -lpthread -lcoexist -lfreemodbus -lspi_flash -lesp-tls -lwpa -lwifi_provisioning -lwps -lesp_adc_cal -lesp_event -lopenssl -lesp_ringbuf -lfr -lstdc++ -Wl,--end-group -Wl,-EL

set filter=*.cpp
if not "%1"=="allLocal" set filter=LEDs_AutoProg.ino.cpp

for %%f in ("%buildDir%\sketch\%filter%") do (
  echo Building %%~nf
	call "%atemp%\compile.cmd" "%%f" -o "%%f.o"
	if errorlevel 1 goto :eof
)

if "%1"=="additional" (
    if exist AdditionalBuildFiles.txt (
       for /F %%f in (AdditionalBuildFiles.txt) do (
          if exist "%%f" (
            echo Building extra file %%f
            call "%atemp%\compile.cmd" "%buildDir%\sketch\%%f" -o "%buildDir%\sketch\%%f.o"
            if errorlevel 1 goto :eof
          )
        )
    )
    if exist AdditionalLibFiles.txt (
       for /F "tokens=1,2 delims=," %%f in (AdditionalLibFiles.txt) do (
          if exist "%userlibs%\%%f" (
            echo Building extra file %%f
            call "%atemp%\compile.cmd" "%userlibs%\%%f" -o "%buildDir%\libraries\%%g.o"
            if errorlevel 1 goto :eof
          )
        )
    )
)

:: generate a linker script with a list of object files
SetLocal EnableDelayedExpansion

if exist "%aTemp%\link.cmd" (
	echo using existing link script
) else (
        echo Creating link file
	call :write %buildTools%/xtensa-esp32-elf-gcc >"%aTemp%\link.cmd"

	FOR /f "tokens=*" %%f in ('dir /B /ON /S "%aTemp%\sketch\*.o"') DO (
		set "fullPath=%%f"
		call :write "!fullPath!" >>"%aTemp%\link.cmd"
	)
	FOR /f "tokens=*" %%f in ('dir /B /ON /S "%aTemp%\libraries\*.o"') DO (
		set "fullPath=%%f"
		call :write "!fullPath!" >>"%aTemp%\link.cmd"
	)
	FOR /f "tokens=*" %%f in ('dir /B /ON /S "%aTemp%\core\*.a"') DO (
		set "fullPath=%%f"
		call :write "!fullPath!" >>"%aTemp%\link.cmd"
	)

	rem call :write %objects% >>"%aTemp%\link.cmd"
	call :write %gccOpts% >>"%aTemp%\link.cmd"
	call :write -o %buildDir%/LEDs_AutoProg.ino.elf >>"%aTemp%\link.cmd"
)
echo Linking
call "%aTemp%\link.cmd"
if errorlevel 1 (
	echo link failed
	exit /b %errorlevel%
)

echo Creating binary files
"%tools%\esptool_py\2.6.1/esptool.exe" --chip esp32 elf2image --flash_mode dio --flash_freq 80m --flash_size 4MB -o "%buildDir%/LEDs_AutoProg.ino.bin" "%buildDir%/LEDs_AutoProg.ino.elf"
if errorlevel 1 (
	echo create binary files failed
	exit /b %errorlevel%
)

:: exit the script
goto :eof


::change drive and directory
:changeDir
%~d1%
cd %~p1%
goto :eof

:: write a text without newline
:write
echo | set /p x="%* "
goto :eof

:short
set %1=%~s2
goto :eof
