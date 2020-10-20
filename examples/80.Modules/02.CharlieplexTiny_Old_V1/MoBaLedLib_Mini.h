#ifndef __MOBALEDLIB_MINI_H__
#define __MOBALEDLIB_MINI_H__
/*
 Special file used if the AnalogPattern module is used without the MobaLedLib


*/


#include <Arduino.h>

#define FASTLED_INTERNAL // Disable version number message in FastLED library (looks like an error)
#include "FastLED.h"     // The FastLED library must be installed in addition if you got the error message "..fatal error: FastLED.h: No such file or directory"
                         // Arduino IDE: Sketch / Include library / Manage libraries                    Deutsche IDE: Sketch / Bibliothek einbinden / Bibliothek verwalten
                         //              Type "FastLED" in the "Filter your search..." field                          "FastLED" in das "Grenzen Sie ihre Suche ein" Feld eingeben
                         //              Select the entry and click "Install"                                         Gefundenen Eintrag auswaehlen und "Install" anklicken

#include "Public_Defines.h"

#include "Dprintf.h"
#include "Macros.h"

#define _USE_PATTERN   1
#define USE_XFADE
//#define BITS_PER_CAHNNEL8         // Only suport 8 Bits per Channel   (Saves 262 byte if 8 Bits are used in the Macro otherwise it saves only 100 byte)

// Constants for AnalogPattern
#define SHIFT_FF 16
#define MIN_M    -2147483647
#define MAX_M     2147483647


#define P_INPCHANEL_W_CM       2  // If the function has a channel mask

#define END_T                  0


#define P_PATERN_LED           0
#define P_PATERN_NSTRU         1
#define P_PATERN_INCH          P_INPCHANEL_W_CM   // 2
#define P_PATERN_ENABLE        3
#define P_PATERN_LEDS          4 //  -+
#define P_PATERN_VAL0          5 //   +  Attention: LEDs, Val0, Val1, Off are read at once.
#define P_PATERN_VAL1          6 //   +  => Don't change the sequence of the parameters
#define P_PATERN_OFF           7 //  -+
#define P_PATERN_MODE          8
#define P_PATERN_T1_L          9
#define P_PATERN_T1_H          10

// Constants for the Button period and the times in the pattern function
#define Min         *60000L    // min in lowercase is not possible because of the standard min macro
#define Sec         *1000L
#define Sek         Sec
#define sek         Sec
#define sec         Sec
#define Ms
#define ms

// The options marked with an '$' below may be disabled to save 896 bytes of FLASH (ATTiny85)
// With all options:  (13.01.20)
//   Der Sketch verwendet 6786 Bytes (82%) des Programmspeicherplatzes. Das Maximum sind 8192 Bytes.
//   Globale Variablen verwenden 368 Bytes (71%) des dynamischen Speichers, 144 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 512 Bytes.
//
// Inserted options disabled:
//   Der Sketch verwendet 5890 Bytes (71%) des Programmspeicherplatzes. Das Maximum sind 8192 Bytes.
//   Globale Variablen verwenden 366 Bytes (71%) des dynamischen Speichers, 146 Bytes fuer lokale Variablen verbleiben. Das Maximum sind 512 Bytes.


// Some modes / functions are disabled
// to save FLASH                        // FLASH
#define PM_NORMAL               0       //    - Normal mode (Als Platzhalter in Excel)
#define PM_SEQUENZ_W_RESTART    1       //$  16 Rising edge-triggered unique sequence. A new edge starts with state 0.
#define PM_SEQUENZ_W_ABORT      2       //$  48 Rising edge-triggered unique sequence. Abort the sequence if new edge is detected during run time.
#define PM_SEQUENZ_NO_RESTART   3       //    - Rising edge-triggered unique sequence. No restart if new edge is detected
#define PM_SEQUENZ_STOP         4       //$  26 Rising edge-triggered unique sequence. A new edge starts with state 0. If input is turned off the sequence is stopped immediately.
#define PM_PINGPONG             5       //$ 194 Change the direction at the end: 236 bytes
#define PM_HSV                  6       //$ 672 Use HSV values instead of RGB for the channels
#define PM_RES                  7       //    - Reserved mode
#define PM_MODE_MASK            0x07    //    - Defines the number of bits used for the modes (currently 3 => Modes 0..7)

// Flags for the Pattern function
#define _PF_XFADE               0x08    //    - Special fade mode which starts from the actual brightness value instead of the value of the previous state
#define PF_NO_SWITCH_OFF        0x10    //  -10 Don't switch of the LEDs if the input is turned off. Useful if several effects use the same LEDs alternated by the input switch.
#define PF_EASEINOUT            0x20    //$  34 Easing function (Uebergangsfunktion) is used because changes near 0 and 255 are noticed different than in the middle
#define PF_SLOW                 0x40    //    - Slow timer (divided by 16) to be able to use longer durations  (11.12.19:  uses 28 bytes more memory if disabled ?)
#define PF_INVERT_INP           0x80    //$  24 Invert the input switch => Effect is active if the input is 0

#define PT_INACTIVE 255

#define _USE_LOCAL_TX                   // The variable t4w is calculated in each function instead of calculating them once in Int_Update()

// Spezial Inputs
#define SI_LocalVar        SI_0 // Input fuer Pattern Funktion zum einlesen der Localen Variable. Diese Nummer wird verwendet weil eine Konstante 0 als Eingang keinen Sinn macht
#define SI_0              (_MAX_INP_CHANNEL -1)  // Immer 0
#define SI_1              (_MAX_INP_CHANNEL   )  // Immer 1


#ifdef _USE_LOCAL_TX
  #define CALCULATE_t4w  uint16_t t4w = (t>>4)  & 0xFFFF; // Time divided by 16
#else
  #define CALCULATE_t4w
#endif

// Bit masks for Special SpInp[]
#define _BIT_OLD           B00000010
#define _BIT_NEW           B00000001
#define _NEW_AND_OLD       (_BIT_NEW + _BIT_OLD)

#define _ALL_OLD_BITS      B10101010
#define _ALL_NEW_BITS      B01010101

// Results generated by Get_Input()
#define INP_OFF            B00000000
#define INP_ON             B00000011
#define INP_TURNED_ON      B00000001
#define INP_TURNED_OFF     B00000010
#define INP_INIT_OFF       B00000110


#define EndCfg END_T




#ifdef USE_EEPROM_CFG
   #define CONFIG_EEPROM      2            // Start adress of the configuration in the EEPROM
   #define CONFIG_MAX_RAM     50

   #define MobaLedLib_Create(leds)          uint8_t Config_RAM[CONFIG_MAX_RAM]; /* RAM reserved for the configuration functions. */ \
                                            MobaLedLib_C MobaLedLib(leds, sizeof(leds)/sizeof(CRGB), (const unsigned char*)CONFIG_EEPROM, Config_RAM, sizeof(Config_RAM)); // MobaLedLib_C class definition
#else // Configuration stored in FLASH
   #define MobaLedLib_Configuration()       const PROGMEM unsigned char Config[] =

   #define MobaLedLib_Create(leds)          uint8_t Config_RAM[__COUNTER__/2]; /* RAM used for the configuration functions. The size is calculated in the macros which are used in the Config[] table.*/ \
                                            MobaLedLib_C MobaLedLib(leds, sizeof(leds)/sizeof(CRGB), Config, Config_RAM, sizeof(Config_RAM)); // MobaLedLib_C class definition
#endif


#ifdef USE_EEPROM_CFG  // EERPROM
  #define Config_read_byte(x)  eeprom_read_byte(x)
  #define Config_read_dword(x) eeprom_read_dword((uint32_t*)(x))
  #define Config_read_word(x)  eeprom_read_word((uint16_t*)(x))

#else
  #define Config_read_byte(x)  pgm_read_byte_near(x)
  #define Config_read_dword(x) pgm_read_dword_near(x)
  #define Config_read_word(x)  pgm_read_word_near(x)
#endif

#ifdef USE_EEPROM_CFG
#else
#endif
//---------------------------------------------
inline bool Inp_Changed_or_InitOff(uint8_t Inp)
//---------------------------------------------
{
  return Inp == INP_TURNED_ON || Inp == INP_TURNED_OFF || Inp == INP_INIT_OFF;
}
// Optimierungsuntersuchung:
// Inp_Changed_or_InitOff():
//   - Wenn man nur den ersten Vergleich (Inp == INP_TURNED_ON) aktiviert, dann dauert
//     der Update 189us anstelle von 193us bei einer Konfiguration mit 13 Pattern aufrufen
//     und einer Logic() Funktion. => 2% schneller.
//   - Ohne die (Inp == INP_INIT_OFF) Abfrage dauert es 190us => 1.5% schneller.

//------------------------------------
inline uint8_t Invert_Inp(uint8_t Inp)
//------------------------------------
{
  return Inp ^ B00000011;  // XOR: 00 => 11, 01 => 10, 10 => 01, 11 => 00
}

//--------------------------------
inline bool Inp_Is_On(uint8_t Inp)
//--------------------------------
{
  return Inp == INP_ON || Inp == INP_TURNED_ON; //Inp & _BIT_NEW;
}

//--------------------------------------------------
inline bool Inp_Is_TurnedOff_or_InitOff(uint8_t Inp)
//--------------------------------------------------
{
  return Inp == INP_TURNED_OFF || Inp == INP_INIT_OFF ;
}

//----------------------------------
inline bool Inp_Changed(uint8_t Inp)
//----------------------------------
{
  return Inp == INP_TURNED_ON || Inp == INP_TURNED_OFF;
}

typedef struct                                                                                                // 07.11.18:
    {
    uint8_t         Val;
    uint8_t         Changed:1;     // Is cleared in New_Local_Var()
    uint8_t         ExtUpdated:1;  // Is set by external functions. In Set_Local_Var() this flag is copied to Changed and cleared
    } ControlVar_t;

typedef struct
    {
    uint8_t         Hue;
    uint8_t         Sat;
    uint8_t         Val;
    } HSV_T;

typedef struct
    {
    uint16_t        Last_t;
    uint16_t        dt;
    uint8_t         State;
    uint16_t        Start_t;
    } AnalogPatternData_T;  // 7 Byte



//:::::::::::::::::::
class MobaLedLib_C
//:::::::::::::::::::
{
public:
                   MobaLedLib_C(struct CRGB* _leds, uint16_t Num_Leds, const unsigned char Config[], uint8_t RAM[], uint16_t RamSize); // Constructor
void               Set_Input(uint8_t channel, uint8_t On);
int8_t             Get_Input(uint8_t channel);
void               Update();
void               Set_ControlVar(uint8_t Val);
void               Restart() { Initialize = 1; }
uint8_t            Proc_CPX_InitMacros();

private: // Variables
 const uint8_t     *cp;          // Pointer to the Config[]            //  2 Byte
 CRGB*              leds;                                              //  2 Byte
 uint16_t           Num_Leds;                                          //  2 Byte
 const uint8_t     *Config;                                            //  2 Byte
 const uint8_t     *Config0;                                           //  2 Byte
 bool               Initialize;                                        //  1 Byte
 uint8_t           *RAM;                                               //  2 Byte
 uint8_t           *rp;          // Pointer to the RAM                 //  2 Byte
 uint32_t           t;           // actual time [ms]                   //  4 Byte                             // ToDo 10.12.19:
#ifndef _USE_LOCAL_TX
 uint16_t           t4w;         // time>>4   = / 16                   //  2 Byte
#endif
 uint8_t            InpStructArray[_INP_STRUCT_ARRAY_SIZE];            // 64 Byte   Array which contains two bits for each input. One bit for the actual state and a second for the new state. (See INP_TURNED_ON)
#ifdef PM_HSV
 HSV_T             *HSV_p;                                             //  2 Byte
#endif
 ControlVar_t      *ActualVar_p;                                       //  2 Byte                                                                  // 07.11.18:
 ControlVar_t       ControlVar;

// Private functions
 void               Proc_AnalogPattern(uint8_t TimeCnt, bool AnalogMode);
 uint8_t            Is_Pattern(uint8_t Type);
 void               IncCP_Pattern(uint8_t TimeCnt);
 void               Inp_Processed();
}; // class MobaLedLib_C




#endif // __MOBALEDLIB_MINI_H__
