/**
 * Task based handler for receiving input from various devices.
 */

#include "control.h"

// ####################### InputHandler class definitions ######################

/**
 * Method to determine if this input handler has any new input available.
 */
bool InputHandler::canRun(uint32_t now) {
    // Exit learn mode if it's active and we have timed out
    if(learnMode && now>=learnTimeOut) {
        // Visual feedback
        D(F("Learn mode exiting: no input received.\n"));
        learnMode = false;
        // Reset anything that has been changed from EEPROM
        // NOTE: Reloading maps here could cause an issue given the following
        //       situation:
        //  1. No mapping has been stored yet, or the stored maps are
        //     invalidated due to an EEPROM signature change.
        //  2. Enter learn mode and learn the first few commands
        //  3. Timeout learn mode to get here
        //  4. Now we try to load the command maps from EEPROM, but the EEPROM
        //     data is invalid.
        //  5. The incomplete trained commands are still in force because it
        //     was not overwritten from a 'valid' map in EEPROM.
        // This is an edge case, but a better solution should be found.
        loadCmdMaps();
    }
    // Call the input handler's own method to check if there is any input.
    return hasInput(now);
};

/**
 * Method to do the work once the canRun() method indicates there is work to
 * do.
 **/
void InputHandler::run(uint32_t now) {
    // If we're in learn mode, let the input be handled by the learn method
    if(learnMode) {
        learn(now);
        return;
    }

    // Call the input handler's instance to decode the new input into a
    // supported command if possible. The new command will be placed in the cmd
    // member variable if the decode was successful.
    if(decodeInput(now)) {
        if (cmd == CMD_LRN) {
            // We are just entering learn mode for this input method. Let the
            // learn method know to initialize itself for learning.
            learn(now, true);
        } else {
            // No learning, so place the new command in the command store for
            // other tasks to pick up on.
            *commandStore = cmd;
        }
    }
};

/**
 * Method to handle learning input codes/keys for commands.
 *
 * @param init: Set to true only on initial entry into learn mode, false
 *              (default) on successive calls while in learn mode.
 **/
void InputHandler::learn(uint32_t now, bool init) {
    static int8_t nextCmd; // Next command to train

    // We got input, so we can extend or start the learn timeout
    learnTimeOut = now + LEARNMODE_TIMEOUT;

    // Are we initializing?
    if(init) {
        // We start with the default skip command
        nextCmd = -1;
        // Indicate we are now in learn mode
        learnMode = true;
        // Visual feedback - only via serial and debug macro for now - to be
        // abstracted so that any output medium can be used.
        D(F("Entering learn mode.\nGive input to train each command presented.\n"));
        // Issue the learn prompt for the next command
        learnPrompt(nextCmd);
        // Init done, we can return
        return;
    }

    // Not initializing, so assign the input to the command we are learning now.
    if (!learnSetCmd(nextCmd)) {
        // Assigning new command failed and we assume the learnSetCmd method
        // has given all the feedback and re-prompted for command to learn, so
        // we can return.
        return;
    }

    // Next command if we are not done
    if(++nextCmd == CMD_ZZZ) {
        // All done
        D(F("\nAll commands trained. Saving to EEPROM.\n"));
        saveCmdMaps();
        // We're done learning
        learnMode = false;
        return;
    }

    // Prompt for the next command
    learnPrompt(nextCmd);
}

#ifdef INP_SERIAL_EN
// ####################### SerialIn class definitions ######################

/**
 * Constructor.
 */
SerialIn::SerialIn(void *cm, uint8_t *cs, uint16_t md, uint16_t rm) :
            InputHandler(cm, cs, md, rm) {
	// Open the serial port with default speed.
	OpenSerial();

    D(F("Starting SerialIn task...\n"));
}

/**
 * Input handler specific method to test if input is available.
 **/
bool SerialIn::hasInput(uint32_t now) {
	return Serial.available();
}

/**
 * Processes any new input received.
 *
 * This method will only be called if serial input is available (the canRun()
 * method ensures this).
 *
 * @param now: The current millis() counter.
 */
bool SerialIn::decodeInput(uint32_t now) {
	// Read the input;
	char c = (char)Serial.read();
	// Calculate the time since the last input was received
	uint32_t rxInterval = now - lastInTm;

	// Is the interval between the last received char and this one less than
	// the min delay allowed between input chars?
	if(rxInterval < minDelay) {
		// Too quick. Ignore it
		D(F("Serial min delay exceeded. Ignoring input...\n"));
		return false;
	}
	
	// Update the last received timer
	lastInTm = now;

	// Is it a repeat of the previous input and are we still within the allowed
	// repeat time?
	if (c==lastRX && rxInterval<=repeatMax) {
		repeatCnt++;
	} else {
		repeatCnt = 0;
		lastRX = c;
	}

    char *cmdMap = (char *)commandMap;
	// Now decode this in the command map
    for(uint8_t i=0; i<CMD_ZZZ; i++) {
        if(c==cmdMap[i]) {
            D(F("Command from SerialInput: ") << cmdName[i] \
                << F("  Repeat: ") << repeatCnt << endl);
            cmd = i;
            return true;
        }
    }
    return false;
}

/**
 * Method to issue the prompt for the next command to learn
 *
 * @param cmd: The command code to generate the prompt for.
 **/
void SerialIn::learnPrompt(int8_t cmd) {
    if(cmd==-1) {
        D(F("\nInput to skip and use default: "));
    } else {
    D(F("\nInput for: ") << cmdName[cmd] << F(" [")
        << ((char *)commandMap)[cmd] << F("] "));
    }
};

/**
 * Method to set the given command to the input received.
 *
 * This method will be called from the learn() method only when there is input
 * available for the command being learned. This method should validate that
 * the input has not been assigned before, issue a warning if so and then
 * return false to indicate that the command was not assigned properly. If
 * there are no conflicts, this method should return true.
 *
 * @param cmd: The command to set.
 **/
bool SerialIn::learnSetCmd(int8_t cmd) {
    // Default skip input - we have this as static since this is where we will
    // test for it, and it is specific to the input type.
    static char skip = 0;
	// Read the input;
	char c = (char)Serial.read();
    // Cast void pointer to command map to char pointer for easier access
    char *cmdMap = (char *)commandMap;

    // Clear out any extra characters in the receive buffer - this is for
    // serial input that consists of multiple characters.
    // It seems we need a very slight delay here before Serial.available
    // registers that more characters are available. Using delay() in a Task is
    // not good practice, but in this case it is OK in order to circumvent a
    // bigger issue.
    delay(1);
    char c2;
    while(Serial.available()) {
        c2 = Serial.read();
        //D(F("\nExtra char: ") << c2 << endl);
    }

    // Is this the first entry to read the skip key?
    if(cmd==-1) {
        skip = c;
        D((skip) << endl);
        return true;
    }

    // Did we get the skip key
    if(c==skip) {
        // Use the default
        c = cmdMap[cmd];
    }

    // Check for conflicts
    for (uint8_t i=0; i<cmd; i++) {
        if(cmdMap[i] != c)
            continue;
        // We have a conflict
        D(F("\nThis input is already assigned to '") << cmdName[i] \
          << F("'. Please try again.\n"));
        learnPrompt(cmd);
        return false;
    }

    // Assign it
    D(F(" = ") << c);
    cmdMap[cmd] = c;

    return true;
}

#endif  // INP_SERIAL_EN


#ifdef INP_BT_EN
// ####################### SerialIn class definitions ######################

/**
 * Constructor.
 */
BTIn::BTIn(void *cm, uint8_t *cs, uint16_t md, uint16_t rm) :
            InputHandler(cm, cs, md, rm) {
	// Open the SSerial port with default speed for output.
	OpenSoftSerial();

    D(F("Starting BTIn task...\n"));
}

/**
 * Input handler specific method to test if input is available.
 **/
bool BTIn::hasInput(uint32_t now) {
	return SSerial.available();
}

/**
 * Processes any new input received.
 *
 * This method will only be called if serial input is available (the canRun()
 * method ensures this).
 *
 * @param now: The current millis() counter.
 */
bool BTIn::decodeInput(uint32_t now) {
	// Read the input;
	char c = (char)SSerial.read();
	// Calculate the time since the last input was received
	uint32_t rxInterval = now - lastInTm;

    D(F("BT rx: ") << c << endl);

	// Is the interval between the last received char and this one less than
	// the min delay allowed between input chars?
	if(rxInterval < minDelay) {
		// Too quick. Ignore it
		D(F("BT min delay exceeded. Ignoring input...\n"));
		return false;
	}
	
	// Update the last received timer
	lastInTm = now;

	// Is it a repeat of the previous input and are we still within the allowed
	// repeat time?
	if (c==lastRX && rxInterval<=repeatMax) {
		repeatCnt++;
	} else {
		repeatCnt = 0;
		lastRX = c;
	}

    char *cmdMap = (char *)commandMap;
	// Now decode this in the command map
    for(uint8_t i=0; i<CMD_ZZZ; i++) {
        if(c==cmdMap[i]) {
            D(F("Command from SerialInput: ") << cmdName[i] \
                << F("  Repeat: ") << repeatCnt << endl);
            cmd = i;
            return true;
        }
    }
    return false;
}

/**
 * Method to issue the prompt for the next command to learn
 *
 * @param cmd: The command code to generate the prompt for.
 **/
void BTIn::learnPrompt(int8_t cmd) {
    if(cmd==-1) {
        D(F("\nInput to skip and use default: "));
    } else {
    D(F("\nInput for: ") << cmdName[cmd] << F(" [")
        << ((char *)commandMap)[cmd] << F("] "));
    }
};

/**
 * Method to set the given command to the input received.
 *
 * This method will be called from the learn() method only when there is input
 * available for the command being learned. This method should validate that
 * the input has not been assigned before, issue a warning if so and then
 * return false to indicate that the command was not assigned properly. If
 * there are no conflicts, this method should return true.
 *
 * @param cmd: The command to set.
 **/
bool BTIn::learnSetCmd(int8_t cmd) {
    // Default skip input - we have this as static since this is where we will
    // test for it, and it is specific to the input type.
    static char skip = 0;
	// Read the input;
	char c = (char)SSerial.read();
    // Cast void pointer to command map to char pointer for easier access
    char *cmdMap = (char *)commandMap;

    // Clear out any extra characters in the receive buffer - this is for
    // serial input that consists of multiple characters.
    // It seems we need a very slight delay here before Serial.available
    // registers that more characters are available. Using delay() in a Task is
    // not good practice, but in this case it is OK in order to circumvent a
    // bigger issue.
    delay(1);
    char c2;
    while(SSerial.available()) {
        c2 = SSerial.read();
        //D(F("\nExtra char: ") << c2 << endl);
    }

    // Is this the first entry to read the skip key?
    if(cmd==-1) {
        skip = c;
        D((skip) << endl);
        return true;
    }

    // Did we get the skip key
    if(c==skip) {
        // Use the default
        c = cmdMap[cmd];
    }

    // Check for conflicts
    for (uint8_t i=0; i<cmd; i++) {
        if(cmdMap[i] != c)
            continue;
        // We have a conflict
        D(F("\nThis input is already assigned to '") << cmdName[i] \
          << F("'. Please try again.\n"));
        learnPrompt(cmd);
        return false;
    }

    // Assign it
    D(F(" = ") << c);
    cmdMap[cmd] = c;

    return true;
}

#endif  // INP_BT_EN


#ifdef INP_IR_EN
// ####################### IrIn class definitions ######################

/**
 * Constructor.
 */
IRIn::IRIn(uint8_t pin, void *cm, uint8_t *cs, uint16_t md, uint16_t rm) :
            InputHandler(cm, cs, md, rm) {
	// Create the IR receiver instance
	irRecv = new IRrecv(pin);
	irRecv->enableIRIn();

	// Open the serial port with default speed.
	OpenSerial();

    D(F("Starting IRIn task...\n"));
}

/**
 * Input handler specific method to test if input is available.
 **/
bool IRIn::hasInput(uint32_t now) {
	// Try to decode any input we may have
	if (irRecv->decode(&irRes)) {
		// Get ready to receive the next input
		irRecv->resume();

        D(F("IR in: ") << _HEX(irRes.value) << endl);
        // IR input repeats aggressively, so we only indicate new input if it
        // is not a repeat, and silently ignore any repeats.
        if(irRes.value!=REPEAT) {
            return true;
        }
        D(F("  - ignoring this repeat.\n"));
	}
	return false;
}

/**
 * Processes any new input received.
 *
 * This method will only be called if hasInput() has already received and
 * decoded IR input into the irRes structure.
 *
 * @param now: The current millis() counter.
 */
bool IRIn::decodeInput(uint32_t now) {
	// Calculate the time since the last input was received
	uint32_t rxInterval = now - lastInTm;
    //#Debugging# D(F(" | Decoding: ") << _HEX(irRes.value));

	// Is the interval between the last received input and this one less than
	// the min delay allowed between input chars?
	if(rxInterval < minDelay) {
		// Too quick. Ignore it
		D(F("\nIR min delay exceeded. Ignoring input...\n"));
		return false;
	}
	
	// Update the last received timer
	lastInTm = now;

	// Is it a repeat of the previous input and are we still within the allowed
	// repeat time?
	if (irRes.value==lastRX && rxInterval<=repeatMax) {
		repeatCnt++;
	} else {
		repeatCnt = 0;
		lastRX = irRes.value;
	}

    //#Debugging# D(F(" | Working with: ") << _HEX(irRes.value) << F("  Repeat: ") << repeatCnt << endl);

    uint32_t *cmdMap = (uint32_t *)commandMap;
	// Now decode this in the command map
    for(uint8_t i=0; i<CMD_ZZZ; i++) {
        if(irRes.value==cmdMap[i]) {
            D(F("Command from IR: ") << cmdName[i] \
                << F("  Repeat: ") << repeatCnt << endl);
            cmd = i;
            return true;
        }
    }
    return false;
}

/**
 * Method to issue the prompt for the next command to learn
 *
 * @param cmd: The command code to generate the prompt for.
 **/
void IRIn::learnPrompt(int8_t cmd) {
    if(cmd==-1) {
        D(F("\nInput to skip and use default: "));
    } else {
    D(F("\nInput for: ") << cmdName[cmd] << F(" [")
        << _HEX(((uint32_t *)commandMap)[cmd]) << F("] "));
    }
};

/**
 * Method to set the given command to the input received.
 *
 * This method will be called from the learn() method only when there is input
 * available for the command being learned. This method should validate that
 * the input has not been assigned before, issue a warning if so and then
 * return false to indicate that the command was not assigned properly. If
 * there are no conflicts, this method should return true.
 *
 * @param cmd: The command to set.
 **/
bool IRIn::learnSetCmd(int8_t cmd) {
    // Default skip input - we have this as static since this is where we will
    // test for it, and it is specific to the input type.
    static uint32_t skip = 0;
    // Storage of the input value
    uint32_t inp = irRes.value;
    // Cast void pointer to command map to char pointer for easier access
    uint32_t *cmdMap = (uint32_t *)commandMap;

    // Is this the first entry to read the skip key?
    if(cmd==-1) {
        skip = inp;
        D(_HEX(skip) << endl);
        return true;
    }

    // Did we get the skip key
    if(inp==skip) {
        // Use the default
        inp = cmdMap[cmd];
    }

    // Check for conflicts
    for (uint8_t i=0; i<cmd; i++) {
        if(cmdMap[i] != inp)
            continue;
        // We have a conflict
        D(F("\nThis input is already assigned to '") << cmdName[i] \
          << F("'. Please try again.\n"));
        learnPrompt(cmd);
        return false;
    }

    // Assign it
    D(F(" = ") << _HEX(inp));
    cmdMap[cmd] = inp;

    return true;
}

#endif // INP_IR_EN


