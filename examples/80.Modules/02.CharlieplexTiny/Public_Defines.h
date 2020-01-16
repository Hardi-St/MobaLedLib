#ifndef _PUBLIC_DEFINES_H_
#define _PUBLIC_DEFINES_H_

//#pragma GCC diagnostic ignored "-Wunused-variable"


#include <Arduino.h>

#define USE_EEPROM_CFG      // Read the Configuration out of the EEPROM
                            // If this line is disabled the configuration is hard coded to the program
                            // like used in the "normal" MobaLedLib program running on the Nano.

// Debugging could be enabled indifidually for the ATTiny or the UNO
#if defined __AVR_ATtiny85__
  //#define DEBUG_TINY      // Enable this to debug the ATTiny with the serial port
                            // Attention the Mux LED 4 pin is used for the serial signal.
                            // => Only 6 LEDs can be controlled and some LEDs behave strange
                            // The Pin2 is connected to a TTL to USB interface which is connected
                            // to the PC (Baud rate 115200). Putty could be used as viewer.
                            //
                            //                                       ATTiny 85       Prog.
                            //                                        +-\/-+
                            //                            Reset PB5  1|    |8  VCC
                            // SERIAL OUTP    Mux LED 4         PB3  2|    |7  PB2    SCK      Mux LED 3
                            //                PWM Inp Green     PB4  3|    |6  PB1    MISO     Mux LED 2
                            //                                  GND  4|    |5  PB0    MOSI     Mux LED 1
                            //                                        +----+
  #ifdef DEBUG_TINY
    #define _PRINT_DEBUG_MESSAGES  // Enable this line to print Dprintf() debug messages to the PC (Port PB3)
                                   // If it's disabled only a few writeString() messages are shown
  #endif

#else // Uno or Nano
  #define _PRINT_DEBUG_MESSAGES // Enable this line to print Dprintf() debug messages to the PC (USB)
                                // If it's disabled only a few Serial.print() messages are shown
#endif




#if !defined __AVR_ATtiny85__
  //#define SHOW_HEARTBEAT  // Enable this for debugging
#endif

#ifdef SHOW_HEARTBEAT
   #define LAST_LED 11  // The last LED is used for the Heartbeat
#else
   #define LAST_LED 12
#endif


#define PWM_PIN      4   // The WS2811 PWM signal is read from this pin


//#define PROG_MODE_START_PWM01 970    // pwm01 must be > PROG_MODE_START_PWM01 to enter the programming mode

#define TIMEOUT_CNT           5000   // Number of PWM_in() calls without change which abort the programming mode
#define PWM0_MIN01            948    // First measured PWM signal (Resolution 0.1%) which is treated as data
#define PWM_MAX               249    // Maximal PWM signal which represents valid data
#define NEXT_DAT_BIT          0x10   // This bit toggles with every received data value to indicate a new value
#define PAYLOAD_MASK          0x0F   // Masks the Payload in the data.
#define BITS_PER_DATA         4      // Number of bits in one data package
#define MAX_DAT               32     // Maximal Data value incusive NEXT_DAT_BIT



extern uint32_t PeriodBuffer; // Defined in "02.CharlieplexTiny.ino"


#if 1 // Saves 18 byte
  #define OptMap(x, in_min, in_max, out_min, out_max) (((long)((x) - (in_min)) * ((out_max) - (out_min))) / ((in_max) - (in_min)) + (out_min))
#else
  #define OptMap map // Standard function defined in "C:\Program Files (x86)\Arduino\hardware\arduino\avr\cores\arduino\WMath.cpp"
#endif

#define CFG_LOAD_OK              0
#define CFG_LOAD_ERR_CRC        -1
#define CFG_LOAD_ERR_INITMACRO  -2


#endif // _PUBLIC_DEFINES_H_
