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
*/

#ifdef ESP32
#include "InMemoryStream.h"

static volatile char    *pSendBuffer;
static volatile char    *rp;        // Queue Read Pointer
static volatile char    *wp;        // Queue Write Pointer
static volatile char    *pEndSendBuffer;
static portMUX_TYPE			my_mutex;

InMemoryStream::InMemoryStream(int queueSize) {
	pSendBuffer = new volatile char[queueSize];
	rp = pSendBuffer;
	wp = pSendBuffer;
	pEndSendBuffer = pSendBuffer+queueSize;
  vPortCPUInitializeMutex(&my_mutex);
}
	
int InMemoryStream::available() {
  //printf("StreamAvailable %d %d\r\n", wp, rp);
  portENTER_CRITICAL(&my_mutex);
	int result = wp-rp;
	if (result<0) result += pEndSendBuffer-pSendBuffer;
  portEXIT_CRITICAL(&my_mutex);
  return result;
}

char InMemoryStream::read() {
  char result = 0;
  portENTER_CRITICAL(&my_mutex);
  if (wp!=rp) {
    result = *rp;
    rp++;
    if (rp >= pEndSendBuffer) rp = pSendBuffer;
  }
  portEXIT_CRITICAL(&my_mutex);
  return result;  
}

//---------------------------------
bool InMemoryStream::write(const char *s)
//---------------------------------
// Attention this is called in the interrupt
{
    portENTER_CRITICAL(&my_mutex);
		volatile char* oldWp = wp;
    while (*s) {
        *wp++ = *s++;
        if (wp >= pEndSendBuffer) wp = pSendBuffer;
        if (wp == rp) {		// buffer overflow
						wp = oldWp;
						return false;
				}
   }
   portEXIT_CRITICAL(&my_mutex);
	 return true;
}

//---------------------------------
bool InMemoryStream::write(char c)
//---------------------------------
// Attention this is called in the interrupt
{
    portENTER_CRITICAL(&my_mutex);
    if (wp == rp) {		// buffer overflow
			return false;
		}
    *wp++ = c;
		if (wp >= pEndSendBuffer) wp = pSendBuffer;
    portEXIT_CRITICAL(&my_mutex);
		return true;
}


#endif