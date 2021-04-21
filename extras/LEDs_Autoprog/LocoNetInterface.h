/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2020 Jürgen Winkler: MobaLedLib@a1.net

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

this class uses the LocoNet Personal Edition, please note the conditions below

https://www.digitrax.com/static/apps/cms/media/documents/loconet/loconetpersonaledition.pdf


LocoNet Personal Edition is the basic definition of the communication protocol used by Digitrax products that communicate via LocoNet.

LocoNet Personal Use Edition information is provided to Digitrax users for their personal use and experimentation under the following terms and conditions:
 

1.  The Personal Use Edition of the LocoNet Specification is publicly accessible from this website.  
		Click on the link in the Related Links section on the right.
2.  Personal Use Edition users may discuss the Specification, privately or publicly and in the various user forums,
		i.e. the Yahoo LocoNet Hackers group or the Yahoo Digitrax Users group. The information is for private use only 
		and my not be used for products that are intended for distribution to others, with or without profit.
3.  LocoNet Personal Edition is solely for non-commercial private use.
4.  Distribution of products that incorporate LocoNet Personal Edition is prohibited if ANY fee or remuneration is 
		charged for such products. You may share information included in LocoNet Personal Edition with other Digitrax 
		users as long as no money is changing hands.
5.  LocoNet is trade secret intellectual property of Digitrax, Inc.  No rights are conveyed for the commercial use 
		of this information.
6.  Digitrax Inc. does not provide technical support for use of LocoNet Personal Edition.
7.  Digitrax conveys no warranty for this information and incurs no obligations for its use or incorrect usage.
8.  LocoNet is a registered trademark of Digitrax, Inc. Possession and/or use of LocoNet Personal Use Edition 
		constitutes acceptance of these terms and conditions.

A "Non Disclosure Agreement" is not required for use of LocoNet Personal Edition.

Revision History :
~~~~~~~~~~~~~~~~~
18.10.20:  Versions 1.0 (Jürgen)

*/

#ifndef LOCONETXINTERFACE_H
#define LOCONETXINTERFACE_H

#ifdef ESP32
#define LOCONET_USE_SOCKET
#endif

#ifdef LOCONET_USE_SOCKET
#include "WiFi.h"
#endif

#include <LocoNetStream.h>
#include "InMemoryStream.h"

class LocoNetInterface 
{
	
private:
	static InMemoryStream* pStream;

	#ifdef LOCONET_USE_SOCKET
		WiFiServer locoNetServer;
public:		
		#define LOCONET_INTERFACE locoNetClient
	#else
		#define LOCONET_INTERFACE Serial
	#endif
private:
	void handlePacket(lnMsg *LnPacket);
public:
	void setup(InMemoryStream& stream);
	void loop();
	void send(byte out);
	static void addToSendBuffer(const char *s);
	
};

#endif