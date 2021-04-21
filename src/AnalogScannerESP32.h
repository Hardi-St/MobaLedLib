#ifndef ANALOGSCANNERESP32_H
#define ANALOGSCANNERESP32_H

// AnalogScannerESP32.h - Header file for the AnalogScanner library
//   for Arduino.
// 
// Copyright 2021 Jürgen Winkler, mobaledlib.gmx.at
// 
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
// 
//     http://www.apache.org/licenses/LICENSE-2.0
// 
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

// Implements an object that can read the AVR analog inputs without
// blocking. This class allows the caller to specify a scan order
// for the analog inputs, then begin scanning. At that point the
// scanner object will iterate over each specified analog input
// pin, starting an analog read and processing the analog value
// in an ADC interrupt handler. The interrupt handling code then
// initiates the read of the next analog pin. In this way there
// is no blocking during the analog reads.
//
// See the accompanying documentation for API details.

#include <stdint.h>

#define ADC_SAMPLES_COUNT 100
#define ADC_CALLBACKS_PER_SEC 25

class AnalogScannerESP32 {

 private:
  // The maximum number of analog input pins. Some Atmel
  // chips support fewer inputs than this.
	uint8_t pinAssignment[16] = { 0, 2, 4, 12, 13, 14, 15, 25, 26, 27, 32, 33, 34, 35, 36, 39 };
	
  // The analog input values.
  static int values[sizeof(pinAssignment)];
  static long intValues[sizeof(pinAssignment)];
  static int16_t sampleCount;
	static TaskHandle_t newValuesHandlerTask;
	static portMUX_TYPE DRAM_ATTR timerMux;
  static hw_timer_t * adcTimer;
	
  // An array of pointers to callback routines invoked when
  // new values are available.
  static void (*pCallback[sizeof(pinAssignment)])(int index, int pin, int value);
	
  // The requested channels - cache values to speed up interrupt handling 
  static uint8_t requestedChannel[sizeof(pinAssignment)+1];
	
	// The list of pins to get the analog values for
	static uint8_t requestedPin[sizeof(pinAssignment)];

	// internal functions
	
	// the timer interrupt handler
	static void onTimer();
	
	// the task handling the callbacks
	static void onADCValues(void *param);
	
  // Gets the pin index for a pin number.
  int getPinIndex(int pin);
	
  // Gets the scan pin index for a pin number.
	int getScanPinIndex(int pin);

 public:
  // Creates a new instance of the analog input scanner.
  AnalogScannerESP32();
  
  // Sets a callback function for an analog pin. The callback
  // function will be invoked as soon as a new ADC value is
  // available.
  void setCallback(int pin, void (*p)(int index, int pin, int value));
  
  // Sets scanned pins. A single pin may be specified once
  void setScanPins(uint8_t count, const int pin[]);
	
  // Gets the most recently read value for an analog pin.
  int getValue(int pin);
};

#endif // ANALOGSCANNERESP32_H
