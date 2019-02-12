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

/*
 *
 * I ended up scrapping my original plan to have a super generic,
 * use anywhere kind of file system.
 *
 * Because that's way too much for me right now.
 *
 * Instead, based on a the define MAX_ENTRIES it will create a index at the beginning of the FRAM
 * in this index, each entry will list the beginning address of each entry
 *
 * Each entry will begin with a 28 char title, then the data
 *
 */

void writeFRAMData(struct FRAM_data writeInfo){
    // Gather the requisite data:
    uint16_t sStartAdr  = writeInfo.startAdr;
    char     *sDataAdr  = writeInfo.dataAdr;
    int      sLength    = writeInfo.length;

    // We have to send the address in two eight bit chunks:
    uint8_t sStartAdrUpper = ( sStartAdr & 0xFF00 ) >> 8;
    uint8_t sStartAdrLower = ( sStartAdr & 0x00FF );

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
    setCS(1);
    transmitSPI(opWREN);
    setCS(0);
    __delay_cycles(20);
    setCS(1);                               // MSP432: "hello there, general kenoFRAM"
    transmitSPI(opWRITE);                   // transmit write op code
    transmitSPI(sStartAdrUpper);            // upper half of starting address
    transmitSPI(sStartAdrLower);            // lower half of starting address
    for(i = 0; i < sLength; i++){
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

    uint8_t i = 0;
    char *curDataAdr;


    //------------------- let's get down to business
    setCS(1);                               // MSP432: "hello there, general kenoFRAM"
    transmitSPI(opREAD);                     // transmit read op code
    transmitSPI(sStartAdrUpper);            // upper half of starting address
    transmitSPI(sStartAdrLower);            // lower half of starting address
    for(i = 0; i < sLength; i++){
        curDataAdr = sDataAdr + i;          // based on the index i, we cycle through the input data array
        uint8_t read = receiveSPI();         // retrieve it from the abyss
        *curDataAdr = read;
    }
    setCS(0);                               // tell the FRAM we're done with it

    return;

}

inline int getNumEntries(void){
    return MAX_ENTRIES;
}
inline uint16_t getEndOfIndex(void){
    return MAX_ENTRIES * LENGTH_OF_ENTRY;
}

void initializeIndex(struct FRAM_libraryEntry *indexEntries){
    // WARNING: WILL EFFECTIVELY DELETE ALL DATA

    // Data will start at MAX_ENTRIES * (28+2+2) = MAX_ENTRIES * 32;
    uint16_t FRAMdataStartAdr = getEndOfIndex();

    uint8_t i;

    for(i = 0; i < MAX_ENTRIES; i++){
        *indexEntries[i].title = {'B','L','A','N','K'};
        *indexEntries[i].startAdr = FRAMdataStartAdr;
        *indexEntries[i].length = 0;
    }

    return;
}
void readIndex(struct FRAM_libraryEntry *indexEntries){
    struct FRAM_data temp;
    uint16_t endOfIndex = MAX_ENTRIES;
    uint8_t i = 0;

    // Initialize temp data to read first entry:
    char curDataAdr = getEndOfIndex();

    // This for loop will read the index entries out of the index,
    // which is the first (MAX_ENTRIES * LENGTH_OF_ENTRY) bytes of data
    for(i = 0; i < MAX_ENTRIES; i++){
        // The first 28 bytes of the data is
        // the title. Read 28 bits and store it to data.
        temp.startAdr = curDataAdr;
        temp.dataAdr = &indexEntries[i].title[0];
        temp.length = 28;
        readFRAMData(temp);
        curDataAdr += temp.length;

        // The next two bytes are the starting
        // address of the data
        temp.startAdr = curDataAdr;
        temp.dataAdr = &indexEntries[i].startAdr;
        temp.length = 2;
        readFRAMData(temp);
        curDataAdr += 2;

        // The next two bytes is the length of the data entry
        // so we know how long to read for
        temp.startAdr = curDataAdr;
        temp.dataAdr = &indexEntries[i].length;
        temp.length = 2;
        readFRAMData(temp);
        curDataAdr += 2;
    }
}

void newItem(char title[28], char *dataLoc, struct FRAM_libraryEntry *indexEntries);
void deleteItem(int indexToDelete);
void readItem(int indexToRead, char *dataLoc);

uint8_t readStatusReg(void){
    uint8_t statusReg;

    setCS(1);
    transmitSPI(opRDSR);
    statusReg = receiveSPI();
    setCS(0);

    return statusReg;
}
uint32_t readDeviceID(void){
    uint32_t deviceID[4];

    setCS(1);
    transmitSPI(opRDID);
    deviceID[0] = receiveSPI();
    deviceID[1] = receiveSPI();
    deviceID[2] = receiveSPI();
    deviceID[3] = receiveSPI();
    setCS(0);

    return (deviceID[0] << 24) | (deviceID[1] << 16) | (deviceID[2] << 8) | deviceID[3];
}
void    writeStatusReg(uint8_t statusRegData);
