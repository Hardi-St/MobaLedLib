#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wunused-variable"
#pragma GCC diagnostic ignored "-Wsign-compare"


#include <MaerklinMotorola.h> // The MaerklinMotorola library must be installed in addition if you got the error message "..fatal error: MaerklinMotorola: No such file or directory"
                              // Download the ZIP file from https://github.com/oliwel/MaerklinMotorola/
                              // Arduino IDE: Sketch / Include library / Add .ZIP library...                 Deutsche IDE: Sketch / Bibliothek einbinden / .ZIP Bibliothek hinzufuegen...
                              //              Navigate to the download directory and select the file                       Zum Download Verzeichnis werchseln und die Datei auswaehlen
#pragma GCC diagnostic pop

/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de

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


 MM2 rail signal decoder and transmitter to LED Arduino                           by oliwel 18.05.21
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 t.b.d - this is a functional clone of the DCC Interface, it reads a Märklin Motorola
 track signal and echos any function command found on the serial line using the same
 format than the DCC version so the receiver side can be reused.

 Note: The underlying MM2 library does not handle addresses above 256 properly, those
 are mapped to other addresses so you can not use this on a system where such addresses
 are used. You also have to use a patched library available at
 https://github.com/oliwel/MaerklinMotorola/ until the library has been updated by the
 arduino team.

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
*/
/*
Revision History :
~~~~~~~~~~~~~~~~~
18.05.21: Initial Version based on DCC Interface example

*/
#define SKETCH_VERSION "1.4"

#include "MobaLedLib.h"

// #include "26_MM2_Interface.h" // Is written by the Excel Tool Program_Generator to set compiler switches

#define STATUSLED_PIN  LED_BUILTIN
#define STATUSLED_MOD

#define MM2_SIGNAL_PIN   2
#define MM2_COMMAND_DEBOUNCE 500
#define SEND_DISABLE_PIN A1
#define HEARTBEAT_PIN    3
#define BUF_GATE_PIN     A5                                                                                   // 18.10.20:

LED_Heartbeat_C LED_HeartBeat(HEARTBEAT_PIN); // Initialize the heartbeat LED which is flashing if the program runs.

volatile MaerklinMotorola mm(MM2_SIGNAL_PIN);
char lastCommand[20];
int lastCommandExpiry;

#define QUEUESIZE  128                    // Must be a "binary" number 32,64,128, ...
volatile char     SendBuffer[QUEUESIZE];
volatile char    *rp = SendBuffer;        // Queue Read Pointer
volatile char    *wp = SendBuffer;        // Queue Write Pointer
volatile char    *EndSendBuffer   = SendBuffer+sizeof(SendBuffer);
uint8_t           Error           = 0;
uint32_t          NextStatusFlash = 0;

//                                       States On    Off   On  Off  On   Off
uint16_t RS232_Flash_Table[]         = { 2,     1500, 1500                     };
uint16_t BufferFull_Flash_Table[]    = { 2,     250,  250                      };
uint16_t PriorBuddFull_Flash_Table[] = { 2,     50,   450                      };
uint8_t  FlashState = 1;

uint8_t LED_Arduino_signal_detected = 0;  // Activated if a signal from the LED Arduino is detected
                                          // Bit 1 is set if A1 = Low is detected
                                          // Bit 2 is set if an SPI interrupt is triggered

#define SERIAL_BAUD      115200           // Should be equal to the DCC_Rail_Decoder_Receiver.ino program
#define SERIAL_DISABLED  0
uint32_t DisableSerial = SERIAL_DISABLED; // Disable the serial port x seconds after the start to be able to program the LED-Arduino.
                                          // This is important because otherwise the receive pin of the LED-Arduino is blocked by this arduino ;-(
                                          // The serial port is enabled again to transmit DCC messages

#define sizemask    (QUEUESIZE-1)         // Bitmask for queue
#define QueueFill() ((sizemask + 1 + wp - rp) & sizemask)

//---------------------------------
void AddToSendBuffer(const char *s)
//---------------------------------
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


//---------------------------------
void Transmit_Sendchar_if_waiting()
//---------------------------------
// Must be called in the loop() function
{
    if (QueueFill() && digitalRead(SEND_DISABLE_PIN) == 0) {
        if (DisableSerial == SERIAL_DISABLED) {
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
  printf("\nMM2_Rail_Decoder Serial Version " SKETCH_VERSION "\n");  // This message is shown in the serial monitor of the
                                                                     // Arduino IDE if the serial port is working.
                                                                     // If the serial port of the LED-Arduino is shown this
                                                                     // message may be corrupted because the interrupts are
                                                                     // disabled if the RGB LEDs are updated.
                                                                     // The next message is only shown if the SEND_DISABLE signal
                                                                     // connected to ground by the LED-Arduino..

    // Initialises the Input defined by MM2_SIGNAL_PIN to process the MM2 track signal
    attachInterrupt(digitalPinToInterrupt(MM2_SIGNAL_PIN), isr, CHANGE);

    AddToSendBuffer("Init Done\n"); // This message is send to the LED Arduino over RS232  (If the Arduino is already active)

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
          else Flash_Table_p = PriorBuddFull_Flash_Table; // Prior error detected but not longer activ
          if (Error > 1 && LED_Arduino_signal_detected) Error--; // Decrement the error counter if the communication is working again
          }
     else {
            Flash_Table_p = RS232_Flash_Table;
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
     digitalWrite(STATUSLED_PIN, STATUSLED_MOD (FlashState%2));
     FlashState++;
     }
}


//-----------
void loop() {
//-----------

    // parse the MM2 input buffer into commands
    mm.Parse();
    MaerklinMotorolaData* Data = mm.GetData();
    // only handle packets for function decoders (not trains)
    if (Data && Data->IsMagnet && Data->MagnetState ) {

        /*
        Serial.print("Address: "); Serial.print(Data->Address);
        Serial.print(" -  Function: "); Serial.print(Data->Function);
        Serial.print(" -  Stop: "); Serial.print(Data->Stop);
        Serial.print(" -  ChangeDir: "); Serial.print(Data->ChangeDir);
        Serial.print(" -  Speed: "); Serial.print(Data->Speed);
        Serial.print(" -  Magnet: " + String(Data->IsMagnet ? "yes" : "no"));
        Serial.println();
        */

        char cmd[20];
        cmd[0] = 0;
        if (Data->DecoderState == MM2DecoderState_Green) {
            sprintf(cmd, "@%4i %02X %02X\n", Data->PortAddress, 0, 0);
        } else if (Data->DecoderState == MM2DecoderState_Red) {
            sprintf(cmd, "@%4i %02X %02X\n", Data->PortAddress, 1, 0);
        }

        /* MM2 devices repeat the command several times and the library has no
         * internal debounce mechanism so each action will show up here more than
         * once. To avoid this noise we suppres identical commands received within
         * the time defined by MM2_COMMAND_DEBOUNCE
         */
        if (cmd[0] != 0) {
            if (lastCommandExpiry < millis() || memcmp(cmd, lastCommand, 12) != 0) {
                AddToSendBuffer(cmd);
                strcpy(lastCommand,cmd);
                lastCommandExpiry = millis() + MM2_COMMAND_DEBOUNCE;
            }
        }

    }

    Transmit_Sendchar_if_waiting();

    Process_Status_and_Error_LED();

    // Disable the serial port if it's not used for a while
    if (DisableSerial != SERIAL_DISABLED && millis() > DisableSerial) {
        digitalWrite(BUF_GATE_PIN, 1);
        Serial.end();
        DisableSerial = SERIAL_DISABLED;
    }

    LED_HeartBeat.Update();
}

void isr() {
  mm.PinChange();
}