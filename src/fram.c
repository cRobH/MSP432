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

#ifdef __ENABLE_FRAM_WRITE
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

#endif
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
    int length = START_OF_INDEX;
    return (start + (max * length) );
}

void initializeIndex(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES] ){
    // WARNING: WILL EFFECTIVELY DELETE ALL DATA



    // Data will start at MAX_ENTRIES * (28+2+2) = MAX_ENTRIES * 32;
    uint16_t FRAMdataStartAdr = getEndOfIndex();
    const char blank[TITLE_LENGTH] = {'B','L','A','N','K',
                      '0','0','0','0','0',
                      '0','0','0','0','0',
                      '0','0','0','0','0',
                      '0','0','0','0','0',
                      '0','0','0'
                     };
    uint8_t i;
    int k = 0;
    uint16_t numEntries = MAX_ENTRIES;
    char dummyChar = ' ';


    // First, we create the array of structs which make up
    // the index at the beginning of the FRAM
    for(i = 0; i < MAX_ENTRIES; i++){
        for(k = 0; k < 28; k++){
            (*indexEntries)[i].title[k] = blank[k];
        }
        (*indexEntries)[i].startAdr = FRAMdataStartAdr; // dummy start adr of the end of the index
        (*indexEntries)[i].length = 0;
    }



    // Next, we write the number of entries to the beginning of the FRAM.
    FRAM_data writeStartOfIndex;
        writeStartOfIndex.startAdr = 0x0000;
        writeStartOfIndex.dataAdr = &numEntries;
        writeStartOfIndex.length = 1;

    writeFRAMData(writeStartOfIndex);

    // Finally, we write the index to the FRAM.
    for(i = 0; i < MAX_ENTRIES; i++){
        newItem( indexEntries, i, blank, &dummyChar, 0);
    }

    return;
}
void readIndex(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES]){
    FRAM_data temp;
    //uint16_t endOfIndex = MAX_ENTRIES;
    uint8_t i = 0;


    // First, we read the number of entries the FRAM is
    // currently initialized to. If we ever change it.
    char numFRAMInit = 0;
    temp.startAdr = 0x00;
    temp.dataAdr = &numFRAMInit;
    temp.length = 1;
    readFRAMData(temp);

    if(numFRAMInit != MAX_ENTRIES){
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
    // TODO: does the *entry pull the whole array?

    // Error check:
    if( entryToWrite > (MAX_ENTRIES - 1) ){

        return 0;
    }

    // Copy input data to entries[]:
    for(i=0;i<TITLE_LENGTH;i++){
        (*indexEntries)[entryToWrite].title[i] = title[i];
    }
    (*indexEntries)[entryToWrite].startAdr = 0;//TODO: get adr from defrag
    (*indexEntries)[entryToWrite].length = lengthOfData;

    // Let's write the index entry!
    // We're gonna start with the title.
    int curDataAdr = START_OF_INDEX + ( entryToWrite * LENGTH_OF_ENTRY );
    temp.startAdr = (uint16_t)curDataAdr;
    temp.dataAdr = &( (*indexEntries)[entryToWrite].title[0] );
    temp.length = TITLE_LENGTH;
    writeFRAMData(temp);
    curDataAdr += temp.length;

    alloc_defrag( indexEntries, 0, entryToWrite, lengthOfData);

    // The next two bytes are the starting
    // address of the data
    temp.startAdr = (uint16_t)curDataAdr;
    // TODO: what is this? -> entries[entryToWrite].startAdr;
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


    return 1;
}
void deleteItem(int indexToDelete);
void readItem(int indexToRead, char *dataLoc);
uint16_t alloc_defrag(FRAM_libraryEntry *indexEntries[MAX_ENTRIES],
                      int mode, int index, int length ){
    uint16_t sStartAdr[MAX_ENTRIES];
    uint16_t sLength[MAX_ENTRIES];
    uint16_t sEndAdr[MAX_ENTRIES];
    int i = 0; int k = 0;

    // Do some error checking:
    for(i=0; i < MAX_ENTRIES; i++){
        sStartAdr[i] = (*indexEntries)[i].startAdr;
        sLength[i] = (*indexEntries)[i].length;
        sEndAdr[i] = ( sStartAdr[i] + sLength[i] );

        if(sEndAdr[i] >= (*indexEntries)[i+1].startAdr){
            // check if the ending address of an entry is beyond
            // the starting address of the next. if so, we have some problems.
            // 0xFFFF is an invalid address, so hopefully it'll get caught.
            return 0xFFFF;
        }
    }

    if(mode == 1){
        // defrag/delete
        // so if k=indextodelete,
        // we need to move k+1 to k's location
        // then k+n until n=max

        for(i=index; i<(MAX_ENTRIES-1); i++){
            char tempChar;
            FRAM_data temp;
            uint16_t readAdr = sStartAdr[i + 1];
            uint16_t writeAdr = sStartAdr[i];
            uint16_t readLength = sLength[i];
            temp.startAdr = readAdr;
            temp.dataAdr = &tempChar;
            temp.length = 1;
            int k = 0;

            for(k=0; k<readLength; k++){
                // First we read:
                readFRAMData(temp);
                // Then we write:
                temp.startAdr = writeAdr;
                writeFRAMData(temp);
                readAdr += 1;
                writeAdr += 1;
            }

            for(k=0;k<TITLE_LENGTH;k++){
                (*indexEntries)[i].title[k] = 0;
            }
            (*indexEntries)[i].startAdr = (*indexEntries)[i+1].startAdr;
            (*indexEntries)[i].length = (*indexEntries)[i+1].length;
        }
        for(k=0;k<TITLE_LENGTH;k++){
            (*indexEntries)[MAX_ENTRIES].title[k] = 0;
        }
        (*indexEntries)[MAX_ENTRIES].startAdr = ((*indexEntries)[MAX_ENTRIES].startAdr - 1);
        (*indexEntries)[MAX_ENTRIES].length = 0;

    }else if(mode == 0){
        // alloc/add
        if(sEndAdr[index] < sStartAdr[index + 1]){

            for(i=index; i<(MAX_ENTRIES-1); i++){
                char tempChar;
                FRAM_data temp;
                uint16_t readAdr = sStartAdr[i + 1];
                uint16_t writeAdr = sEndAdr[i];
                uint16_t readLength = sLength[i];
                temp.startAdr = readAdr;
                temp.dataAdr = &tempChar;
                temp.length = 1;
                int k = 0;

                for(k=0; k<readLength; k++){
                    // First we read:
                    readFRAMData(temp);
                    // Then we write:
                    temp.startAdr = writeAdr;
                    writeFRAMData(temp);
                    readAdr += 1;
                    writeAdr += 1;
                }

            }

            return sStartAdr[index];

        }else if(sEndAdr[index] == (sStartAdr[index + 1] - 1)){
            // somehow you manage the impossible.
            // you are replace data with data of the same size.
            // lucky you, we do nothing. we're just going to return
            // the starting address of the slot you're writing to.
            return sStartAdr[index];
        }else{
            // god only knows how you ended up in here
            return 0xFFFF;
        }



    }else{
        // 0 and 1 are the only two valid modes
        return 0xFFFF;
    }

    return 0xFFFF;
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
