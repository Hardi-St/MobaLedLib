#ifndef __INC_FASTPIN_H
#define __INC_FASTPIN_H

#include <stddef.h>

#ifndef FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_BEGIN
#define FASTLED_NAMESPACE_END
#define FASTLED_USING_NAMESPACE
#endif

typedef volatile uint32_t RoReg;
typedef volatile uint32_t RwReg;
typedef unsigned long prog_uint32_t;

#include <hardware/gpio.h>

#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wignored-qualifiers"

///@file fastpin.h
/// Class base definitions for defining fast pin access

FASTLED_NAMESPACE_BEGIN

#define NO_PIN 255

//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
//
// Pin access class - needs to tune for various platforms (naive fallback solution?)
//
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

class Selectable {
public:
	virtual void select() = 0;
	virtual void release() = 0;
	virtual bool isSelected() = 0;
};

#if !defined(FASTLED_NO_PINMAP)

class Pin : public Selectable {
	volatile RwReg *mPort;
	volatile RoReg *mInPort;
	RwReg mPinMask;
	uint8_t mPin;

	void _init() {
		mPinMask = digitalPinToBitMask(mPin);
		mPort = (volatile RwReg*)portOutputRegister(digitalPinToPort(mPin));
		mInPort = (volatile RoReg*)portInputRegister(digitalPinToPort(mPin));
	}

public:
	Pin(int pin) : mPin(pin) { _init(); }

	typedef volatile RwReg * port_ptr_t;
	typedef RwReg port_t;

	inline void setOutput() { pinMode(mPin, OUTPUT); }
	inline void setInput() { pinMode(mPin, INPUT); }

	inline void hi() __attribute__ ((always_inline)) { *mPort |= mPinMask; }
	inline void lo() __attribute__ ((always_inline)) { *mPort &= ~mPinMask; }

	inline void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }
	inline void toggle() __attribute__ ((always_inline)) { *mInPort = mPinMask; }

	inline void hi(port_ptr_t port) __attribute__ ((always_inline)) { *port |= mPinMask; }
	inline void lo(port_ptr_t port) __attribute__ ((always_inline)) { *port &= ~mPinMask; }
	inline void set(port_t val) __attribute__ ((always_inline)) { *mPort = val; }

	inline void fastset(port_ptr_t port, port_t val) __attribute__ ((always_inline)) { *port  = val; }

/*	port_t hival() __attribute__ ((always_inline)) { return *mPort | mPinMask;  }
	port_t loval() __attribute__ ((always_inline)) { return *mPort & ~mPinMask; }
	port_ptr_t  port() __attribute__ ((always_inline)) { return mPort; }
	port_t mask() __attribute__ ((always_inline)) { return mPinMask; }*/

	virtual void select() { hi(); }
	virtual void release() { lo(); }
	virtual bool isSelected() { return (*mPort & mPinMask) == mPinMask; }
};

class OutputPin : public Pin {
public:
	OutputPin(int pin) : Pin(pin) { setOutput(); }
};

class InputPin : public Pin {
public:
	InputPin(int pin) : Pin(pin) { setInput(); }
};

#else
// This is the empty code version of the raw pin class, method bodies should be filled in to Do The Right Thing[tm] when making this
// available on a new platform
class Pin : public Selectable {
	volatile RwReg *mPort;
	volatile RoReg *mInPort;
	RwReg mPinMask;
	uint8_t mPin;

	void _init() {
		// TODO: fill in init on a new platform
		mPinMask = 0;
		mPort = NULL;
		mInPort = NULL;
	}

public:
	Pin(int pin) : mPin(pin) { _init(); }

	void setPin(int pin) { mPin = pin; _init(); }

	typedef volatile RwReg * port_ptr_t;
	typedef RwReg port_t;

	inline void setOutput() { /* TODO: Set pin output */ }
	inline void setInput() { /* TODO: Set pin input */ }

	inline void hi() __attribute__ ((always_inline)) { *mPort |= mPinMask; }
	inline void lo() __attribute__ ((always_inline)) { *mPort &= ~mPinMask; }

	inline void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }
	inline void toggle() __attribute__ ((always_inline)) { *mInPort = mPinMask; }

	inline void hi(port_ptr_t port) __attribute__ ((always_inline)) { *port |= mPinMask; }
	inline void lo(port_ptr_t port) __attribute__ ((always_inline)) { *port &= ~mPinMask; }
	inline void set(port_t val) __attribute__ ((always_inline)) { *mPort = val; }

	inline void fastset(port_ptr_t port, port_t val) __attribute__ ((always_inline)) { *port  = val; }

	port_t hival() __attribute__ ((always_inline)) { return *mPort | mPinMask;  }
	port_t loval() __attribute__ ((always_inline)) { return *mPort & ~mPinMask; }
	port_ptr_t  port() __attribute__ ((always_inline)) { return mPort; }
	port_t mask() __attribute__ ((always_inline)) { return mPinMask; }

	virtual void select() { hi(); }
	virtual void release() { lo(); }
	virtual bool isSelected() { return (*mPort & mPinMask) == mPinMask; }
};

class OutputPin : public Pin {
public:
	OutputPin(int pin) : Pin(pin) { setOutput(); }
};

class InputPin : public Pin {
public:
	InputPin(int pin) : Pin(pin) { setInput(); }
};

#endif

/// The simplest level of Pin class.  This relies on runtime functions durinig initialization to get the port/pin mask for the pin.  Most
/// of the accesses involve references to these static globals that get set up.  This won't be the fastest set of pin operations, but it
/// will provide pin level access on pretty much all arduino environments.  In addition, it includes some methods to help optimize access in
/// various ways.  Namely, the versions of hi, lo, and fastset that take the port register as a passed in register variable (saving a global
/// dereference), since these functions are aggressively inlined, that can help collapse out a lot of extraneous memory loads/dereferences.
///
/// In addition, if, while writing a bunch of data to a pin, you know no other pins will be getting written to, you can get/cache a value of
/// the pin's port register and use that to do a full set to the register.  This results in one being able to simply do a store to the register,
/// vs. the load, and/or, and store that would be done normally.
///
/// There are platform specific instantiations of this class that provide direct i/o register access to pins for much higher speed pin twiddling.
///
/// Note that these classes are all static functions.  So the proper usage is Pin<13>::hi(); or such.  Instantiating objects is not recommended,
/// as passing Pin objects around will likely -not- have the effect you're expecting.
#ifdef FASTLED_FORCE_SOFTWARE_PINS
template<uint8_t PIN> class FastPin {
	static RwReg sPinMask;
	static volatile RwReg *sPort;
	static volatile RoReg *sInPort;
	static void _init() {
#if !defined(FASTLED_NO_PINMAP)
		sPinMask = digitalPinToBitMask(PIN);
		sPort = portOutputRegister(digitalPinToPort(PIN));
		sInPort = portInputRegister(digitalPinToPort(PIN));
#endif
	}

public:  
	typedef volatile RwReg * port_ptr_t;
	typedef RwReg port_t;

	inline static void setOutput() { _init(); pinMode(PIN, OUTPUT); }
	inline static void setInput() { _init(); pinMode(PIN, INPUT); }

	inline static void hi() __attribute__ ((always_inline)) { *sPort |= sPinMask; }
	inline static void lo() __attribute__ ((always_inline)) { *sPort &= ~sPinMask; }

	inline static void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }

	inline static void toggle() __attribute__ ((always_inline)) { *sInPort = sPinMask; }

	inline static void hi(port_ptr_t port) __attribute__ ((always_inline)) { *port |= sPinMask; }
	inline static void lo(port_ptr_t port) __attribute__ ((always_inline)) { *port &= ~sPinMask; }
	inline static void set(port_t val) __attribute__ ((always_inline)) { *sPort = val; }

	inline static void fastset(port_ptr_t port, port_t val) __attribute__ ((always_inline)) { *port  = val; }

	static port_t hival() __attribute__ ((always_inline)) { return *sPort | sPinMask;  }
	static port_t loval() __attribute__ ((always_inline)) { return *sPort & ~sPinMask; }
	static port_ptr_t  port() __attribute__ ((always_inline)) { return sPort; }
	static port_t mask() __attribute__ ((always_inline)) { return sPinMask; }
};

template<uint8_t PIN> RwReg FastPin<PIN>::sPinMask;
template<uint8_t PIN> volatile RwReg *FastPin<PIN>::sPort;
template<uint8_t PIN> volatile RoReg *FastPin<PIN>::sInPort;

#else

template<uint8_t PIN> class FastPin {
	constexpr static bool validpin() { return false; }

	static_assert(validpin(), "Invalid pin specified");

	static void _init() { }

public:
	typedef volatile RwReg * port_ptr_t;
	typedef RwReg port_t;

	inline static void setOutput() { }
	inline static void setInput() { }

	inline static void hi() __attribute__ ((always_inline)) { }
	inline static void lo() __attribute__ ((always_inline)) { }

	inline static void strobe() __attribute__ ((always_inline)) { }

	inline static void toggle() __attribute__ ((always_inline)) { }

	inline static void hi(port_ptr_t port) __attribute__ ((always_inline)) { }
	inline static void lo(port_ptr_t port) __attribute__ ((always_inline)) { }
	inline static void set(port_t val) __attribute__ ((always_inline)) { }

	inline static void fastset(port_ptr_t port, port_t val) __attribute__ ((always_inline)) { }

	static port_t hival() __attribute__ ((always_inline)) { return 0; }
	static port_t loval() __attribute__ ((always_inline)) { return 0;}
	static port_ptr_t  port() __attribute__ ((always_inline)) { return NULL; }
	static port_t mask() __attribute__ ((always_inline)) { return 0; }
};

#endif

template<uint8_t PIN> class FastPinBB : public FastPin<PIN> {};

typedef volatile uint32_t & reg32_t;
typedef volatile uint32_t * ptr_reg32_t;

// Utility templates for tracking down information about pins and ports
template<uint8_t port> struct __FL_PORT_INFO {
	static bool hasPort() { return 0; }
	static const char *portName() { return "--"; }
	static const void *portAddr() { return NULL; }
};

// Give us our instantiations for defined ports - we're going to abuse this later for
// auto discovery of pin/port mappings for new variants.  Use _FL_DEFINE_PORT for ports that
// are numeric in nature, e.g. GPIO0, GPIO1.  Use _FL_DEFINE_PORT3 for ports that are letters.
// The first parameter will be the letter, the second parameter will be an integer/counter of smoe kind
// (this is because attempts to turn macro parameters into character constants break in some compilers)
#define _FL_DEFINE_PORT(L, BASE) template<> struct __FL_PORT_INFO<L> { \
	static bool hasPort() { return 1; } \
	static const char *portName() { return #L; } \
	typedef BASE __t_baseType;  \
	static const void *portAddr() { return (void*)&__t_baseType::r(); } };

#define _FL_DEFINE_PORT3(L, LC, BASE) template<> struct __FL_PORT_INFO<LC> { \
	static bool hasPort() { return 1; } \
	static const char *portName() { return #L; } \
	typedef BASE __t_baseType;  \
	static const void *portAddr() { return (void*)&__t_baseType::r(); } };

FASTLED_NAMESPACE_END

#pragma GCC diagnostic pop

#endif // __INC_FASTPIN_H

#pragma once

FASTLED_NAMESPACE_BEGIN

template<uint8_t PIN, uint32_t MASK> class _PICOPIN {
public:
  typedef volatile uint32_t * port_ptr_t;
  typedef uint32_t port_t;

  inline static void setOutput() { pinMode(PIN, OUTPUT); }
  inline static void setInput() { pinMode(PIN, INPUT); }

  inline static void hi() __attribute__ ((always_inline)) {
		gpio_set_mask(MASK);  
	}

  inline static void lo() __attribute__ ((always_inline)) {
		gpio_clr_mask(MASK);  
  }

  inline static void set(port_t val) __attribute__ ((always_inline)) {
		if (val)
				gpio_set_mask(MASK);  
		else
				gpio_clr_mask(MASK);  
  }

  inline static void strobe() __attribute__ ((always_inline)) { toggle(); toggle(); }

  inline static void toggle() __attribute__ ((always_inline)) {
		set(gpio_get(PIN));
  }

  inline static void hi(port_ptr_t port) __attribute__ ((always_inline)) { hi(); }
  inline static void lo(port_ptr_t port) __attribute__ ((always_inline)) { lo(); }
  inline static void fastset(port_ptr_t port, port_t val) __attribute__ ((always_inline)) { *port = val; }

/*  inline static port_t hival() __attribute__ ((always_inline)) {
      if (PIN < 32) return GPIO.out | MASK;
      else return GPIO.out1.val | MASK;
  }

  inline static port_t loval() __attribute__ ((always_inline)) {
      if (PIN < 32) return GPIO.out & ~MASK;
      else return GPIO.out1.val & ~MASK;
  }

  inline static port_ptr_t port() __attribute__ ((always_inline)) {
      if (PIN < 32) return &GPIO.out;
      else return &GPIO.out1.val;
  }

  inline static port_ptr_t sport() __attribute__ ((always_inline)) {
      if (PIN < 32) return &GPIO.out_w1ts;
      else return &GPIO.out1_w1ts.val;
  }

  inline static port_ptr_t cport() __attribute__ ((always_inline)) {
      if (PIN < 32) return &GPIO.out_w1tc;
      else return &GPIO.out1_w1tc.val;
  }

  inline static port_t mask() __attribute__ ((always_inline)) { return MASK; }
*/
  inline static bool isset() __attribute__ ((always_inline)) {
		 return gpio_get(PIN);
  }
};

#define _FL_DEFPIN(PIN)  template<> class FastPin<PIN> : public _PICOPIN<PIN, 1ul << PIN> {};

_FL_DEFPIN(0);
_FL_DEFPIN(1);
_FL_DEFPIN(2);
_FL_DEFPIN(3);
_FL_DEFPIN(4);
_FL_DEFPIN(5);
_FL_DEFPIN(6);
_FL_DEFPIN(7);
_FL_DEFPIN(8);
_FL_DEFPIN(9);
_FL_DEFPIN(10);
_FL_DEFPIN(11);
_FL_DEFPIN(12);
_FL_DEFPIN(13);
_FL_DEFPIN(14);
_FL_DEFPIN(15);
_FL_DEFPIN(16);
_FL_DEFPIN(17);
_FL_DEFPIN(18);
_FL_DEFPIN(19);
_FL_DEFPIN(20);
_FL_DEFPIN(21);
_FL_DEFPIN(22);
_FL_DEFPIN(23);
_FL_DEFPIN(24);
_FL_DEFPIN(25);
_FL_DEFPIN(26);
_FL_DEFPIN(27);
_FL_DEFPIN(28);
_FL_DEFPIN(29);

#define HAS_HARDWARE_PIN_SUPPORT

FASTLED_NAMESPACE_END

	static inline void pinMode2(uint8_t pin, uint8_t val) { pinMode(pin, val);}
	static inline void digitalWrite2(uint8_t pin, uint8_t val) { uint32_t mask = 1ul << pin; if (val) gpio_set_mask(mask); else gpio_clr_mask(mask); }
	static inline uint8_t digitalRead2(uint8_t pin) { return gpio_get(pin);}
