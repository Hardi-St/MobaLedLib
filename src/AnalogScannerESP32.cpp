/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2021 Jürgen Winkler: MobaLedLib@a1.net

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

// see also https://www.toptal.com/embedded/esp32-audio-sampling



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


#ifdef ESP32
#include <Arduino.h>
#include "AnalogScannerESP32.h"
#include <soc/sens_reg.h>
#include <soc/sens_struct.h>

//static members
int AnalogScannerESP32::values[sizeof(pinAssignment)];
long AnalogScannerESP32::intValues[sizeof(pinAssignment)];
uint8_t AnalogScannerESP32::requestedChannel[sizeof(pinAssignment)+1];
uint8_t AnalogScannerESP32::requestedPin[sizeof(pinAssignment)];
void (*AnalogScannerESP32::pCallback[sizeof(pinAssignment)])(int index, int pin, int value);
int16_t AnalogScannerESP32::sampleCount = 0;
TaskHandle_t AnalogScannerESP32::newValuesHandlerTask;
portMUX_TYPE DRAM_ATTR AnalogScannerESP32::timerMux = portMUX_INITIALIZER_UNLOCKED; 
hw_timer_t *AnalogScannerESP32::adcTimer = NULL; // our timer


// Creates a new instance of the analog input scanner. Initializes
// all scanned values to zero and all callbacks to null. 
AnalogScannerESP32::AnalogScannerESP32() {
  for (int i=0; i < sizeof(pinAssignment); ++i) {
    values[i] = 0;
		intValues[i] = 0;
    pCallback[i] = NULL;
  }
	requestedChannel[0] = 0xff;
	
	// use the full voltage range
	analogSetAttenuation(ADC_11db);
	
	// to avoid running (maybe long lasting) callbacks within the timer intterupt we create a task that handles the callbacks
  xTaskCreate(onADCValues, "handleADCValues", 1024, NULL, 1, &newValuesHandlerTask);
	//Serial.printf("Prescaler=%d\n", (80000000/(ADC_SAMPLES_COUNT*ADC_CALLBACKS_PER_SEC)));

	// configure the timer
  adcTimer = timerBegin(3, (80000000/(ADC_SAMPLES_COUNT*ADC_CALLBACKS_PER_SEC)), true); // 80 MHz base frequency
	// attach the timer interupt handler
  timerAttachInterrupt(adcTimer, &onTimer, true); // Attaches the handler function to the timer 
	// go!
  timerAlarmWrite(adcTimer, 1, true); // Interrupts when counter == 45, i.e. 22.222 times a second
  timerAlarmEnable(adcTimer);
}

// set the list of pins the AnalogScanner should request the values for
void AnalogScannerESP32::setScanPins(uint8_t count, const int pin[]) {
  uint8_t pins = sizeof(pinAssignment);
	if (count<pins) pins=count;
	uint8_t i=0;
  for (; i < pins; ++i) 
	{
		int8_t channel = digitalPinToAnalogChannel(pin[i]);
    if(channel < 0) break;
    requestedChannel[i] = channel;
		requestedPin[i] = pin[i];
		analogRead(pin[i]);					// make it an analog pin
  }
	requestedChannel[i] = 0xff;
	requestedPin[i] = 0xff;
}

// Gets the most recently read value for an analog pin.
int AnalogScannerESP32::getValue(int pin) {
	int8_t index = getScanPinIndex(pin);
	if (index < 0) return 0;
	
	//Serial.print("adc value for " );
	//Serial.print(pin);
	//Serial.print(" = ");
	//Serial.println(values[index]>>2);
	return values[index]>>2;
}



// internal functions

// high speed read of analog value
int IRAM_ATTR local_adc1_read(int channel) {
    uint16_t adc_value;
    SENS.sar_meas_start1.sar1_en_pad = (1 << channel); // only one channel is selected
    while (SENS.sar_slave_addr1.meas_status != 0);
    SENS.sar_meas_start1.meas1_start_sar = 0;
    SENS.sar_meas_start1.meas1_start_sar = 1;
    while (SENS.sar_meas_start1.meas1_done_sar == 0);
    adc_value = SENS.sar_meas_start1.meas1_data_sar;
    return adc_value;
}

// task handling callbacks asynchonously
void AnalogScannerESP32::onADCValues(void *param) {
  while (true) {

    // Sleep until the ISR gives us something to do, or for 1 second
    uint32_t tcount = ulTaskNotifyTake(pdFALSE, pdMS_TO_TICKS(1000));  
    if (tcount) {
			
			// iterate the callbacks
			for (int index=0;index<sizeof(pinAssignment);++index)
			{
				if (requestedChannel[index]==0xff) break;
				//Serial.printf("Value of pin %d = [%d] ", requestedPin[index], values[index]);
				if (pCallback[index]!= NULL)
				{
					//Serial.print("adc value for " );
					//Serial.print(requestedPin[index]);
					//Serial.print(" = ");
					//Serial.println(values[index] >> 2);
					pCallback[index](index, requestedPin[index], values[index] >> 2);		// make a 10 bit value out of the 12 bit adc value to beATMEga compatible
				}
				
			}
			//Serial.println();
    }
  }
}

// the timer interrupt handler
IRAM_ATTR void AnalogScannerESP32::onTimer() {
  portENTER_CRITICAL_ISR(&timerMux);

	uint8_t i = 0;
	uint8_t* pChannel = &requestedChannel[0];
	
	// get current adc value for each request pin and add it to a pins total counter
	while(*pChannel != 0xff)
	{
		intValues[i++] += local_adc1_read(*pChannel);
		pChannel++;
	}
	
	// when to expected sample count is reached
  if (++sampleCount == ADC_SAMPLES_COUNT) 
	{ 
		// calculated the average value of each pin and store it into the values array
		while(i!=0)
		{
			--i;
			//Serial.printf("value of index %d is %d\n", i, intValues[i]);
			values[i] = intValues[i]/ADC_SAMPLES_COUNT;
			intValues[i] = 0;
		}
		sampleCount = 0;
		
    // Notify newValuesHandlerTask that we have new values
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		vTaskNotifyGiveFromISR(newValuesHandlerTask, &xHigherPriorityTaskWoken);
    if (xHigherPriorityTaskWoken) {
      portYIELD_FROM_ISR();
    }
	}
  portEXIT_CRITICAL_ISR(&timerMux);
}

// Gets the index (0-15) corresponding to a pin number. If
// the pin number is already an index in the range 0-15, it is
// returned unchanged. Otherwise it is converted to a pin index.
//
int AnalogScannerESP32::getPinIndex(int pin) {
	for (uint8_t index = 0; index<sizeof(pinAssignment); index++)
	{
		if (pinAssignment[index]==pin) return index;
	}
	return -1;
}

// get the index for a pin inside the requestedPin array
int AnalogScannerESP32::getScanPinIndex(int pin) {
	for (uint8_t index = 0; index<sizeof(pinAssignment); index++)
	{
		if (requestedPin[index]==pin) return index;
	}
	return -1;
}

// Sets a callback function for a particular analog pin. The
// function is called by the main loop as soon
// as a new analog value is available.
void AnalogScannerESP32::setCallback(int pin, void (*p)(int index, int pin, int value)) {
	uint8_t index = getScanPinIndex(pin);
	if (index>=0) pCallback[index] = p;
	//Serial.printf("Callback for %d set to %d\n", pin);
}
#endif