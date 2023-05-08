// Disable the warning:                                                                                       // 26.12.19:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
// and:
//   ... warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
//
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#include <NmraDcc.h>
#pragma GCC diagnostic pop

/*
 * sx_interface.ino
 *
 *  Created on: 10.11.2013
 *  Changed on: 06.05.2020   by Wilfried, Frank and Hardi
 *
 *  Author: Michael Blank
 *
 *  Based on the Example program for the Selectrix(TM) Library
 *
 *  Purpose:
 *  ~~~~~~~~
 *  Read the Selectrix data and send them to the RS232 for the MoBaLedLib
 *  Pin A1 is used to disable the comunication while the LED Arduino updates the LEDs
 *  because then the interrupts are locked and messages will get lost.
 *
 Documents:
 ~~~~~~~~~~
- https://www.mec-arnsdorf.de/index.php/selectrix/selectrix-protokoll/

Revision History :
~~~~~~~~~~~~~~~~~
05.05.20:  Versions 1.2
           - Adding the changes made in the DCC Program to this progarm also:
             - The serial port was not disabled at startup
               If DCC messages have been received and send to the LED Arduino it was disabled
               This generates problems when flashing the LED Arduino.
             - The build in LED alsow shows a slow heartbeat with a period of 3 seconds if it's running normal
               The buffer overflow signaling is not changed:
               - 1 Hz 50%: active buffer owerflow 1 Hz
               - 1 Hz 10%: prior buffer owerflow, but now it's working fine
18.10.20:  - Switching the external buffer gate which disables the TX pin in PCB version 1.7 (Not tested)
           Versions 1.4
01.05.23:  - synchronize code with DCC version
           - add signal loss detection
           Versions 1.5
*/

#define SKETCH_VERSION "1.5"

#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                             // 13.05.20:
  #include<SPI.h>
  #define STATUSLED_PIN  8
  #define STATUSLED_MOD  !  // LED is connected to GND => It has to be inverted
  #define RX_PIN         0
#else
  #define STATUSLED_PIN  LED_BUILTIN
  #define STATUSLED_MOD
#endif

#define SEND_DISABLE_PIN A1
#define BUF_GATE_PIN     A5                                                                                   // 18.10.20:

#include "SX20.h"                             // this is the Selectrix library (We use a modified version from Wilfried which is stored localy)
#define QUEUESIZE  512                        // Must be a "binary" number 32,64,128, ...

volatile char     SendBuffer[QUEUESIZE];
volatile char    *rp = SendBuffer;        // Queue Read Pointer
volatile char    *wp = SendBuffer;        // Queue Write Pointer
volatile char    *EndSendBuffer   = SendBuffer+sizeof(SendBuffer);
uint8_t           Error = 0;
uint32_t          NextStatusFlash = 0;
uint8_t           SPI_is_Active   = 1;
static uint32_t   LastSyncTime  = 0;
unsigned long     lastWriteSuccess = millis();
bool              communicationDetected = false;

//                                       States On    Off   On  Off  On   Off  On   Off
uint16_t RS232_Flash_Table[]         = { 2,     1500, 1500                                };
uint16_t NoCable_Flash_Table[]       = { 4,     50,   200,  50, 1000                      };
uint16_t SPI_Act_Flash_Table[]       = { 4,     500,  500, 500, 1500           };
uint16_t SPI_Deact_Flash_Table[]     = { 6,     500,  500, 500, 500, 500, 1500 };
uint16_t BufferFull_Flash_Table[]    = { 2,     250,  250                                 };
uint16_t PriorBuddFull_Flash_Table[] = { 2,     50,   450                                 };
uint16_t NoSSync_Flash_Table[]       = { 8,     0,    200,  50, 200, 50, 200 , 50,  800   };        // start with off to see 3 blinks also on trnasistion to NoSync
uint16_t NoSignal_Flash_Table[]      = { 2,     50,   450                                 };
uint8_t  FlashState = 1;
uint8_t LED_Arduino_signal_detected = 0;  // Activated if a signal from the LED Arduino is detected
                                          // Bit 1 is set if A1 = Low is detected
                                          // Bit 2 is set if an SPI interrupt is triggered
uint32_t Last_SPI_Signal            = 0;


#define SERIAL_BAUD      115200               // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program

#define FILTER_COUNT     1                    // Filter to eliminate noise. Disabled if set to 0.
                                              // 1: Changed byte must be repeated one time before it's send to the LED Arduino
                                              // The reaction time will be increased if a greater number is used.
                                              // 3: => reaction time 231ms - 308ms

SX20 sx;                                      // library

#define MAX_USED_CHANNEL_NUMBER   104

static byte oldSx[MAX_CHANNEL_NUMBER];

#if FILTER_COUNT > 0
  static byte newSx[MAX_CHANNEL_NUMBER], SXcnt[MAX_CHANNEL_NUMBER];
#endif

#define SERIAL_DISABLED  0
uint32_t DisableSerial = SERIAL_DISABLED; // Disable the serial port x seconds after the start to be able to program the LED-Arduino.
                                          // This is important because otherwise the receive pin of the LED-Arduino is blocked by this arduino ;-(
                                          // The serial port is enabled again to transmit DCC messages

#define sizemask    (QUEUESIZE-1)         // Bitmask for queue
#define QueueFill() ((sizemask + 1 + wp - rp) & sizemask)

uint8_t Use_RS232 = 1;                    // Flag which enables the RS232 to send the DCC states to the LED Arduino
void setLastSyncTime(uint32_t lastSyncTime)
{
  if ((millis() - lastSyncTime) > 1000)
  {
    LastSyncTime = 0;
    LED_Arduino_signal_detected &= ~2;
    Error = 0;              // don't show an error when SX signal comes back
  }
  else
  {
    LED_Arduino_signal_detected |= 2;
    LastSyncTime = lastSyncTime;
  }
}

#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0
  #define SPI_RCV_BUF_SIZE 10
  volatile char SPI_Rcv_Buff[SPI_RCV_BUF_SIZE];
  volatile char *SPI_Rcv_Ptr = SPI_Rcv_Buff;
  //---------------
  ISR(SPI_STC_vect)        // Interrput routine called if the master sends data
  //---------------
  {
    *SPI_Rcv_Ptr = SPDR;   // Store the value received from the master
    if (*SPI_Rcv_Ptr && *SPI_Rcv_Ptr < SPI_Rcv_Buff[SPI_RCV_BUF_SIZE]) SPI_Rcv_Ptr++; // Store the commands (Not used at the moment)
    if (*SPI_Rcv_Ptr == 6) // Check the communication
         SPDR = 0xF9;      // Answer is send with the next request
    else {
         if (QueueFill())  // Prepare the next character to be send
              {
              SPDR = *rp;
              rp++;
              if (rp >= EndSendBuffer) rp = SendBuffer;
              }
         else SPDR = 0;
         }
    setLastSyncTime(millis());
    LED_Arduino_signal_detected |= 2;
  }
#endif // USE_SPI_SLAVE

//---------------------------------
void AddToSendBuffer(const char *s)
//---------------------------------
// Attention this is called in the interrupt
{
    while (*s)
        {
        *wp++ = *s++;
        if (wp >= EndSendBuffer) wp = SendBuffer;
        if (wp == rp) if (Error < 16) Error += 2; // buffer overflow
        }
    if (DisableSerial != SERIAL_DISABLED) DisableSerial = millis() + 10 * QueueFill();
}

#include <stdarg.h>
#include <WString.h>
#define printf(Format, ...) printf_proc(F(Format), ##__VA_ARGS__)   // see: https://gcc.gnu.org/onlinedocs/cpp/Variadic-Macros.html
//-------------------------------------------------------
void printf_proc(const __FlashStringHelper *format, ...)
//-------------------------------------------------------
// Achtung: Es durfen keine Zeichen ueber die serielle Schnittstelle ausgegeben werden wenn der LED Arduino
//          programmiert wird und die Beiden ueber die TX Leitung verbunden sind. Die serielle Schnittstelle
//          muss nach jeder Ausgabe abgeschaltet werden. Sonst zieht der TX Ausgang dieses Nanos die
//          RX Leitung des LED Arduinos auf 5V.
//          Bei der normalen Kommunikation wird das ueber die A1 Leitung zwischen den beiden Rechnern gesteuert.
//          Wenn der SPI Mode aktiviert ist, dann wird die A1 Leitung als Input geschaltet damit sie auf dem
//          LED Arduino als Eingang fuer die Schalter genutzt werden kann.
{
  if (Use_RS232 || (millis() - Last_SPI_Signal < 100))
     {
     char buf[100];           // increase buffer size                                                         // 16.12.2022
     va_list ap;
     va_start(ap, format);
     #ifdef __AVR__
        vsnprintf_P(buf, sizeof(buf), (const char *)format, ap); // progmem for AVR
     #else
        vsnprintf  (buf, sizeof(buf), (const char *)format, ap); // for the rest of the world
     #endif
     va_end(ap);
     if (DisableSerial == SERIAL_DISABLED)
         {
         digitalWrite(BUF_GATE_PIN, 0); // enable the external buffer gate for the TX pin                     // 18.10.20:
         Serial.begin(SERIAL_BAUD);
         DisableSerial = millis() + 10 * strlen(buf);
         }
     Serial.print(buf);
     }
}
//---------------------------------
void Transmit_Sendchar_if_waiting()
//---------------------------------
// Must be called in the loop() function
{
    if (QueueFill() && digitalRead(SEND_DISABLE_PIN) == 0)
        {
        if (DisableSerial == SERIAL_DISABLED)
            {
            digitalWrite(BUF_GATE_PIN, 0); // enable the external buffer gate for the TX pin                  // 18.10.20:
            Serial.begin(SERIAL_BAUD);
            }
         DisableSerial = millis() + 10 * QueueFill();  // Disable the serial port after xx ms if it's not used to be able to flash the LED program
                                                       // which is connected to the TX pin of this Arduino

         if (Serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE - 1) // Add only one character to the serial buffer if it's total empty to prevent sending characters per interrupt
            {
            char c = *rp;
            Serial.print(c);
            rp++;
            if (rp >= EndSendBuffer) rp = SendBuffer;
            }
        }

    if (digitalRead(SEND_DISABLE_PIN) == 0)
      {
      setLastSyncTime(millis());
      LED_Arduino_signal_detected |= 1;
      } 
}

//-----------------------------------------------------
void Print2Buf(uint16_t Addr, uint8_t Dir, uint8_t Pow)
//-----------------------------------------------------
  {
	char s[20];
	sprintf(s, "@%4i %02X %02X\n", Addr, Dir, Pow?1:0);
	AddToSendBuffer(s);
  }

//-----------------------------------------
void printSXValue(int i,int data, byte old)
//-----------------------------------------
  {
      // One selectrix channel is mapped to 8 DCC adresses.
      // selectrix 1 => DCC  4-11
      //           2 => DCC 12-19
      // Only the "key pressed" signal is generated. It's not possible to detect the release
      uint16_t Addr   = i*8;
      uint8_t Changed = data ^ old;
      for (uint8_t ci = 0; ci < 8; ci++)
          {
          if (i >= 0 && i <= 99 && Changed & B00000001) Print2Buf(Addr, data & B00000001, 1);	// WM geaendert auf >= 0
          Addr++;
          data    >>= 1;
          Changed >>= 1;
          }
  }

#if FILTER_COUNT > 0
  //----------------------------------------------------------------------------------------------
  void CheckByte_with_filter(uint8_t i, uint8_t d, uint8_t &oldSx, uint8_t &newSx, uint8_t &SXcnt)
  //----------------------------------------------------------------------------------------------
  {
    if (oldSx != d)  // data have changed on SX bus
         {
         if (SXcnt++ == 0) // First time the change is detected
              newSx = d;   // Store the new value
         else {
              if (newSx == d) // Still the same byte ?
                   {
                   //Serial.print(millis()); Serial.print("SXcnt="); Serial.println(SXcnt);
                   if (SXcnt >= FILTER_COUNT)
                      {
                      printSXValue(i, d, oldSx);   // send new value to serial port
                      oldSx = d;
                      SXcnt = 0;
                      }
                   }
              else {
                   SXcnt = 1;
                   newSx = d;
                   }
              }
         }
    else SXcnt = 0;
  }
#else // Filter is disabled
  //--------------------------------------------------
  void CheckByte(uint8_t i, uint8_t d, uint8_t &oldSx)
  //--------------------------------------------------
  {
  if (oldSx != d)                                   // data have changed on SX bus
       {
       printSXValue(i, d, oldSx);                   // send new value to serial port
       oldSx = d;
       }
  }
#endif // FILTER_COUNT

void sxisr(void)
{
    // if you want to understand this, see:
    // http://www.arduino.cc/cgi-bin/yabb2/YaBB.pl?num=1239522239
    sx.isr();
}
#if USE_SPI_SLAVE                                                                                             // 13.05.20:
   //-----------------
   void Activate_SPI()
   //-----------------
   {
      SPDR = 0;              // First answer
      pinMode(MISO,OUTPUT);  // Sets MISO as OUTPUT (Have to Send data to Master IN)
      SPCR |= _BV(SPE);      // Turn on SPI in Slave Mode
      SPI.attachInterrupt(); // Interuupt ON is set for SPI commnucation
   }

   //-------------------
   void Deactivate_SPI()
   //-------------------
   {
      SPI.end();
      pinMode(12, INPUT);  // SPI SO pin
   }
#endif // USE_SPI_SLAVE

//-----------
void setup() {
//-----------
    pinMode(SCOPE,OUTPUT);

    digitalWrite(BUF_GATE_PIN, 0); // enable the external buffer gate for the TX pin                          // 18.10.20:
    pinMode(BUF_GATE_PIN, OUTPUT);

  // Attention: Don't use Serial.print in the program for debugging
  //            because the serial port has to be disabled after each usage
  //            to be able to falsh the LED Arduino. If the serial port of
  //            this Arduino is active the TX line is pulled to 5V. Since
  //            the TX line is connected to the RX line of the LED Arduino
  //            this generates problems.
    printf("\nSelectrix_Interface " SKETCH_VERSION "\n");            // This message is shown in the serial monitor of the
                                                                     // Arduino IDE if the serial port is working.
                                                                     // If the serial port of the LED-Arduino is shown this
                                                                     // message may be corrupted because the interrupts are
                                                                     // disabled if the RGB LEDs are updated.
                                                                     // The next message is only shown if the SEND_DISABLE signal
                                                                     // connected to ground by the LED-Arduino..
  #if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0
      printf("SPI Mode supported.\n");
      printf("Connect J13 and remove TX pin from DCC/Selectrix Nano\n");
      Activate_SPI();
  #endif
    for (int i=0; i<MAX_USED_CHANNEL_NUMBER; i++)
        {
        oldSx[i]=0;   // initialize to zero
        #if FILTER_COUNT > 0
          SXcnt[i]=0;
        #endif
        }
    DisableSerial = millis()+100; // Disable the serial port after the power on messages have been transmitted  // 05.05.20:
                                            // 4 DCC adr per channel        8 DCC adr per channel
    #if 0 // Debug                          // ~~~~~~~~~~~~~~~~~~~~~~~~     ~~~~~~~~~~~~~~~~~~~~~~~~~~
      printSXValue(1,B00000101, 0);         // Dir 0 pressed  addr 4, 5     Dir 1 pressed  addr  8, 10
      printSXValue(1,0,         B00000101); // Dir 0 released addr 4, 5     Dir 0 pressed  addr  8, 10
      printSXValue(1,B10100000, 0);         // Dir 1 pressed  addr 6, 7     Dir 1 pressed  addr 13, 15
      printSXValue(1,0,         B10100000); // Dir 1 released addr 6, 7     Dir 0 pressed  addr 13, 15
      printSXValue(1,B01000001, 0);         // Dir 0 pressed  addr 4 & 7    Dir 0 pressed  addr  8, 14
    #endif

    // initialize interrupt routine
    sx.init();                              // no scope trigger
                                            // RISING slope on INT0 triggers the interrupt routine sxisr (see above)
    attachInterrupt(0, sxisr, RISING);      // only the rising edge may be triggered
    AddToSendBuffer("Init Done\n"); // This message is send to the LED Arduino over RS232 or SPI (If the Arduino is already active)
    pinMode(SEND_DISABLE_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin. 
                                           // This is important to disable the communication while the LED Arduino is flashed
    pinMode(STATUSLED_PIN,   OUTPUT);
}

//---------------------------------
void Process_Status_and_Error_LED()                                                                           // 13.05.20:
//---------------------------------
{
  static uint16_t *Flash_Table_p     = RS232_Flash_Table;
  static uint16_t *Old_Flash_Table_p = RS232_Flash_Table;
  static uint32_t NextCheck = 0;

  uint32_t t = millis();
  if (t >= NextCheck) // Check the Status and the error every 100 ms
  {
    NextCheck = t + 100;
    if (Error) // Fast flash frequency if an buffer overflow was detected
    {
      if (Error > 1)
        Flash_Table_p = BufferFull_Flash_Table;
      else
        Flash_Table_p = PriorBuddFull_Flash_Table; // Prior error detected but not longer activ
      if (Error > 0 && LED_Arduino_signal_detected) Error--; // Decrement the error counter if the communication is working again
    }
    else
    {
      if (!communicationDetected)
      {
        Error = 0;              // don't show an error when signal comes back
        Flash_Table_p = NoSignal_Flash_Table;
      }
      else 
      {
        if ((millis() - LastSyncTime) > 1000)
        {
          LastSyncTime = 0;
          Error = 0;              // don't show an error when signal comes back
        }
        if (LastSyncTime == 0)   // no signal
        {
          Flash_Table_p = NoSSync_Flash_Table;
        }
        else
        {
          Flash_Table_p = RS232_Flash_Table;
        }
          #if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0
          else {
               if (SPI_is_Active)
                     Flash_Table_p = SPI_Act_Flash_Table;
               else  Flash_Table_p = SPI_Deact_Flash_Table;
               }
          #endif
      }
    }

    if (Old_Flash_Table_p != Flash_Table_p) // If the state has changed show it immediately
    {
      Old_Flash_Table_p = Flash_Table_p;
      NextStatusFlash = 0;
      FlashState = 1;
    }
    LED_Arduino_signal_detected = 0;
  }
  if (t >= NextStatusFlash) // Flash the status LED
  {
    if (FlashState > Flash_Table_p[0]) FlashState = 1;
    NextStatusFlash = t + Flash_Table_p[FlashState];
    if (Flash_Table_p[FlashState]>0)
      {
      digitalWrite(STATUSLED_PIN, STATUSLED_MOD(FlashState % 2));
      }
#if USE_SPI_SLAVE                                                                                        // 15.05.20:
    if (!Use_RS232)
      { // SPI Mode is active
      if (DisableSerial == SERIAL_DISABLED) // Flash also the RX LED if the SPI mode is used in case the user has no LED connected to pin D8
        {
        pinMode(RX_PIN, OUTPUT);
        digitalWrite(RX_PIN, !(FlashState % 2));
        }
      }
#endif
    FlashState++;
  }
}

#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                           // 15.05.20:
  //-------------------------
  void SPI_Sleep_and_Wakeup()
  //-------------------------
  {
    if (LED_Arduino_signal_detected & 2)
       {
       Use_RS232 = 0;               // Disable the RS232 communication
       Last_SPI_Signal = millis();
       }

    if (SPI_is_Active)
         {
         if (millis() - Last_SPI_Signal > 3000)
            {
            Deactivate_SPI();
            //Problem: printf Ausgaben duerfen nicht kommen beim Upload
            // printf("Slave disabled SPI because no signals reveived\n");
            SPI_is_Active = 0;
            }
         }
    else {
         if (digitalRead(13) == 1)
            { // enable the SPI again
            Last_SPI_Signal = millis();
            Activate_SPI();
            //Problem: printf Ausgaben duerfen nicht kommen beim Upload
            //printf("Slave reactivating SPI (D13=High)\n");
            SPI_is_Active = 1;
            }
         }
  }
#endif

//-----------
void loop() {
//-----------
  // check selectrix channels for changes
  uint32_t LastT = millis();
  for (int i=0; i<MAX_USED_CHANNEL_NUMBER; i++)
  {
      byte d=sx.readChannel(i);
      #if FILTER_COUNT > 0
         CheckByte_with_filter(i, d, oldSx[i], newSx[i], SXcnt[i]);
      #else
         CheckByte(i, d, oldSx[i]);
      #endif
  }
  if (sx.writing()==0)
  {
    communicationDetected = true;
    lastWriteSuccess = millis();
    sx.ClearBit(0,0);
  }
  while (millis()-LastT < 77) {
  if (Use_RS232) Transmit_Sendchar_if_waiting();

  #if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                           // 15.05.20:
    static uint8_t Show_Message_Once = 1;
    if (Use_RS232 == 0 && Show_Message_Once)
       {
       Show_Message_Once = 0; // Show only once
       printf("Slave disabled serial communication\n");
       pinMode(SEND_DISABLE_PIN, INPUT);
       }

    SPI_Sleep_and_Wakeup();
  #endif
  }
  
  if ((millis()-lastWriteSuccess)>1000)
  {
    communicationDetected = false;
  }
  Process_Status_and_Error_LED();                                                                             // 13.05.20:
  // Disable the serial port if it's not used for a while
  if (DisableSerial != SERIAL_DISABLED && millis() > DisableSerial)                                           // 13.05.20:
     {
     digitalWrite(BUF_GATE_PIN, 1); // disable the external buffer gate for the TX pin                        // 18.10.20:
     Serial.end(); // disable the serial port to be able to flash the LED-Arduino.
     DisableSerial = SERIAL_DISABLED;
     }
}
