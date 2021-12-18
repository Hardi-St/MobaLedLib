#ifndef HELPERS_H
#define HELPERS_H

#include <stdarg.h>
#ifndef ARDUINO_RASPBERRY_PI_PICO
  #include <WString.h>
#else
  #if defined(ARDUINO) && ARDUINO >= 100
    #include "Arduino.h"
  #endif
#endif

#define printf(Format, ...) printf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
void printf_proc(const __FlashStringHelper *format, ...);

#endif //HELPERS_H