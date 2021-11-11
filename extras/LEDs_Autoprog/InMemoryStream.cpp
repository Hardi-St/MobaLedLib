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


 InMemory Stream implementation       
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Revision History :
~~~~~~~~~~~~~~~~~
12.11.20:  Versions 1.0 (Jürgen)
22.04.21:   add support for Raspberry Pico (Jürgen)
*/

#if defined(ESP32) || defined(ARDUINO_RASPBERRY_PI_PICO)
#include "InMemoryStream.h"

static volatile char    *pSendBuffer;
static volatile char    *rp;        // Queue Read Pointer
static volatile char    *wp;        // Queue Write Pointer
static volatile char    *pEndSendBuffer;

#if defined(ESP32)
	static portMUX_TYPE			stream_mutex;
	#define STREAM_MUTEX_ENTER   portENTER_CRITICAL(&stream_mutex);
	#define STREAM_MUTEX_EXIT    portEXIT_CRITICAL(&stream_mutex);
#elif defined(ARDUINO_RASPBERRY_PI_PICO)
	auto_init_mutex(stream_mutex);
	#define STREAM_MUTEX_ENTER   mutex_enter_blocking(&stream_mutex);
	#define STREAM_MUTEX_EXIT    mutex_exit(&stream_mutex);
#endif

InMemoryStream::InMemoryStream(int queueSize) {
	pSendBuffer = new volatile char[queueSize];
	rp = pSendBuffer;
	wp = pSendBuffer;
	pEndSendBuffer = pSendBuffer+queueSize;
#if defined(ESP32)
  vPortCPUInitializeMutex(&stream_mutex);
#endif
}
int InMemoryStream::available() {
  //printf("StreamAvailable %d %d\r\n", wp, rp);
	STREAM_MUTEX_ENTER;
	int result = wp-rp;
	if (result<0) result += pEndSendBuffer-pSendBuffer;
	STREAM_MUTEX_EXIT;
  return result;
}

char InMemoryStream::read() {
  char result = 0;
	STREAM_MUTEX_ENTER;
  if (wp!=rp) {
    result = *rp;
    rp++;
    if (rp >= pEndSendBuffer) rp = pSendBuffer;
  }
	STREAM_MUTEX_EXIT;
  return result;  
}

//---------------------------------
bool InMemoryStream::write(const char *s)
//---------------------------------
// Attention this is called in the interrupt
{
		STREAM_MUTEX_ENTER;
		volatile char* oldWp = wp;
    while (*s) {
        *wp++ = *s++;
        if (wp >= pEndSendBuffer) wp = pSendBuffer;
        if (wp == rp) {		// buffer overflow
						wp = oldWp;
					  STREAM_MUTEX_EXIT;
						return false;
				}
		}
		STREAM_MUTEX_EXIT;
		return true;
}

//---------------------------------
bool InMemoryStream::write(char c)
//---------------------------------
// Attention this is called in the interrupt
{
		STREAM_MUTEX_ENTER;
    if (wp == rp) {		// buffer overflow
			return false;
		}
    *wp++ = c;
		if (wp >= pEndSendBuffer) wp = pSendBuffer;
   	STREAM_MUTEX_EXIT;
		return true;
}

#endif