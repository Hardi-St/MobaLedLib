#ifndef _CHARLIELEDS_H_
#define _CHARLIELEDS_H_

#define PWM_PARTS        8               // 1, 2, 4, or 8  Increase the brightness levels by splitting the PWM signal of the LEDs
#define BRIGHT_LEVELS    (16*PWM_PARTS)  // => 16, 32, 64 or 128 brightness levels
                                         // If PWM_PARTS is > 1 the LED is flickering in a video if the brightness
                                         // is below PWM_PARTS/2

#define MAX_BRIGHT       (BRIGHT_LEVELS-1)

#define LED_HEART_BEAT   11

#if defined __AVR_ATtiny85__ && defined DEBUG_TINY
    #define LED_MASK           B00000111   // Debug
    // Normaly the LED_MASK is not defined.
    // In this case the whole port is changed and not only the masked bits !
    // This is no problem since the only available other pin is always used as input.
    // If this module is used for an other processor the LED_MASK may be used.
    // When debuging the program wit an Uno/Nano the other pins of Port B can't be
    // used as output.
#endif

void LED_IntProc(); // Interrupt proc

void    Set_LED(uint8_t LED_Nr, uint8_t brightness);
uint8_t Get_LED(uint8_t LED_Nr);

void Upd_LED_Heartbeat();

void Setup_LEDs();

void Set_one_LED_Assignement(uint8_t Dst, uint8_t Src);

void Power_on_Ani();


#endif // _CHARLIELEDS_H_
