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
 */
/*
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
*/

#define SKETCH_VERSION "1.4"

#define DCC_SIGNAL_PIN   2
#define SEND_DISABLE_PIN A1
#define STATUSLED_PIN    LED_BUILTIN
#define BUF_GATE_PIN     A5                                                                                   // 18.10.20:

#include "SX20.h"                             // this is the Selectrix library (We use a modified version from Wilfried which is stored localy)
#define QUEUESIZE  512                        // Must be a "binary" number 32,64,128, ...
char     SendBuffer[QUEUESIZE];
char    *rp = SendBuffer;                     // Queue Read Pointer
char    *wp = SendBuffer;                     // Queue Write Pointer
char    *EndSendBuffer = SendBuffer+sizeof(SendBuffer);
uint8_t  Error = 0;
uint32_t NextStatusFlash = 0;
#define SERIAL_BAUD      115200               // Should be equal to the DCC_Rail_Decoder_Transmitter.ino program

#define FILTER_COUNT     1                    // Filter to eliminate noise. Disabled if set to 0.
                                              // 1: Changed byte must be repeated one tine before it's send to the LED Arduino
                                              // The reaction time will be increased if a greater number is used.
                                              // 3: => reaction time 231ms - 308ms

SX20 sx;                                      // library

#define MAX_USED_CHANNEL_NUMBER   104

static byte oldSx[MAX_CHANNEL_NUMBER];

#if FILTER_COUNT > 0
  static byte newSx[MAX_CHANNEL_NUMBER], SXcnt[MAX_CHANNEL_NUMBER];
#endif

#define SERIAL_DISABLED  0
uint32_t DisableSerial = SERIAL_DISABLED; // Disable the serial port 1000 seconds after the start to be able to program the LED-Arduino.
                                          // This is important because otherwise the receive pin of the LED-Arduino is blocked by this arduino ;-(
                                          // The serial port is enabled again to transmit DCC messages

#define sizemask    (QUEUESIZE-1)         // Bitmask for queue
#define QueueFill() ((sizemask + 1 + wp - rp) & sizemask)


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
            DisableSerial = millis() + 10 * QueueFill();
            }
        if (Serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE - 1)
            {
            char c = *rp;
            Serial.print(c);
            rp++;
            if (rp >= EndSendBuffer) rp = SendBuffer;
            }
        }
    else if (DisableSerial != SERIAL_DISABLED && millis() > DisableSerial)
        {
        Serial.end(); // disable the serial port to be able to flash the LED-Arduino.
        digitalWrite(BUF_GATE_PIN, 1); // disable the external buffer gate for the TX pin                     // 18.10.20:
        DisableSerial = SERIAL_DISABLED;
        }

    static bool Send_Disable_Low_detected = false;
    if (digitalRead(SEND_DISABLE_PIN) == 0) Send_Disable_Low_detected = true;

    uint32_t t = millis();
    if (t >= NextStatusFlash) // Flash the status LED
       {
       if (Error) // Fast flash frequency if an buffer overflow was detected
            {
            if (Error > 1)
                NextStatusFlash = t + 250;  // Error still active
            else {                          // Prior error detected but not longer activ
                 if (digitalRead(STATUSLED_PIN))
                     NextStatusFlash = t + 450;
                 else NextStatusFlash = t + 50; // Short flash
                 }
            if (Error > 1 && Send_Disable_Low_detected) Error--;
            Send_Disable_Low_detected = false;
            }
       else {
            NextStatusFlash = t + 1500;
            }
       digitalWrite(STATUSLED_PIN, !digitalRead(STATUSLED_PIN));
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

//----------
void setup()
//----------
{
    digitalWrite(BUF_GATE_PIN, 0); // enable the external buffer gate for the TX pin                          // 18.10.20:
    pinMode(BUF_GATE_PIN, OUTPUT);

    pinMode(SCOPE,OUTPUT);
    pinMode(SEND_DISABLE_PIN, INPUT_PULLUP);                    // Activate an internal pullup resistor for the input pin
    pinMode(STATUSLED_PIN,   OUTPUT);
    Serial.begin(SERIAL_BAUD);                                  // open the serial port
    //Serial.println(F("\nSelectrix_Transmitter serial test")); // This message is shown in the serial monitor of the
    Serial.println(SKETCH_VERSION);                             // Arduino IDE if the serial port is working.
    delay(100);                                                 // If the serial port of the LED-Arduino is shown this
                                                                // message may be corrupted because the interrupts are
                                                                // disabled if the RGB LEDs are updated.
                                                                // The next message is only shown if the SEND_DISABLE signal
                                                                // connected to ground by the LED-Arduino..

    AddToSendBuffer((char*)"\nSelectrix_Interface\n");          // Don't use Serial.print because characters get lost if the LED-Arduinio updates the LEDs
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
}

//---------
void loop()
//---------
{
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
  while (millis()-LastT < 77)   Transmit_Sendchar_if_waiting();
}
