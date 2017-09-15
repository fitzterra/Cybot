/**
 * Library to manage commands and command mappings, including saving and
 * loading from EEPROM.
 */

#include "commands.h"

/**** Command names map ***/
const char *cmdName[] = {
	"Forward",			// CMD_FWD 0	// Forward
	"Reverse",      	// CMD_REV 1	// Reverse
	"Left",         	// CMD_LFT 2	// Left
	"Right",        	// CMD_RGT 3	// Right
	"Speed up",     	// CMD_SUP 4	// Speed up
	"Slow down",    	// CMD_SDN 5	// Slow down
	"Brake",        	// CMD_BRK 6	// Brake
//	"Freewheel",    	// CMD_FRW 5	// Freewheel
//	"Spin Left",    	// CMD_SPL 8	// Spin left
//	"Spin Right",   	// CMD_SPR 9	// Spin right
	"Info",         	// CMD_INF 7	// Info
	"Demo",         	// CMD_DMO 8	// Demo
	"Learn",         	// CMD_LRN 9	// Learn command maps
//	"Talk",         	// CMD_TLK 13	// Talk??
};

eepromData commandMaps = {
    /****** EEPROM Signature *****/
    // Should be changed everytime the commands or command structures change,
    // including everytime an input method (serial, IR, BT, etc.) is enabled or
    // disabled.
    0xAA55,

    /****** Number of commands in maps *****/
    CMD_ZZZ,

#ifdef INP_SERIAL_EN
    /**** Default map of serial input character codes to commands ****/
    {
        0,	// CMD_FWD 0	// Forward
        0,  // CMD_REV 1	// Reverse
        0,  // CMD_LFT 2	// Left
        0,  // CMD_RGT 3	// Right
        0,  // CMD_SUP 4	// Speed up
        0,  // CMD_SDN 5	// Slow down
        0,  // CMD_BRK 6	// Brake
    //	0,	// CMD_FRW 5	// Freewheel
    //	0,	// CMD_SPL 8	// Spin left
    //	0,	// CMD_SPR 9	// Spin right
        0,  // CMD_INF 7	// Info
        0,	// CMD_DMO 8	// Demo      
        'l',// CMD_LRN 9	// Learn command maps
    //	0,  // CMD_TLK 13	// Talk?
    },
#endif

#ifdef INP_IR_EN
    /**** Default map of IR codes to commands ****/
    {
        0x00,		// CMD_FWD 0	// Forward
        0x00,   	// CMD_REV 1	// Reverse
        0x00,   	// CMD_LFT 2	// Left
        0x00,   	// CMD_RGT 3	// Right
        0x00,   	// CMD_SUP 4	// Speed up
        0x00,   	// CMD_SDN 5	// Slow down
        0x00,   	// CMD_BRK 6	// Brake
    //	0x00,      	// CMD_FRW 5	// Freewheel
    //	0x00,      	// CMD_SPL 8	// Spin left
    //	0x00,      	// CMD_SPR 9	// Spin right
        0x00,   	// CMD_INF 7	// Info
        0x00,      	// CMD_DMO 8	// Demo      
        0x2FD5AA5,  // CMD_LRN 9	// Learn command maps - Changhong remote S.MODE button (music note)
    //	0x00,      	// CMD_TLK 13	// Talk?
    },
#endif
#ifdef INP_BT_EN
    /**** Default map of BT input character codes to commands ****/
    {
        0,	// CMD_FWD 0	// Forward
        0,  // CMD_REV 1	// Reverse
        0,  // CMD_LFT 2	// Left
        0,  // CMD_RGT 3	// Right
        0,  // CMD_SUP 4	// Speed up
        0,  // CMD_SDN 5	// Slow down
        0,  // CMD_BRK 6	// Brake
    //	0,	// CMD_FRW 5	// Freewheel
    //	0,	// CMD_SPL 8	// Spin left
    //	0,	// CMD_SPR 9	// Spin right
        0,  // CMD_INF 7	// Info
        0,	// CMD_DMO 8	// Demo      
        'l',// CMD_LRN 9	// Learn command maps
    //	0,  // CMD_TLK 13	// Talk?
    },
#endif
};



/**
 * Loads the command maps from EEPROM into the commandMaps object if the
 * correct signature is found in the EEPROM.
 *
 * Returns true if valid data was found and loaded into commandMaps, false
 * otherwise with no changes to commandMaps.
 */
bool loadCmdMaps() {
    // A validator to test if the EEPROM signature and command mount in the
    // EEPROM data matches what we expect before we load the data from EEPROM.
    struct {
        uint16_t eepromSig;         // EEPROM storage signature.
        uint8_t numCmds;            // Number of commands
    } validator;

    // Read the signature and command count into validator starting from
    // address 0 in EEPROM
    EEPROM.get(0, validator);

	if(validator.eepromSig==commandMaps.eepromSig &&
       validator.numCmds==commandMaps.numCmds) {
		// Signature and cound is good, read the maps
		EEPROM.get(0, commandMaps);
        // Debug output on success
		D(F("Command maps read from EEPROM.\n"));
	} else {
        // Debug output on failure
		D(F("Command maps signature not found in EEPROM.\n"));
        return false;
	}

    return true;
}

/**
 * Saves the commandMaps object to EEPROM.
 */
void saveCmdMaps() {
    // Write the commandMaps object
    EEPROM.put(0, commandMaps);
	
    // Debug output
	D(F("Command maps written to EEPROM.\n"));
}

