/************************************************************************

name:           MB85RS64V FRAM Library
desription:     Expands off the spiBase files to provide support for the MB85RS64V Ferroelectric RAM IC
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
#include "driverlib.h"
#include <stdbool.h>

#include "../inc/fram.h"
#include "../inc/spiBase.h"


void writeDataToFRAM(struct FRAM_data writeInfo){
    // Gather the requisite data:
    uint16_t sStartAdr  = writeInfo.startAdr;
    char     *sDataAdr  = writeInfo.dataAdr;
    int      sLength    = writeInfo.length;

    // We have to send the address in two eight bit chunks:
    uint16_t sStartAdrUpper = ( sStartAdr & 0xFF00 ) >> 8;
    uint16_t sStartAdrLower = ( sStartAdr & 0x00FF );

    // Some rambling to help me figure out how memory works
    // Each memory address holds 8 bits; to find the ending
    // memory address, we need to know the number of 8 bit chunks of data
    // Each 8 bit chunk of data holds one byte
//  uint16_t sEndingAdr = sStartAdr + sLength;
    // e.g. if we want to store 0b00, 0b01, 0b10, and 0b11, starting at 0x00:
    //  0x00 - 0b00
    //  0x01 - 0b01
    //  0x02 - 0b10;
    //  0x03 - 0b11;


    int i = 0;
    char *curDataAdr;


    //------------------- let's get down to business
    setCS(1);                               // MSP432: "hello there, general kenoFRAM"
    transmitSPI(WRITE);                     // transmit write op code
    transmitSPI(sStartAdrUpper);            // upper half of starting address
    transmitSPI(sStartAdrLower);            // lower half of starting address
    for(i = 0; i <= sLength; i++){
        curDataAdr = sDataAdr + i;          // based on the index i, we cycle through the input data array
        uint8_t dataToTX = *curDataAdr;     // we deref curDataAdr to get the data at that location
        transmitSPI(dataToTX);              // send it into the abyss
    }
    setCS(0);                               // tell the FRAM we're done with it

    return;

    /***************************************************************
     * Example of how to use this struct and func

        uint16_t writeAdr = 0x00FF;
        char testString[11] = {'h','e','l','l','o',' ','w','o','r','l','d'};
        int length = 11;

        struct FRAM_data entry1;
            entry1.startAdr = writeAdr;
            entry1.dataAdr = &testString;
            entry1.length = length;
        writeDataToFRAM(entry1);

    ************************************************************/
}
void readFRAMData(struct FRAM_data readInfo){
    // Gather the requisite data:
    uint16_t sStartAdr  = readInfo.startAdr;
    char     *sDataAdr  = readInfo.dataAdr;
    int      sLength    = readInfo.length;

    // We have to send the address in two eight bit chunks:
    uint16_t sStartAdrUpper = ( sStartAdr & 0xFF00 ) >> 8;
    uint16_t sStartAdrLower = ( sStartAdr & 0x00FF );

    int i = 0;
    char *curDataAdr;


    //------------------- let's get down to business
    setCS(1);                               // MSP432: "hello there, general kenoFRAM"
    transmitSPI(WRITE);                     // transmit read op code
    transmitSPI(sStartAdrUpper);            // upper half of starting address
    transmitSPI(sStartAdrLower);            // lower half of starting address
    for(i = 0; i <= sLength; i++){
        curDataAdr = sDataAdr + i;          // based on the index i, we cycle through the input data array
        *curDataAdr = receiveSPI();         // retreive it from the abyss
        transmitSPI(0x00);                  // tell the FRAM we want more
    }
    setCS(0);                               // tell the FRAM we're done with it

    return;

}

uint8_t readNumEntries(void){
    uint8_t numEntries = 0;

    setCS(1);
    transmitSPI(READ);                      // transmit read op code
    transmitSPI(0x00);                      // upper 8 bits of memory loc
    transmitSPI(0x00);                      // lower 8 bits of memory loc
    numEntries = receiveSPI();              // receive the data containing the number of entries
    setCS(0);

    return numEntries;

    /*
     *  Example Use:
     *
     *      uint8_t numEntries = readNumEntries();
     *
     *  see? not the hardest one here
     */
}

void readIndex(int numEntries, struct FRAM_libraryEntry *indexEntries){
    struct FRAM_data temp;

    uint8_t masterIndex = 0;
    uint16_t lastMemLoc = 0x0002;

    if(numEntries == 0){
        return;
    }

    for(masterIndex = 0; masterIndex < numEntries; masterIndex++){
        // First of all, we need to know how long the title is!
        temp.startAdr = lastMemLoc;                             // running location of current memory address in the FRAM
        // i know the next line gives a warning but char and uint8_t are secretly the same thing so i'm ignoring it for now
        temp.dataAdr = &indexEntries[masterIndex].titleLength;  // we want to write this to the titleLength member of the upmost struct
        temp.length = 1;                                        // it's an 8 bit value
        readFRAMData(temp);




    }

    return;

    /*
     *  Example of use, given we have five entries:
     *
     *      int numEntries = 5;
     *      struct FRAM_libraryEntry entries[numEntries]; // 5 entries, 5 structs
     *
     *      readIndex(numEntries, &entries);
     *
     */

}
void writeIndex(struct FRAM_libraryEntry entry, int entryNumber){


    return;
}

void newItem(char title[20], char *dataLoc);
void deleteItem(int indexToDelete);
void readItem(int indexToRead, char *dataLoc);
void writeIndexString(char *stringToWrite);// needs a memory location of where you wanna store this long ass string of every data entry in the fram



uint8_t readStatusReg(void){
    uint8_t statusReg;

    setCS(1);
    transmitSPI(RDSR);
    statusReg = receiveSPI();
    setCS(0);

    return statusReg;
}

uint8_t readDeviceID(void){
    uint8_t deviceID;

    setCS(1);
    transmitSPI(RDID);
    deviceID = receiveSPI();
    setCS(0);

    return deviceID;
}
void    writeStatusReg(uint8_t statusRegData);
