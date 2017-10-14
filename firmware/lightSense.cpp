/**
 * LDR light sensors task
 **/

#include "lightSense.h"

// Light Sensor constructor
LightSensor::LightSensor(uint32_t r, uint8_t pL, uint8_t pR) :
    TimedTask(millis()), rate(r), pinL(pL), pinR(pR) {
        // Debug
        D("LightSensor init...\n");
};

// Task runner
void LightSensor::run(uint32_t now) {
            static int c = 0;
            // Set the next time we should be run
            incRunTime(rate);
            //D("L rt:" << getRunTime() << endl);

            //D(c++);
            // Get the sensor readings and map to between 0 and 100
            int8_t lft = map(analogRead(pinL), 0, 1023, 0, 100);
            int8_t rgt = map(analogRead(pinR), 0, 1023, 0, 100);
            //D(F(" LDR-L: ") << lft << F(" LDR-R: ") << rgt << endl);

            // Calculate the approximate direction with left being negative and
            // right positive. Place it in the shared light sensor vector
            shared.lightSense.dir = rgt - lft;
            // Calculate the relative light intensity and place in the shared
            // light sensor vector
            shared.lightSense.intensity = ((int16_t)lft + rgt)/2;
        };
