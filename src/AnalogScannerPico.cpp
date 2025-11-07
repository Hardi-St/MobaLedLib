/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2025  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2025 Jürgen Winkler: MobaLedLib@a1.net

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
*/
	// the ATMega AnalogScanner delivers up to 10.000 samples per second
	// as the LDR callback function expects a new analog value only every 20ms 
	// we can slow down the callback interval
	// the Init_DarknessSensor function has a kind of divider to address 
	// different callback frequencies. The number of skipped samples it 
	// calculated with 500/divider.
	// 
	// with ATMega and one analog pin the divider is set to 1
	// so a analog sample is only taken every 500/1 = 500 cycles
	// which gives a internal update time of 10.000/500 = 20ms
	//
	// to address same behaviour with ESP we will call the callback 40 time per seconds
	// so we must init the Init_DarknessSensor divider with 250
	// the divider is then 500/250 = 2
	// which gives a internal update time of 40/2 = 20ms


#ifdef ARDUINO_RASPBERRY_PI_PICO
#include <Arduino.h>
#include "AnalogScannerPico.h"

//static members
int AnalogScannerPico::values[sizeof(pinAssignment)];
long AnalogScannerPico::intValues[sizeof(pinAssignment)];
uint8_t AnalogScannerPico::requestedPin[sizeof(pinAssignment)+1];
void (*AnalogScannerPico::pCallback[sizeof(pinAssignment)])(int index, int pin, int value);
int16_t AnalogScannerPico::sampleCount = 0;

// Creates a new instance of the analog input scanner. Initializes
// all scanned values to zero and all callbacks to null. 
AnalogScannerPico::AnalogScannerPico() {
  for (int i=0; i < sizeof(pinAssignment); ++i) {
    values[i] = 0;
		intValues[i] = 0;
    pCallback[i] = NULL;
  }
  requestedPin[0] = 0xff;
  pTimer = NULL;
	
}

// set the list of pins the AnalogScanner should request the values for
void AnalogScannerPico::setScanPins(uint8_t count, const int pin[]) {
  uint8_t pins = sizeof(pinAssignment);
	if (count<pins) pins=count;
	uint8_t i=0;
  for (; i < pins; ++i) 
	{
    if (getPinIndex(pin[i])>=0)
    {
		  requestedPin[i] = pin[i];
    }
  }
	requestedPin[i] = 0xff;
  if (i>0)
  {
    if (pTimer==NULL)
    {
      pTimer = new repeating_timer();
      add_repeating_timer_us(1000000/(ADC_SAMPLES_COUNT*ADC_CALLBACKS_PER_SEC), AnalogScannerPico::timer_callback, NULL, pTimer);
    }
  }
  else
  {
    cancel_repeating_timer(pTimer);
    pTimer = NULL;
  }
}

// Gets the most recently read value for an analog pin.
int AnalogScannerPico::getValue(int pin) {
	int8_t index = getScanPinIndex(pin);
	if (index < 0) return 0;
	
	/*Serial.print("adc value for " );
	Serial.print(pin);
	Serial.print(" = ");
	Serial.println(values[index]);*/
	return values[index];
}

// the timer interrupt handler
bool AnalogScannerPico::timer_callback(struct repeating_timer *t)
{
	uint8_t i = 0;
	uint8_t* pPin = &requestedPin[0];
	
	// get current adc value for each request pin and add it to a pins total counter
	while(*pPin != 0xff)
	{
		intValues[i] += analogRead(*pPin);
    //Serial.printf(" analog value for pin %d is %d\n", *pPin, intValues[i]);
    i++;
		pPin++;
	}
 
	// when to expected sample count is reached
  if (++sampleCount == ADC_SAMPLES_COUNT) 
	{ 
		// calculated the average value of each pin and store it into the values array
		while(i!=0)
		{
			--i;
			values[i] = intValues[i]/ADC_SAMPLES_COUNT;
			//Serial.printf("value of index %d is %d\n", i, values[i]);
			intValues[i] = 0;
		}
		sampleCount = 0;
		
    if (pCallback[i]!= NULL)
    {
        pCallback[i](i, requestedPin[i], values[i]);
    }
	}
  return true;
}

// Gets the index (0-15) corresponding to a pin number. If
// the pin number is already an index in the range 0-15, it is
// returned unchanged. Otherwise it is converted to a pin index.
//
int AnalogScannerPico::getPinIndex(int pin) {
	for (uint8_t index = 0; index<sizeof(pinAssignment); index++)
	{
		if (pinAssignment[index]==pin) return index;
	}
	return -1;
}

// get the index for a pin inside the requestedPin array
int AnalogScannerPico::getScanPinIndex(int pin) {
	for (uint8_t index = 0; index<sizeof(pinAssignment); index++)
	{
		if (requestedPin[index]==pin) return index;
	}
	return -1;
}

// Sets a callback function for a particular analog pin. The
// function is called by the main loop as soon
// as a new analog value is available.
void AnalogScannerPico::setCallback(int pin, void (*p)(int index, int pin, int value)) {
	uint8_t index = getScanPinIndex(pin);
	if (index>=0) pCallback[index] = p;
	//Serial.printf("Callback for %d set to %d\n", pin);
}
#endif