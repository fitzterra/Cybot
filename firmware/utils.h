/**
 * Various utility functions.
 */

#include <stdint.h>
#include "config.h"
#include "debug.h"

#ifndef _UTILS_H_
#define _UTILS_H_

// If SERIAL_SPEED is not defined elsewhere, default it here 
#ifndef SERIAL_SPEED
#define SERIAL_SPEED 57600
#endif // SERIAL_SPEED

void OpenSerial(uint32_t speed = SERIAL_SPEED);

#endif // _UTILS_H_
