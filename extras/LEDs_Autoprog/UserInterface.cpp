/*
 MobaLedLib: LED library for model railways
 ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

 Copyright (C) 2018 - 2020  Hardi Stengelin: MobaLedLib@gmx.de
 
 this file: Copyright (C) 2020 Jürgen Winkler: MobaLedLib@a1.net

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
 
 
 Revision History :
~~~~~~~~~~~~~~~~~
10.11.20:  Versions 1.0 (Jürgen)

*/
#ifdef ESP32

#include "UserInterface.h"

UserInterface::UserInterface()
{
}

UserInterface::UserInterface(UserInterface* (*getUserInterface)()) {
	if (getUserInterface) ui = getUserInterface();
}

void UserInterface::setup()
{
	if (ui) ui->setup();
}

void UserInterface::loop()
{
	if (ui) ui->loop();
}

void UserInterface::setConnected(bool connected)
{
	if (ui) ui->setConnected(connected);
}

void UserInterface::setIPAddress(String adress)
{
	if (ui) ui->setIPAddress(adress);
}

void UserInterface::setCounters(int delayCount, int reviveCount)
{
	if (ui) ui->setCounters(delayCount, reviveCount);
}
#endif