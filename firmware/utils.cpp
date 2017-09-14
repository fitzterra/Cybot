/**
 * Various utility functions.
 */

#include <Arduino.h>
#include "utils.h"

/**
 * Helper for opening the serial port only once.
 *
 * At times is it necessary for multiple objects to try to open the serial port
 * without knowing if it has already been opened. This function can be used as a
 * proxy to open the serial port. It may be called multiple times but will ignore
 * any requests if the port has already been opened.
 * If a new request to open the ports comes in with a different speed, it will
 * write an error to the serial output and then ignore the request. Only the port
 * speed setting is supported at this time.
 *
 * @param speed The port speed. It may be defined as SERIAL_SPEED in config.h, or
 *        else it will default to 57600 if this value is not supplied.
 */
void OpenSerial(uint32_t speed) {
	static bool isOpen=false;
	static uint32_t s=0;

	// Not open yet?
	if (!isOpen) {
		// Save the speed
		s = speed;
		// Open it
		Serial.begin(speed);
		// Indicate that it is open
		isOpen = true;
		return;
	}

	#ifdef _DEBUG
	// Different speed attempted?
	if(speed!=s) {
		D(F("Serial open attempted with new speed: ") << speed \
			   << F(". Not changed from: ") << s << endl);
	}
	#endif // _DEBUG
};

#ifdef SOFTSERIAL_EN
/**
 * Helper for opening the SSerial (software serial) port only once.
 *
 * @param speed: The port speed.
 */
void OpenSoftSerial(uint32_t speed) {
	static bool isOpen=false;
	static uint32_t s=0;

	// Not open yet?
	if (!isOpen) {
		// Save the speed
		s = speed;
		// Open it
		SSerial.begin(speed);
		// Indicate that it is open
		isOpen = true;
		return;
	}

	#ifdef _DEBUG
	// Different speed attempted?
	if(speed!=s) {
		D(F("SoftSerial open attempted with new speed: ") << speed \
			   << F(". Not changed from: ") << s << endl);
	}
	#endif // _DEBUG
};
#endif // SOFTSERIAL_EN
