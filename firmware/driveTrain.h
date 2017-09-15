/**
 * Objects for controlling drive train.
 **/


#ifndef _DRIVETRAIN_H_
#define _DRIVETRAIN_H_

#include "config.h"
#include "debug.h"
#include "commands.h"
#include <Task.h>
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

/**
 * Class that combines wheels into a single drive train control
 **/
class DriveTrain : public Task {
    private:
        // Pointer to external command store from which to fetch any new
        // movement commands to execute.
        uint8_t *commandStore;

#ifdef HBRIDGE_DRV_EN
        Wheel_HB *wheel[2];    // Left and Right wheels for HBrige drivers
#else
        Wheel_CRS *wheel[2];  // Left and Right wheels for Continues Servo Drivers
#endif // HBRIDGE_DRV_EN
        int8_t speed;        // Current relative speed as percentage of full speed
        int8_t dir;          // Direction of travel, -100 to 100. See MovementControl docs.

        /**
         * Update the wheels speed based on the current direction and speed.
         **/
        void updateWheels(); // Updates the wheel rotation from speed and dir.

    public:
#ifdef HBRIDGE_DRV_EN
        /**
         * Sets up a drive train for HBridge controlled motors.
         *
         * @param cs: Pointer to external command store to check for new commands.
         * @param pinLeftF: Left HBridge forward pin
         * @param pinLeftR: Left HBridge reverse pin
         * @param pinRightF: Right HBridge forward pin
         * @param pinRightF: Right HBridge reverse pin
         * @param leftInv: Set to true to invert left motor rotation direction
         * @param rightInv: Set to true to invert right motor rotation direction
         **/
        DriveTrain(uint8_t *cs, uint8_t pinLeftF, uint8_t pinLeftR,
                   uint8_t pinRightF, uint8_t pinRightR,
                   bool leftInv=false, bool rightInv=false);
#else
        /**
         * Sets up a drive train for Continues Rotation Servos.
         *
         * @param cs: Pointer to external command store to check for new commands.
         * @param pinLeft: Left servo control pin
         * @param pinRight: Right servo control pin
         * @param leftInv: Set to true to invert left servo rotation direction
         * @param rightInv: Set to true to invert right servo rotation direction
         **/
        DriveTrain(uint8_t *cs, uint8_t pinLeft, uint8_t pinRight,
                   bool leftInv=false, bool rightInv=false);
#endif // HBRIDGE_DRV_EN

        /**
         * Method from Task class to test if the DriveTrain task can run.
         *
         * It will run anytime there is a new command in the command store.
         **/
		bool canRun(uint32_t now) { return (*commandStore != CMD_ZZZ); };

        /**
         * Method from Task class that will be called when there is a new
         * command in the command store that needs to be considered for
         * execution by the drive train.
         **/
		void run(uint32_t now);

        /**
         * Sets full speed forward
         **/
        void forward();

        /**
         * Sets full speed reverse
         **/
        void reverse();

        /**
         * Stops motion by setting speed to 0
         **/
        void stop();

        /**
         * Adjust direction by TURN_STEPs to the left
         **/
        void left();

        /**
         * Adjust direction by TURN_STEPs to the right
         **/
        void right();

        /**
         * Adjust direction by the supplied direction.
         *
         * @param dir: A direction value between MAX_LEFT (full turn left) and
         *             MAX_RIGHT (full turn right), with 0 being going straight
         *             forward. See diagrams in docs.
         **/
        void direction(int8_t direct);

        /**
         * Speed up by SPEED_STEPs up to MAX_SPEED
         **/
        void speedUp();

        /**
         * Slow down by SPEED_STEPs down to MIN_SPEED
         **/
        void slowDown();

        /**
         * Sets the speed to s if it is between MIN_SPEED and MAX_SPEED.
         * Ignores the speed change otherwise.
         *
         * @param s: The new speed between MIN_SPEED and MAX_SPEED
         **/
        void setSpeed(int8_t speed);

        /**
         * Returns the current speed.
         **/
        int8_t getSpeed() {return speed;};

        /**
         * Returns the current direction.
         **/
        int8_t getDirection() {return dir;};
};

#endif // _DRIVETRAIN_H_
