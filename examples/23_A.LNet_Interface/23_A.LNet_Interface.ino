// Disable the warning:                                                                                       // 26.12.19:
//   ... warning: 'EEPROM' defined but not used [-Wunused-variable]
//   static EEPROMClass EEPROM;
// and:
//   ... warning: comparison between signed and unsigned integer expressions [-Wsign-compare]
//
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"
#pragma GCC diagnostic pop

//#define DEBUG_LNET_PROCESS

/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2022  Hardi Stengelin: MobaLedLib@gmx.de
 Copyright (C) 2022 - 2023  Juergen Winkler: MobaLedLib@gmx.at

 This library is free software; you can redistribute it and/or
 modify it under the terms of the GNU Lesser General Public
 License as published by the Free Software Foundation; either
 version 2.1 of the License, or (at your option) any later version.

 This library is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 Lesser General Public License for more details.

 You should have received a copy of the GNU Lesser General Public
 License along with this library; if not, write to the Free Software
 Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 -------------------------------------------------------------------------------------------------------------


 Loconet(TM) decoder and transmitter to LED Arduino
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes how to read Loconet(TM) signals of a model railway and transmit the switch
 commands to a second Arduino which runs the MobaLedLib. On the second Arduino two light signal
 are controlled with the received messages.

 The Loconet(TM) Protocol is (C) by digitrax. This example is developed using the Loconet library,
 which doesn't contain a licensce for Loconet(TM).

 This example is provided solely for non-commercial private use by Digitrax customers.
 see https://www.digitrax.com/static/apps/cms/media/documents/loconet/loconetpersonaledition.pdf

 This program is the intended for the Communication-Arduino.

 The Loconet(TM) decoder can't be used on the same Arduino where the LEDs are connected because the
 interrupts are blocked while the LEDs are updated. During this time Loconet(TM)(TM) commands get lost.
 Therefore two Arduinos are used. One to collect the data, the other to control the LEDs.
 This program is used in the Communication-Arduino which reads the Loconet(TM) signals.

 The example is based on the example LoconetMonitor.ino from the Loconet library.

 The data from the Communication-Arduino are send to the LED-Arduino via the serial port. This port is
 also used to flash the Adruinos with an build in USB to serial converter. Therefore normally
 the connection between the two Arduinos have to be removed to flash the Arduino. To avoid
 this the serial port on the sending Arduino is disabled if no Loconet(TM) commands have to be
 transmitted. The second Arduino may not be flashed if the first arduino is not programmed
 with this program. The problem will only occurre if an old program is loaded to the Communication-Arduino
 which uses the serial port. Normally you have to flash the program on the Communication-Arduino only once.

 The A1 line is used to pause the transmitting of signals from the Communication-Arduino to the LED-Arduino
 while the LED-Arduino updates the LEDs. Loconet(TM) commands which are received during this time are
 stored in the Communication-Arduino and send after the SEND_DISABLE wire is low.
 If the connection between the two Arduinos is broken or if the LED-Arduino disables the
 SEND_DISABLE line to long the buffer to store the Loconet(TM) commands will overflow. This is
 indicated by the internal LED which is flashing fast (250 ms on, 250 ms off). It will
 generate a short flash (50 ms on, 450 ms off) if an buffer overflow occurred before, but
 now the SEND_DISABLE line is working again.

 Testing / Debugging:
 ~~~~~~~~~~~~~~~~~~~~

 Video:
 ~~~~~~
 This video demonstrates the example:
   https://vimeo.com/311996452          (Only the Light Signals are shown in the video)

 Extras:
 ~~~~~~~
 The MobaLedLib could be used without any programming experience by using excel sheets which are
 located in the directory extras. With the help of the program "Prog_Generator_MobaLedLib.xlsm"
 all effects could be used very comfortable.

 In the Wiki you find any information:
   https://wiki.mobaledlib.de/doku.php

 Hardware:    //TODO
 ~~~~~~~~~
 The example can be used with two Arduino compatible boards (Uno, Nano, Mega, ...),
 a WS2812 LED stripe and an opto coppler (6N137). In addition a diode (1n148) and a 1K
 resistor is needed. The two Arduinos are connected by three wires (TX1-RX1, GND, A1).

 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).

                                                 .----------------.
                                                 |     +5V        |
  Communication-Arduino:   +-----+               |      | | | |   |
              +------------| USB |------------+  |    .---------. |
              |            +-----+            |  |    | 8   LM  | |
              | [ ]D13/SCK        MISO/D12[ ] |  |    | 1   393 | |            1
 SEND_DISABLE | [ ]3.3V           MOSI/D11[ ]~|  |    '---------' |           O
 LINE |       | [ ]V.ref     ___    SS/D10[ ]~|  |      | | | |   |           O
      v       | [ ]A0       / N \       D9[ ]~|  |      | | | GND |       .---O  LOCO
    .---------| [ ]A1      /  A  \      D8[ ] |--+------' | +-----+-[47K]-+---O  -NET
    |         | [ ]A2      \  N  /      D7[ ] |           | '-[150K]-.        O
    |         | [ ]A3       \_O_/       D6[ ]~|           |          |        O
    |         | [ ]A4/SDA               D5[ ]~| +5V-[10K]-+--[22K]--GND        6
    |         | [ ]A5/SCL               D4[ ] |
    |    +5V  | [ ]A6              INT1/D3[ ]~|-HB LED | with 470 ohm resistor (Opt.)
    |     ^   | [ ]A7              INT0/D2[ ] |
    |     '---| [ ]5V                  GND[ ] |
    |         | [ ]RST                 RST[ ] |
    |  .-GND--| [ ]GND   5V MOSI GND   TX1[ ] |--------.
    |  |      | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |        |
    |  |      |          [ ] [ ] [ ]          |        |
    |  |      |          MISO SCK RST         |        |
    |  |      | NANO-V3                       |        |
    |  |      +-------------------------------+        |
    |  |                                               |
    |  |                                               |
    |  | LED-Arduino Nano: +-----+                     |
    |  |      +------------| USB |------------+        |
    |  |      |            +-----+            |        |
    |  |      | [ ]D13/SCK        MISO/D12[ ] |        |
    |  |      | [ ]3.3V           MOSI/D11[ ]~|        |
    |  |      | [ ]V.ref     ___    SS/D10[ ]~|        |  <== This second program must be send
    |  |      | [ ]A0       / N \       D9[ ]~|        |      to the Communication-Arduino
    '---------| [ ]A1      /  A  \      D8[ ] |        |      (23_B.DCC_Rail_Decoder_Receiver.ino)
       |      | [ ]A2      \  N  /      D7[ ] |        |
       |      | [ ]A3       \_O_/       D6[ ]~|---LEDs | WS281x
       |      | [ ]A4/SDA               D5[ ]~|        |
       |      | [ ]A5/SCL               D4[ ] |        |
       | +5V  | [ ]A6              INT1/D3[ ]~|        |
       |  ^   | [ ]A7              INT0/D2[ ] |        |
       |  '---| [ ]5V                  GND[ ] |        |
       |      | [ ]RST                 RST[ ] |        |
       '-GND--| [ ]GND   5V MOSI GND   TX1[ ] |        |
              | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |--------*----[3.9K]---,   Prevent problems flashing the Nano
              |          [ ] [ ] [ ]          |                      |   (See: https://www.stummiforum.de/viewtopic.php?f=7&t=165060&sd=a&start=2307)
              |          MISO SCK RST         |                     GND
              | NANO-V3                       |
              +-------------------------------+

  The +5V supply lines of both arduinos could be connected together if they are powerd from one power supply (one USB).

 Documents:
  ~~~~~~~~~~
 - https://www.digitrax.com/static/apps/cms/media/documents/loconet/loconetpersonaledition.pdf
 - https://www.opendcc.de/info/dcc/dcc.html
 - https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
 - https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
*/
/*
Revision History :
~~~~~~~~~~~~~~~~~
11.12.22:  Versions 0.9 (Juergen)
  - port DCC Arduino to LNet Arduino
14.04.23:  Versions 1.0 (Juergen)
  - improve functionality
23.04.23:  Versions 1.1 (Juergen)
  - add cable plug detection
18.05.23:  Versions 1.2 (Juergen)
  - improve code, add define for transmit pin
*/

#define SKETCH_VERSION "1.2"

#include "23_A.LNet_Interface.h" // Is written by the Excel Tool Program_Generator to set compiler switches

#include "LocoNet.h"

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

#ifndef LN_SIGNAL_DETECTION_SECONDS
#define LN_SIGNAL_DETECTION_SECONDS 60                                                                        // set to 60 seconds by default
#endif
#ifndef LN_TX_PIN
#define LN_TX_PIN     7		                                                                                  // LNet Arduino
#endif


#define QUEUESIZE  128                    // Must be a "binary" number 32,64,128, ...
volatile char     SendBuffer[QUEUESIZE];
volatile char    *rp = SendBuffer;        // Queue Read Pointer
volatile char    *wp = SendBuffer;        // Queue Write Pointer
volatile char    *EndSendBuffer   = SendBuffer+sizeof(SendBuffer);
uint8_t           Error           = 0;
uint32_t          NextStatusFlash = 0;
uint8_t           SPI_is_Active   = 1;
static uint32_t   LastSyncTime  = 0;
static uint32_t   lastHighTime = millis();
static bool       cablePlugged;
static bool       locoMsgTimeout;
static uint32_t   LastMsgTime = 0;
static lnMsg*     LnPacket;

//                                       States On    Off   On  Off  On   Off
uint16_t RS232_Flash_Table[]         = { 2,     1500, 1500                     };
uint16_t SPI_Act_Flash_Table[]       = { 4,     500,  500, 500, 1500           };
uint16_t SPI_Deact_Flash_Table[]     = { 6,     500,  500, 500, 500, 500, 1500 };
uint16_t BufferFull_Flash_Table[]    = { 2,     250,  250                      };
uint16_t PriorBuddFull_Flash_Table[] = { 2,     50,   450                      };
uint16_t NoSSync_Flash_Table[]       = { 8,     0,    200,  50, 200, 50, 200 , 50,  800   };        // start with off to see 3 blinks also on trnasistion to NoSync
uint16_t NoCable_Flash_Table[]       = { 4,     50,  200,  50, 1000            };
uint16_t NoSignal_Flash_Table[]      = { 2,     50,   450                      };
uint8_t  FlashState = 1;

uint8_t LED_Arduino_signal_detected = 0;  // Activated if a signal from the LED Arduino is detected
                                          // Bit 1 is set if A1 = Low is detected
                                          // Bit 2 is set if an SPI interrupt is triggered
uint32_t Last_SPI_Signal            = 0;


#define SERIAL_BAUD      115200           // Should be equal to the LNet_Decoder_Receiver.ino program
#define SERIAL_DISABLED  0
uint32_t DisableSerial = SERIAL_DISABLED; // Disable the serial port x seconds after the start to be able to program the LED-Arduino.
                                          // This is important because otherwise the receive pin of the LED-Arduino is blocked by this arduino ;-(
                                          // The serial port is enabled again to transmit LNet messages

#define sizemask    (QUEUESIZE-1)         // Bitmask for queue
#define QueueFill() ((sizemask + 1 + wp - rp) & sizemask)

uint8_t Use_RS232 = 1;                    // Flag which enables the RS232 to send the LNet states to the LED Arduino
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

#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                           // 15.05.20:
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
void AddToSendBuffer(const char* s)
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
void printf_proc(const __FlashStringHelper* format, ...)
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
    vsnprintf_P(buf, sizeof(buf), (const char*)format, ap); // progmem for AVR
#else
    vsnprintf(buf, sizeof(buf), (const char*)format, ap); // for the rest of the world
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
      digitalWrite(BUF_GATE_PIN, 0); // enable the external buffer gate for the TX pin                 // 18.10.20:
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

// This call-back function is called from LocoNet.processSwitchSensorMessage
// for all Switch Request messages
void notifySwitchRequest(uint16_t Address, uint8_t Output, uint8_t Direction)
{
  char s[20];
  sprintf(s, "@%4i %02X %02X\n", Address, Direction, Output);
  AddToSendBuffer(s);
#ifdef DEBUG_LNET_PROCESS
  printf("notifySwitchRequest: %u %s %s\n", Address, Direction ? "Closed" : "Thrown", Output ? "On" : "Off");
#endif
}

#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                           // 15.05.20:
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
  pinMode(13, INPUT); // IF the D13 pins are connected together they must be used as inputs. The pin is activated as OUTPUT in the boot loader ?!?

  digitalWrite(BUF_GATE_PIN, 0); // enable the external buffer gate for the TX pin                            // 18.10.20:
  pinMode(BUF_GATE_PIN, OUTPUT);

  // Attention: Don't use Serial.print in the program for debugging
  //            because the serial port has to be disabled after each usage
  //            to be able to falsh the LED Arduino. If the serial port of
  //            this Arduino is active the TX line is pulled to 5V. Since
  //            the TX line is connected to the RX line of the LED Arduino
  //            this generates problems.
  printf("\nLNet_Decoder Serial Version " SKETCH_VERSION "\n");      // This message is shown in the serial monitor of the
                                                                     // Arduino IDE if the serial port is working.
                                                                     // If the serial port of the LED-Arduino is shown this
                                                                     // message may be corrupted because the interrupts are
                                                                     // disabled if the RGB LEDs are updated.
                                                                     // The next message is only shown if the SEND_DISABLE signal
                                                                     // connected to ground by the LED-Arduino..
#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                           // 15.05.20:
      printf("SPI Mode supported.\n");
      printf("Connect J13 and remove TX pin from DCC/Selectrix Nano\n");
      Activate_SPI();
#endif

  cablePlugged = true;
  locoMsgTimeout = false;

  // First initialize the LocoNet interface
  LocoNet.init(LN_TX_PIN);

  printf("Init Done\n"); // This message is send to the LED Arduino over RS232 or SPI (If the Arduino is already active)

  pinMode(SEND_DISABLE_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin. 
                                           // This is importand to disable the communication while the LED Arduino is flashed
  pinMode(STATUSLED_PIN, OUTPUT);

#ifdef DEBUG_LNET_PROCESS
  printf("Timeout %d\n", LNET_SIGNAL_DETECTION_SECONDS);
#endif

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
      if (!cablePlugged)
      {
        Error = 0;              // don't show an error when signal comes back
        Flash_Table_p = NoCable_Flash_Table;
        locoMsgTimeout = true;  // after cable is reconnected stay in "NoMessage" until next Loco telegram received
      }
      else if (locoMsgTimeout)
      {
        Error = 0;              // don't show an error when signal comes back
        Flash_Table_p = NoSignal_Flash_Table;
      }
      else if (Use_RS232)
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
#if defined(USE_SPI_SLAVE) && USE_SPI_SLAVE>0                                                                                           // 15.05.20:
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
void loop(){
//-----------

    // Check for any received LNet packets
  LnPacket = LocoNet.receive();
  if (LnPacket) {
#ifdef DEBUG_LNET_PROCESS
    if (locoMsgTimeout) printf("signal restored\n");
#endif      
    locoMsgTimeout = false;
    LastMsgTime = millis();

#ifdef DEBUG_LNET_PROCESS
    // First print out the packet in HEX
    printf("RX: ");
    uint8_t msgLen = getLnMsgSize(LnPacket);
    for (uint8_t x = 0; x < msgLen; x++)
    {
      uint8_t val = LnPacket->data[x];
      // Print a leading 0 if less than 16 to make 2 HEX digits
      if (val < 16)
        printf("0");

      printf("%X ", val);
    }
    printf("\n");
#endif
    LocoNet.processSwitchSensorMessage(LnPacket);
  }

  if (digitalRead(8) == LOW)
  {
    if (cablePlugged && millis() - lastHighTime > 1000)
    {
      cablePlugged = false;
#ifdef DEBUG_LNET_PROCESS
      printf("cable unplugged\n");
#endif      
    }
  }
  else
  {
    lastHighTime = millis();
    if (!cablePlugged)
    {
      cablePlugged = true;
#ifdef DEBUG_LNET_PROCESS
      printf("cable plugged in\n");
#endif        
    }
  }

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

  if ((millis() - LastMsgTime) > (uint32_t)LN_SIGNAL_DETECTION_SECONDS * 1000)
  {
#ifdef DEBUG_LNET_PROCESS
    if (!locoMsgTimeout) printf("signal lost\n");
#endif      
    locoMsgTimeout = true;
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