/**
 * LDR light sensors task
 **/
#ifndef _LIGHTSENSE_H_
#define _LIGHTSENSE_H_

#include "config.h"
#include "debug.h"
#include <Task.h>

/**
 * Light sensors reader task.
 *
 * This task reads two LDR sensors locate at around, and facing in, the 10
 * o'clock (left LDR) and 2 o'clock (right LDR) position on the robot.
 *
 * The LDR circuits are like:
 *
 *            \ \
 *  Vcc       V V            R 
 *   `------'\/\/\,---+---`\/\/\,------.
 *            LDR     |                GND
 *                    |
 *                  Analog
 *                   Pin
 *
 * The circuit forms a voltage divider allowing the relative resistance of the
 * LDR to be measured with an analog input to the microcontroller.
 * As the light intensity increases, the resistance of the LDR decreases, and
 * the voltage read increases.
 *
 * Various ways of determining the value of the pull down resistor are
 * discussed here:
 * https://learn.adafruit.com/photocells/using-a-photocell
 * https://arduinodiy.wordpress.com/2014/07/07/chosing-a-pull-down-resistor-for-an-ldr-axel-benz-formula/
 * 
 * This sensor task will read the light values on either side of the robot and
 * then generate a vector format value that will give an indication of the
 * direction from which the strongest light is detected, as well as an
 * indication of light intensity.
 *
 * The directional part will be a value between -100 and +100 giving an
 * indication of direction from which the strongest light is detected.
 *
 * Value : Approximate light direction
 * -100  : from around 6 to about 10 o'clock
 *  -50  : around 11 o'clock
 *    0  : 12 o'clock (possibly 6 o'clock), or full light or full dark
 *   50  : around 1 o'clock
 *  100  : from about 2 o'clock to around 6 o' clock
 *
 *  An approximate intensity will also be calculated with a value between 0 and
 *  100.
 *
 *  The direction and intensity values can directly be used as the direction
 *  and speed values to the DriveTrain to make the robot follow the strongest
 *  light.
 *  By inverting the direction (multiply by -1) it will run away from the
 *  strongest light source.
 *
 *  The light sensor vector will be stored in the shared.lightSense structure.
 **/
class LightSensor : public TimedTask {
    private:
        // Time scheduling rate. Always contains the next millis time when the
        // task should be run
        uint32_t rate;
        uint8_t pinL, pinR; // Left and right analog pins for sensors


    public:
        /**
         * Constructor.
         *
         * @param r: Run frequency in milliseconds
         **/
        LightSensor(uint32_t r, uint8_t pL, uint8_t pR);

        /**
         * Called every time the task is scheduled to be run.
         *
         * @param now: Current millis() value
         **/
        void run(uint32_t now);
};
#endif //_LIGHTSENSE_H_
