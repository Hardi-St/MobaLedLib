/*!
 * @file DFRobotDFPlayerMini.cpp
 * @brief DFPlayer - An Arduino Mini MP3 Player From DFRobot
 * @n Header file for DFRobot's DFPlayer
 *
 * @copyright	[DFRobot]( http://www.dfrobot.com ), 2016
 * @copyright	GNU Lesser General Public License
 *
 * @author [Angelo](Angelo.qiao@dfrobot.com)
 * @version  V1.0.3
 * @date  2016-12-07
 */

#include "DFPlayerMini.h"

uint16_t DFPlayerMini::calculateCheckSum(uint8_t *buffer){
  uint16_t sum = 0;
  for (int i=Stack_Version; i<Stack_CheckSum; i++) {
    sum += buffer[i];
  }
  return -sum;
}

void DFPlayerMini::disableACK(){
  _sending[Stack_ACK] = 0x00;
}

bool DFPlayerMini::begin(Stream &stream){
  _serial = &stream;
  
  disableACK();
  
  //reset();
  //waitAvailable(2000);
  delay(200);

  return true;
}

void DFPlayerMini::sendStack() {
  #ifdef MP3_DEBUG
    Print("MP3-TF-16P cmd ");Println(command);
  #endif
  _serial->write(_sending, DFPLAYER_SEND_LENGTH);
  _timeOutTimer = millis();
  _isSending = _sending[Stack_ACK];
  
  delay(10);
}

void DFPlayerMini::sendStack(uint8_t command){
  sendStack(command, 0);
}

void DFPlayerMini::sendStack(uint8_t command, uint16_t argument){
  #ifdef MP3_DEBUG
    Print("MP3-TF-16p command: ");Print(command);Print(" larg: ");Println(argument);
  #endif
  _sending[Stack_Command] = command;
  uint16ToArray(argument, _sending+Stack_Parameter);
  uint16ToArray(calculateCheckSum(_sending), _sending+Stack_CheckSum);
  sendStack();
}

void DFPlayerMini::sendStack(uint8_t command, uint8_t argumentHigh, uint8_t argumentLow){
  #ifdef MP3_DEBUG
    Print("MP3-TF-16p command: ");Print(command);Print(" msb: ");Print(argumentHight);Print(" lsb: ");Println(argumentLow);
  #endif
  uint16_t buffer = argumentHigh;
  buffer <<= 8;
  sendStack(command, buffer | argumentLow);
}

void DFPlayerMini::uint16ToArray(uint16_t value, uint8_t *array){
  *array = (uint8_t)(value>>8);
  *(array+1) = (uint8_t)(value);
}

void DFPlayerMini::setModuleType(uint8_t moduleType) {
  _currentType = moduleType;
}

void DFPlayerMini::sendCmd(uint8_t command) {
  #ifdef MP3_DEBUG
    Print("Module type: ");Println(_currentType);
  #endif
  switch (_currentType) {
    case MODULE_MP3_TF_16P:
      sendStack(command);
      break;
    case MODULE_JQ6500:
      sendJQ(command);
      break;
  }
}

void DFPlayerMini::sendCmd(uint8_t command, uint8_t argument) {
  #ifdef MP3_DEBUG
    Print("Module type: ");Println(_currentType);
  #endif
  switch (_currentType) {
    case MODULE_MP3_TF_16P:
      sendStack(command, argument);
      break;
    case MODULE_JQ6500:
      sendJQ(command, argument);
      break;
  }
}

void DFPlayerMini::sendCmd(uint8_t command, uint16_t argument) {
  #ifdef MP3_DEBUG
    Print("Module type: ");Println(_currentType);
  #endif
  switch (_currentType) {
    case MODULE_MP3_TF_16P:
      sendStack(command, argument);
      break;
    case MODULE_JQ6500:
      sendJQ(command, argument);
      break;
  }
}

void DFPlayerMini::sendCmd(uint8_t command, uint8_t argumentHigh, uint8_t argumentLow) {
  #ifdef MP3_DEBUG
    Print("Module type: ");Println(_currentType);
  #endif
  switch (_currentType) {
    case MODULE_MP3_TF_16P:
      sendStack(command, argumentHigh, argumentLow);
      break;
    /*case MODULE_JQ6500:
      sendJQ(command, argumentHigh, argumentLow);
      break;*/
  }
  
}

void DFPlayerMini::sendJQ(uint8_t command) {
  #ifdef MP3_DEBUG
    Print("JQ6500 cmd ");Println(command);
  #endif
  _sendJQ[1] = 2;
  _sendJQ[2] = command;
  _sendJQ[3] = 0xef;
  _serial->write(_sendJQ, 4);
}

void DFPlayerMini::sendJQ(uint8_t command, uint8_t argument) {
  #ifdef MP3_DEBUG
    Print("JQ6500 cmd ");Print(command);Print(" arg ");Println(argument);
  #endif
  _sendJQ[1] = 3;
  _sendJQ[2] = command;
  _sendJQ[3] = argument;
  _sendJQ[4] = 0xef;
  _serial->write(_sendJQ, 5);
}

void DFPlayerMini::sendJQ(uint8_t command, uint16_t argument) {
  #ifdef MP3_DEBUG
    Print("JQ6500 cmd ");Print(command);Print(" larg ");Println(argument);
  #endif
  _sendJQ[1] = 4;
  _sendJQ[2] = command;
  _sendJQ[3] = (uint8_t) (argument>>8);
  _sendJQ[4] = (uint8_t) argument;
  _sendJQ[5] = 0xef;
  _serial->write(_sendJQ, 6);
}
