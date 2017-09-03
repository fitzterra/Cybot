/*
 * Basic debug header.
 *
 * Define DEBUG before including this header to enable debugging.
 */

#include "config.h"
#include "Streaming.h"

// Macro to send debug details via the serial stream if DEBUG is defined.
#ifdef _DEBUG
	#define D(x) Serial << x
#else
	#define D(x)
#endif
