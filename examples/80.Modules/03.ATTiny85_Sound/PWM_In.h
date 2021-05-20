/*
 Module to read PWM signals with a high resolution and acurency.

*/


#include "Arduino.h"

uint16_t PWM_in(uint8_t pin, uint8_t state, unsigned int timeout, uint32_t &PeriodBuffer);
//
// Returns the PWM value with a resolution of 0.1%  (0..1000)
//
// - State may be HIGH to detect the high PWM pulse or LOW to read a low active PWM.
// - timeout defines the maximal time to detect the signal in us.
// - PeriodBuffer is used to generate a filtered period of the signal. This is importand if the
//   PWM signal is changed just in the moment when signal is read. It also stabilizes the result.
//   In addition the period has to be stored because it can't allways be read if the timeout is
//   to short.

