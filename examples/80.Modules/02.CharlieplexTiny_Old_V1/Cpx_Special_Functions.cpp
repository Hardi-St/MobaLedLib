#include <Arduino.h>

// Disable the warning:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <EEPROM.h>
#pragma GCC diagnostic pop

#include "MoBaLedLib_Mini.h"
#include "CharlieLEDs12.h"
#include "Cpx_Special_Functions.h"

//-----------------------------------------
void Set_LED_Assignement(const uint8_t* &cp)
//-----------------------------------------
{
  for (uint8_t i = 0; i < 12; )
    {
    uint8_t a = Config_read_byte(cp);
    cp++;
    Set_one_LED_Assignement(i++, a>>4);
    Set_one_LED_Assignement(i++, a & 0xF);
    }
}

uint8_t ReadAnalog = 0;
uint16_t AnalogLimmits[ANALOG_KEYS];

uint16_t Default_AnalogLimmits[ANALOG_KEYS] = {768, 272, 157, 96, 58, 40, 28, 20, 12, 5 }; // 1K PullUp + 22K Intern im ATTiny
//uint16_t Default_AnalogLimmits[ANALOG_KEYS] = {761, 256, 144, 86, 50, 34, 22, 14,  7, 2 }; // WS2811 + 1K PullUp + 22K Intern im ATTiny

//--------------------------------------
void Enable_Analog_Input(uint8_t Enable)
//--------------------------------------
{
  ReadAnalog = Enable;
  if (Enable)
     {
     memcpy(AnalogLimmits, Default_AnalogLimmits, sizeof(AnalogLimmits));
     }
}

//-----------------------------------------
void Read_Analog_Limits(const uint8_t* &cp)
//-----------------------------------------
{
  ReadAnalog = 1;
  //Dprintf("AnaLim: ");
  for (uint8_t i = 0; i < ANALOG_KEYS; i++)
    {
    AnalogLimmits[i] = Config_read_word(cp);
    //Dprintf("%i ", AnalogLimmits[i]);
    cp += 2;
    }
  //Dprintf("\n");
}
