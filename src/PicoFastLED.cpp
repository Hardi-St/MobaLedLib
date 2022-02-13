/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2021  Hardi Stengelin: MobaLedLib@gmx.de
 this file: Copyright (C) 2021 Jürgen Winkler: MobaLedLib@gmx.at

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


 Pico FastLED:
 ~~~~~~~~~~~~~~~~~

 a minimum implementation for Raspberry Pico being compatible to FastLED library
 only implementing FastLED functions needed for MobaLedLib to work
 
*/

#ifdef ARDUINO_RASPBERRY_PI_PICO
#include <Arduino.h>
#include "PicoFastLED.h"
#include <hardware/clocks.h>
#include <hardware/pio.h>
#include "ws2812.pio.h"

/// add one byte to another, saturating at 0xFF
/// @param i - first byte to add
/// @param j - second byte to add
/// @returns the sum of i & j, capped at 0xFF
uint8_t qadd8( uint8_t i, uint8_t j)
{
    unsigned int t = i + j;
    if( t > 255) t = 255;
    return t;
}

/// subtract one byte from another, saturating at 0x00
/// @returns i - j with a floor of 0
uint8_t qsub8( uint8_t i, uint8_t j)
{
    int t = i - j;
    if( t < 0) t = 0;
    return t;
}

///       Calculate the remainder of one unsigned 8-bit
///       value divided by anoter, aka A % M.
uint8_t mod8( uint8_t a, uint8_t m)
{
	return a % m;
}

uint8_t scale8(uint8_t i, uint8_t scale)
{
		 return (((uint16_t)i) * (1+(uint16_t)(scale))) >> 8;
}

uint8_t random8()
{
    return random();
}

uint8_t random8(int high)
{
    return random(high);
}

uint8_t random8(int low, int high)
{
    return random(low, high);
}

uint16_t random16(int high)
{
    return random(high);
}

uint16_t random16(int low, int high)
{
    return random(low, high);
}

void cleanup_R1()
{
}

/// This version of scale8_video does not clean up the R1 register on AVR
/// If you are doing several 'scale8_video's in a row, use this, and
/// then explicitly call cleanup_R1.
uint8_t scale8_video_LEAVING_R1_DIRTY( uint8_t i, fract8 scale)
{
    uint8_t j = (((int)i * (int)scale) >> 8) + ((i&&scale)?1:0);
    return j;
}

/// This version of scale8 does not clean up the R1 register on AVR
/// If you are doing several 'scale8's in a row, use this, and
/// then explicitly call cleanup_R1.
uint8_t scale8_LEAVING_R1_DIRTY( uint8_t i, fract8 scale)
{
#if (FASTLED_SCALE8_FIXED == 1)
    return (((uint16_t)i) * ((uint16_t)(scale)+1)) >> 8;
#else
    return ((int)i * (int)(scale) ) >> 8;
#endif
}

void hsv2rgb_rainbow( const CHSV& hsv, CRGB& rgb)
{
    // Yellow has a higher inherent brightness than
    // any other color; 'pure' yellow is perceived to
    // be 93% as bright as white.  In order to make
    // yellow appear the correct relative brightness,
    // it has to be rendered brighter than all other
    // colors.
    // Level Y1 is a moderate boost, the default.
    // Level Y2 is a strong boost.
    const uint8_t Y1 = 1;
    const uint8_t Y2 = 0;
    
    // G2: Whether to divide all greens by two.
    // Depends GREATLY on your particular LEDs
    const uint8_t G2 = 0;
    
    // Gscale: what to scale green down by.
    // Depends GREATLY on your particular LEDs
    const uint8_t Gscale = 0;
    
    
    uint8_t hue = hsv.hue;
    uint8_t sat = hsv.sat;
    uint8_t val = hsv.val;
    
    uint8_t offset = hue & 0x1F; // 0..31
    
    // offset8 = offset * 8
    uint8_t offset8 = offset;
    {
        // On ARM and other non-AVR platforms, we just shift 3.
        offset8 <<= 3;
    }
    
    uint8_t third = scale8( offset8, (256 / 3)); // max = 85
    
    uint8_t r, g, b;
    
    if( ! (hue & 0x80) ) {
        // 0XX
        if( ! (hue & 0x40) ) {
            // 00X
            //section 0-1
            if( ! (hue & 0x20) ) {
                // 000
                //case 0: // R -> O
                r = K255 - third;
                g = third;
                b = 0;
                FORCE_REFERENCE(b);
            } else {
                // 001
                //case 1: // O -> Y
                if( Y1 ) {
                    r = K171;
                    g = K85 + third ;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
                if( Y2 ) {
                    r = K170 + third;
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    g = K85 + twothirds;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
            }
        } else {
            //01X
            // section 2-3
            if( !  (hue & 0x20) ) {
                // 010
                //case 2: // Y -> G
                if( Y1 ) {
                    //uint8_t twothirds = (third << 1);
                    uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                    r = K171 - twothirds;
                    g = K170 + third;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
                if( Y2 ) {
                    r = K255 - offset8;
                    g = K255;
                    b = 0;
                    FORCE_REFERENCE(b);
                }
            } else {
                // 011
                // case 3: // G -> A
                r = 0;
                FORCE_REFERENCE(r);
                g = K255 - third;
                b = third;
            }
        }
    } else {
        // section 4-7
        // 1XX
        if( ! (hue & 0x40) ) {
            // 10X
            if( ! ( hue & 0x20) ) {
                // 100
                //case 4: // A -> B
                r = 0;
                FORCE_REFERENCE(r);
                //uint8_t twothirds = (third << 1);
                uint8_t twothirds = scale8( offset8, ((256 * 2) / 3)); // max=170
                g = K171 - twothirds; //K170?
                b = K85  + twothirds;
                
            } else {
                // 101
                //case 5: // B -> P
                r = third;
                g = 0;
                FORCE_REFERENCE(g);
                b = K255 - third;
                
            }
        } else {
            if( !  (hue & 0x20)  ) {
                // 110
                //case 6: // P -- K
                r = K85 + third;
                g = 0;
                FORCE_REFERENCE(g);
                b = K171 - third;
                
            } else {
                // 111
                //case 7: // K -> R
                r = K170 + third;
                g = 0;
                FORCE_REFERENCE(g);
                b = K85 - third;
                
            }
        }
    }
    
    // This is one of the good places to scale the green down,
    // although the client can scale green down as well.
    if( G2 ) g = g >> 1;
    if( Gscale ) g = scale8_video_LEAVING_R1_DIRTY( g, Gscale);
    
    // Scale down colors if we're desaturated at all
    // and add the brightness_floor to r, g, and b.
    if( sat != 255 ) {
        if( sat == 0) {
            r = 255; b = 255; g = 255;
        } else {
            //nscale8x3_video( r, g, b, sat);
#if (FASTLED_SCALE8_FIXED==1)
            if( r ) r = scale8_LEAVING_R1_DIRTY( r, sat);
            if( g ) g = scale8_LEAVING_R1_DIRTY( g, sat);
            if( b ) b = scale8_LEAVING_R1_DIRTY( b, sat);
#else
            if( r ) r = scale8_LEAVING_R1_DIRTY( r, sat) + 1;
            if( g ) g = scale8_LEAVING_R1_DIRTY( g, sat) + 1;
            if( b ) b = scale8_LEAVING_R1_DIRTY( b, sat) + 1;
#endif
            cleanup_R1();
            
            uint8_t desat = 255 - sat;
            desat = scale8( desat, desat);
            
            uint8_t brightness_floor = desat;
            r += brightness_floor;
            g += brightness_floor;
            b += brightness_floor;
        }
    }
    
    // Now scale everything down if we're at value < 255.
    if( val != 255 ) {
        
        val = scale8_video_LEAVING_R1_DIRTY( val, val);
        if( val == 0 ) {
            r=0; g=0; b=0;
        } else {
            // nscale8x3_video( r, g, b, val);
#if (FASTLED_SCALE8_FIXED==1)
            if( r ) r = scale8_LEAVING_R1_DIRTY( r, val);
            if( g ) g = scale8_LEAVING_R1_DIRTY( g, val);
            if( b ) b = scale8_LEAVING_R1_DIRTY( b, val);
#else
            if( r ) r = scale8_LEAVING_R1_DIRTY( r, val) + 1;
            if( g ) g = scale8_LEAVING_R1_DIRTY( g, val) + 1;
            if( b ) b = scale8_LEAVING_R1_DIRTY( b, val) + 1;
#endif
            cleanup_R1();
        }
    }
    
    // Here we have the old AVR "missing std X+n" problem again
    // It turns out that fixing it winds up costing more than
    // not fixing it.
    // To paraphrase Dr Bronner, profile! profile! profile!
    //asm volatile(  ""  :  :  : "r26", "r27" );
    //asm volatile (" movw r30, r26 \n" : : : "r30", "r31");
    rgb.r = r;
    rgb.g = g;
    rgb.b = b;
}

void nscale8x3( uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale)
{
    uint16_t scale_fixed = scale + 1;
    r = (((uint16_t)r) * scale_fixed) >> 8;
    g = (((uint16_t)g) * scale_fixed) >> 8;
    b = (((uint16_t)b) * scale_fixed) >> 8;
}

fract8 ease8InOutApprox( fract8 i)
{
    if( i < 64) {
        // start with slope 0.5
        i /= 2;
    } else if( i > (255 - 64)) {
        // end with slope 0.5
        i = 255 - i;
        i /= 2;
        i = 255 - i;
    } else {
        // in the middle, use slope 192/128 = 1.5
        i -= 64;
        i += (i / 2);
        i += 32;
    }

    return i;
}

CRGB ColorFromPalette( const TProgmemRGBPalette16& pal, uint8_t index, uint8_t brightness, TBlendType blendType)
{
	return CRGB(0,0,0);
    //      hi4 = index >> 4;
    // uint8_t hi4 = lsrX4(index);
    // uint8_t lo4 = index & 0x0F;

    // CRGB entry   =  FL_PGM_READ_DWORD_NEAR( &(pal[0]) + hi4 );
    

    // uint8_t red1   = entry.red;
    // uint8_t green1 = entry.green;
    // uint8_t blue1  = entry.blue;

    // uint8_t blend = lo4 && (blendType != NOBLEND);

    // if( blend ) {

        // if( hi4 == 15 ) {
            // entry =   FL_PGM_READ_DWORD_NEAR( &(pal[0]) );
        // } else {
            // entry =   FL_PGM_READ_DWORD_NEAR( &(pal[1]) + hi4 );
        // }

        // uint8_t f2 = lo4 << 4;
        // uint8_t f1 = 255 - f2;

        // uint8_t red2   = entry.red;
        // red1   = scale8_LEAVING_R1_DIRTY( red1,   f1);
        // red2   = scale8_LEAVING_R1_DIRTY( red2,   f2);
        // red1   += red2;

        // uint8_t green2 = entry.green;
        // green1 = scale8_LEAVING_R1_DIRTY( green1, f1);
        // green2 = scale8_LEAVING_R1_DIRTY( green2, f2);
        // green1 += green2;

        // uint8_t blue2  = entry.blue;
        // blue1  = scale8_LEAVING_R1_DIRTY( blue1,  f1);
        // blue2  = scale8_LEAVING_R1_DIRTY( blue2,  f2);
        // blue1  += blue2;

        // cleanup_R1();
    // }

    // if( brightness != 255) {
        // if( brightness ) {
            // ++brightness; // adjust for rounding
            // // Now, since brightness is nonzero, we don't need the full scale8_video logic;
            // // we can just to scale8 and then add one (unless scale8 fixed) to all nonzero inputs.
            // if( red1 )   {
                // red1 = scale8_LEAVING_R1_DIRTY( red1, brightness);
// #if !(FASTLED_SCALE8_FIXED==1)
                // ++red1;
// #endif
            // }
            // if( green1 ) {
                // green1 = scale8_LEAVING_R1_DIRTY( green1, brightness);
// #if !(FASTLED_SCALE8_FIXED==1)
                // ++green1;
// #endif
            // }
            // if( blue1 )  {
                // blue1 = scale8_LEAVING_R1_DIRTY( blue1, brightness);
// #if !(FASTLED_SCALE8_FIXED==1)
                // ++blue1;
// #endif
            // }
            // cleanup_R1();
        // } else {
            // red1 = 0;
            // green1 = 0;
            // blue1 = 0;
        // }
    // }

    // return CRGB( red1, green1, blue1);
}

/// Approximate "black body radiation" palette, akin to
/// the FastLED 'HeatColor' function.
/// Recommend that you use values 0-240 rather than
/// the usual 0-255, as the last 15 colors will be
/// 'wrapping around' from the hot end to the cold end,
/// which looks wrong.
extern const TProgmemRGBPalette16 HeatColors_p FL_PROGMEM =
{
    0x000000,
    0x330000, 0x660000, 0x990000, 0xCC0000, 0xFF0000,
    0xFF3300, 0xFF6600, 0xFF9900, 0xFFCC00, 0xFFFF00,
    0xFFFF33, 0xFFFF66, 0xFFFF99, 0xFFFFCC, 0xFFFFFF
};

CFastLED FastLED;
CLEDController *CLEDController::m_pHead = NULL;
CLEDController *CLEDController::m_pTail = NULL;

CLEDController &CFastLED::addLeds(CLEDController *pLed,
								  struct CRGB *data,
								  int nLedsOrOffset, int nLedsIfOffset) {
	int nOffset = (nLedsIfOffset > 0) ? nLedsOrOffset : 0;
	int nLeds = (nLedsIfOffset > 0) ? nLedsIfOffset : nLedsOrOffset;

	pLed->setLeds(data + nOffset, nLeds);
	return *pLed;
}

void CFastLED::show()
{
	CLEDController *pCur = CLEDController::head();
	while(pCur) {
		pCur->showLeds(100);
		pCur = pCur->next();
	}
	delayMicroseconds(500);
}

const int PIN_TX = 0;

CPicoController::CPicoController()
{
	int sm=0;
	uint offset = pio_add_program(pio1, &ws2812_program);
  ws2812_program_init(pio1, sm, offset, PIN_TX, 800000, true);
}

void CPicoController::show(const struct CRGB *data, int nLeds, uint8_t brightness)
{
	uint32_t val = 0;
	uint32_t offset;
	uint16_t pixels = nLeds*3;
	uint16_t max = ((pixels+3)/4)*4;
	for (uint i = 0; i < max; ++i) {
			switch(i%3)
			{
				case 0: offset = 1; break;
				case 1: offset = -1; break;
				default: offset = 0;
			}
			val = val << 8;
			if (i<pixels) val = val + *(((const uint8_t*)data)+i+offset);
			if ((i&0x03)==3) {
					pio_sm_put_blocking(pio1, 0, val);
					val = 0;
			}
	}
}
#endif