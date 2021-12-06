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
  
*/

#include "InMemoryStream.h"

class CommInterface
{
private:
  static InMemoryStream* pStream;

public:
  virtual void	setup(int statusLedPin, InMemoryStream& stream);
  virtual void	process();
  static void   addToSendBuffer(const char *s);
  static void   setLastSignalTime(uint32_t lastSignalTime);

private:	
  static void   processErrorLed();
};

#endif