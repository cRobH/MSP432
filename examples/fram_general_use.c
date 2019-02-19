
/*******************************************************************************/

/* Standard Includes */
#include <stdint.h>
#include <stdbool.h>
#include "MSP.h"
#include "spiBase.h"
#include "fram.h"


int main(void)
{
    /* Stop Watchdog  */
    WDTCTL = 0x5A80;


  // Initialize SPI
    initSPI();

// Read device registers
    //uint8_t sr = readStatusReg();
    //uint32_t did = readDeviceID();

// These are the control Structs: (you only need one)
    FRAM_libraryEntry entries[MAX_ENTRIES];   // Main struct
    FRAM_libraryEntry readEnt[MAX_ENTRIES];   // I used this one to verify the index was being written correctly; don't use this

// initializeIndex will effectively delete all data on the FRAM. 
// Use it ONCE at the beginning, then comment out
    initializeIndex( &entries );
// Read back to verify it's blank
    readIndex( &readEnt );

// Sample Data (all 1 sector in length)
    char title1[TITLE_LENGTH] = {'E','n','t','r',' ','O','n','e'};
    char dummyChar1[325] = {'t','h','i','s',' ','i','s',' ','e','n','t','r','y',' ','o','n','e'};
    // Write data & entry:
    newItem(&entries, 0, title1, &dummyChar1[0], 17);

    char title2[TITLE_LENGTH] = {'E','n','t','r',' ','T','w','o'};
    char dummyChar2[17] = {'t','h','i','s',' ','i','s',' ','e','n','t','r','y',' ','t','w','o'};
    // Write data & entry:
    newItem(&entries, 1, title2, &dummyChar2[0], 17);

    char title3[TITLE_LENGTH] = {'E','n','t','r',' ','T','h','r'};
    char dummyChar3[17] = {'t','h','i','s',' ','i','s',' ','e','n','t','r','y',' ','t','h','e'};
    // Write data & entry:
    newItem(&entries, 2, title3, &dummyChar3[0], 17);

    char title4[TITLE_LENGTH] = {'E','n','t','r',' ','F','o','r'};
    char dummyChar4[17] = {'t','h','i','s',' ','i','s',' ','e','n','t','r','y',' ','f','o','u'};


    char buffer1[100];
    char buffer2[100];
    char buffer3[100];
    char buffer4[100];
    __delay_cycles(10); // the delays are only to provide conveinient places for breakpoints and serve no real purpose
    // Get the index written to the FRAM
    readIndex( &readEnt );
    __delay_cycles(10);
    // Delete entry 1
    // TODO: update this function s.t. it updates the index to condense together
    deleteEntry( &entries, 1 );

    __delay_cycles(10);
//    readEntry( entries[0], &buffer1[0] ); // TODO: it hangs here with length > sector
// Otherwise, read the entries back for verification: 
    readEntry( entries[1], &buffer2[0] );
    readEntry( entries[2], &buffer3[0] );
    readEntry( entries[3], &buffer4[0] );

    __delay_cycles(10);
    // Read  the index to make sure the deleteEntry updated the index
    readIndex( &readEnt );
    __delay_cycles(10);
    // Add an item into the empty slot: 
    // TODO: update this function such that it finds an empty slot in the index
    newItem(&entries, 1, title4, &dummyChar4[0], 17);
    __delay_cycles(10);
    
    // Again, read back for verification
    readEntry( entries[0], &buffer1[0] );
    readEntry( entries[1], &buffer2[0] );
    readEntry( entries[2], &buffer3[0] );
    readEntry( entries[3], &buffer4[0] );
    __delay_cycles(10);






    while(1)
    {
        while(1);
    }
}

