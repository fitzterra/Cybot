/**
 * Application compile time config settings.
 *
 * This file is empty and only serves to work around this problem, although
 * this may be a good place to add any config handling code in future.
 */

#include "config.h"

// Make the SSerial port available if needed
#ifdef SOFTSERIAL_EN
SoftwareSerial SSerial(SOFTSERIAL_RX, SOFTSERIAL_TX);
#endif // SOFTSERIAL_EN
