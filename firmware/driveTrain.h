/**
 * Objects for controlling drive train.
 **/


#ifndef _DRIVETRAIN_H_
#define _DRIVETRAIN_H_

#include "config.h"
#include "debug.h"
#include <Arduino.h>
#ifdef SERVO_DRV_EN
#include <Servo.h>
#endif // SERVO_DRV_EN
#ifdef HBRIDGE_SOFT_PWM
#include <SoftPWM.h>
#endif // HBRIDGE_SOFT_PWM


#define SPEED_STEP 5    // Increments for speed changes 
#define TURN_STEP 5     // Increments for turning left or right
#define MAX_LEFT -100   // Max value for the left direction
#define MAX_RIGHT 100   // Max value for the right direction
#define MAX_SPEED 100   // Max speed value
#define MIN_SPEED -100  // Min speed value

// #### PWM Hardware/Software Abstraction functions ####
void PWMSetupPin(uint8_t pin);
void PWMSetValue(uint8_t pin, uint8_t percent);

/**
 * Abstract base class for controlling a wheel.
 *
 * This class is only concerned about rotating the wheel forward or backwards
 * at a given percentage of full speed, with a speed of 0 indicating stop.
 *
 * The details on how the wheel speed and direction is controlled needs to be
 * implemented in subclasses of this base class for the specific type rotation
 * device for example a brushed DC motor (BDM), continues rotation servo
 * (CRS), etc.
 **/
class Wheel {
    protected:
        // The side of the robot the wheel is attached to. Either LEFT (0) or RIGHT (1)
        uint8_t side;
        // The current speed of the wheel as a percentage between -100% (full
        // speed reverse) and 100% (full speed forward)
        int8_t speed;
        // Whether to invert the rotation direction
        bool inverted;

    public:
        Wheel(uint8_t sd, bool inv=false) : 
            side(sd), inverted(inv) {};
        // Utility method to test if the speed is in the valid range
        bool validSpeed(int8_t speed) { return (speed>=MIN_SPEED && speed<=MAX_SPEED); }
        // Method to be provided by sub class.
        virtual void setSpeed(int8_t speed) =0;
};

#ifdef SERVO_DRV_EN
/**
 * Subclass of Wheel() to represent a Continues Rotation Servo (CRS).
 **/
class Wheel_CRS : public Wheel {;
    private:
        // The pin the servo is connected to. This must be a pin supported by
        // the Arduino standard Servo library. It seems that the latest Arduino
        // code supports any digital pin.
        uint8_t pin;
        // A Servo object for controlling the servo
        Servo servo;

    public:
        Wheel_CRS(uint8_t p, int8_t sd, bool inv=false);
        void setSpeed(int8_t speed);
};
#endif // SERVO_DRV_EN

#ifdef HBRIDGE_DRV_EN
/**
 * Subclass of Wheel() to represent a Brushed DC Motor (BDM) driven via an
 * HBridge.
 *
 * NOTE: The HBridge *never* be driven with both forward and reverse pins on,
 *       even if the HBridge was designed to handle this. Doing so with most
 *       HBridges is a sure way to overload and destroy the driver transistors.
 **/
class Wheel_HB : public Wheel {
    private:
        // The pin to set high to drive the motor forward. If using the SoftPWM
        // library for PWM output (HBRIDGE_SOFT_PWM is defined), then any
        // digital pin can be used, else for hardware supported PWM, only pins
        // with PWM output may be used.
        uint8_t pinFwd;
        // The pin to set high to drive the motor reversed. The same PWM pin
        // considerations applies as for pinFwd.
        uint8_t pinRev;

    public:
        Wheel_HB(uint8_t pinF, uint8_t pinR, int8_t sd, bool inv=false);
        void setSpeed(int8_t speed);
};
#endif // HBRIDGE_DRV_EN

#endif // _DRIVETRAIN_H_
