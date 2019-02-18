/************************************************************************
name:           MB85RS64V FRAM Library
description:     Expands off the spiBase files to provide support for the MB85RS64V Ferroelectric RAM IC
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

#pragma once

// op-codes:
#define opWREN  0b00000110    // Set Write Latch Enable
#define opWRDI  0b00000100    // Reset Write Enable Latch
#define opRDSR  0b00000101    // Read Status Register
#define opWRSR  0b00000001    // Write Status Register
#define opREAD  0b00000011    // Read Memory Code
#define opWRITE 0b00000010    // Write Memory Code
#define opRDID  0b10011111    // Read Device ID
//----------
#define MAX_ENTRIES     10      // Max number of entries; change based on what kind of data we're storing
#define LENGTH_OF_ENTRY 32      // = title_length + 2(length) + 2(addr)
#define START_OF_INDEX  0x0004
#define TITLE_LENGTH    28
#define SECTOR_START    0x0144  // (max_entries * length_of_entries) + start_of_index = 10*32 + 4 = 324 = 0x0144
#define SECTOR_SIZE     300
#define MEM_SIZE_BYTES  8000
//----------


typedef struct {
    // This is the FRAM starting address of where we will write/read the data
    uint16_t startAdr; // (FRAM ADDRESS)

    // This is the address of the data we want to write/read to/from the FRAM.
    char *dataAdr; // (MSP ADDRESS)

    // This is the legnth (in bytes) of the data we're going to write/read
    uint16_t length;
} FRAM_data;

typedef struct {
    // The title of the entry
    // This has a hard limit of 28 characters
    char title[28];

    // The starting address of where the entry is stored on the FRAM
    // The min value of this is 0x0002 due to the endOfIndexLoc using 00-01
    uint16_t startAdr;

    // the number of chars in the entry
    uint16_t length;
} FRAM_libraryEntry;

// TODO: see if i need this in the future
// Globals:
    // Since this boy is pretty beefy, I'm only going to declare it once
    // and everyone can share.
    //char universalBuffer[300];



// Prototypes:
void writeFRAMData(FRAM_data writeInfo);
void readFRAMData(FRAM_data readInfo);

// Small inlines to get hardcoded data:
inline int getNumEntries(void);
inline uint16_t getEndOfIndex(void);
inline uint16_t sectorToAdr(int sector);
inline int adrToSector(uint16_t adr);

void initializeIndex(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES] );
void readIndex(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES] );

int newItem(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES], int entryToWrite,
             char title[TITLE_LENGTH], char *dataToWrite, int lengthOfData);
void readEntry(FRAM_libraryEntry entryToRead, char *dataLoc);
void deleteEntry( FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES], int entryToDelete);
void streamSector(FRAM_libraryEntry entryToRead);
uint16_t alloc(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES], int length );
int getFreeSectors(FRAM_libraryEntry (*indexEntries)[MAX_ENTRIES]);

uint8_t readStatusReg(void);
uint32_t readDeviceID(void);
void    writeStatusReg(uint8_t statusRegData);


