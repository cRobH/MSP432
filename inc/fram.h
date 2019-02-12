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

#pragma once

// op-codes:
#define WREN  0b00000110    // Set Write Latch Enable
#define WRDI  0b00000100    // Reset Write Enable Latch
#define RDSR  0b00000101    // Read Status Register
#define WRSR  0b00000001    // Write Status Register
#define READ  0b00000010    // Read Memory Code
#define WRITE 0b00000010    // Write Memory Code
#define RDID  0b10011111    // Read Device ID
//----------
#define MAX_ENTRIES     10      // Max number of entries; change based on what kind of data we're storing

struct FRAM_data {
    // This is the FRAM starting address of where we will write/read the data
    uint16_t startAdr; // (FRAM ADDRESS)

    // This is the address of the data we want to write/read to/from the FRAM.
    char *dataAdr; // (MSP ADDRESS)

    // This is the legnth (in bytes) of the data we're going to write/read
    uint16_t length;
};

struct FRAM_libraryEntry {
    // The title of the entry
    // This has a hard limit of 28 characters
    char title[28];

    // The starting address of where the entry is stored on the FRAM
    // The min value of this is 0x0002 due to the endOfIndexLoc using 00-01
    uint16_t startAdr;

    // the number of chars in the entry
    uint16_t length;
};




// Prototypes:
void writeDataToFRAM(struct FRAM_data writeInfo);
void readFRAMData(struct FRAM_data readInfo);

uint8_t readNumEntries(void);
void readIndex(int numEntries, struct FRAM_libraryEntry *indexEntries);
void writeIndex(int numEntries, struct FRAM_libraryEntry *indexEntries);

void newItem(int numEntries, char title[28], char *dataLoc, struct FRAM_libraryEntry *indexEntries);
void deleteItem(int indexToDelete);
void readItem(int indexToRead, char *dataLoc);
//void writeIndexString(char *stringToWrite);// needs a memory location of where you wanna store this long ass string of every data entry in the fram


uint8_t readStatusReg(void);
uint32_t readDeviceID(void);
void    writeStatusReg(uint8_t statusRegData);


