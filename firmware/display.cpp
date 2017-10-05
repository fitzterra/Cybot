/**
 * Display handler task
 **/

#include "display.h"

// Display constructor
Display::Display(uint32_t r) : TimedTask(millis()), rate(r) {
        // Initialize the display
        oled.init();
        // Set header
        oled.setTextXY(0, 5);
        oled.putString("Cybot");
        // Debug
        D("Display init...\n");
};

// Task runner
void Display::run(uint32_t now) {
            // Set the next time we should be run
            incRunTime(rate);

            // Update speed and direction display
            sprintf(buf, "S:%4d D:%4d", shared.speed, shared.dir);
            oled.setTextXY(4, 0);
            oled.putString(buf);
        };
