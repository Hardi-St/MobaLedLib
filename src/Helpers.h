#ifndef HELPERS_H
#define HELPERS_H

#include <stdarg.h>
#ifdef ARDUINO_RASPBERRY_PI_PICO
  #include <WString.h>
#else
  #if defined(ARDUINO) && ARDUINO >= 100
    #include <Arduino.h>
  #endif
#endif

//#define _DEBUG_INTERFACE

// the if_printf makro output debug messages depending on the _DEBUG_INTERFACE define
#ifdef _DEBUG_INTERFACE	
#define if_printf(Format, ...) printf_serial(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#else
#define if_printf(Format, ...) 
#endif

//-------------------------------------------------------
void printf_serial(const __FlashStringHelper *format, ...);
//-------------------------------------------------------

#endif //HELPERS_H