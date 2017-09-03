/**
 * Task based handlers for receiving input from various input devices.
 */

#ifndef _CONTROL_H_
#define _CONTROL_H_

#include "config.h"
#include "debug.h"
#include "utils.h"
#include "commands.h"
#include <Task.h>
#include <TaskScheduler.h>
#ifdef INP_BT_EN
    #include <SoftwareSerial.h>
#endif // INP_BT_EN
#ifdef INP_IR_EN
    #include <IRremote.h>
#endif // INP_IR_EN

/**
 * Abstract base class for all input methods.
 *
 * This class defines the standard input method handling behavior and should be
 * sub-classed by the various input methods and customised by overrides specific
 * to the input method.
 *
 * Note that this class subclasses the Task class which means each input method
 * should be run as a scheduled task. The basic usage is as follows:
 *
 * - Override the hasInput() method to have a way for this input method to
 *   detect if new input is available.
 * - Override the decodeInput() method to fetch the new input and to decode
 *   that to a specific command via the internal command map for this input
 *   method.
 * - Instantiate the input method object by passing in the specific meta data
 *   for the method:
 *   * Pointer to it's command map
 *   * Any repeat interval and count settings
 *   * A pointer to an external command storage variable where any newly
 *     received command would be placed for external consumption.
 * - When the task is scheduled to run, it will call the hasInput() to check if
 *   any new input from the raw input device is available.
 * - If so, it will call the run() method which will:
 *   * Call the decodeInput() to read and decode the input to a command.
 *   * If a new command is available, it will be pushed to the new command storage
 * - Another task will pick up the control command from the command storage,
 *   and dispatch it from there to the correct handler.
 **/
class InputHandler : public Task {
    protected:
        // Pointer to command map array. This is declared a void pointer to
        // have a uniform constructor for all derived input method classes. Each
        // derived class should cast this pointer to the type of input it will
        // receive wherever the pointer is referenced.
        void *commandMap;
        // Pointer to an external command storage value where this input
        // handler will deposit any new commands found after decoding raw
        // input.
        uint8_t *commandStore;
        // Minimum delay required between successive inputs. Anything received
        // within this delay after a previous input, will be discarded. This
        // value is in milliseconds but can only hold up to a 2 byte unsigned
        // value (0xFFFF)
        uint16_t minDelay;
        // Some commands will behave differently depending on whether the
        // command is being repeated or just sent in quick succession. This
        // value determines the max time interval between successive receipt of
        // the same character which would count as a repeat of this input.
        uint16_t repeatMax;
        // Repeat input counter that will be incremented when the same input is
        // received within the repeatMax period.
        uint8_t repeatCnt;
        // Local storage for decoded command. The decodeInput method should
        // place a valid command decoded in here.
        uint8_t cmd;
        // The last time input was received. This is a milliseconds value and
        // is updated every time input is received.
        uint32_t lastInTm;
        // Set true when in learn mode, false otherwise
        bool learnMode;
        // When in learn mode, this will be set to when learn mode should time
        // out without any new input.
        uint32_t learnTimeOut;

    public:
        InputHandler(void *cm, uint8_t *cs, uint16_t md, uint16_t rm) :
            commandMap(cm), commandStore(cs), minDelay(md), repeatMax(rm),
            repeatCnt(0), cmd(CMD_ZZZ), lastInTm(0), learnMode(false) {};
        virtual bool hasInput(uint32_t now) =0;
        virtual bool decodeInput(uint32_t now) =0;
		virtual void learn(uint32_t now, bool init=false);
        virtual void learnPrompt(int8_t cmd) =0;
        virtual bool learnSetCmd(int8_t cmd) =0;
		virtual bool canRun(uint32_t now);
		virtual void run(uint32_t now);
};

#ifdef INP_SERIAL_EN
/**
 * Task to handle serial input.
 */
class SerialIn : public InputHandler {
	private:
		char lastRX;	// The last input character received

	public:
        SerialIn(void *cm, uint8_t *cs, uint16_t md, uint16_t rm);
        bool hasInput(uint32_t now);
        bool decodeInput(uint32_t now);
        void learnPrompt(int8_t cmd);
        bool learnSetCmd(int8_t cmd);
};
#endif // INP_SERIAL_EN

#ifdef INP_BT_EN
/**
 * Task to handle Bluetooth input.
 */
class BTIn : public InputHandler {
	private:
		char lastRX;	// The last input character received
        SoftwareSerial *bts; // BT serial port

	public:
        BTIn(uint8_t rxPin, uint8_t txPin, void *cm, uint8_t *cs, uint16_t md,
             uint16_t rm);
        bool hasInput(uint32_t now);
        bool decodeInput(uint32_t now);
        void learnPrompt(int8_t cmd);
        bool learnSetCmd(int8_t cmd);
};
#endif // INP_BT_EN

#ifdef INP_IR_EN
/**
 * Task to handle IR input.
 */
class IRIn : public InputHandler {
	private:
		decode_results irRes; // The decoded IR result
		IRrecv *irRecv;	// Pointer to IR Receiver instance.
		uint32_t lastRX;	// Time the last code was received.

	public:
        IRIn(uint8_t pin, void *cm, uint8_t *cs, uint16_t md, uint16_t rm);
        bool hasInput(uint32_t now);
        bool decodeInput(uint32_t now);
        void learnPrompt(int8_t cmd);
        bool learnSetCmd(int8_t cmd);
};
#endif // INP_IR_EN

#endif	// _CONTROL_H_
