#ifndef _LEDS_H_
#define _LEDS_H_

#define BRIGHT_LEVELS 16    // Maximal 32 (

void LED_IntProc(); // Interrupt proc

void Set_LED_Reset_as_IO(uint8_t on);
void Set_LED_Norm_ResetF(uint8_t on);
void Set_LED_Heart_Beat(uint8_t on);
void Set_LED_Error      (uint8_t on);
void Set_LED_Prog       (uint8_t on);
void Set_LED_Read       (uint8_t on);

uint8_t Get_LED_Reset_as_IO();
uint8_t Get_LED_Norm_ResetF();
uint8_t Get_LED_Heart_Beat ();
uint8_t Get_LED_Error      ();
uint8_t Get_LED_Prog       ();
uint8_t Get_LED_Read       ();



void Upd_LED_Heartbeat();

void Setup_LEDs(void (*Add_Int_Proc)(void));
void Power_on_Ani();
void Setup_LEDs_with_Power_on_Ani(void (*Add_Int_Proc)(void));

void Test_LEDs(); // Debug

#endif // _LEDS_H_
