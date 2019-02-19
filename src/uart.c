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
#include "../inc/uart.h"

/*
void set_bpm_timer(float);                 //function prototypes
void UARTO_init(void);
void UART2_init(void);                      //UART A2 3.2-RX, 3.3-TX


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
void UART2_init(void){
    EUSCI_A2->CTLW0|=1;         //reset mode
    EUSCI_A2->MCTLW=0;          //disable over-sampling
    EUSCI_A2->CTLW0 = 0x0081;   //1 stop bit,no parity, SMCLK,8bit data
    EUSCI_A2->BRW = 312;        //3000000/11500
    P3->SEL0 |= 0x0C;           //p3.3, p3.2 for UART
    P3->SEL1 &= ~0x0C;
    EUSCI_A2->CTLW0&= ~1;       //disable reset mode
    EUSCI_A2->IE|= 1;           //enabling interrupt
}

void EUSCIA2_IRQHandler(void){
    static int j = 0;

    inByte = EUSCI_A2->RXBUF;       //receiving byte
    if((inByte!='\0')&&(j<340)){    //if we're not at the end of the string
    received_com[j] = inByte;       //load array
    j++;
    }
    else{
    received_com[j] = '\0';         //end of string, set flag
    serial_flag = 1;
    j =0;
    }
}

int parseCommand(void)
{
    char *tok;
    char recmsg[300];
    char dataSize[6];
    char fileName[28];
    int i, l, number;
    int index;
    uint32_t filesize;

               tok = strtok(received_com," /0");
               l = strlen(tok);                             //tokenizing first word of command
               if(strcmp(tok,"dir")==0){                    //directory
                   for(i=0;i<l; i++){
                       recmsg[i] = tok[i];                  //reading incoming bytes
                       while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                       EUSCI_A2->TXBUF = tok[i];            //echoing back the received bytes
                   }
                   while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                   EUSCI_A2->TXBUF ='\0';               //sending end of string character
                   serial_flag = 0;
               }

               else if(strcmp(tok,"mem")==0){               //memory
                   for(i=0;i<l; i++){
                       recmsg[i] = tok[i];                  //reading incoming bytes
                       while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                       EUSCI_A2->TXBUF = tok[i];            //echoing back the received bytes
                   }
                   while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                   EUSCI_A2->TXBUF ='\0';               //sending end of string character
                   serial_flag = 0;
               }

               else if(strcmp(tok,"clear")==0){
                   for(i=0;i<l; i++){
                       while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                       EUSCI_A2->TXBUF = tok[i];            //echoing back the received bytes
                   }
                   while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                   EUSCI_A2->TXBUF ='\0';               //sending end of string character
                   serial_flag = 0;
               }

               else if(strcmp(tok,"read")==0){           //tokenizing 2nd command word
                   for(i=0;i<l; i++){
                       recmsg[i] = tok[i];
                       while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                       EUSCI_A2->TXBUF = tok[i];            //echoing back the received bytes
                   }
                       index = atoi(tok[5]);

                       tok = strtok(NULL," \0");            //tokenizing number
                       if(tok != NULL)
                         {
                           l = strlen(tok);

                           for(i=0;i<l; i++){
                               dataSize[i] = tok[i];
                               while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                               EUSCI_A2->TXBUF = dataSize[i];        //echoing back the received bytes
                           }
                           number = myAtoi(dataSize);
                          }
                       while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                       EUSCI_A2->TXBUF ='\n';
                       while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                       EUSCI_A2->TXBUF ='\0';               //sending end of string character
                       serial_flag = 0;
                   }

                else if(strcmp(tok,"delete")==0){           //tokenizing 2nd command word
                   for(i=0;i<l; i++){
                       recmsg[i] = tok[i];
                        while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                        EUSCI_A2->TXBUF = tok[i];
                       }
                       index = atoi(tok[7]);

                       tok = strtok(NULL," \0");            //tokenizing number
                       if(tok != NULL)
                        {
                           l = strlen(tok);

                           for(i=0;i<l; i++){
                               dataSize[i] = tok[i];
                               while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                               EUSCI_A2->TXBUF = dataSize[i];
                               }
                            number = myAtoi(dataSize);
                            }
                           while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                           EUSCI_A2->TXBUF ='\n';
                           while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                           EUSCI_A2->TXBUF ='\0';               //sending end of string character
                           serial_flag = 0;

                    }

                 else if(strcmp(tok,"store")==0){           //tokenizing 2nd command word
                   for(i=0;i<l; i++){
                         recmsg[i] = tok[i];
                        while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                        EUSCI_A2->TXBUF = tok[i];
                       }
                   while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                   EUSCI_A2->TXBUF ='\n';

                   tok = strtok(NULL," \0");
                    if(tok != NULL)
                    {
                        l = strlen(tok);

                        for(i=0;i<l; i++){
                         dataSize[i] = tok[i];
                         while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                         EUSCI_A2->TXBUF = dataSize[i];
                        }
                        number = myAtoi(dataSize);
                   }
                    while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                    EUSCI_A2->TXBUF ='\n';

                    tok = strtok(NULL," \0");
                     if(tok != NULL)
                     {
                         l = strlen(tok);

                         for(i=0;i<l; i++){
                          fileName[i] = tok[i];
                          while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                          EUSCI_A2->TXBUF = fileName[i];
                         }
                    }
                     while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                     EUSCI_A2->TXBUF ='\n';

                       tok = strtok(NULL,"\0");
                        if(tok != NULL)
                        {
                            l = strlen(tok);

                            for(i=0;i<l; i++){
                             recmsg[i] = tok[i];
                             while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                             EUSCI_A2->TXBUF = recmsg[i];
                            }
                       }

                    while(!(EUSCI_A2->IFG & 0x02)){};    //waiting for tx buffer to empty
                    EUSCI_A2->TXBUF ='\0';               //sending end of string character
                    serial_flag = 0;
                    }
}


int myAtoi(char *str)
{
    // A simple atoi() function

    int i;
    int res = 0; // Initialize result

    // Iterate through all characters of input string and
    // update result
    for ( i = 0; str[i] != '\0'; ++i)
        res = res*10 + str[i] - '0';

    // return result.
    return res;
}
