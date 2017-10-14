/**
 * Display handler task
 **/
#ifndef _DISPLAY_H_
#define _DISPLAY_H_

#include "config.h"
#include "debug.h"
#include <Task.h>
#ifdef DISPLAY_EN

/**
 * Display manager task.
 *
 * This task will update the display ever DISP_UPD_FREQ (config.h)
 * milliseconds.
 *
 * At the moment it specifically only supports a 128x64 OLED display driven by
 * an SSD1306 chip over I2C, and needs the ACROBOTIC SDD1306 library. This lib
 * is installable via the Arduino library manager, and included via config.h
 * when OLED_EN is set.
 *
 * The library makes a the `oled` instance available through which the display
 * is manipulated.
 **/
class Display : public TimedTask {
    private:
        // Time scheduling rate. Always contains the next millis time when the
        // task should be run
        uint32_t rate;
        // A buffer to use with sprintf to prepare a line of text to go on the
        // display. The 128px wide display has room for 16 chars when using an
        // 8x8 font. The buffer adds one char for the trailing 0;
        char buf[17];

    public:
        /**
         * Constructor.
         *
         * @param r: Run frequency in milliseconds
         **/
        Display(uint32_t r);

        /**
         * Called every time the task is scheduled to be run.
         *
         * @param now: Current millis() value
         **/
        void run(uint32_t now);
};
#endif //DISPLAY_EN
#endif //_DISPLAY_H_
