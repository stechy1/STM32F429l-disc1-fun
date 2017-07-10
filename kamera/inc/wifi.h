/*
*	==========================================================================
*   wifi.h
*
*   Description:
*   wifi file sn8200 chip.
*
*   This program is free software: you can redistribute it and/or modify
*   it under the terms of the GNU General Public License as published by
*   the Free Software Foundation, either version 3 of the License, or
*   any later version.
*
*   This program is distributed in the hope that it will be useful,
*   but WITHOUT ANY WARRANTY; without even the implied warranty of
*   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
*   GNU General Public License for more details.
*
*   You should have received a copy of the GNU General Public License
*   along with this program.  If not, see <http://www.gnu.org/licenses/>.
*	==========================================================================
*/


#ifndef WIFI_H
#define WIFI_H

#include "sn8200_api.h"
#include "sn8200_core.h"

#define MSG "Hello"
#define IP_ADR "172.31.0.13"
#define IP_PORT "333"

int initWIFI(void);
int connectWIFI(int8_t* mySock);

#endif    //WIFI_H
