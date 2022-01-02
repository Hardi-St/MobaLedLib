#ifndef COMMINTERFACE_H
#define COMMINTERFACE_H

/*
MobaLedLib: LED library for model railways
~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

Copyright (C) 2021  Jürgen Winkler: MobaLedLib@a1.net

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

  Base class for MLL external communication 
  A concrete interface implementation is needed as a derived class
  
  
Revision History :
~~~~~~~~~~~~~~~~~
06.12.21:  Versions 1.0 (Jürgen)
02.01.22:  Juergen add support for DCC receive on LED Arduino  
*/

#if !defined(__AVR__)                                                                                      // 02.01.22: Juergen add support for DCC receive on LED Arduino
  #include "InMemoryStream.h"
#else
  #include <Arduino.h>
#endif

class CommInterface
{
private:
#if !defined(__AVR__)                                                                                      // 02.01.22: Juergen add support for DCC receive on LED Arduino
  static InMemoryStream* pStream;
#endif  

public:
  virtual void	process();

// dual core CPUs use a stream to exchange data between cores
#if !defined(__AVR__)                                                                                     // 02.01.22: Juergen add support for DCC receive on LED Arduino
  virtual void	setup(int statusLedPin, InMemoryStream& stream);
  static void   addToSendBuffer(const char *s);
#else
  virtual void	setup(int statusLedPin);
#endif
  static void   setLastSignalTime(unsigned long lastSignalTime);

private:	
  static void   processErrorLed();
};

#endif