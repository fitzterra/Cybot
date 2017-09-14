/**
 * Various utility functions.
 *
 * OpenSerial: Generic serial port open function than can be called multiple
 *   times from various places and it only open the serial port once,
 *   regardless of how many times it is called. This is useful when writing
 *   standalone modules that uses the serial port, but you do not want to have
 *   to rely on the serial port to be opened in the main sketch's setup()
 *   function. Each module can open the serial when/where they need it.
 *
 **/

#ifndef _UTILS_H_
#define _UTILS_H_

#include <stdint.h>
#include "config.h"
#include "debug.h"


// If SERIAL_SPEED is not defined elsewhere, default it here 
#ifndef SERIAL_SPEED
#define SERIAL_SPEED 57600
#endif // SERIAL_SPEED

void OpenSerial(uint32_t speed = SERIAL_SPEED);

#ifndef SOFTSERIAL_EN
#error Expected SOFTSERIAL EN to have been defined here
#endif

// Set up a software serial port?
#ifdef SOFTSERIAL_EN
// If SOFTSERIAL_SPEED is not defined elsewhere, default it here 
#ifndef SOFTSERIAL_SPEED
#define SOFTSERIAL_SPEED 9600
#endif // SOFTSERIAL_SPEED

void OpenSoftSerial(uint32_t speed=SOFTSERIAL_SPEED);

#endif // SOFTSERIAL_EN


#endif // _UTILS_H_
