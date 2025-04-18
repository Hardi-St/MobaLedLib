/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2020 Jürgen Winkler: MobaLedLib@gmx.at

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

 Revision History :
 ~~~~~~~~~~~~~~~~~
 currently no release deployed, still under construction

*/

#ifndef ESP32EXTENSIONS_H
#define ESP32EXTENSIONS_H
#define ESP32WIFI_H
#include "UserInterface.h"

//#define USE_WIFI
//#define USE_OTA
//#define USE_UI

#ifdef USE_OTA
  #ifndef USE_WIFI
  #define USE_WIFI
  #endif
  #include <ArduinoOTA.h>
#endif
#ifdef USE_WIFI
  #ifndef WIFI_DNS_NAME 
    #define WIFI_DNS_NAME "MobaLedLib"
    #undef Sec
#endif
#include <WiFi.h>
#include <WiFiClient.h>
#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager
#include <WiFiUdp.h>
#include <ESPmDNS.h>
//#include "RASCIIInterface.h"
//RASCIIInterface interface;

#endif

#define DEBUG Serial

#ifdef USE_UI
#ifndef OLED_TYP
  #define OLED_TYP 2
#endif
#ifndef UI_MLLTime					// Peter
  #define UI_MLLTime 0				// Peter   0 = keine Anzeige, 1 = MLL-Zeit, 2 = LDR-Wert, 3 = Zeit und LDR-Wert
#endif								// Peter
#include "U8G2UserInterface.h"
UserInterface ui(getUserInterface);
#endif

#ifdef USE_WIFI
bool isConnected = false;
bool isInOTA = false;
WiFiManager wm; // global wm instance
String hostString;

String toStringIp(IPAddress ip) {
  String res = "";
  for (int i = 0; i < 3; i++) {
    res += String((ip >> (8 * i)) & 0xFF) + ".";
  }
  res += String(((ip >> 8 * 3)) & 0xFF);
  return res;
}

void setConnected(bool connected)
{
	if (connected==isConnected) return;		// connection state didn't change

	isConnected=connected;
	if (isConnected)
	{
		Serial.println("starting config portal");
		wm.startConfigPortal(hostString.c_str());
		Serial.println("starting config portal done");
#ifdef USE_OTA
		Serial.println("starting Arduino OTA");

		ArduinoOTA.onStart([]() {
			String type;
                        isInOTA = true;
			if (ArduinoOTA.getCommand() == U_FLASH)
			type = "sketch";
			else // U_SPIFFS
			type = "filesystem";

			// NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
			Serial.println("Start updating " + type);
			FastLED.setBrightness(0); // turn leds off
			FastLED.clear(256);
		})
		.onEnd([]() {
			Serial.println("\nEnd");
                        isInOTA = false;
		})
		.onProgress([](unsigned int progress, unsigned int total) {
			//Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
		})
		.onError([](ota_error_t error) {
			Serial.printf("Error[%u]: ", error);
			if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
			else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
			else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
			else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
			else if (error == OTA_END_ERROR) Serial.println("End Failed");
                        isInOTA = false;
			FastLED.setBrightness(255); // turn leds on
		});

		ArduinoOTA.begin();
#endif

    if(!MDNS.begin(WIFI_DNS_NAME)) {
       Serial.println("Error starting mDNS");
    }
   
	}

#ifdef USE_UI
	if (isConnected)
	{
		ui.setIPAddress(toStringIp(WiFi.localIP()));
	}
	else
	{
		ui.setIPAddress(toStringIp(WiFi.softAPIP()));
	}
	ui.setConnected(isConnected);
#endif
}
#endif

/**********************************************************************************/
void setupESP32Extensions() {
#ifdef USE_UI
	ui.setup();
#endif
	
#ifdef USE_WIFI
	WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP

    //reset settings - wipe credentials for testing
    //wm.resetSettings();

    wm.setConfigPortalBlocking(false);
    wm.setTitle(String(WIFI_DNS_NAME).c_str());

    //automatically connect using saved credentials if they exist
    //If connection fails it starts an access point with the specified name
    //generate the accespoint name

	hostString = String(WIFI_getChipId(),HEX);
	hostString.toUpperCase();
	hostString = "MLL_" + hostString;
	wm.setHostname(hostString.c_str());
#ifdef USE_OTA
	ArduinoOTA.setHostname(hostString.c_str());
#endif

	bool connected = false;

	for (int i=0;i<2&& !connected;i++)
	{
    connected = wm.autoConnect(hostString.c_str());
	}
	setConnected(connected);

#if defined(DEBUG)
	if (connected)
	{
		Serial.println("connected");
	}
	else
	{
		Serial.println("local AP started");
	}
#endif
#endif
}


void loopESP32Extensions() {
#ifdef USE_WIFI
	wm.process();
	#ifdef USE_OTA
	if (isConnected) 
  {
    // handle and OTA upload and block other operations while OTA running
    do
    {
       ArduinoOTA.handle();   
       if (isInOTA) 
       {
         esp_task_wdt_reset();                                                                                    // 05.03.21: Juergen -  reset watchdog timer, because we are in an endless loop
         vTaskDelay(10);
         yield();
       }
    } 
    while(isInOTA);
  }
	#endif
#endif	

#ifdef USE_UI
  #ifdef DISPLAY_FASTLED_FAULTS
    ui.setCounters(delayCount, reviveCount);
  #endif
	ui.loop();
#endif	
}
void loopESP32Extensions2() {
#ifdef USE_WIFI
#ifdef USE_OTA
   // block other operations while OTA upload
   while (isInOTA) 
   {
     esp_task_wdt_reset();                                                                                    // 05.03.21: Juergen -  reset watchdog timer, because we are in an endless loop
     vTaskDelay(10);
     yield();
   }
#endif
#endif
}
#endif //ESP32EXTENSIONS_H