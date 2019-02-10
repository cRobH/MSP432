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

struct FRAM_data {
    // This is the FRAM starting address of where we will write/read the data
    uint16_t startAdr;
    // This is the addres of tvhe data we want to write/read to/from the FRAM
    uint32_t *dataAdr;
    // This is the legnth (in bytes) of the data we're going to write/read
    int length;
};


// Prototypes:
bool writeDataToFRAM(struct FRAM_data writeInfo);
bool readFRAMData(struct FRAM_data readInfo);

uint8_t readStatusReg(void);
uint8_t readDeviceID(void);
void    writeStatusReg(uint8_t statusRegData);


