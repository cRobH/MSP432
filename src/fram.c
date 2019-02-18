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
#include <stdbool.h>
#include "MSP.h"

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
//TODO: do i need these anymore?
void writeFRAMData(FRAM_data writeInfo){
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
void readFRAMData(FRAM_data readInfo){
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

inline uint16_t getEndOfIndex(void){
    int start = START_OF_INDEX;
    int max = MAX_ENTRIES;
    int length = LENGTH_OF_ENTRY;
    return (start + (max * length) );
}


inline uint16_t sectorToAdr(int sector){
    // TODO: verify
    uint16_t adr = ( sector * 300) + SECTOR_START;
    return adr;
}
inline int adrToSector(uint16_t adr){
    // TODO: verify
    int sector = ( adr  - SECTOR_START ) / SECTOR_SIZE;
    return sector;
}

void initializeIndex(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES] ){
    // WARNING: WILL EFFECTIVELY DELETE ALL DATA

    const char blank[TITLE_LENGTH] = {'B','L','A','N','K',
                                      ' ',' ',' ',' ',' ',
                                      ' ',' ',' ',' ',' ',
                                      ' ',' ',' ',' ',' ',
                                      ' ',' ',' ',' ',' ',
                                      ' ',' ',
                                     };
    int i; int k = 0;
    FRAM_data temp;
    uint16_t numEntries = MAX_ENTRIES;
    uint16_t sectorSize = SECTOR_SIZE;
    uint16_t curDataAdr = START_OF_INDEX;


    // First, we create the array of structs which make up
    // the index at the beginning of the FRAM
    for(i = 0; i < MAX_ENTRIES; i++){
        for(k = 0; k < 28; k++){
            (*indexEntries)[i].title[k] = blank[k];
        }
        (*indexEntries)[i].startAdr = (i * SECTOR_SIZE) + SECTOR_START; // dummy start adr of the end of the index
        (*indexEntries)[i].length = 0;

        // The first 28 bytes of the data is
        // the title. Read 28 bits and store it to data.
        temp.startAdr = curDataAdr;
        temp.dataAdr = &(*indexEntries)[i].title[0];
        temp.length = TITLE_LENGTH;
        writeFRAMData(temp);
        curDataAdr += temp.length;

        // The next two bytes are the starting
        // address of the data
        temp.startAdr = curDataAdr;
        temp.dataAdr = &(*indexEntries)[i].startAdr;
        temp.length = 2;
        writeFRAMData(temp);
        curDataAdr += 2;

        // The next two bytes is the length of the data entry
        // so we know how long to read for
        temp.startAdr = curDataAdr;
        temp.dataAdr = &(*indexEntries)[i].length;
        temp.length = 2;
        writeFRAMData(temp);
        curDataAdr += 2;

    }

    char clear[4] = {0,0,0,0};
    FRAM_data clearFirstFour;
        clearFirstFour.startAdr = 0x0000;
        clearFirstFour.dataAdr  = &clear[0];
        clearFirstFour.length   = 4;
    writeFRAMData(clearFirstFour);
    // Next, we write the number of entries to the beginning of the FRAM.
    FRAM_data writeStartOfIndex;
        writeStartOfIndex.startAdr = 0x0000;
        writeStartOfIndex.dataAdr = &numEntries;
        writeStartOfIndex.length = 2;
    writeFRAMData(writeStartOfIndex);
    // Next, we write the sector size after the number of entries
    FRAM_data writeSectorSize;
        writeSectorSize.startAdr = 0x0002;
        writeSectorSize.dataAdr = &sectorSize;
        writeSectorSize.length = 2;
    writeFRAMData(writeSectorSize);


    return;
}
void readIndex(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES]){
    FRAM_data temp;
    uint8_t i = 0;
    int numEntries, sectorSize;


    // First, we read the number of entries the FRAM is
    // currently initialized to. If we ever change it.
    uint8_t tempRead[2];
    temp.startAdr = 0x00;
    temp.dataAdr = &tempRead[0];
    temp.length = 2;
    readFRAMData(temp);
    numEntries = (tempRead[1] << 8) | (tempRead[0]);
    if(numEntries != MAX_ENTRIES){
        while(1); // Hold. The FRAM is not initialized for this number of entries.
    }
    temp.startAdr = 0x0002;
    readFRAMData(temp);
    sectorSize = (tempRead[1] << 8) | (tempRead[0]);
    if(sectorSize != SECTOR_SIZE){
        while(1); // Hold. The FRAM is not initialized for this number of entries.
    }

    // Get the address for the beginning of the index
    // This is where we will begin to read data
    uint16_t curDataAdr = START_OF_INDEX;
    // This for loop will read the index entries out of the index,
    // which is the first (MAX_ENTRIES * LENGTH_OF_ENTRY) bytes of data
    for(i = 0; i < MAX_ENTRIES; i++){
        // The first 28 bytes of the data is
        // the title. Read 28 bits and store it to data.
        temp.startAdr = curDataAdr;
        temp.dataAdr = &(*indexEntries)[i].title[0];
        temp.length = TITLE_LENGTH;
        readFRAMData(temp);
        curDataAdr += temp.length;

        // The next two bytes are the starting
        // address of the data
        temp.startAdr = curDataAdr;
        temp.dataAdr = &(*indexEntries)[i].startAdr;
        temp.length = 2;
        readFRAMData(temp);
        curDataAdr += 2;

        // The next two bytes is the length of the data entry
        // so we know how long to read for
        temp.startAdr = curDataAdr;
        temp.dataAdr = &(*indexEntries)[i].length;
        temp.length = 2;
        readFRAMData(temp);
        curDataAdr += 2;
    }
}

int newItem(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES], int entryToWrite,
             char title[TITLE_LENGTH], char *dataToWrite, int lengthOfData){

    FRAM_data temp;
    int i = 0;

    // Error check:
    if( entryToWrite > (MAX_ENTRIES - 1) ){
        return 0;
    }

    // Copy input data to entries[]:
    for(i=0;i<TITLE_LENGTH;i++){
        (*indexEntries)[entryToWrite].title[i] = title[i];
    }
    (*indexEntries)[entryToWrite].startAdr = (entryToWrite * SECTOR_SIZE) + SECTOR_START;
    (*indexEntries)[entryToWrite].length = lengthOfData;

    // Let's write the index entry!
    // We're gonna start with the title.
    int curDataAdr = START_OF_INDEX + ( entryToWrite * LENGTH_OF_ENTRY );
    temp.startAdr = (uint16_t)curDataAdr;
    temp.dataAdr = &( (*indexEntries)[entryToWrite].title[0] );
    temp.length = TITLE_LENGTH;
    writeFRAMData(temp);
    curDataAdr += temp.length;

    // The next two bytes are the starting
    // address of the data
    temp.startAdr = (uint16_t)curDataAdr;
    temp.dataAdr = &( (*indexEntries)[entryToWrite].startAdr );
    temp.length = 2;
    writeFRAMData(temp);
    curDataAdr += 2;

    // The next two bytes is the length of the data entry
    // so we know how long to read for
    temp.startAdr = (uint16_t)curDataAdr;
    temp.dataAdr = &( (*indexEntries)[entryToWrite].length );
    temp.length = 2;
    writeFRAMData(temp);
    curDataAdr += 2;

    // Now let's write the data!
    temp.startAdr = (*indexEntries)[entryToWrite].startAdr;
    temp.dataAdr = &dataToWrite;
    temp.length = (*indexEntries)[entryToWrite].length;
    writeFRAMData(temp);


    return 1;
}
void readEntry(FRAM_libraryEntry entryToRead, char *dataLoc){
    FRAM_data temp;
        temp.startAdr = entryToRead.startAdr;
        temp.dataAdr  = dataLoc;
        temp.length   = entryToRead.length;
    readFRAMData(temp);

    return;
}
void deleteEntry( FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES], int entryToDelete){
    int k = 0;

    for(k=0; k<TITLE_LENGTH; k++){
        (*indexEntries)[entryToDelete].title[k] = 0;
    }
    (*indexEntries)[entryToDelete].startAdr =  SECTOR_START;
    (*indexEntries)[entryToDelete].length =  0;
}
uint16_t alloc(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES], int length ){
    int i = 0; int k = 0; int j = 0;
    int consecutiveEmptySectors = 0;
    int lastSectorState = 0;
    FRAM_data temp;
    char buffer[300];

    int numberOfSectors = (MEM_SIZE_BYTES - SECTOR_START) / SECTOR_SIZE;
    int sectorsToWrite = (length / SECTOR_SIZE) + 1;
    char sectorMap[(MEM_SIZE_BYTES - SECTOR_START) / SECTOR_SIZE] = {0}; // the value in the cell is the index# written there;
                                     // an ASCII value of 0 or ' ' indicates it is empty
    int freeSectors = getFreeSectors( indexEntries );

    if( sectorsToWrite > freeSectors){
        // Insufficient space.
        return 0xFFFF;
    }

    for(i=0; i<MAX_ENTRIES; i++){
        int startSector = adrToSector( (*indexEntries)[i].startAdr );
        int lengthInSectors = 0;
        if( (*indexEntries)[i].length != 0){
            lengthInSectors = ( (*indexEntries)[i].length / SECTOR_SIZE ) + 1;
        }
        for(k=startSector; k<lengthInSectors; k++){
            sectorMap[k] = 1 + '0'; // turn int into a char
        }
    }
    for(i=0; i<numberOfSectors; i++){
        if( (sectorMap[i] == 0) && (lastSectorState == 0) ){
            consecutiveEmptySectors++;
        }
        if(consecutiveEmptySectors >= sectorsToWrite){
            return sectorToAdr( i );
        }
        lastSectorState = sectorMap[i];
    }

/*
    while(consecutiveEmptySectors < sectorsToWrite){
        for(i=0; i<MAX_ENTRIES; i++){
            int startSector = adrToSector( (*indexEntries)[i].startAdr );
            int lengthInSectors = 0;
            if( (*indexEntries)[i].length != 0){
                lengthInSectors = ( (*indexEntries)[i].length / SECTOR_SIZE ) + 1;
            }
            for(k=startSector; k<lengthInSectors; k++){
                sectorMap[k] = 1 + '0'; // turn int into a char
            }
        }
        for(i=0; i<numberOfSectors; i++){
            if( (sectorMap[i] == 0) && (lastSectorState == 0) ){
                consecutiveEmptySectors++;
            }
            if(consecutiveEmptySectors >= sectorsToWrite){
                return sectorToAdr( i );
            }
            lastSectorState = sectorMap[i];
        }
        for(i=0; i<numberOfSectors; i++){
            // TODO: verify this works
            while(1);
            if(sectorMap[i] == 0){
                k = 0; int flag = 0;
                while(flag == 0){
                    if(sectorMap[i+k] != 0){
                        // copy i+k to i
                        temp.startAdr = (*indexEntries)[j].startAdr;
                        temp.dataAdr = &buffer[0];
                        temp.length = (*indexEntries)[j].length;
                        readFRAMData(temp);
                        (*indexEntries)[j].startAdr = sectorToAdr(i);

                        temp.startAdr = sectorToAdr(i);
                        writeFRAMData(temp);
                        flag = 1;
                    }else{
                        k++;
                    }
                }
            }
        }
    }
*/
    return 0xFFFF;
}

int getFreeSectors(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES]){
    char sectorMap[(MEM_SIZE_BYTES - SECTOR_START) / SECTOR_SIZE] = {0};
    int numberOfSectors = (MEM_SIZE_BYTES - SECTOR_START) / SECTOR_SIZE;
    int i = 0; int k = 0;
    int emptySectors = 0;

    for(i=0; i<MAX_ENTRIES; i++){
        int startSector = adrToSector( (*indexEntries)[i].startAdr );
        int lengthInSectors = 0;
        if( (*indexEntries)[i].length != 0){
            lengthInSectors = ( (*indexEntries)[i].length / SECTOR_SIZE ) + 1;
        }
        for(k=startSector; k<lengthInSectors; k++){
            sectorMap[k] = 1 + '0'; // turn int into a char
        }
    }

    for(i=0; i<numberOfSectors; i++){
        if( sectorMap[i] == 0 ){
            emptySectors++;
        }
    }

    return emptySectors;
}

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
