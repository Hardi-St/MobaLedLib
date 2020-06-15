#ifndef _GLOBAL_DEFINES_H_
#define _GLOBAL_DEFINES_H_

#include <Arduino.h>

// Defined in printf.h
#define printf(Format, ...) printf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
void printf_proc(const __FlashStringHelper *format, ...);

// Configure the baud rate:
#define BAUDRATE	19200


// Pin numbers valid for the Tiny_UniProg PCB
#define RESET_INV                         // NPN Transistor used to pull down the RESET Pin
#define PIN_RESET             5
#define RELAIS_RST_PIN        6
#define RELAIS_PRG_PIN        7
#define RELAIS_PWR_PIN        8
#define PIN_POWER             9           // Power supply pin for ATTiny
#define PIN_MOSI              11          // SPI Pins
#define PIN_MISO              12          //   "
#define PIN_SCK               13          //   "

#define HV_RESET_BUTTON_PIN   A0
//                            A1          // CS MCP2515 CAN
//                            A1          // Charliplexing LEDs
//                            A2          //     "
//                            A3          //     "
#define PIN_HV_AD_12V         A5          // Analog input to measure the 12V

// HV Reset (Same pins with different function)
#define PIN_HV_SCI            10          // Target Clock Input                   pinMode(OUTPUT)
#define PIN_HV_SDI            11          // Target Data Input                    pinMode(OUTPUT)
#define PIN_HV_SII            12          // Target Instruction Input             pinMode(OUTPUT)
#define PIN_HV_SDO            13          // Target Data Output                   pinMode(INPUT/OUTPUT)

// Define Direct I/O pins for Charge Pump Port D
#define PIN_MSKD_P1           0x04        // Pin D2
#define PIN_MSKD_P2           0x08        // Pin D3
#define PIN_MSKD_PWR          0x10        // Pin D4
#define PIN_MSKD_GND          0x20        // Pin D5
#define PIN_MSKD_REL         (0x40|0x80)  // Pin D6 | D7

typedef struct
    {
    uint16_t Signatur;
    uint8_t  lfuse;
    uint8_t  hfuse;
    uint8_t  efuse;
    } Chip_Data_T;

void end_pmode();

// Define ATTiny series signatures
#define ATTINY13   0x9007  // L: 0x6A, H: 0xFF             8 pin
#define ATTINY24   0x910B  // L: 0x62, H: 0xDF, E: 0xFF   14 pin
#define ATTINY25   0x9108  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
#define ATTINY44   0x9207  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
#define ATTINY45   0x9206  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
#define ATTINY84   0x930C  // L: 0x62, H: 0xDF, E: 0xFFF  14 pin
#define ATTINY85   0x930B  // L: 0x62, H: 0xDF, E: 0xFF    8 pin
#define ATTINY48   0x9203  //                             28 pin
#define ATTINY88   0x9311  //                             28 pin

#endif // _GLOBAL_DEFINES_H_
