#include <Arduino.h>

// Disable the warning:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#include <EEPROM.h>
#pragma GCC diagnostic pop


#include "Public_Defines.h"

#include "MoBaLedLib_Mini.h"
#include "PWM_In.h"
#include "crc16.h"
#include "CharlieLEDs12.h"

extern MobaLedLib_C MobaLedLib;


#ifdef _PRINT_DEBUG_MESSAGES // Debug
//-----------------------------------------------
void Debug_Print_Char(uint8_t b, const char *Txt)
//-----------------------------------------------
{
    //if (b >= ' ') Dprintf("'%c' ", b);
    Dprintf("0x%02X %s\n", b, Txt);
}
#else
  #define Debug_Print_Char(b, Txt)
#endif

//---------------------------------------
void Store_Byte(uint16_t Addr, uint8_t b)
//---------------------------------------
{
  Debug_Print_Char(b, "");
  EEPROM.write(Addr, b);
}


//-------------------------------------
int16_t Get_Dat_from_PWM(int16_t pwm01)
//-------------------------------------
// pwm01    return    see sheet "Disabled Int & Asm" in "LED_PWM Kennlinie.xlsx"
// 948-921  0
// 920-894  1
// 893-867  2
// :
//  73-  1  32
{
  return OptMap(pwm01, 948, 100, 0, 31);
}

#include <avr/wdt.h>

//------------------
void softwareReset()
//------------------
{
  /*
  Der Reset funktioniert beim ATTiny nicht richtig. Hier startet das Programm zwar
  neu, haengt aber dann in einer Schleife in der es staendig neu gestartet wird ;-(
  Aber es funktioniert auch ohne den Reset.
  Komisch beim Uno war er noetig => Untersuchen

  // start watchdog with the provided prescaller
  wdt_enable(WDTO_250MS);
  // wait for the prescaller time to expire
  // without sending the reset signal by using
  // the wdt_reset() method

  while(1)
    {
    }
 */
}

// Programmier Mode:
// ~~~~~~~~~~~~~~~~~
// Der Programmier Mode wird aktiviert indem man den WS2811 PWM Wert 10 schickt.
// Dieser kleine Wert wurde gewaehlt weil so dunkle LEDs unwarscheinlich sind.
// Damit wird die Funktion "Check_ProgramMode()" gestartet.
// In der Routine wird das WS2811 PWM Signal bei deaktivierten Interrupts gelesen.
// Dadurch ist das Signal nicht gestoert. Es schwankt nur um eine Stelle weshalb kein
// nachgeschalteter Teifpass Filter noetig ist. So koennen die Daten schnell gelesen werden.
// Es werden 32 verschiedene PWM Signale erkannt. Zwischen zwei Daten liegen 7 PWM Stufen
// so dass genuegend Sicherheitsabstand vorhanden ist.
// Die Daten sollen beliebig langsam gesendet werden koennen. Zur Erkennung eines neuen
// Datenwerts wird das oberste Bit bei jedem neuen Datum umgeschaltet.
// Dadurch bleiben 4 Bit zur Uebertragung der Daten welche nacheinander empfangen werden.
// Es wird zunaechst das niederwertige Nibbel und dann das hoeherwertige Nibbel uebertragen.
// ...

// Zur Datenuebertragung werden die interrupts gesperrt.
// Dann werden die WS2811 PWM Signale sehr genau gemessen.
// Das funktioniert auch bei verschiedenen Temperaturen (Kaeltespray)
// und Spannungen.
// Abstand zwischen zwei Datenwerten: 4,  Start bei 40 => 200/4

// ToDo:
// ~~~~~
// - Doku
// - Wie wird sichergestellt, dass die Routinen nicht aufgerufen wird wenn auf einen anderen Wert umgeschaltet wird
//   0 -> x  oder x > 0
//   Eigentlich sollte das in der Hauptschleife gemacht werden damit die Check_ProgramMode() routine gar
//   nicht erst aufgerrufen wird. Ich kann das Problem aber gar nicht generieren => Spaeter

#define HEAD_STR "CP1"  // This is the magic headder which must be received at the beginning. Otherwise the
                        // programming mode is aborted.

//-------------------------
uint8_t Check_ProgramMode()
//-------------------------
// Return 1 if the configuration has changed
{
  int8_t   LastDat      = -99;
  int8_t   StableDat    = -99;
  uint8_t  DatEqualCnt  = 0;
  uint16_t Buf = 0;
  uint8_t  Used_Bits    = 0;
  uint8_t  EndeWhile    = 0;
  uint8_t  CheckHeadPos = 0;
  //uint16_t crc          = 0xFFFF;  // CRC-16/MODBUS   Poly: 0x8005
  uint16_t EEAdr        = 0;
  uint8_t  FirstDat     = 1;

  for (uint8_t i = 0; i < LAST_LED; i++) // Enable all LEDs => They are flashing while prog. mode is active
      Set_LED(i, MAX_BRIGHT);

  while (!EndeWhile)
    {
    cli(); // disable the interrupts
    int16_t pwm01 = PWM_in(PWM_PIN, HIGH, 5800, PeriodBuffer); // timeout set to 5.8 ms to have a reserve because CPU clock is not very accurate
    sei(); // enable the interrupts again

    int8_t Dat = Get_Dat_from_PWM(pwm01);
    if (Dat == LastDat)
         {
         switch (DatEqualCnt)
            {
            case 255: break; // Don't increment DatEqualCnt any more to prevent an overflow
            case   6: StableDat = Dat;  // sometimes we get a wrong value if the stable count is <= 3
                      //Dprintf("     PWM:%3i ", pwm01); // Debug
                      if (0 <= StableDat && StableDat <= 31)
                           {
                           if (FirstDat)
                                FirstDat = 0; // Ignore the first Data because it's the  Start Prog Mode command
                           else {
                                //Dprintf("Stable %i (%i)\n", StableDat&0x0F, StableDat); // Debug
                                Buf |= (Dat & PAYLOAD_MASK) << Used_Bits;
                                Used_Bits += BITS_PER_DATA;
                                if (Used_Bits >= 8)
                                   {
                                   uint8_t b = Buf & 0xFF;
                                   if (CheckHeadPos < sizeof(HEAD_STR)-1)
                                        {
                                        if (b != HEAD_STR[CheckHeadPos])
                                           {
                                           Dprintf("Head Error (Pos %i=%02X in '" HEAD_STR "')\n", CheckHeadPos, b);// Debug
                                           #ifndef __AVR_ATtiny85__
                                             Dprintf("Nach dem Start muss zunaechst der ");                   // Debug
                                             Dprintf("Headder mit 'c' geschickt werden.\n");                  // Debug
                                           #endif
                                           EndeWhile = 1;
                                           break;
                                           }
                                        else Debug_Print_Char(b, " Head Check");                                   // Debug
                                        CheckHeadPos++;
                                        if (CheckHeadPos == sizeof(HEAD_STR)-1) Dprintf("Head O.K.\n");
                                        }
                                   else {
                                        Store_Byte(EEAdr++, b);
                                        //Dprintf("Add CRC %i\n", b); // Debug
                                        //crc = _crc16_update(crc, b);
                                        }
                                   Buf >>= 8;
                                   Used_Bits -= 8;
                                   }
                                }
                           }
                      else {
                           // End progamming Mode with LED PWM = 0  or >= 250
                           Dprintf("Special %i pwm:%i\n", StableDat,pwm01); // Debug   9356 Bytes
                           EndeWhile = 1;
                           break;
                           }
                      // No break to increment DatEqualCnt
            default : DatEqualCnt++;
            }
         }
    else {
         LastDat     = Dat;
         DatEqualCnt = 0;
         StableDat   = -99;
         }
    }
  if (Used_Bits) // Wird nur aufgerufen wenn die Anzahl der Bits nicht in ein Byte passen
     {
     Store_Byte(EEAdr, Buf & 0xFF);
     Dprintf("Remaining data Add CRC %i\n", Buf & 0xFF); // Debug
     //crc = _crc16_update(crc, Buf & 0xFF);
     }
  //Dprintf("\nEnd crc %04X\n", crc);

  if (CheckHeadPos >= sizeof(HEAD_STR)-1)
     {
     softwareReset();
     return 1;
     }
  return 0;
}


#ifdef USE_EEPROM_CFG
  //-------------------------
  int8_t Check_EEPROM_CRC()
  //-------------------------
  {
    uint16_t crc  = 0xFFFF;  // CRC-16/MODBUS   Poly: 0x8005
    uint16_t Size = Config_read_word(0);
    Dprintf("EEConfig size %i:\n", Size);
    if (Size > 512) Size = 20; // Just show the first bytes
    const uint8_t* p;
    for (p = (const uint8_t*)0; Size > 0; p++, Size--)
      {
      uint8_t b = Config_read_byte(p);
      crc = _crc16_update(crc, b);
      Dprintf("%i, ", b);
      }
    uint16_t Exp_crc = Config_read_word(p);
    Dprintf("\n"
            "Calc.CRC: %04X\n", crc);
    Dprintf("Exp. CRC: %04X\n", Exp_crc);
    return (crc == Exp_crc) ? CFG_LOAD_OK : CFG_LOAD_ERR_CRC;
  }


#else // Config is read out of the FLASH
  //-----------------------------------------------------------------
  void Debug_Print_Flash_Config(const uint8_t *Config, uint16_t Size)
  //-----------------------------------------------------------------
  //        S_Hauptsignal_Zs3_Zs1         SI_1         TimeCnt Mode
  //        XPatternT11(0,128,SI_LocalVar,|    4,0,255,0, |      0, 200 ms,  500 ms,  200 ms, 500 ms, 200 ms, 500 ms,  200 ms, 500 ms, 200 ms, 500 ms, 200 ms,        32,64,80,160,42,2  ,0,63,128,63,128,63,128,64,0,0,1)     // KS_Hauptsignal_Zs3_Zs1
  // Bytes: 50,         0,128,254,        255, 4,0,255,0, 8,        200,0,   244,1,   200,0,  244,1,  200,0,  244,1,   200,0,  244,1,  200,0,  244,1,  200,0,  17, 0, 32,64,80,160,42,2  ,0,63,128,63,128,63,128,64,0,0,1,
  //        0,  <= Ende
  //
  // C:    50, 0, 128, 254, 255, 4, 0, 255, 0, 8, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 17, 0, 32, 64, 80, 160, 42, 2  , 0, 63, 128, 63, 128, 63, 128, 64, 0, 0, 1,
  // Excel 50, 0, 128, 254, 255, 4, 0, 255, 0, 8, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 244, 1, 200, 0, 17, 0, 32, 64, 80, 160, 42, 2  , 0, 63, 128, 63, 128, 63, 128, 64, 0, 0, 1,
  {
    #ifdef _PRINT_DEBUG_MESSAGES
      Dprintf("Flash Config: \n");
      for (const uint8_t* p = Config; Size > 0; p++, Size--)
        {
        Dprintf("%i, ", Config_read_byte(p));
        }
      Dprintf("\n");
    #endif
  }
#endif


