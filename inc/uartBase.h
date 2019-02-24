/************************************************************************

name:           UART Base Library
desription:     Provides basic read/write operations over UART, specifically the ??? bus
                of the MSP432
auhor:          generalannoyance



Copyright (C) 2019  generalannoyance

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
************************************************************************/
#define __ENABLE_UCA0
#define __ENABLE_UCA1
#define __ENABLE_UCA2
#define __ENABLE_UCA3

#ifdef __ENABLE_UCA0
//-------------------------------
// EUSCI A0 Uses Pins:
//      P1.2 -> RX
//      P1.3 -> TX
void initUART_A0(void);
void transmitUART_A0(uint8_t data);
uint8_t receiveUART_A0(void);
#endif

#ifdef __ENABLE_UCA1
//-------------------------------
// EUSCI A1 Uses Pins:
//      P2.2 -> RX
//      P2.3 -> TX
void initUART_A1(void);
void transmitUART_A1(uint8_t data);
uint8_t receiveUART_A1(void);
#endif

#ifdef __ENABLE_UCA2
//-------------------------------
// EUSCI A2 Uses Pins:
//      P3.2 -> RX
//      P3.3 -> TX
void initUART_A2(void);
void transmitUART_A2(uint8_t data);
uint8_t receiveUART_A2(void);
#endif

#ifdef __ENABLE_UCA3
//-------------------------------
// EUSCI A3 Uses Pins:
//      P9.6 -> RX
//      P9.7 -> TX
void initUART_A3(void);
void transmitUART_A3(uint8_t data);
uint8_t receiveUART_A3(void);
#endif



