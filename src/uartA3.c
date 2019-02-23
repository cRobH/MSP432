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
#include "../inc/uartA3.h"

/*
void set_bpm_timer(float);                 //function prototypes
void UARTO_init(void);
void UART2_init(void);                      //UART A9.3-RX, 9.7-TX


int main(void){
    int k;
    float BPM;
    MAP_WDT_A_holdTimer();                  //disabling watch dog timer
    __disable_irq();                        //temporarily disabling interrupts

    UART2_init();


    P2 -> SEL1 &= ~2;                       //P2.1 as GPIO
    P2 -> SEL0 &= ~2;
    P2 -> DIR |=2;                          //P2.1 as output
    serial_flag = 0;                        //initializing serial flag to 0


    NVIC_SetPriority(EUSCIA2_IRQn, 3);
    NVIC_EnableIRQ(EUSCIA2_IRQn);
   __enable_irq();

   while(1)                                //while-loop
    {

        if(serial_flag ==1)
        {
            parseCommand();                 //parsing function
            serial_flag = 0;
        }
    }
}

*/
void initUART_A3(void){
    EUSCI_A3->CTLW0|=1;         //reset mode
    EUSCI_A3->MCTLW=0;          //disable over-sampling
    EUSCI_A3->CTLW0 = 0x0081;   //1 stop bit,no parity, SMCLK,8bit data
    EUSCI_A3->BRW = 312;        //3000000/11500
    P9->SEL0 |= 0xC0;           //p3.3, p3.2 for UART
    P9->SEL1 &= ~0xC0;
    EUSCI_A3->CTLW0&= ~1;       //disable reset mode
    EUSCI_A3->IE|= 1;           //enabling interrupt
}

void EUSCIA3_IRQHandler(void){


}

