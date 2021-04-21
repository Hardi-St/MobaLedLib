/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2020  Jürgen Winkler: MobaLedLib@a1.net

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
 
  Base class for MLL Zentral User Interfaces and also a adapter class
	
	it defines a weak function getUserInterface. 
	A concrete user interface implementation must return its own instance pointer as a result of getUserInterface.
	
	
Revision History :
~~~~~~~~~~~~~~~~~
10.11.20:  Versions 1.0 (Jürgen)
	
*/

#ifndef USERINTERFACE_H
#define USERINTERFACE_H

#include "Arduino.h"

class UserInterface 
{
private:
	UserInterface* ui;
protected:
	UserInterface();
public:		
	// pass a pointer to the getUserInterface function 
	// may by null to disable the user interface
	UserInterface(UserInterface* (*getUserInterface)());
	virtual void setup();
	virtual void loop();
	virtual void setConnected(bool connected);
	virtual void setIPAddress(String adress);
};

UserInterface* getUserInterface() __attribute__((weak));

#endif

