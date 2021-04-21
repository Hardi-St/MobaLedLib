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

*/

#ifdef ESP32
#include "LocoNetInterface.h"

#define printf(Format, ...) printf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
extern void printf_proc(const __FlashStringHelper *format, ...);

#ifdef LOCONET_USE_SOCKET
WiFiClient locoNetClient;
#endif

LocoNetSystemVariableClass LocoNetSV;
LocoNetFastClockClass LocoFCClass;
InMemoryStream* LocoNetInterface::pStream = 0;

void LocoNetInterface::setup(InMemoryStream& stream) {
	LocoNetInterface::pStream = &stream;
#ifdef LOCONET_USE_SOCKET
	locoNetServer.begin(1001);
#endif
}

SV_STATUS SvStatus ;
bool powerIsOn = false;

void LocoNetInterface::loop()
{
#ifdef LOCONET_USE_SOCKET
	if (!locoNetClient) locoNetClient = locoNetServer.available();	
	if (locoNetClient) {
		if (!locoNetClient.connected())
		{
			locoNetClient.stop();
			printf("LocoNetClient disconnected\n");
			return;
		}
	}
#endif
	while (LOCONET_INTERFACE.available()>0) {
	  LocoNetStream.addToBuffer(LOCONET_INTERFACE.read());
	  
	  // Check for any received LocoNet packets
	  lnMsg* LnPacket = LocoNetStream.receive() ;
	  if ( LnPacket ) handlePacket(LnPacket);
	}
}

void LocoNetInterface::handlePacket(lnMsg *LnPacket)
{
	// First print out the packet in HEX

#ifdef DEBUG_PACKET
	printf("RX: ");
	uint8_t msgLen = getLnMsgSize(LnPacket);
	for (uint8_t x = 0; x < msgLen; x++)
	{
	  uint8_t val = LnPacket->data[x];
 
	  printf("%02X ", val);
	}
	printf("Command: %02X\n", LnPacket->sr.command);
#endif	
	// mirror the message
	LocoNetStream.send(LnPacket);
	if( LocoNetSV.processMessage( LnPacket ) == SV_DEFERRED_PROCESSING_NEEDED)
	  SvStatus = SV_DEFERRED_PROCESSING_NEEDED;

	if(SvStatus == SV_DEFERRED_PROCESSING_NEEDED)
	  SvStatus = LocoNetSV.doDeferredProcessing();

	LocoFCClass.processMessage( LnPacket );

	if (LnPacket->sr.command==OPC_RQ_SL_DATA)
	{
		lnMsg SendPacket;
		SendPacket.data[ 0 ] = OPC_SL_RD_DATA;
		SendPacket.data[ 1 ] = 0x0e;
		SendPacket.data[ 2 ] = 3;  
		SendPacket.data[ 3 ] = GTRK_POWER;
		SendPacket.data[ 4 ] = 0;
		SendPacket.data[ 5 ] = 0;
		SendPacket.data[ 6 ] = 0;
		SendPacket.data[ 7 ] = 0;
		SendPacket.data[ 8 ] = 0;
		SendPacket.data[ 9 ] = 0;
		SendPacket.data[ 10 ] = 0;
		SendPacket.data[ 11 ] = 0;
		SendPacket.data[ 12 ] = 0;
		SendPacket.data[ 13 ] = 0;
		LocoNetStream.send( &SendPacket );
	}

	// Process the packet in case its a OPC_GPON
	LocoNetStream.processSwitchSensorMessage(LnPacket);
}

void LocoNetInterface::send(byte out) 
{
	LOCONET_INTERFACE.write(out);
}

void notifyPower (uint8_t Power) {
	if (Power) printf("Power ON\n"); else printf("Power OFF\n");
  powerIsOn = Power !=0;
}

LN_STATUS sendLocoNetPacketTry(lnMsg *TxData, unsigned char ucPrioDelay)
{
  uint8_t  CheckSum ;

  int lnTxLength = getLnMsgSize( TxData ) ;
  int lnTxIndex;

  // First calculate the checksum as it may not have been done
  uint8_t CheckLength = lnTxLength - 1 ;
  CheckSum = 0xFF ;

  for( lnTxIndex = 0; lnTxIndex < CheckLength; lnTxIndex++ ) {
    CheckSum ^= TxData->data[ lnTxIndex ] ;
  }

  TxData->data[ CheckLength ] = CheckSum ; 

  for (lnTxIndex=0;lnTxIndex<lnTxLength;lnTxIndex++)
  {
    uint8_t val = TxData->data[lnTxIndex];
		LOCONET_INTERFACE.write(val);
  
  }
  return LN_DONE;
}

//---------------------------------
void LocoNetInterface::addToSendBuffer(const char *s)
//---------------------------------
// Attention this is called in the interrupt
{
	pStream->write(s);
}

// This call-back function is called from LocoNetStream.processSwitchSensorMessage
// for all Sensor messages
void notifySensor( uint16_t Address, uint8_t State ) {
  //printf("Sensor: ");
  //Serial.print(Address, DEC);
  //Serial.print(" - ");
  //Serial.println( State ? "Active" : "Inactive" );
}

// This call-back function is called from LocoNetStream.processSwitchSensorMessage
// for all Switch Request messages
void notifySwitchRequest( uint16_t Address, uint8_t Output, uint8_t Direction ) {
  printf("Switch Request: %d:%d - %d\n", Address, Direction, Output);
	char s[20];
  sprintf(s, "@%4i %02X %02X\n", Address, Direction, Output);
	LocoNetInterface::addToSendBuffer(s);
}

// This call-back function is called from LocoNetStream.processSwitchSensorMessage
// for all Switch Output Report messages
void notifySwitchOutputsReport( uint16_t Address, uint8_t ClosedOutput, uint8_t ThrownOutput) {
  //printf("Switch Outputs Report: ");
  // Serial.print(Address, DEC);
  // Serial.print(": Closed - ");
  // Serial.print(ClosedOutput ? "On" : "Off");
  // Serial.print(": Thrown - ");
  // Serial.println(ThrownOutput ? "On" : "Off");
}

// This call-back function is called from LocoNetStream.processSwitchSensorMessage
// for all Switch Sensor Report messages
void notifySwitchReport( uint16_t Address, uint8_t State, uint8_t Sensor ) {
  //printf("Switch Sensor Report: ");
  // Serial.print(Address, DEC);
  // Serial.print(':');
  // Serial.print(Sensor ? "Switch" : "Aux");
  // Serial.print(" - ");
  // Serial.println( State ? "Active" : "Inactive" );
}

// This call-back function is called from LocoNetStream.processSwitchSensorMessage
// for all Switch State messages
void notifySwitchState( uint16_t Address, uint8_t Output, uint8_t Direction ) {
  //printf("Switch State: ");
  // Serial.print(Address, DEC);
  // Serial.print(':');
  // Serial.print(Direction ? "Closed" : "Thrown");
  // Serial.print(" - ");
  // Serial.println(Output ? "On" : "Off");
}

#endif