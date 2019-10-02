#include <NmraDcc.h>
/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018, 2019  Hardi Stengelin: MobaLedLib@gmx.de

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


 DCC rail signal decoder and transmitter to LED Arduino                                    by Hardi   14.01.19
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
                                    | [ ]Vin   [ ] [ ] [ ]   RX1[ ] |--------'
                                    |          [ ] [ ] [ ]          |
                                    |          MISO SCK RST         |
                                    | NANO-V3                       |
                                    +-------------------------------+

  The +5V supply lines of both arduinos could be connected together if they are powerd from one power supply (one USB).
*/
#include "MobaLedLib.h"



#define DCC_SIGNAL_PIN   2
#define SEND_DISABLE_PIN A1
#define HEARTBEAT_PIN    3
#define ERROR_LED_PIN    LED_BUILTIN

NmraDcc  Dcc ;                                // Instance of the NmraDcc class

LED_Heartbeat_C LED_HeartBeat(HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.


#define QUEUESIZE  128                    // Must be a "binary" number 32,64,128, ...
char     SendBuffer[QUEUESIZE];
char    *rp = SendBuffer;                 // Queue Read Pointer
char    *wp = SendBuffer;                 // Queue Write Pointer
char    *EndSendBuffer = SendBuffer+sizeof(SendBuffer);
uint8_t  Error = 0;
uint32_t NextErrorFlash = 0;

#define SERIAL_BAUD      115200           // Should be equal to the DCC_Rail_Decoder_Receiver.ino program
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
  if (DisableSerial == SERIAL_DISABLED)
     Serial.begin(SERIAL_BAUD);

  while (*s)
    {
    *wp++ = *s++;
    if (wp >= EndSendBuffer) wp = SendBuffer;
    if (wp == rp) if (Error<16) Error+=2; // buffer overflow
    }
  DisableSerial = millis() + 10*QueueFill();
}

//---------------------------------
void Transmit_Sendchar_if_waiting()
//---------------------------------
// Must be called in the loop() function
{
  if (QueueFill() && digitalRead(SEND_DISABLE_PIN) == 0 && Serial.availableForWrite() >= SERIAL_TX_BUFFER_SIZE-1)
       {
       char c = *rp;
       Serial.print(c);
       rp++;
       if (rp >= EndSendBuffer) rp = SendBuffer;
       }
  else if (DisableSerial != SERIAL_DISABLED && millis() > DisableSerial)
          {
          Serial.end(); // disable the serial port to be able to flash the LED-Arduino.
          DisableSerial = SERIAL_DISABLED;
          }

  static bool Send_Disable_Low_detected = false;
  if (digitalRead(SEND_DISABLE_PIN) == 0) Send_Disable_Low_detected = true;
  if (Error)
     {
     uint32_t t = millis();
     if (t >= NextErrorFlash) // Flash the error LED if an buffer overflow was detected
        {
        if (Error > 1)
             NextErrorFlash = t + 250;  // Error still active
        else {                          // Prior error detected but not longer activ
             if (digitalRead(ERROR_LED_PIN))
                  NextErrorFlash = t + 450;
             else NextErrorFlash = t +  50; // Short flash
             }
        if (Error > 1 && Send_Disable_Low_detected) Error--;
        Send_Disable_Low_detected = false;
        digitalWrite(ERROR_LED_PIN, !digitalRead(ERROR_LED_PIN));
        }
     }
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
  /*
  Serial.print(millis());
  Serial.print("  notifyDccAccTurnoutOutput: ") ;
  Serial.print(Addr,DEC) ;
  Serial.print(',');
  Serial.print(Direction,DEC) ;
  Serial.print(',');
  Serial.println(OutputPower, HEX) ;
  */
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
  /*
  Serial.print("notifyDccSigState: ") ;
  Serial.print(Addr,DEC) ;
  Serial.print(',');
  Serial.println(State, HEX) ;
  */
}


//-----------
void setup(){
//-----------
  Serial.begin(SERIAL_BAUD);  // One char ~ 90 us
  Serial.println(F("\nDCC_Rail_Decoder Serial test")); // This message is shown in the serial monitor of the
  delay(100);                                          // Arduino IDE if the serial port is working.
                                                       // If the serial port of the LED-Arduino is shown this
                                                       // message may be corrupted because the interrupts are
                                                       // disabled if the RGB LEDs are updated.
                                                       // The next message is only shown if the SEND_DISABLE signal
                                                       // connected to ground by the LED-Arduino..

  AddToSendBuffer("\nDCC_Rail_Decoder_Transmitter Example A\n");  // Don't use Serial.print because characters get lost if the LED-Arduinio updates the LEDs

  // Setup which External Interrupt, the Pin it's associated with that we're using and enable the Pull-Up
  Dcc.pin(0, DCC_SIGNAL_PIN, 1);

  // Call the main DCC Init function to enable the DCC Receiver
  Dcc.init( MAN_ID_DIY, 10, CV29_ACCESSORY_DECODER | CV29_OUTPUT_ADDRESS_MODE, 0 );  // ToDo: Was bedeuten die Konstanten ?

  AddToSendBuffer("Init Done\n");

  pinMode(SEND_DISABLE_PIN, INPUT_PULLUP); // Activate an internal pullup resistor for the input pin
  pinMode(ERROR_LED_PIN,   OUTPUT);

  EEPROM.read(0); // Prevent EEPROM warning: "EEPROM.h:145:20: warning: 'EEPROM' defined but not used "
                  // Unfortunately there are some other signed/unsigned warnings in the lib which can't be disabled
}

//-----------
void loop(){
//-----------
  Dcc.process(); // You MUST call the NmraDcc.process() method frequently from the Arduino loop() function for correct library operation

  Transmit_Sendchar_if_waiting();

  LED_HeartBeat.Update();

}

