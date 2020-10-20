/*
 Module to read PWM signals with a high resolution and acurency.

*/


#include "Arduino.h"
#include "pins_arduino.h"


// Attention: The C-Function doesn't work correctly with GCC 7.3.0                                            // 12.10.19:
//            It works fine with GCC 4.9.2 (Installed on the tablet) and GCC 5.4.0

// 12.10.19:  USE_PWM_IN_ASM war vorher ausgeschaltet
#define USE_PWM_IN_ASM // Disable this flag to use the the C-function for tests.
                       // In the final version the ASM Module should be used to be
                       // independend for future compiler optimizations.
                       //
                       //
                       // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!
                       // !! Attention if this switch is disabled LTO (Link-Time-Optimization) in !!
                       // !! the Arduino IDE must be disabled. Otherwise the results are wrong.   !!
                       // !!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!

#ifdef USE_PWM_IN_ASM

  extern "C" unsigned int PWM_in_Raw(volatile uint8_t *port, uint8_t bit, uint8_t stateMask, unsigned int maxloops, unsigned int *Period);

#else // Test with a C-Function

// Compile the following function with
//   cd "C:\Program Files (x86)\Arduino\hardware\tools\avr\bin"
//   avr-gcc.exe -gstabs -Wa,-ahlmsd="C:\Dat\Maerklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ATTiny85_Servo\PWM_In_Asm\PWM_in_Asm.lst" -dp -fverbose-asm -O "C:\Dat\Maerklin\Arduino\LEDs_Eisenbahn\Prog_ATTiny85\ATTiny85_Servo\PWM_In_Asm\PWM_in_Asm.c"
//
// and manually convert the generated PWM_in_Asm.lst to PWM_in_Asm.S
//
// If the optimization -O2 is used the program is 8-10% faster. But unfortunately only some
// of the while() loops are faster ;-(
// Thereby the period is different if the PWM signal changes ;-((
//
//  PWM  Period  with optimization -O2
//  25%  2078
//  96%  1930
//
// Therefore the optimization -O is used.
//
// Maybe this could be improved by modifying the ASM code manually.
//
// GCC: (GNU) 5.4.0

//--------------------------------------------------------------------------------------------------------------------------
unsigned int PWM_in_Raw(volatile uint8_t *port, uint8_t bit, uint8_t stateMask, unsigned int maxloops, unsigned int *Period)
//--------------------------------------------------------------------------------------------------------------------------
// Try to measure also the opposite pulse to be able to calculate the PWM. If the timeout is to short the
// opposite pulse may not measured. It depends from the time the function is called if the opposite pulse
// is measured.
// If the opposite pulse can't be measured Period is 0.
// => The period has to be stored outside to use prior values in this case.
//
// The "pictures" below show the situation when stateMask is xxx1xxx which is used to detect HIGH pulses.
// If LOW pulses are detected stateMask is set to xxx0xxx. In this case the pictures hafe do be flipped.
{
    unsigned int Start_maxloop = maxloops;
    unsigned int End_Prev = 0;
    unsigned int Start_Pulse;
    unsigned int End_Pulse;
    *Period = 0;

    // wait for any previous pulse to end    // ====.
    while ((*port & bit) == stateMask)       //     #
        if (--maxloops == 0)                 //     '=====
            return 0;

    End_Prev = maxloops;                     //             If End_Prev < Start_maxloop => we have detected the falling edge

    // wait for the pulse to start           // ....          .=====
    while ((*port & bit) != stateMask)       //     :         #
        if (--maxloops == 0)                 //     ......===='
            return 0;

    Start_Pulse = maxloops;
                                             //             End_Prev - maxloops = duration before the edge
    // wait for the pulse to stop            // .....         ......====.
    while ((*port & bit) == stateMask)       //     :         :         #
        if (--maxloops == 0)                 //     '.........'         '=====
            return 0;

    if (End_Prev < Start_maxloop)
         { // we have detected the falling edge at the beginning
         *Period = End_Prev - maxloops;
         return Start_Pulse - maxloops;
         }

    // we didn't detect the falling edge at the beginning
    End_Pulse = maxloops;
    // wait for the next pulse to start      // .....         ...........         .=====
    while ((*port & bit) != stateMask)       //     :         :         :         #
        if (--maxloops == 0)                 //     '.........'         '.....===='
            return Start_Pulse - End_Pulse; // Timeout before detecting the next pulse, but the first pulse was received

    // We have detected the next pulse
    *Period = Start_Pulse - maxloops;
    return Start_Pulse - End_Pulse;
}
#endif // USE_PWM_IN_ASM


//-------------------------------------------------------------------------------------------
uint16_t PWM_in(uint8_t pin, uint8_t state, unsigned int timeout, uint32_t &PeriodBuffer)
//-------------------------------------------------------------------------------------------
// Returns the PWM value with a resolution of 0.1%  (0..1000)
//
// - State may be HIGH to detect the high PWM pulse or LOW to read a low active PWM.
// - timeout defines the maximal time to detect the signal in us.
//   It should be at least 2*1000000/MinFreq = 5000 for the WS2811 PWM of 400Hz.
// - PeriodBuffer is used to generate a filtered period of the signal. This is importand if the
//   PWM signal is changed just in the moment when signal is read. It also stabilizes the result.
//   In addition the period has to be stored because it can't allways be read if the timeout is
//   to short.
//
// Short pulses below 3us are not allways detected! => Don't use WS2811 PWM values < 2
// Short low low pulses may also generate results > 1000 in case the filtered period is smaller then
// the actual period+width
{
  uint8_t bit       = digitalPinToBitMask(pin);
  uint8_t port      = digitalPinToPort(pin);
  uint8_t stateMask = (state ? bit : 0);
  unsigned int Period;

  // convert the timeout from microseconds to a number of times through
  unsigned long maxloops = microsecondsToClockCycles(timeout)/11;                                             // clockCyclesToMicroseconds Teilt den Wert durch 8 @ 8MHz

  unsigned long width = PWM_in_Raw(portInputRegister(port), bit, stateMask, maxloops, &Period);

  if (width)
     {
    // width += 2; // Compensate the time to save
     uint32_t FilterConst = 200;
     if (Period)
        {
        if (PeriodBuffer)
             PeriodBuffer = (PeriodBuffer * FilterConst + Period * (256-FilterConst) + 256/2)>>8;
        else PeriodBuffer = Period;
        }
     if (PeriodBuffer)
        return 1 + ((width*1000L)+(PeriodBuffer>>2)) / PeriodBuffer;

     }
  return 0;
}

