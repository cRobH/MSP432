/************************************************************************

name:           SPI Base Library
desription:     Provides basic read/write operations over SPI, specifically the UCB0 bus
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

// Standard Includes
#include <stdint.h>
#include <stdbool.h>
#include "MSP.h"

#include "../inc/spiBase.h"

// This assumes the following settings:
/*//      SPI PINS

    MISO    P1.7    UCB0SOMI -- master rx
    MOSI    P1.6    UCB0SIMO -- master tx
    CLK     P1.5    UCB0CLK

    CS      P2.3    GPIO
*********************************************/



void initSPI(void){
    // UCB0 CONTROL WORD REGISTER 0
    UCB0CTLW0 = 0x0001; // Hold SPI bus in reset state while
    /*
     *  Clock Phase Select      0   15      Data captured on rising edge, changed on following
     *  Clock Polarity Select   1   14      inactive state is high
     *  MSB First/Last          1   13      MSB first
     *  Character Length        0   12      8 bit
     *
     *  Master Mode Select      1   11      Master
     *  SPI Mode                0   10      SPI Mode 0 - 3 Pin
     *  SPI Mode                0   9       SPI Mode 0 - 3 Pin
     *  Synchronus Mode         1   8       Synchronus Mode
     *
     *  Clock Source Select     0   7       SMCLK or 1
     *  Clock Source Select     1   6       SMCLK    1 for ACLK
     *  Reserved                -   5
     *  Reserved                -   4
     *
     *  Reserved                -   3
     *  Reserved                -   2
     *  STE Mode In Master      0   1       Ignored in 3 wire mode
     *  Software Reset Enable   1   0       SPI Is disable (for now)
     *
     */

    UCB0CTLW0 = 0x6941;

    // UCB0 BIT RATE CONTROL WORD REGISTER
    // 3 MHz / 1 = 3 MHz
    // ( assuming SMCLK is 3Mhz )
    UCB0BRW = 0x0001;

    // UCB0 INTERRUPT ENABLE REGISTER
    UCB0IE |= 0x01; // Enable RX Interrupt
    UCB0IFG &= ~0x01; // Clear RX Interrupt Flag

//------
    UCB0CTLW0 &= ~0x0001; // Enable the SPI bus
//------
    P1SEL0 |=  0xE0; // Set P1.5, 1.6, & 1.7 as SPI
    P1SEL1 &= ~0xE0;

    P2SEL0 &= ~0x08; // Set P2.3 as GPIO
    P2SEL1 &= ~0x08; // This is the CS pin
    P2DIR |=   0x08;
    P2OUT |=   0x08;
}

void setCS(int state){
    if( !state ){
        P2OUT |= 0x08;
    }
    if( state ){
        P2OUT &= ~0x08;
    }
}

void transmitSPI(uint8_t data){

    //setCS(1);
    while( !(UCB0IFG & 2) ); // Wait for Transmit buffer to empty

    UCB0TXBUF = data; //TRANSMIT BUFFER

    while( UCB0STATW & 1 ); // Wait for transmit to finish
    //setCS(0);
}

inline int  checkForReceived(void){
    return (UCB0IFG & 0x01);
}

inline void clearRxFlag(void){
    UCB0IFG &= ~0x01;
}

uint8_t receiveSPI(void){
    uint8_t rxData = 0;
    transmitSPI(0x00);
    rxData = UCB0RXBUF;
    clearRxFlag();
    return rxData;
}
