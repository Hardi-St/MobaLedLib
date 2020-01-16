/*
 This module is used to store messages in the persistent RAM to be able to print them after a reboot.
 Additional persistent data could be used. They have to be stored in a structure and passed to the
 functions Save_Persist_Checksum() and Check_Persist_Checksum().
 If no additional data are used the first parameter = NULL.

 Usage:
   setup():
      if (Check_Persist_Checksum(&Pers, sizeof(Pers)))
         Serial.println(Get_Persist_Msg());

   Some where before a reboot
      Store_Persistent_Msg(Format, ...);
      Save_Persist_Checksum(&Pers, sizeof(Pers));

*/

#ifndef _PERSIST_MSG_H_
#define _PERSIST_MSG_H_

#define Store_Persistent_Msg(Format, ...) Persist_printf_proc(0, F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
#define Add_Persistent_Msg(  Format, ...) Persist_printf_proc(1, F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html

char     __attribute__((section(".noinit"))) Persist_Msg[160];  // Variable located in the RAM section which is not initialized => It keeps the value after a warm start
uint16_t __attribute__((section(".noinit"))) Persist_MsgChk;

//--------------------------------------------------------
uint16_t crc16(char Buff[], uint16_t length, uint16_t crc)
//--------------------------------------------------------
// Init crc with ~0L
{
  const unsigned long crc_table[16] = {
    0x00000000, 0x1db71064, 0x3b6e20c8, 0x26d930ac,
    0x76dc4190, 0x6b6b51f4, 0x4db26158, 0x5005713c,
    0xedb88320, 0xf00f9344, 0xd6d6a3e8, 0xcb61b38c,
    0x9b64c2b0, 0x86d3d2d4, 0xa00ae278, 0xbdbdf21c
  };

  for (uint16_t index = 0 ; index < length  ; ++index)
    {
    crc = crc_table[(crc ^  Buff[index]) & 0x0f] ^ (crc >> 4);
    crc = crc_table[(crc ^ (Buff[index] >> 4)) & 0x0f] ^ (crc >> 4);
    crc = ~crc;
    }
  return crc;
}

//---------------------------------------------------------------------------
void Persist_printf_proc(uint8_t Add, const __FlashStringHelper *format, ...)
//---------------------------------------------------------------------------
{
  va_list ap;
  va_start(ap, format);
  char *s = Persist_Msg;
  uint16_t MaxLen = sizeof(Persist_Msg);
  if (Add)
     {
     uint16_t ActLen = strlen(Persist_Msg);
     if (ActLen < MaxLen) // Check for corrupted string
        {
        MaxLen -= ActLen;
        s += ActLen;
        }
     }
  #ifdef __AVR__
     vsnprintf_P(s, MaxLen, (const char *)format, ap); // progmem for AVR
  #else
     vsnprintf  (s, MaxLen, (const char *)format, ap); // for the rest of the world
  #endif

  va_end(ap);
}

/*
08389767 B _end
08389766 b Pers_Flags
08389747 B param
08389587 B Persist_Msg
08389585 b Persist_MsgChk
08389585 B __bss_end

08389769 B _end
08389768 b Pers_Flags
08389749 B param
08389747 b A
08389587 B Persist_Msg
08389585 b Persist_MsgChk
08389585 B __bss_end

08389769 B _end
08389768 b Pers_Flags
08389749 B param
08389747 b Z
08389587 B Persist_Msg
08389585 b Persist_MsgChk
08389585 B __bss_end
*/

//----------------------------------------------
uint16_t Calc_CRC(void *Pers_Dat, uint16_t Size)
//----------------------------------------------
{
  uint16_t crc = crc16(Persist_Msg, sizeof(Persist_Msg), ~0L);
  if (Pers_Dat)
       return crc16((char*)Pers_Dat, Size, crc);
  else return crc;
}

//-------------------------------------------------------
void Save_Persist_Checksum(void *Pers_Dat, uint16_t Size)
//-------------------------------------------------------
{
  Persist_MsgChk = Calc_CRC(Pers_Dat, Size);
}

//--------------------------------------------------------
bool Check_Persist_Checksum(void *Pers_Dat, uint16_t Size)
//--------------------------------------------------------
{
  return Persist_MsgChk == Calc_CRC(Pers_Dat, Size);
}

//---------------------------
const char* Get_Persist_Msg()
//---------------------------
{
  return Persist_Msg;
}

#endif // _PERSIST_MSG_H_
