

#include "MobaLedLib_Mini.h"

// Disable the warning:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <EEPROM.h>
#pragma GCC diagnostic pop

#include "Cpx_Special_Functions.h"


//------------------------------------------------------------------------------------------------------------------------------
MobaLedLib_C::MobaLedLib_C(struct CRGB* _leds, uint16_t Num_Leds, const unsigned char Config[], uint8_t RAM[], uint16_t RamSize) // Constructor
//------------------------------------------------------------------------------------------------------------------------------
 : leds(_leds),
   Num_Leds(Num_Leds),
   Config(Config),
   Config0(Config),
   Initialize(true),
   RAM(RAM),
   ActualVar_p(&ControlVar)
 #if _USE_USE_GLOBALVAR
   ,GlobalVar_Cnt(0)
 #endif
{
  memset(RAM, 0, RamSize);

  memset(InpStructArray, 0x00, _INP_STRUCT_ARRAY_SIZE);
  Set_Input(SI_1, 1);            // Special input which is always 1

  Inp_Processed();        // Clear the Old_Inp[] array

  // Is called in the setup after the EEPROM Check is o.k.
  //Update();   // Must be called once before the inputs are read. (Attention: srandom() must be called before to get unpredictable random numbers)
}

//-----------------------------------------
uint8_t MobaLedLib_C::Proc_CPX_InitMacros()
//-----------------------------------------
// Process the Charlieplexing initialisation macros
// Return 0 if o.k.
{
  const uint8_t *p = Config0;
  while (1)
    {
    uint8_t Typ = Config_read_byte(p);
    Dprintf("Typ:%i\n", Typ);
    switch (Typ)
      {
      case CPX_LED_ASSIGNEMENT: p++;
                                Set_LED_Assignement(p);
                                break;
      case CPX_ANALOG_INPUT:    p++;
                                Enable_Analog_Input(1);
                                break;
      case CPX_ANALOG_LIMMITS:  p++;
                                Read_Analog_Limits(p);
                                break;
      default: if (Typ >= PATTERNT1_T && Typ <= LAST_APATTERN_T)
                     {
                     Config = p;
                     return CFG_LOAD_OK;
                     }
               else  {
                     Dprintf("Unknown Init Macro %i\n", Typ);
                     return CFG_LOAD_ERR_INITMACRO;
                     }
      }
    }
}

//---------------------------------------------
int8_t MobaLedLib_C::Get_Input(uint8_t channel)
//---------------------------------------------
// Return the state of one input channel
//  INP_OFF
//  INP_ON
//  INP_TURNED_ON
//  INP_TURNED_OFF
{
  uint8_t ByteNr = channel>>2;       // channel / 4;
  uint8_t Shift  = (channel % 4)<<1;
  return (InpStructArray[ByteNr]>>Shift) & B00000011;
}

// Optimierungen:
//   - Wenn die ByteNr und Shift Berechnung durch Konstanten ersetzt wird, dann dauert es
//     156us anstelle von 193us. => 20% schneller
//   - Die Verwendung vom mod8() macht es deutlich langsamer ! 228us anstelle von 193us!
//   - ByteNr berechnung in die [] => Kein Unterschied
//   => Keine weitere Idee zur Verbesserung


//-------------------------------------------------------
void MobaLedLib_C::Set_Input(uint8_t channel, uint8_t On)
//-------------------------------------------------------
{
  //if (channel>2) Dprintf("Set_Input %i=%i\n", channel, On?1:0);
  uint8_t ByteNr  = channel>>2;  // / 4;
  uint8_t BitMask = 1 << ((channel % 4)<<1);  // 2^((channel%4)*2)
  if (On)
       InpStructArray[ByteNr] |=  BitMask;
  else InpStructArray[ByteNr] &= ~BitMask;
}

//--------------------------------
void MobaLedLib_C::Inp_Processed()
//--------------------------------
// Must be called after the Update loop to store the old input values
{
  for (uint8_t *p = InpStructArray, *e = p + _INP_STRUCT_ARRAY_SIZE; p < e; p++)
    {
    uint8_t Act = *p & _ALL_NEW_BITS;
    *p &= _ALL_NEW_BITS;
    *p |= Act << 1;
    }
}

//--------------------------------------------
uint8_t MobaLedLib_C::Is_Pattern(uint8_t Type)
//--------------------------------------------
// return  0             if it's no pattern
//         PATTERNT1_T   if it's a digital pattern
//        APATTERNT1_T   for analog pattern or XPatternT
{
  if      (Type >= PATTERNT1_T  && Type <= LAST_PATTERN_T)  return PATTERNT1_T;
  else if (Type >= APATTERNT1_T && Type <= LAST_APATTERN_T) return APATTERNT1_T; // 24.08.18:
  return 0;
}

//--------------------------------------------
void MobaLedLib_C::Set_ControlVar(uint8_t Val)
//--------------------------------------------
{
  ControlVar.Val = Val;
  ControlVar.Changed = 1;
}

//-------------------------
void MobaLedLib_C::Update()
//-------------------------
// This function has to be called periodicly in the loop() function
// to update all LEDs
{
  ActualVar_p = &ControlVar;
  t   = millis();
  #ifndef _USE_LOCAL_TX                                                                                       // 22.11.18:
    t4w = (t>>4)  & 0xFFFF; // Time divided by 16
  #endif

  cp = Config;
  rp = RAM;
  uint8_t Type = Config_read_byte(cp++);
  uint8_t pt = Is_Pattern(Type);
  uint8_t TimeCnt = Type - pt + 1;
  Proc_AnalogPattern(TimeCnt, pt != PATTERNT1_T);


  Initialize = false;
  //Inp_Processed();
  ControlVar.Changed = 0;
}
