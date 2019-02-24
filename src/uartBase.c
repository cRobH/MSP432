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

#include "msp.h"
#include "string.h"
/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "../inc/uartBase.h"

/*
 * To enable interrupts, use the following commands:
 *      __disable_irq();
 *      NVIC_SetPriority(EUSCIAx_IRQn, 3);
 *      NVIC_EnableIRQ(EUSCIAx_IRQn);
 *      __enable_irq();
 *
 *  Where x is the EUSCI interface you are trying to interrupt
 */


//----------------------------------------------------------------------------
#ifdef __ENABLE_UCA0
void initUART_A0(void){
    UCA1CTLW0   |=  1;          // reset mode
    UCA1MCTLW    =  0;          // disable over-sampling
    UCA1CTLW0    =  0x0081;     // 1 stop bit,no parity, SMCLK,8bit data
    UCA1BRW      =  312;        // 3000000/9600
    P1SEL0      |=  0x0C;       // p1.2, p1.3 for UART
    P1SEL1      &=  ~0x0C;
    UCA1CTLW0   &=  ~0x01;      //disable reset mode
    UCA1IE      |=  0x01;       //enabling interrupt
    return;
}

void transmitUART_A0(uint8_t data){

    while( !(UCA0IFG & 0x02) );  // Wait for transmit buffer to empty

    UCA0TXBUF = data;           // move data into transmit buffer

    while( !(UCA0IFG & 0x02) );    // Wait for transmit to finish

    return;
}
uint8_t receiveUART_A0(void){
    return UCA0RXBUF;
}
#endif
//----------------------------------------------------------------------------
#ifdef __ENABLE_UCA1
void initUART_A1(void){
    UCA1CTLW0   |=  1;          // reset mode
    UCA1MCTLW    =  0;          // disable over-sampling
    UCA1CTLW0    =  0x0081;     // 1 stop bit,no parity, SMCLK,8bit data
    UCA1BRW      =  312;        // 3000000/9600
    P2SEL0      |=  0x0C;       //p9.6, p9.7 for UART
    P2SEL1      &=  ~0x0C;
    UCA1CTLW0   &=  ~0x01;      //disable reset mode
    UCA1IE      |=  0x01;       //enabling interrupt
    return;
}

void transmitUART_A1(uint8_t data){

    while( !(UCA1IFG & 0x02) );  // Wait for transmit buffer to empty

    UCA3TXBUF = data;           // move data into transmit buffer

    while( !(UCA1IFG & 0x02) );    // Wait for transmit to finish

    return;
}
uint8_t receiveUART_A1(void){
    return UCA1RXBUF;
}
#endif
//----------------------------------------------------------------------------
#ifdef __ENABLE_UCA2
void initUART_A2(void){
    UCA2CTLW0   |=  1;          // reset mode
    UCA2MCTLW    =  0;          // disable over-sampling
    UCA2CTLW0    =  0x0081;     // 1 stop bit,no parity, SMCLK,8bit data
    UCA2BRW      =  312;        // 3000000/9600
    P3SEL0      |=  0x0C;       //p9.6, p9.7 for UART
    P3SEL1      &=  ~0x0C;
    UCA2CTLW0   &=  ~0x01;      //disable reset mode
    UCA2IE      |=  0x01;       //enabling interrupt
    return;
}

void transmitUART_A2(uint8_t data){

    while( !(UCA2IFG & 0x02) );  // Wait for transmit buffer to empty

    UCA2TXBUF = data;           // move data into transmit buffer

    while( !(UCA2IFG & 0x02) );    // Wait for transmit to finish

    return;
}
uint8_t receiveUART_A2(void){
    return UCA2RXBUF;
}
#endif
//----------------------------------------------------------------------------
#ifdef __ENABLE_UCA3
void initUART_A3(void){
    UCA3CTLW0   |=  1;          // reset mode
    UCA3MCTLW    =  0;          // disable over-sampling
    UCA3CTLW0    =  0x0081;     // 1 stop bit,no parity, SMCLK,8bit data
    UCA3BRW      =  312;        // 3000000/9600
    P9SEL0      |=  0xC0;       //p9.6, p9.7 for UART
    P9SEL1      &=  ~0xC0;
    UCA3CTLW0   &=  ~0x01;      //disable reset mode
    UCA3IE      =  0x01;       //enable Receive Interrupt
    return;
}


void transmitUART_A3(uint8_t data){

    while( !(UCA3IFG & 0x02) );  // Wait for transmit buffer to empty

    UCA3TXBUF = data;           // move data into transmit buffer

    while( !(UCA3IFG & 0x02) );    // Wait for transmit to finish

    return;
}
uint8_t receiveUART_A3(void){
    return UCA3RXBUF;
}
#endif

