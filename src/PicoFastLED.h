#include "Arduino.h"
#include "PicoFastpin.h"
#ifndef __FASTLED_H__
#define __FASTLED_H__

#define K255 255
#define K171 171
#define K170 170
#define K85  85
#define FORCE_REFERENCE(var)  asm volatile( "" : : "r" (var) )

#define FL_PROGMEM PROGMEM

#define DISABLE_DITHER 0x00
#define BINARY_DITHER 0x01
typedef uint8_t EDitherMode;

struct CRGB;
struct CHSV;
typedef uint8_t   fract8;   ///< ANSI: unsigned short _Fract
typedef uint32_t TProgmemRGBPalette16[16];

void hsv2rgb_rainbow( const CHSV& hsv, CRGB& rgb);

/// ease8InOutApprox: fast, rough 8-bit ease-in/ease-out function
///                   shaped approximately like 'ease8InOutCubic',
///                   it's never off by more than a couple of percent
///                   from the actual cubic S-curve, and it executes
///                   more than twice as fast.  Use when the cycles
///                   are more important than visual smoothness.
/// 
fract8 ease8InOutApprox( fract8 i);

/// scale three one byte values by a fourth one, which is treated as
///         the numerator of a fraction whose demominator is 256
///         In other words, it computes r,g,b * (scale / 256)
///
///         THIS FUNCTION ALWAYS MODIFIES ITS ARGUMENTS IN PLACE
void nscale8x3( uint8_t& r, uint8_t& g, uint8_t& b, fract8 scale);

/// Clean up the r1 register after a series of *LEAVING_R1_DIRTY calls
void cleanup_R1();

/// This version of scale8_video does not clean up the R1 register on AVR
/// If you are doing several 'scale8_video's in a row, use this, and
/// then explicitly call cleanup_R1.
uint8_t scale8_video_LEAVING_R1_DIRTY( uint8_t i, fract8 scale);

/// Representation of an HSV pixel (hue, saturation, value (aka brightness)).
struct CHSV {
    union {
		struct {
		    union {
		        uint8_t hue;
		        uint8_t h; };
		    union {
		        uint8_t saturation;
		        uint8_t sat;
		        uint8_t s; };
		    union {
		        uint8_t value;
		        uint8_t val;
		        uint8_t v; };
		};
		uint8_t raw[3];
	};

    /// Array access operator to index into the chsv object
	inline uint8_t& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return raw[x];
    }

    /// Array access operator to index into the chsv object
    inline const uint8_t& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return raw[x];
    }

    /// default values are UNITIALIZED
    inline CHSV() __attribute__((always_inline)) = default;

    /// allow construction from H, S, V
    inline CHSV( uint8_t ih, uint8_t is, uint8_t iv) __attribute__((always_inline))
        : h(ih), s(is), v(iv)
    {
    }

    /// allow copy construction
    inline CHSV(const CHSV& rhs) __attribute__((always_inline)) = default;

    inline CHSV& operator= (const CHSV& rhs) __attribute__((always_inline)) = default;

    inline CHSV& setHSV(uint8_t ih, uint8_t is, uint8_t iv) __attribute__((always_inline))
    {
        h = ih;
        s = is;
        v = iv;
        return *this;
    }
};


/// Representation of an RGB pixel (Red, Green, Blue)
struct CRGB {
	union {
		struct {
            union {
                uint8_t r;
                uint8_t red;
            };
            union {
                uint8_t g;
                uint8_t green;
            };
            union {
                uint8_t b;
                uint8_t blue;
            };
        };
		uint8_t raw[3];
	};

     /// Array access operator to index into the crgb object
  	inline uint8_t& operator[] (uint8_t x) __attribute__((always_inline))
    {
        return raw[x];
    }

    /// Array access operator to index into the crgb object
    inline const uint8_t& operator[] (uint8_t x) const __attribute__((always_inline))
    {
        return raw[x];
    }

    // default values are UNINITIALIZED
    inline CRGB() __attribute__((always_inline)) = default;

    /// allow construction from R, G, B
    inline CRGB( uint8_t ir, uint8_t ig, uint8_t ib)  __attribute__((always_inline))
        : r(ir), g(ig), b(ib)
    {
    }
	
     /// allow assignment from H, S, and V
	 CRGB& setHSV (uint8_t hue, uint8_t sat, uint8_t val) __attribute__((always_inline))
    {
        hsv2rgb_rainbow( CHSV(hue, sat, val), *this);
        return *this;
    }
				
    /// allow assignment from one RGB struct to another
	inline CRGB& operator= (const CRGB& rhs) __attribute__((always_inline)) = default;

    /// allow assignment from 32-bit (really 24-bit) 0xRRGGBB color code
	inline CRGB& operator= (const uint32_t colorcode) __attribute__((always_inline))
    {
        r = (colorcode >> 16) & 0xFF;
        g = (colorcode >>  8) & 0xFF;
        b = (colorcode >>  0) & 0xFF;
        return *this;
    }

    /// allow assignment from R, G, and B
	inline CRGB& setRGB (uint8_t nr, uint8_t ng, uint8_t nb) __attribute__((always_inline))
    {
        r = nr;
        g = ng;
        b = nb;
        return *this;
    }

    /// allow assignment from just a Hue, saturation and value automatically at max.
	inline CRGB& setHue (uint8_t hue) __attribute__((always_inline))
    {
        hsv2rgb_rainbow( CHSV(hue, 255, 255), *this);
        return *this;
    }

    /// allow assignment from HSV color
	inline CRGB& operator= (const CHSV& rhs) __attribute__((always_inline))
    {
        hsv2rgb_rainbow( rhs, *this);
        return *this;
    }

    /// allow assignment from 32-bit (really 24-bit) 0xRRGGBB color code
	inline CRGB& setColorCode (uint32_t colorcode) __attribute__((always_inline))
    {
        r = (colorcode >> 16) & 0xFF;
        g = (colorcode >>  8) & 0xFF;
        b = (colorcode >>  0) & 0xFF;
        return *this;
    }
};

/// This version of scale8 does not clean up the R1 register on AVR
/// If you are doing several 'scale8's in a row, use this, and
/// then explicitly call cleanup_R1.
uint8_t scale8_LEAVING_R1_DIRTY( uint8_t i, fract8 scale);

void hsv2rgb_rainbow( const CHSV& hsv, CRGB& rgb);

typedef enum { NOBLEND=0, LINEARBLEND=1 } TBlendType;

CRGB ColorFromPalette( const TProgmemRGBPalette16& pal,
                       uint8_t index,
                       uint8_t brightness=255,
                       TBlendType blendType=LINEARBLEND);


uint8_t random8();
uint8_t random8(int high);
uint8_t random8(int low, int high);
uint16_t random16(int high);
uint16_t random16(int low, int high);

uint8_t qadd8( uint8_t i, uint8_t j);
uint8_t qsub8( uint8_t i, uint8_t j);
uint8_t mod8( uint8_t a, uint8_t m);
uint8_t scale8(uint8_t i, uint8_t j); 
	
class CLEDController {
protected:
    friend class CFastLED;
    CRGB *m_Data;
    CLEDController *m_pNext;
    int m_nLeds;
    static CLEDController *m_pHead;
    static CLEDController *m_pTail;
public:
	/// create an led controller object, add it to the chain of controllers
    CLEDController() : m_Data(NULL), m_nLeds(0) {
        m_pNext = NULL;
        if(m_pHead==NULL) { m_pHead = this; }
        if(m_pTail != NULL) { m_pTail->m_pNext = this; }
        m_pTail = this;
    }
	
    /// get the first led controller in the chain of controllers
    static CLEDController *head() { return m_pHead; }
    /// get the next controller in the chain after this one.  will return NULL at the end of the chain
    CLEDController *next() { return m_pNext; }
	
    CLEDController & setLeds(CRGB *data, int nLeds) {
        m_Data = data;
        m_nLeds = nLeds;
        return *this;
    }
		virtual void clearLeds(int nLeds) {};
		virtual void showLeds(uint8_t brightness=255) = 0;
    virtual void show(const struct CRGB *data, int nLeds, uint8_t brightness) = 0;
};

class CPicoController : public CLEDController {
	
public:
	  CPicoController();
		
    /// show function using the "attached to this controller" led data
    void showLeds(uint8_t brightness=255) {
        show(m_Data, m_nLeds, brightness);
    }
    void show(const struct CRGB *data, int nLeds, uint8_t brightness);
};

template<uint8_t DATA_PIN> class NEOPIXEL : public CPicoController {
};

class CFastLED
{
public:
	/// Add a CLEDController instance to the world.  Exposed to the public to allow people to implement their own
	/// CLEDController objects or instances.  There are two ways to call this method (as well as the other addLeds)
	/// variations.  The first is with 3 arguments, in which case the arguments are the controller, a pointer to
	/// led data, and the number of leds used by this controller.  The second is with 4 arguments, in which case
	/// the first two arguments are the same, the third argument is an offset into the CRGB data where this controller's
	/// CRGB data begins, and the fourth argument is the number of leds for this controller object.
	/// @param pLed - the led controller being added
	/// @param data - base point to an array of CRGB data structures
	/// @param nLedsOrOffset - number of leds (3 argument version) or offset into the data array
	/// @param nLedsIfOffset - number of leds (4 argument version)
	/// @returns a reference to the added controller
	static CLEDController &addLeds(CLEDController *pLed, struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0);

	template<template<uint8_t DATA_PIN> class CHIPSET, uint8_t DATA_PIN>
	static CLEDController &addLeds(struct CRGB *data, int nLedsOrOffset, int nLedsIfOffset = 0) {
		static CHIPSET<DATA_PIN> c;
		return addLeds(&c, data, nLedsOrOffset, nLedsIfOffset);
	}
	
	
	/// Update all our controllers with the current led colors
	static void show();

	/// Set the dithering mode.  Sets the dithering mode for all added led strips, overriding
	/// whatever previous dithering option those controllers may have had.
	/// @param ditherMode - what type of dithering to use, either BINARY_DITHER or DISABLE_DITHER
	void setDither(uint8_t ditherMode = BINARY_DITHER) {};
};

extern CFastLED FastLED;

extern const TProgmemRGBPalette16 HeatColors_p;

#endif