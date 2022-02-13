#include "Helpers.h"

//-------------------------------------------------------
void printf_proc(const __FlashStringHelper *format, ...)
//-------------------------------------------------------
// Achtung: Es durfen keine Zeichen über die serielle Schnittstelle ausgegeben werden wenn der LED Arduino
//          programmiert wird und die Beiden über die TX Leitung verbunden sind. Die serielle Schnittstelle
//          muss nach jeder Ausgabe abgeschaltet werden. Sonst zieht der TX Ausgang dieses Nanos die
//          RX Leitung des LED Arduinos auf 5V.
//          Bei der normalen Kommunikation wird das über die A1 Leitung zwischen den beiden Rechnern gesteuert.
//          Wenn der SPI Mode aktiviert ist, dann wird die A1 Leitung als Input geschaltet damit sie auf dem
//          LED Arduino als Eingang für die Schalter genutzt werden kann.
{
#ifdef DEBUG_INTERFACE	
     char buf[50];
     va_list ap;
     va_start(ap, format);
     #ifdef __AVR__
        vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
     #else
        vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
     #endif
     va_end(ap);
     Serial.print(buf);
#endif		 
}

