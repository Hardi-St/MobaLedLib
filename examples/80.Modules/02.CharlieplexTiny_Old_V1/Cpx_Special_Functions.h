#include <Arduino.h>

#define ANALOG_KEYS          10

extern uint8_t ReadAnalog;
extern uint16_t AnalogLimmits[ANALOG_KEYS];


void Set_LED_Assignement(const uint8_t* &cp);

void Enable_Analog_Input(uint8_t Enable);
void Read_Analog_Limits(const uint8_t* &cp);
