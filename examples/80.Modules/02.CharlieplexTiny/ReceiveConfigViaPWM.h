#ifndef _RECEIVE_CONFIG_VIA_PWM_H_
#define _RECEIVE_CONFIG_VIA_PWM_H_

uint8_t Check_ProgramMode();

void Debug_Print_Flash_Config(const uint8_t *Config, uint16_t Size);

int8_t Check_EEPROM_CRC();

#endif // _RECEIVE_CONFIG_VIA_PWM_H_
