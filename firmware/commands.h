#ifndef _COMMANDS_H_
#define _COMMANDS_H_

#include "config.h"
#include "debug.h"
#include <EEPROM.h>

/*** All possible Commands ****/
enum cmdDefs {
    CMD_FWD,	// Forward
    CMD_REV,	// Reverse
    CMD_LFT,	// Left
    CMD_RGT,	// Right
    CMD_SUP,	// Speed up
    CMD_SDN,	// Slow down
    CMD_BRK,	// Brake
    //CMD_FRW,	// Freewheel
    //CMD_SPL,	// Spin left
    //CMD_SPR,	// Spin right
    CMD_INF,	// Info
    CMD_DMO,	// Demo      
    CMD_LRN,	// Learn command maps
    //CMD_TLK,  // Talk?
    CMD_ZZZ,    // End indicator; also counts number of commands
};

/*** Any other defines ****/
#define ESC_KEY 0x1B		// Escape key code
#define CR_KEY 0x0D			// Carriage return
#define LF_KEY 0x0A			// Line feed

/**** Command names map ***/
extern const char *cmdName[CMD_ZZZ];

/**** EEPROM structure for storing command maps ***/
struct eepromData {
    uint16_t eepromSig;         // EEPROM storage signature.
    uint8_t numCmds;            // Number of commands
#ifdef INP_SERIAL_EN
    char serialMap[CMD_ZZZ];    // Map of serial input characters to commands
#endif
#ifdef INP_IR_EN
    uint32_t irMap[CMD_ZZZ];    // Map of IR input codes to commands
#endif
#ifdef INP_BT_EN
    char btMap[CMD_ZZZ];        // Map of bluetooth input characters to commands
#endif
};

/*** Command maps to de initialized in commands.cpp ***/
extern eepromData commandMaps;

void saveCmdMaps();
bool loadCmdMaps();

#endif // _COMMANDS_H_

