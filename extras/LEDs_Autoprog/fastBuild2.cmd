@echo off
goto :start
/*

 Fast Build Script for building ESP32 version of LEDs_AutoProg

 Copyright (C) 2020-2025 Jürgen Winkler: MobaLedLib@gmx.at

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
 23.01.25:  - Versions 1.0 (Juergen) for board version 2.0.17
*/

:start
SETLOCAL EnableDelayedExpansion

set home=%~d0%~p0

call :short espbase "%USERPROFILE%\AppData\Local\Arduino15\packages\esp32\"
call :short aTemp "%USERPROFILE%\AppData\Local\Temp\MobaLedLib_build\ESP32"
call :short aCache "%USERPROFILE%\AppData\Local\Temp\MobaLedLib_cache\ESP32"
if not exist "%aTemp%"  md "%aTemp%"
if not exist "%aCache%" md "%aCache%"
set tools=%espbase%tools
set hardware=%espbase%hardware\esp32\2.0.17
set include=%hardware%\tools\sdk\esp32\include
call :short userlibs "%USERPROFILE%\Documents\Arduino\libraries"
set buildDir=%aTemp%

set buildTools=%tools%\xtensa-esp32-elf-gcc\esp-2021r2-patch5-8.4.0/bin

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

rem if exist "%aTemp%\compile.txt" goto next
rem for /f "delims=*" %%a in ('dir %include% /AD /B /S') do if "%%~na"=="include" call ::write "-I%%a" >>"%aTemp%\compile.cmd"
rem goto next

REM echo/>compile.txt
REM for /f %%a in (includeDir.txt) do call ::write "-I%include%/%%a" >>"%aTemp%\compile.txt"
REM call :write @"%aTemp%\compile.txt" >>"%atemp%\compile.cmd"

call ::write "-I%include%/freertos/include" >>"%aTemp%\compile.cmd"
call ::write "-I%include%/freertos/include/esp_additions/freertos" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%/freertos/include/esp_additions" >>"%aTemp%\compile.cmd" 
call ::write "-I%fullPath%/tools\sdk\esp32\include\freertos\include\esp_additions" >>"%aTemp%\compile.cmd" 
call ::write "-I%fullPath%/tools/sdk/esp32/qio_qspi/include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%/esp_common/include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%/freertos/port/xtensa/include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%/xtensa/include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%/xtensa/esp32/include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%/esp_rom/include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\soc\esp32\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\esp_hw_support\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\hal\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\hal\esp32\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\soc\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\esp_system\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\esp_timer\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\newlib\platform_include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\heap\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\log\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\driver\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\spi_flash\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\esp_hw_support\include\soc" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\hal\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%include%\esp_ringbuf\include" >>"%aTemp%\compile.cmd" 
call ::write "-I%hardware%/cores/esp32" >>"%aTemp%\compile.cmd" 
call ::write "-I%hardware%/tools/sdk/esp32/qio_qspi/include" >>"%aTemp%\compile.cmd" 

call :write %%%%* >>"%atemp%\compile.cmd"

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

set gccOpts=-nostdlib "-L%hardware%/tools/sdk/esp32/lib" "-L%hardware%/tools/sdk/esp32/ld" "-L%hardware%/tools/sdk/esp32/qio_qspi" 
set gccOpts2=-T esp32.rom.redefined.ld -T memory.ld -T sections.ld -T esp32.rom.ld -T esp32.rom.api.ld -T esp32.rom.libgcc.ld -T esp32.rom.newlib-data.ld -T esp32.rom.syscalls.ld -T esp32.peripherals.ld -mlongcalls -Wno-frame-address -Wl,--cref -Wl,--gc-sections -fno-rtti -fno-lto -Wl,--wrap=esp_log_write -Wl,--wrap=esp_log_writev -Wl,--wrap=log_printf -u ld_include_hli_vectors_bt -u _Z5setupv -u _Z4loopv -u esp_app_desc -u pthread_include_pthread_impl -u pthread_include_pthread_cond_var_impl -u pthread_include_pthread_local_storage_impl -u pthread_include_pthread_rwlock_impl -u include_esp_phy_override -u ld_include_highint_hdl -u start_app -u start_app_other_cores -u __ubsan_include -Wl,--wrap=longjmp -u __assert_func -u vfs_include_syscalls_impl -Wl,--undefined=uxTopUsedPriority -u app_main -u newlib_include_heap_impl -u newlib_include_syscalls_impl -u newlib_include_pthread_impl -u newlib_include_assert_impl -u __cxa_guard_dummy -DESP32 -DCORE_DEBUG_LEVEL=0 -DARDUINO_RUNNING_CORE=1 -DARDUINO_EVENT_RUNNING_CORE=1 -DARDUINO_USB_CDC_ON_BOOT=0 -Wl,--start-group  -lesp_ringbuf -lefuse -lesp_ipc -ldriver -lesp_pm -lmbedtls -lapp_update -lbootloader_support -lspi_flash -lnvs_flash -lpthread -lesp_gdbstub -lespcoredump -lesp_phy -lesp_system -lesp_rom -lhal -lvfs -lesp_eth -ltcpip_adapter -lesp_netif -lesp_event -lwpa_supplicant -lesp_wifi -lconsole -llwip -llog -lheap -lsoc -lesp_hw_support -lxtensa -lesp_common -lesp_timer -lfreertos -lnewlib -lcxx -lapp_trace -lasio -lbt -lcbor -lunity -lcmock -lcoap -lnghttp -lesp-tls -lesp_adc_cal -lesp_hid -ltcp_transport -lesp_http_client -lesp_http_server -lesp_https_ota -lesp_https_server -lesp_lcd -lprotobuf-c -lprotocomm -lmdns -lesp_local_ctrl -lsdmmc -lesp_serial_slave_link -lesp_websocket_client -lexpat -lwear_levelling -lfatfs -lfreemodbus -ljsmn -ljson -llibsodium -lmqtt -lopenssl -lperfmon -lspiffs -lulp -lwifi_provisioning -lrmaker_common -lesp_diagnostics -lrtc_store -lesp_insights -ljson_parser -ljson_generator -lesp_schedule -lespressif__esp_secure_cert_mgr -lesp_rainmaker -lgpio_button -lqrcode -lws2812_led -lesp32-camera -lesp_littlefs -lespressif__esp-dsp -lfb_gfx -lasio -lcmock -lunity -lcoap -lesp_lcd -lesp_websocket_client -lexpat -lfreemodbus -ljsmn -llibsodium -lperfmon -lesp_adc_cal -lesp_hid -lfatfs -lwear_levelling -lopenssl -lspiffs -lesp_insights -lcbor -lesp_diagnostics -lrtc_store -lesp_rainmaker -lesp_local_ctrl -lesp_https_server -lwifi_provisioning -lprotocomm -lbt -lbtdm_app -lprotobuf-c -lmdns -ljson -ljson_parser -ljson_generator -lesp_schedule -lespressif__esp_secure_cert_mgr -lqrcode -lrmaker_common -lmqtt -lcat_face_detect -lhuman_face_detect -lcolor_detect -lmfn -ldl -lesp_ringbuf -lefuse -lesp_ipc -ldriver -lesp_pm -lmbedtls -lapp_update -lbootloader_support -lspi_flash -lnvs_flash -lpthread -lesp_gdbstub -lespcoredump -lesp_phy -lesp_system -lesp_rom -lhal -lvfs -lesp_eth -ltcpip_adapter -lesp_netif -lesp_event -lwpa_supplicant -lesp_wifi -lconsole -llwip -llog -lheap -lsoc -lesp_hw_support -lxtensa -lesp_common -lesp_timer -lfreertos -lnewlib -lcxx -lapp_trace -lnghttp -lesp-tls -ltcp_transport -lesp_http_client -lesp_http_server -lesp_https_ota -lsdmmc -lesp_serial_slave_link -lulp -lmbedtls_2 -lmbedcrypto -lmbedx509 -lcoexist -lcore -lespnow -lmesh -lnet80211 -lpp -lsmartconfig -lwapi -lesp_ringbuf -lefuse -lesp_ipc -ldriver -lesp_pm -lmbedtls -lapp_update -lbootloader_support -lspi_flash -lnvs_flash -lpthread -lesp_gdbstub -lespcoredump -lesp_phy -lesp_system -lesp_rom -lhal -lvfs -lesp_eth -ltcpip_adapter -lesp_netif -lesp_event -lwpa_supplicant -lesp_wifi -lconsole -llwip -llog -lheap -lsoc -lesp_hw_support -lxtensa -lesp_common -lesp_timer -lfreertos -lnewlib -lcxx -lapp_trace -lnghttp -lesp-tls -ltcp_transport -lesp_http_client -lesp_http_server -lesp_https_ota -lsdmmc -lesp_serial_slave_link -lulp -lmbedtls_2 -lmbedcrypto -lmbedx509 -lcoexist -lcore -lespnow -lmesh -lnet80211 -lpp -lsmartconfig -lwapi -lesp_ringbuf -lefuse -lesp_ipc -ldriver -lesp_pm -lmbedtls -lapp_update -lbootloader_support -lspi_flash -lnvs_flash -lpthread -lesp_gdbstub -lespcoredump -lesp_phy -lesp_system -lesp_rom -lhal -lvfs -lesp_eth -ltcpip_adapter -lesp_netif -lesp_event -lwpa_supplicant -lesp_wifi -lconsole -llwip -llog -lheap -lsoc -lesp_hw_support -lxtensa -lesp_common -lesp_timer -lfreertos -lnewlib -lcxx -lapp_trace -lnghttp -lesp-tls -ltcp_transport -lesp_http_client -lesp_http_server -lesp_https_ota -lsdmmc -lesp_serial_slave_link -lulp -lmbedtls_2 -lmbedcrypto -lmbedx509 -lcoexist -lcore -lespnow -lmesh -lnet80211 -lpp -lsmartconfig -lwapi -lesp_ringbuf -lefuse -lesp_ipc -ldriver -lesp_pm -lmbedtls -lapp_update -lbootloader_support -lspi_flash -lnvs_flash -lpthread -lesp_gdbstub -lespcoredump -lesp_phy -lesp_system -lesp_rom -lhal -lvfs -lesp_eth -ltcpip_adapter -lesp_netif -lesp_event -lwpa_supplicant -lesp_wifi -lconsole -llwip -llog -lheap -lsoc -lesp_hw_support -lxtensa -lesp_common -lesp_timer -lfreertos -lnewlib -lcxx -lapp_trace -lnghttp -lesp-tls -ltcp_transport -lesp_http_client -lesp_http_server -lesp_https_ota -lsdmmc -lesp_serial_slave_link -lulp -lmbedtls_2 -lmbedcrypto -lmbedx509 -lcoexist -lcore -lespnow -lmesh -lnet80211 -lpp -lsmartconfig -lwapi -lesp_ringbuf -lefuse -lesp_ipc -ldriver -lesp_pm -lmbedtls -lapp_update -lbootloader_support -lspi_flash -lnvs_flash -lpthread -lesp_gdbstub -lespcoredump -lesp_phy -lesp_system -lesp_rom -lhal -lvfs -lesp_eth -ltcpip_adapter -lesp_netif -lesp_event -lwpa_supplicant -lesp_wifi -lconsole -llwip -llog -lheap -lsoc -lesp_hw_support -lxtensa -lesp_common -lesp_timer -lfreertos -lnewlib -lcxx -lapp_trace -lnghttp -lesp-tls -ltcp_transport -lesp_http_client -lesp_http_server -lesp_https_ota -lsdmmc -lesp_serial_slave_link -lulp -lmbedtls_2 -lmbedcrypto -lmbedx509 -lcoexist -lcore -lespnow -lmesh -lnet80211 -lpp -lsmartconfig -lwapi -lphy -lrtc -lesp_phy -lphy -lrtc -lesp_phy -lphy -lrtc -lxt_hal -lc -lm -lnewlib -lstdc++ -lpthread -lgcc -lcxx -Wl,--end-group -Wl,-EL

if exist "%aTemp%\link.cmd" (
	echo using existing link script
) else (
	echo Creating link file
	call :write %buildTools%/xtensa-esp32-elf-g++ @%aTemp%\link.txt > "%aTemp%\link.cmd"
	set fullPath=%aTemp%"
	set fullPath=!fullPath:\=\\!
	set fullPath=!fullPath:"=!
	call :write "-Wl,--Map=!fullPath!\\LEDs_AutoProg.ino.map" >> "%aTemp%\link.txt"

	set fullPath=%hardware%
	set fullPath=!fullPath:\=\\!
	call :write "-L!fullPath!\\tools\\sdk\\esp32\\ld" >> "%aTemp%\link.txt"
	call :write "-L!fullPath!\\tools\\sdk\\esp32\\lib" >> "%aTemp%\link.txt"
	call :write "-L!fullPath!\\tools\\sdk\\esp32\\qio_qspi" >> "%aTemp%\link.txt" 

	FOR /f "tokens=*" %%f in ('dir /B /ON /S "%aTemp%\sketch\*.o"') DO (
		set "fullPath=%%f"
        set fullPath=!fullPath:\=\\!
		call :write "!fullPath!" >>"%aTemp%\link.txt"
	)
	FOR /f "tokens=*" %%f in ('dir /B /ON /S "%aTemp%\libraries\*.o"') DO (
		set "fullPath=%%f"
        set fullPath=!fullPath:\=\\!
		call :write "!fullPath!" >>"%aTemp%\link.txt"
	)
	FOR /f "tokens=*" %%f in ('dir /B /ON /S "%aTemp%\core\*.a"') DO (
		set "fullPath=%%f"
        set fullPath=!fullPath:\=\\!
		call :write "!fullPath!" >>"%aTemp%\link.txt"
	)

	call :write %gccOpts% %gccOpts2% >>"%aTemp%\link.txt"

    set fullPath=%buildDir%/LEDs_AutoProg.ino.elf
    set fullPath=!fullPath:\=\\!
	call :write -o !fullPath! >>"%aTemp%\link.txt"
)
echo Linking
call "%aTemp%\link.cmd"
if errorlevel 1 (
	echo link failed
	exit /b %errorlevel%
)

echo Creating binary files
"%hardware%\tools\gen_esp32part.exe" -q "%hardware%\tools\partitions\default.csv" "%buildDir%//LEDs_AutoProg.ino.partitions.bin"
"%tools%\esptool_py\4.5.1/esptool.exe" --chip esp32 elf2image --flash_mode dio --flash_freq 80m --flash_size 4MB -o "%buildDir%/LEDs_AutoProg.ino.bin" "%buildDir%/LEDs_AutoProg.ino.elf"
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
