#include "Fuses.h"
#include "Global_Defines.h"

/*
 ATTiny85: 8-Pin
 ~~~~~~~~~~~~~~~

 C:\Dat\Maerklin\Datenblaetter\atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf

 Page: 148 (ATTiny84, ATTiny85, ATtiny88)
Table 20-4. Fuse High Byte
RSTDISBL    7 External reset disabled
DWEN        6 DebugWIRE enabled
SPIEN       5 Serial program and data download
WDTON       4 Watchdog timer always on
EESAVE      3 EEPROM preserves chip erase
BODLEVEL2   2 Brown-out Detector trigger level
BODLEVEL1   1 Brown-out Detector trigger level
BODLEVEL0   0 Brown-out Detector trigger level



Table 20-5. Fuse Low Byte (ATTiny84, ATTiny85)       ATTiny88
CKDIV8      7 Clock divided by 8                     CKDIV8      7 Clock divided by 8
CKOUT       6 Clock output enabled                   CKOUT       6 Clock output enabled
SUT1        5 Start-up time setting                  SUT1        5 Start-up time setting
SUT0        4 Start-up time setting                  SUT0        4 Start-up time setting
CKSEL3      3 Clock source setting
CKSEL2      2 Clock source setting
CKSEL1      1 Clock source setting                   CKSEL1      1 Clock source setting
CKSEL0      0 Clock source setting                   CKSEL0      0 Clock source setting

ATTiny84: Table 6-4.                   ATTiny85: Table 6-6.           ATTiny88
CKSEL[3:0] Nominal Frequency           CKSEL3:0 Nominal Frequency
0000                 Ext                                              00    Ext
0001         16 MHz  PLL                                              01    Reserved
0010        8.0 MHz                    0010        8.0 MHz            10    8 MHz
0011        6.4 MHz  (Tiny15 mode)     0011        6.4 MHz
0100        128 kHz                    0100        128 kHz            11    128 kHz
0110     32.768 kHz  Ext
0101      0.9-3 MHz  Ext
0110        3-8 MHz  Ext
0111        8-  MHz  Ext

Alle Moeglichkeiten (See: Table 6-1. Device Clocking Options Select)

Fuse Calculator:
 8   MHz Divide by 8 0x62
 8   MHz Divide by 1 0xE2
 128 kHz Divide by 8 0x64
 128 kHz Divide by 1 0xE4


ATTiny88 Table 6-1. Device Clocking Options
CKSEL[1:0]
00 External Clock 0 – 12 MHz
01 Reserved –
10 Calibrated Internal 8MHz Oscillator 8.0 MHz
11 Internal 128 kHz Oscillator 128 kHz



ATtiny25 0x1E 0x91 0x08  Table 20-7, C:\Dat\Maerklin\Datenblaetter\atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
ATtiny45 0x1E 0x92 0x06
ATtiny85 0x1E 0x93 0x0B


**************************************************************************************************************

ATTiny84: 14-Pin
~~~~~~~~~~~~~~~~

C:\Dat\Maerklin\Datenblaetter\ATTiny24_44_84.pdf



**************************************************************************************************************

ATTiny88 28-Pin
~~~~~~~~~~~~~~~

C:\Dat\Maerklin\Datenblaetter\ATTiny88.pdf

Table 20-4. Fuse High Byte
Fuse High Byte Bit No Description Default Value
RSTDISBL    7 External Reset Disable
DWEN        6 debugWIRE Enable
SPIEN       5 Enable Serial Program and Data Downloading
WDTON       4 Watchdog Timer Always On
EESAVE      3 EEPROM memory preserved through chip erase
BODLEVEL2   2 Brown-out Detector trigger level
BODLEVEL1   1 Brown-out Detector trigger level
BODLEVEL0   0 Brown-out Detector trigger level


**************************************************************************************************************

Device Signature Bytes
~~~~~~~~~~~~~~~~~~~~~~
ATtiny24 0x1E 0x91 0x0B  Table 19-7, C:\Dat\Maerklin\Datenblaetter\ATTiny24_44_84.pdf
ATtiny44 0x1E 0x92 0x07
ATtiny84 0x1E 0x93 0x0C
ATtiny25 0x1E 0x91 0x08  Table 20-7, C:\Dat\Maerklin\Datenblaetter\atmel-2586-avr-8-bit-microcontroller-attiny25-attiny45-attiny85_datasheet.pdf
ATtiny45 0x1E 0x92 0x06
ATtiny85 0x1E 0x93 0x0B
ATtiny48 0x1E 0x92 0x09  Table 20-6, C:\Dat\Maerklin\Datenblaetter\ATTiny88.pdf
ATtiny88 0x1E 0x93 0x11

BODLEVEL
~~~~~~~~
111 BOD Disabled (ATtiny84, ATTiny85, ATTiny88)
110 1.8V
101 2.7V
100 4.3V
0XX Reserved

*/


// Fuse High Byte
#define RSTDISBL   B10000000
#define WDTON      B00010000
#define EESAVE     B00001000


// Fuse Low Byte
#define CKDIV8     B10000000

//------------------------------------------
void Print_Fuses_txt(Chip_Data_T &Chip_Data)
//------------------------------------------
// Print the important fuses as text
{
  // Clock:
  uint8_t CKSEL_Msk = 0;
  switch (Chip_Data.Signatur)
    {
    //case ATTINY13: ??
    case ATTINY24:
    case ATTINY44:
    case ATTINY84:
    case ATTINY25:
    case ATTINY45:
    case ATTINY85: CKSEL_Msk = B00001111; break;
    case ATTINY48:
    case ATTINY88: CKSEL_Msk = B00000011; break;
    case 0xFFFF:   return ;
    default: printf("Undef. signatur %04X in Print_Fuses_txt\n", Chip_Data.Signatur);
             return ;
    }
  if (CKSEL_Msk)
     {
     switch (Chip_Data.lfuse & CKSEL_Msk)
        {
        case B00000001: printf("16 MHz");                                       break;
        case B00000010: printf("%c MHz", Chip_Data.lfuse & CKDIV8 ? '8':'1');   break; // 8MHz / 1 MHz
        case B00000011: if (CKSEL_Msk == B00000011) // ATTINY48 / 88
                             printf("128 kHz");
                        else printf("6.4 MHz");
                        break;
        case B00000100: printf("128 kHz");                                      break;
        default:        printf("Ext. clock");
        }
     }


  printf(", BOD ");
  switch (Chip_Data.hfuse & B00000111)
    {
    case B00000111: printf("Disabled"); break;
    case B00000110: printf("1.8V");     break;
    case B00000101: printf("2.7V");     break;
    case B00000100: printf("4.3V");     break;
    default: printf("Reserved");
    }
  printf(", Reset pin ");
  if (Chip_Data.hfuse & B10000000)
       printf("normal");
  else printf("as IO ");

  if ((Chip_Data.hfuse & WDTON ) == 0) printf(", Watchdog on");
  if ((Chip_Data.hfuse & EESAVE) == 0) printf(", EEPROM preserve");
  printf("\n");
}

//--------------------------------------
void Print_Fuses(Chip_Data_T &Chip_Data)
//--------------------------------------
{
  printf(  "LFuse: %02X",   Chip_Data.lfuse);
  printf(", HFuse: %02X",   Chip_Data.hfuse);
  printf(", EFuse: %02X\n", Chip_Data.efuse);
  Print_Fuses_txt(Chip_Data);
}

