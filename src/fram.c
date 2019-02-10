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


bool writeDataToFRAM(struct FRAM_data writeInfo){
    // Gather the requisite data:
    uint16_t sStartAdr  = writeInfo.startAdr;
    uint32_t *sDataAdr  = writeInfo.dataAdr;
    int      sLength    = writeInfo.length;

    // We have to send the address in two eight bit chunks:
    uint16_t sStartAdrUpper = ( sStartAdr & 0xFF00 ) >> 8;
    uint16_t sStartAdrLower = ( sStartAdr & 0x00FF );

    // Each memory address holds 8 bits; to find the ending
    // memory address, we need to know the number of 8 bit chunks of data
    // Each 8 bit chunk of data holds one byte
//  uint16_t sEndingAdr = sStartAdr + sLength;
    // e.g. if we want to store 0b00, 0b01, 0b10, and 0b11, starting at 0x00:
    //  0x00 - 0b00
    //  0x01 - 0b01
    //  0x02 - 0b10;
    //  0x03 - 0b11;

    uint32_t i = 0;

    //-------------------
    setCS(1);
    transmitSPI(WRITE);
    transmitSPI(sStartAdrUpper);
    transmitSPI(sStartAdrLower);
    for(i = 0; i <= sLength; i++){
        uint32_t *curDataAdr = *sDataAdr + i;
        uint8_t dataToXmt = *curDataAdr;
        transmitSPI(dataToXmt);
    }
    setCS(0);

    return 1;

}
bool readFRAMData(struct FRAM_data readInfo){
    // Gather the requisite data:
    uint16_t sStartAdr  = readInfo.startAdr;
    uint32_t *sDataAdr  = readInfo.dataAdr;
    int      sLength    = readInfo.length;

    // We have to send the address in two eight bit chunks:
    uint16_t sStartAdrUpper = ( sStartAdr & 0xFF00 ) >> 8;
    uint16_t sStartAdrLower = ( sStartAdr & 0x00FF );

    uint32_t i = 0;

    setCS(1);
    transmitSPI(WRITE);
    transmitSPI(sStartAdrUpper);
    transmitSPI(sStartAdrLower);
    for(i = 0; i <= sLength; i++){
        uint32_t *curDataAdr = *sDataAdr + i;
        curDataAdr = receiveSPI();
    }
    setCS(0);


}


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
    while(checkForReceived() == 0){
        deviceID = receiveSPI();
    }
    setCS(0);
    return deviceID;
}
void    writeStatusReg(uint8_t statusRegData);
