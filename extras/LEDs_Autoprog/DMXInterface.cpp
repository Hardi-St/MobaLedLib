/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2020 Jürgen Winkler: MobaLedLib@gmx.at
 DMX send code is published here https://code.google.com/archive/p/tinkerit/
 Copyright (c) 2008-2009 Peter Knight, Tinker.it!, License: GNU Lesser GPL

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

 DMX Signal generator for AVR and ESP32       18.10.2020 Jürgen
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Documents:
 ~~~~~~~~~~
 - https://github.com/PaulStoffregen/DmxSimple
 - https://de.wikipedia.org/wiki/DMX_(Lichttechnik)
 - https://www.fjksufa.com/index.php?main_page=product_info&products_id=239341
 - https://embetronicx.com/tutorials/wireless/esp32/idf/esp32-idf-serial-communication-tutorial/

Revision History :
~~~~~~~~~~~~~~~~~
12.12.20:  Versions 1.0 (Jürgen)
*/

#if  defined(__AVR__) ||  defined(ESP32)
#include "DMXInterface.h"
#include "pins_arduino.h"

#if defined(__AVR__)
#include <avr/io.h>
#include <util/delay.h>
#endif

void DMXInterface::setup(uint8_t DMXSignalPin, uint8_t* buffer, uint16_t numberOfChannels)
{
  this->buffer = buffer;
  this->numberOfChannels = numberOfChannels;
  if (this->numberOfChannels>512) this->numberOfChannels = 512;
#ifdef __AVR__
  this->dmxPin=DMXSignalPin;
  this->dmxPort = portOutputRegister(digitalPinToPort(dmxPin));
  this->dmxBit = digitalPinToBitMask(dmxPin);
  // Set DMX pin to output
  pinMode(dmxPin,OUTPUT);
#endif
#ifdef ESP32
  uart_config_t uart_config;
  uart_config.baud_rate = 250000;
  uart_config.data_bits = UART_DATA_8_BITS,
  uart_config.stop_bits = UART_STOP_BITS_2,
  uart_config.flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
  uart_config.rx_flow_ctrl_thresh = 122,
	uart_param_config(URAT_USED, &uart_config);

  // The RX queue size must be greater than UART_FIFO_LEN
  // The TX queue size must be UART_FIFO_LEN + expected data to send
  //uart_driver_install(URAT_USED, UART_FIFO_LEN+1, UART_FIFO_LEN+numberOfChannels, 0, NULL, 0);
  uart_driver_install(URAT_USED, UART_FIFO_LEN+1, UART_FIFO_LEN+numberOfChannels, 0, NULL, 0);

  Serial1.begin(250000, SERIAL_8N2,9,DMXSignalPin);
  //uart_set_pin(URAT_USED, 19, 18, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
#endif
}

void DMXInterface::loop()
{
	// send out channels only every 20ms -> gives a maximum frame rate of 50, thats pretty good
	// and only dogs can see faster framerates - or were it cats? Or camelions?
	//
	//Sending more than ~400 channels will take longer than 20ms !!!

	if (millis()-lastSend>=20)
       {
       lastSend = millis();

       #define DEBUG_DMX_TIMING 0
       #if DEBUG_DMX_TIMING // Debug
         unsigned long startMicros = micros();
       #endif

       sendSynchron(buffer,numberOfChannels);

       #if DEBUG_DMX_TIMING // Debug
         unsigned long elapsed = micros()-startMicros;
         Serial.print(numberOfChannels);
         Serial.print(" channels sent in ");
         Serial.print(elapsed);
         Serial.println(" us");
       #endif
       }
}

#ifdef ESP32

// sendSynchron send out a buffer in DMX 512 format
//
void DMXInterface::sendSynchron(uint8_t* buffer, uint16_t len)
{
	// Temporarily change baudrate to lower and send a 0 byte
	// DMX needs a 88us low (break) follow by 12us high
	// we set the protocol to 8 bit even parity 1 stop -> together with start bit it gives a total of 10 low bits when sending the '0' byte
	// so one bit must be (88 / 10) us long

	uart_set_baudrate(URAT_USED, ((1000000*10)/88));
	uart_set_parity(URAT_USED, UART_PARITY_EVEN);
    uart_write_bytes(URAT_USED, &nullChar, 1);

	delayMicroseconds(88+10);		// 88us break + some time to be sure that byte is fully sent

	// now we need 12us mark, don't delay here, because the baud rate switch takes some time anyway

	unsigned long startBreak = micros();

	// switching the baudrate costs ~ 17us
	uart_set_baudrate(URAT_USED, 250000);
	uart_set_parity(URAT_USED, UART_PARITY_DISABLE);

  uart_write_bytes(URAT_USED, &nullChar, 1);
  uart_write_bytes(URAT_USED, (const char*)buffer, numberOfChannels);
}
#elif defined(__AVR__)
void DMXInterface::sendSynchron(uint8_t* buffer, uint16_t len)
{
	*dmxPort &= ~dmxBit;
	//for (i=0; i<11; i++) delayMicroseconds(8);
	delayMicroseconds(8*11+1);		// one micro extra, otherwise I measure only a pulse of 87,5us
	*dmxPort |= dmxBit;
	delayMicroseconds(12);
	dmxSendByte(0);
	for (uint16_t i=0;i<len;i++)
	{
		dmxSendByte(*(buffer+i));
	}
}
#endif

/** Transmit a complete DMX byte
 * We have no serial port for DMX, so everything is timed using an exact
 * number of instruction cycles.
 *
 * Really suggest you don't touch this function.
 * DMX send code is published here https://code.google.com/archive/p/tinkerit/
 * Copyright (c) 2008-2009 Peter Knight, Tinker.it!, License: GNU Lesser GPL
 */
#if defined(__AVR__)
void DMXInterface::dmxSendByte(volatile uint8_t value)
{
  uint8_t bitCount, delCount;
  __asm__ volatile (
    "cli\n"
    "ld __tmp_reg__,%a[dmxPort]\n"
    "and __tmp_reg__,%[outMask]\n"
    "st %a[dmxPort],__tmp_reg__\n"
    "ldi %[bitCount],11\n" // 11 bit intervals per transmitted byte
    "rjmp bitLoop%=\n"     // Delay 2 clock cycles.
    "bitLoop%=:\n"\
    "ldi %[delCount],%[delCountVal]\n"
    "delLoop%=:\n"
    "nop\n"
    "dec %[delCount]\n"
    "brne delLoop%=\n"
    "ld __tmp_reg__,%a[dmxPort]\n"
    "and __tmp_reg__,%[outMask]\n"
    "sec\n"
    "ror %[value]\n"
    "brcc sendzero%=\n"
    "or __tmp_reg__,%[outBit]\n"
    "sendzero%=:\n"
    "st %a[dmxPort],__tmp_reg__\n"
    "dec %[bitCount]\n"
    "brne bitLoop%=\n"
    "sei\n"
    :
      [bitCount] "=&d" (bitCount),
      [delCount] "=&d" (delCount)
    :
      [dmxPort] "e" (dmxPort),
      [outMask] "r" (~dmxBit),
      [outBit] "r" (dmxBit),
      [delCountVal] "M" (F_CPU/1000000-3),
      [value] "r" (value)
  );
}
#endif
#endif // defined(__AVR__) ||  defined(ESP32)
