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
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de

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


 DCC rail signal decoder and transmitter to LED Arduino                                    by Hardi   19.01.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 This example describes how to read DCC signals (Digital Command Control) from the rail signal
 of a model railway and transmit the accessory and the signalling commands to a second Arduino
 which runs the MobaLedLib. On the second Arduino two light signal are controlled with the
 received messages. In addition two houses and a welding simulation could be activated with
 DCC commands.

 This program is the part for the DCC-Arduino.

 The DCC decoder can't be used on the same Arduino where the LEDs are connected because the
 interrupts are blocked while the LEDs are updated. During this time DCC commands get lost.
 Therefore two Arduinos are used. One to collect the data, the other to control the LEDs.
 This program is used in the DCC-Arduino which reads the DCC signals.

 The example is based on the example NmraDccAccessoryDecoder_1.ino from the NmraDcc library.

 The data from the DCC-Arduino are send to the LED-Arduino via the serial port. This port is
 also used to flash the Adruinos with an build in USB to serial converter. Therefore normally
 the connection between the two Arduinos have to be removed to flash the Arduino. To avoid
 this the serial port on the sending Arduino is disabled if no DCC commands have to be
 transmitted. The second Arduino may not be flashed if the first arduino is not programmed
 with this program. The problem will only occurre if an old program is loaded to the DCC-Arduino
 which uses the serial port. Normally you have to flash the program on the DCC-Arduino only once.

 The A1 line is used to pause the transmitting of signals from the DCC-Arduino to the LED-Arduino
 while the LED-Arduino updates the LEDs. DCC commands which are received during this time are
 stored in the DCC-Arduino and send after the SEND_DISABLE wire is low.
 If the connection between the two Arduinos is broken or if the LED-Arduino disables the
 SEND_DISABLE line to long the buffer to store the DCC commands will overflow. This is
 indicated by the internal LED which is flashing fast (250 ms on, 250 ms off). It will
 generate a short flash (50 ms on, 450 ms off) if an buffer overflow occurred before, but
 now the SEND_DISABLE line is working again.

 Testing / Debugging:
 ~~~~~~~~~~~~~~~~~~~~
 If it doesn't work check the following:
 - The railway commander must generate DCC signals (The MS2 for instance is configured to use the
   MM protocol for the switches.
 - The TX LED of the DCC arduino should generate a very short flash if DCC commands are received
   If not the SEND_DISABLE line may be broken. In this case the res internal LED (L) of the Arduino
   will flash after a view DCC commands.
   The serial monitor of the Arduino IDE could be used to check if messages are send. But this
   will only work if the SEND_DISABLE is low. The baud rate must be set to 115200.
 - If the connection between the Adruinos is working the RX LED of the LED-Arduino generates two
   short flashes if a command from the DCC-Arduino is received.

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

 Hardware:
 ~~~~~~~~~
 The example can be used with two Arduino compatible boards (Uno, Nano, Mega, ...),
 a WS2812 LED stripe and an opto coppler (6N137). In addition a diode (1n148) and a 1K
 resistor is needed. The two Arduinos are connected by three wires (TX1-RX1, GND, A1).

 The DIN pin of the first LED is connected to pin D6 (LED_DO_PIN).
 An optional heartbeat LED could be connected to pin D3 togeter with a 470 ohm resistor.

                       .-----------------------------------------------------.
                       |                                                     |
                       |       DCC-Arduino Nano: +-----+                     |
                       |            +------------| USB |------------+        |
                   +5V |            |            +-----+            |        |
                    ^  |            | [ ]D13/SCK        MISO/D12[ ] |        |
            ______  |  |            | [ ]3.3V           MOSI/D11[ ]~|        |
           |1     |-'  |            | [ ]V.ref     ___    SS/D10[ ]~|        |
 ---[1K]---|      |    |            | [ ]A0       / N \       D9[ ]~|        |
 t      .--|      |----'  .---------| [ ]A1      /  A  \      D8[ ] |        |  <== This program must be send
 o      |  |______|-.     |         | [ ]A2      \  N  /      D7[ ] |        |      to the DCC-Arduino at first
        |   6N137   |     |         | [ ]A3       \_0_/       D6[ ]~|        |      (23_A.DCC_Rail_Decoder_Transmitter.ino)
 r      |          GND    |         | [ ]A4/SDA               D5[ ]~|        |
 a     \/ 1N148           |         | [ ]A5/SCL               D4[ ] |        |
 i     --                 |    +5V  | [ ]A6              INT1/D3[ ]~|-HB LED | with 470 ohm resistor (Opt.)
 l      |                 |     ^   | [ ]A7              INT0/D2[ ] |--------'
 -------'                 |     '---| [ ]5V                  GND[ ] |
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
                          |  |      | [ ]A0       / N \       D9[ ]~|        |      to the LED-Arduino
          SEND_DISABLE -> '---------| [ ]A1      /  A  \      D8[ ] |        |      (23_B.DCC_Rail_Decoder_Receiver.ino)
          line               |      | [ ]A2      \  N  /      D7[ ] |        |
                             |      | [ ]A3       \_0_/       D6[ ]~|---LEDs | WS281x
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
 - https://www.opendcc.de/info/dcc/dcc.html
 - https://www.nmra.org/sites/default/files/s-9.2.1_2012_07.pdf
 - https://www.nmra.org/sites/default/files/s-92-2004-07.pdf
*/
/*
Revision History :
~~~~~~~~~~~~~~~~~
15.04.20:  Versions 1.1 (Juergen)
           - don't open serial port on receive of DCC packet, reopen only if sending is allowed
             (avoid breaking sketch upload process of led arduino, see https://www.stummiforum.de/viewtopic.php?f=7&t=165060&p=2103546#p2103546)
           - add output of version numer on program startup
05.05.20:  Versions 1.2
           - The serial port was not disabled at startup
             If DCC messages have been received and send to the LED Arduino it was disabled
             This generates problems when flashing the LED Arduino.
           - The build in LED also shows a slow heartbeat with a period of 3 seconds if it's running normal
             The buffer overflow signaling is not changed:
             - 1 Hz 50%: active buffer owerflow 1 Hz
             - 1 Hz 10%: prior buffer owerflow, but now it's working fine
13.05.20:  - Started the SPI interface
           - New, more flexible status LED function
           - printf() function
17.05.20:  - The SPI communication is no longer necessary since the TX LED Problem could also be
             solved wit a 3.9K resistor between TX and ground.
             The SPI functions are kept in the software but the are normally disabled with USE_SPI_SLAVE 0
           Versions 1.3
18.10.20:  - Switching the external buffer gate which disables the TX pin in PCB version 1.7 (Not tested)
           Versions 1.4
18.12.21:  - Add DCC Signal detection on Signal loss indication
           Versions 1.5

*/
#define SKETCH_VERSION "1.5"

#include "MobaLedLib.h"

#include "23_A.DCC_Interface.h" // Is written by the Excel Tool Program_Generator to set compiler switches

#if USE_SPI_SLAVE                                                                                             // 13.05.20:
  #include<SPI.h>
  #define STATUSLED_PIN  8
  #define STATUSLED_MOD  !  // LED is connected to GND => It has to be inverted
  #define RX_PIN         0
#else
  #define STATUSLED_PIN  LED_BUILTIN
  #define STATUSLED_MOD
#endif

#define DCC_SIGNAL_PIN   2
#define SEND_DISABLE_PIN A1
#define HEARTBEAT_PIN    3
#define BUF_GATE_PIN     A5                                                                                   // 18.10.20:

NmraDcc  Dcc ;                                // Instance of the NmraDcc class

LED_Heartbeat_C LED_HeartBeat(HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.


#define QUEUESIZE  128                    // Must be a "binary" number 32,64,128, ...
volatile char     SendBuffer[QUEUESIZE];
volatile char    *rp = SendBuffer;        // Queue Read Pointer
volatile char    *wp = SendBuffer;        // Queue Write Pointer
volatile char    *EndSendBuffer   = SendBuffer+sizeof(SendBuffer);
uint8_t           Error           = 0;
uint32_t          NextStatusFlash = 0;
uint8_t           SPI_is_Active   = 1;
static uint32_t   LastSignalTime  = 0;

//                                       States On    Off   On  Off  On   Off
uint16_t RS232_Flash_Table[]         = { 2,     1500, 1500                     };
uint16_t SPI_Act_Flash_Table[]       = { 4,     500,  500, 500, 1500           };
uint16_t SPI_Deact_Flash_Table[]     = { 6,     500,  500, 500, 500, 500, 1500 };
uint16_t BufferFull_Flash_Table[]    = { 2,     250,  250                      };
uint16_t PriorBuddFull_Flash_Table[] = { 2,     50,   450                      };
uint16_t NoSignal_Flash_Table[]      = { 2,     50,   450                      };
uint8_t  FlashState = 1;

uint8_t LED_Arduino_signal_detected = 0;  // Activated if a signal from the LED Arduino is detected
                                          // Bit 1 is set if A1 = Low is detected
                                          // Bit 2 is set if an SPI interrupt is triggered
uint32_t Last_SPI_Signal            = 0;


#define SERIAL_BAUD      115200           // Should be equal to the DCC_Rail_Decoder_Receiver.ino program
#define SERIAL_DISABLED  0
uint32_t DisableSerial = SERIAL_DISABLED; // Disable the serial port x seconds after the start to be able to program the LED-Arduino.
                                          // This is important because otherwise the receive pin of the LED-Arduino is blocked by this arduino ;-(
                                          // The serial port is enabled again to transmit DCC messages

#define sizemask    (QUEUESIZE-1)         // Bitmask for queue
#define QueueFill() ((sizemask + 1 + wp - rp) & sizemask)

uint8_t Use_RS232 = 1;                    // Flag which enables the RS232 to send the DCC states to the LED Arduino
void setLastSignalTime(uint32_t lastSignalTime) 
{
  if ((millis()-lastSignalTime) > 1000)
  {
    LastSignalTime = 0;
    LED_Arduino_signal_detected &= ~2;
    Error = 0;              // don't show an error when SX signal comes back
  }
  else
  {
    LED_Arduino_signal_detected |= 2;
    LastSignalTime = lastSignalTime;
  }
}

#if USE_SPI_SLAVE
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
    setLastSignalTime(millis());
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
     char buf[50];
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
    if (digitalRead(SEND_DISABLE_PIN) == 0) LED_Arduino_signal_detected |= 1;
}

//-------------------------------------------------------------------------------------
void notifyDccAccTurnoutOutput( uint16_t Addr, uint8_t Direction, uint8_t OutputPower )
//-------------------------------------------------------------------------------------
// This function is called whenever a normal DCC Turnout Packet is received
{
  //if (!OutputPower) return ; // debug: Simulate the Lenz LZV100 behavior which doesn't send the button release signal

  char s[20];
  sprintf(s, "@%4i %02X %02X\n", Addr, Direction, OutputPower);
  AddToSendBuffer(s);
  // printf("%4i notifyDccAccTurnoutOutput: %i, %i, %02X\n", millis(), Addr, Direction, OutputPower);
}

//---------------------------------------------------------
void notifyDccSigOutputState( uint16_t Addr, uint8_t State)
//---------------------------------------------------------
// notifyDccSigOutputState() Callback for a signal aspect accessory decoder.
//                      Defined in S-9.2.1 as the Extended Accessory Decoder Control Packet.
{
  char s[20];
  sprintf(s, "$%4i %02X\n", Addr, State); // Bei der CAN Geschichte hab ich herausgefunden dass es 2048 Adressen gibt. Ich hoffe das stimmt...
  AddToSendBuffer(s);
  // printf("notifyDccSigState: %i,%02X\n", Addr, State) ;
}

//---------------------------------------------------------
void notifyDccMsg( DCC_MSG * Msg )
//---------------------------------------------------------
{
  setLastSignalTime(millis());
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
void setup(){
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
  printf("\nDCC_Rail_Decoder Serial Version " SKETCH_VERSION "\n");  // This message is shown in the serial monitor of the
                                                                     // Arduino IDE if the serial port is working.
                                                                     // If the serial port of the LED-Arduino is shown this
                                                                     // message may be corrupted because the interrupts are
                                                                     // disabled if the RGB LEDs are updated.
                                                                     // The next message is only shown if the SEND_DISABLE signal
                                                                     // connected to ground by the LED-Arduino..
  #if USE_SPI_SLAVE
      printf("SPI Mode supported.\n");
      printf("Connect J13 and remove TX pin from DCC/Selectrix Nano\n");
      Activate_SPI();
  #endif

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  Dcc.pin(0, DCC_SIGNAL_PIN, 1);

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // ToDo: Was bedeuten die Konstanten ?

  AddToSendBuffer("Init Done\n"); // This message is send to the LED Arduino over RS232 or SPI (If the Arduino is already active)

  pinMode(SEND_DISABLE_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin. This is importand to disable the communikation while the LED Arduino is flashed
  pinMode(STATUSLED_PIN, OUTPUT);
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
          if (Error > 1 && LED_Arduino_signal_detected) Error--; // Decrement the error counter if the communication is working again
          }
    else 
    {
          if (Use_RS232)
	    {
	    if ((millis()-LastSignalTime) > 1000)
	      {
	      LastSignalTime = 0;
	      Error = 0;              // don't show an error when signal comes back
	      }
	    if (LastSignalTime==0)   // no SX signal
	      {
	      Flash_Table_p = NoSignal_Flash_Table;
  	      }
	    else
	      {
              Flash_Table_p = RS232_Flash_Table;
	      }
          #if USE_SPI_SLAVE
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
    #ifdef USE_SPI_SLAVE
     LED_Arduino_signal_detected = 0;
    #endif
     }
  if (t >= NextStatusFlash) // Flash the status LED
     {
     if (FlashState > Flash_Table_p[0]) FlashState = 1;
     NextStatusFlash = t + Flash_Table_p[FlashState];
     digitalWrite(STATUSLED_PIN, STATUSLED_MOD (FlashState%2));
     #if USE_SPI_SLAVE                                                                                        // 15.05.20:
        if (!Use_RS232)
           { // SPI Mode is active
           if (DisableSerial == SERIAL_DISABLED) // Flash also the RX LED if the SPI mode is used in case the user has no LED connected to pin D8
              {
              pinMode(RX_PIN, OUTPUT);
              digitalWrite(RX_PIN, !(FlashState%2));
              }
           }
     #endif
     FlashState++;
     }
}

#if USE_SPI_SLAVE                                                                                           // 15.05.20:
  //-------------------------
  void SPI_Sleep_and_Wakeup()
  //-------------------------
  {
    if (LED_Arduino_signal_detected & 2)
       {
       Use_RS232 = 0;               // Disable the RS232 communikation
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
  Dcc.process(); // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation

  if (Use_RS232) Transmit_Sendchar_if_waiting();

  #if USE_SPI_SLAVE                                                                                           // 15.05.20:
    static uint8_t Show_Message_Once = 1;
    if (Use_RS232 == 0 && Show_Message_Once)
       {
       Show_Message_Once = 0; // Show only once
       printf("Slave disabled serial communication\n");
       pinMode(SEND_DISABLE_PIN, INPUT);
       }

    SPI_Sleep_and_Wakeup();
  #endif

  Process_Status_and_Error_LED();                                                                             // 13.05.20:

  // Disable the serial port if it's not used for a while
  if (DisableSerial != SERIAL_DISABLED && millis() > DisableSerial)                                           // 13.05.20:
     {
     digitalWrite(BUF_GATE_PIN, 1); // disable the external buffer gate for the TX pin                        // 18.10.20:
     Serial.end(); // disable the serial port to be able to flash the LED-Arduino.
     DisableSerial = SERIAL_DISABLED;
     }

  LED_HeartBeat.Update();
}
