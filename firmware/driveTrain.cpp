/**
 * Objects for controlling drive train.
 **/

#include "driveTrain.h"

// ####################### PWM handling Abstraction ######################

/**
 * Function to set a pin up for PWM control.
 *
 * This function is an abstraction for hardware and software PWM control. It
 * provides a single interface for setting up a pin for either hardware or
 * software PWM, for use with an HBridge driver module.
 *
 * The complementary function to this one, PWMSetValue(), is an abstration for
 * setting a PWM value on a PWM pin.
 *
 * Whether hardware or software PWM is in affect, will be determined by the
 * HBRIDGE_SOFT_PWM define set up in `config.h`.
 *
 * If HBRIDGE_SOFT_PWM is defined, then software PWM will be used via the
 * SoftPWM library by Brett Hagman. This library is available via the normal
 * Arduino library manager.
 *
 * This function should be called to set up a specific pin for PWM control and
 * does the following for:
 *
 *   * Hardware PWM:
 *     - Simply set the pin as output
 *
 *   * Software PWM:
 *     - Call the SoftPWMBegin() if it has not already been called. Internally
 *       it will ensure this only gets called once irrespective of how many
 *       software PWM pins are being set up.
 *     - Set the fade up and down times for the pin to 0 to ensure instant PWM
 *       changes when setting a new PWM value. See the SoftPWMSetFadeTime()
 *       function documentation.
 *
 * @param pin: The pin to set up for PWM.
 **/
void PWMSetupPin(uint8_t pin) {
#ifdef HBRIDGE_SOFT_PWM
    // Indicates if SoftPWMBegin has been called yet
    static bool initialized = false;

    // Initialize SoftPWM if not done yet
    if(!initialized) {
        SoftPWMBegin();
        initialized = true;
    }

    SoftPWMSet(pin, 0);

    // Zero fade times for the pin
    // This is important to ensure that a pin goes to 0 immediatly to avoid
    // having both HBridge pins high at any give ntime.
    SoftPWMSetFadeTime(pin, 0, 0);
#else
    // For hardware PWM, we simply set the pin to output
    pinMode(pin, OUTPUT);
#endif // HBRIDGE_SOFT_PWM
}

/**
 * PWM pin value change abstraction function.
 *
 * This function abstracts setting a pin to a given PWM output value for
 * hardware and software PWM control.
 *
 * It is a complimentary function to PWMSetupPin(). Please see PWMSetupPin()
 * for more details.
 *
 * This function should be called to set the PWM output for a pin which was
 * previously set up via PWMSetupPin()
 *
 * @param pin: The pin to set the value for.
 * @param percent: a value between 0 and 100 as a percentage of pulse width
 *        to set, with 0 being full off and 100 being full on.
 */
void PWMSetValue(uint8_t pin, uint8_t percent) {
#ifdef HBRIDGE_SOFT_PWM
    // For SoftPWM we can pass the percentage directly
    SoftPWMSetPercent(pin, percent);
    //SoftPWMSet(pin, map(percent, 0, 100, 0, 255));
#else
    // For hardware PWM we need to map the percentage value to valid PWM values
    // between 0 and 255
    analogWrite(pin, map(percent, 0, 100, 0, 255));
#endif
}

// ####################### Wheel class definitions ######################

#ifdef SERVO_DRV_EN
/**
 * Continues Rotation Servo wheel constructor
 *
 * @param p: The pin the servo is connected to.
 * @param sd: The side (LEFT or RIGHT) the wheel is attached to.
 * @param inv: Optional boolean to invert the direction of rotation.
 **/
Wheel_CRS::Wheel_CRS(uint8_t p, int8_t sd, bool inv) :
    Wheel(sd, inv), pin(p) {

    // Attach the servo to the pin
    servo.attach(pin);
    // Set to stationary
    setSpeed(0);
}

/**
 * Sets the wheel speed for Continues Rotation Servo
 *
 * @param sp: The rotation speed: -100% to 100% with positive as forward speed
 *        and negative as reverse speed. A speed of 0 is stop.
 */
void Wheel_CRS::setSpeed(int8_t sp) {
    uint8_t angle;

    // Validate the speed
    if (!validSpeed(sp))
        return;

    // Save the speed
    speed = sp;

    // Map the speed value between -100 (full speed reverse) and 100 (full speed
    // ahead) to an angle between 0 (full speed rotate left) to 180 (full speed
	// rotate right). Reverse the angle for a wheel with inverted direction.
    if (!inverted) {
        angle = map(speed, -100, 100, 0, 180);
    } else {
        angle = map(speed, 100, -100, 0, 180);
    }

    // Set the speed and direction
    servo.write(angle);
    // Debug output
    D(F("Setting ") << side << F(" angle to ") << angle << F(" for speed ") \
        << speed << endl);
}
#endif // SERVO_DRV_EN

#ifdef HBRIDGE_DRV_EN
/**
 * HBridge motor driver wheel constructor
 *
 * @param pinF: The pin for forward rotation
 * @param pinR: The pin for reverse rotation
 * @param sd: The side (LEFT or RIGHT) the wheel is attached to.
 * @param inv: Optional boolean to invert the direction of rotation.
 **/
Wheel_HB::Wheel_HB(uint8_t pinF, uint8_t pinR, int8_t sd, bool inv) :
    Wheel(sd, inv), pinFwd(pinF), pinRev(pinR) {

    // Set the pins up for PWM
    PWMSetupPin(pinFwd);
    PWMSetupPin(pinRev);

    // Set to stationary
    setSpeed(0);
}

/**
 * Sets the wheel speed for HBridge motor driver
 *
 * @param sp: The rotation speed: -100% to 100% with positive as forward speed
 *        and negative as reverse speed. A speed of 0 is stop.
 */
void Wheel_HB::setSpeed(int8_t sp) {
    // Validate the speed
    if (!validSpeed(sp))
        return;

    // Save the speed
    speed = sp;

    // One of the pins needs to turned off (set to zero) depending on the
    // direction and inverted settings
    uint8_t zeroPin = speed<0 ? pinFwd : pinRev;
    // The other pin will be the control pin
    uint8_t ctrlPin = speed<0 ? pinRev : pinFwd;
    // Invert the direction if required.
    if(inverted) {
        uint8_t tmp = zeroPin;
        zeroPin = ctrlPin;
        ctrlPin = tmp;
    }

    // Set the pins, making sure to ALWAYS set the zero pin first to make
    // *SURE* we never have a situation where both forward and reverse pins are
    // high, which would destroy the H-Bridge!
    PWMSetValue(zeroPin, 0);
    // Now we can set the PWM duty cycle on the control pin
    PWMSetValue(ctrlPin, abs(speed));
    D(F("Setting ") << side << F(" side pin ") << zeroPin << F(" to off and pin ") \
            << ctrlPin << F(" to ") << abs(speed) << endl);
}
#endif // HBRIDGE_DRV_EN

// ####################### DriveTrain class definitions ######################

/**
 * Contstructor
 **/
#ifdef HBRIDGE_DRV_EN
DriveTrain::DriveTrain(uint8_t *cs, uint8_t pinLeftF, uint8_t pinLeftR,
                       uint8_t pinRightF, uint8_t pinRightR,
                       bool leftInv, bool rightInv) {
    // Save pointer to command store
    commandStore = cs;
    // Create wheel instances
    wheel[LEFT] = new Wheel_HB(pinLeftF, pinLeftR, LEFT, leftInv);
    wheel[RIGHT] = new Wheel_HB(pinRightF, pinRightR, RIGHT, rightInv);
	// Default speed and direction to 0
	speed = dir = 0;
    // Zero power start timer
    powerStartTimer = 0;
};

void DriveTrain::powerStart(uint32_t now, int8_t targetSpeed) {
    static int8_t tSpeed=0; // Internal target speed to get to.
    static int8_t tDir=1;  // Internal direction (1 or -1) for positive speeds
    static uint32_t nextPoint=0; // Next point to adjust the speed
    static uint32_t startPoint=0;// The time when we started

    // We only allow a power start to be initiated if the current speed
    // is 0.
    if(speed==0) {
        // The target speed has to be non zero, else we ignore it
        if(targetSpeed == 0) {
            D(F("Target speed should not be 0 for power start init.\n"));
            return;
        }
        // Determine and save the target direction
        tDir = targetSpeed>0 ? 1 : -1;
        // Save the target speed
        tSpeed = abs(targetSpeed);
        // Set the timer for when to time out
        powerStartTimer = now + POWER_START_TIME;
        // Set the next adjustment point
        nextPoint = now + POWER_START_TAPER;
        // Record the start time
        startPoint = now;
        // Set speed to 100% and update the wheels
        speed = 100*tDir;
        updateWheels();
        // Power start init is done, we can return
        return;
    }

    // Not a power start init. Return if we're not at the next update
    // point?
    if (now < nextPoint) return;

    // Have we reached the timer end point yet?
    if(now >= powerStartTimer) {
        // Reset the timer to indicate we are done with the power start
        powerStartTimer = 0;
        // Set the final speed
        speed = tSpeed * tDir;
        updateWheels();
        // We're done
        return;
    }

    // Set the next update point
    nextPoint = now + POWER_START_TAPER;
    // Set the new relative speed
    speed = map(now, startPoint, powerStartTimer, 100, tSpeed) * tDir;
    updateWheels();
}

#else
DriveTrain::DriveTrain(uint8_t *cs, uint8_t pinLeft, uint8_t pinRight,
                       bool leftInv, bool rightInv) {

    // Save pointer to command store
    commandStore = cs;
    // Create wheel instances
    wheel[LEFT] = new Wheel_CRS(pinLeft, LEFT, leftInv);
    wheel[RIGHT] = new Wheel_CRS(pinRight, RIGHT, rightInv);
	// Default speed and direction to 0
	speed = dir = 0;
};
#endif // HBRIDGE_DRV_EN

bool DriveTrain::canRun(uint32_t now) {
#ifdef HBRIDGE_DRV_EN
    // Call powerStart() if we're in power start mode.
    if(powerStartTimer != 0)
        powerStart(now);
#endif // HBRIDGE_DRV_EN

    // In order to not hog all the task time slices, we only want to call the
    // run method if one of the commands we deal with is in the command store.
    // These are the commands we handle:
    uint8_t dtCmds[] = {CMD_FWD, CMD_REV, CMD_BRK, CMD_LFT, CMD_RGT, CMD_SUP,
                        CMD_SDN};
    // One of our commands waiting?
    for (int8_t i=sizeof(dtCmds)/sizeof(*dtCmds)-1; i>=0; i--) {
        if(*commandStore == dtCmds[i])
            return true;
    }
    // Nope, so we do not need to run now, and another task can get an
    // opportunity.
    return false;
};

void DriveTrain::run(uint32_t now) {
    bool foundCommand = true; // Indicates if valid command was found.

    // The commands we can handle
    switch (*commandStore) {
        case CMD_FWD:
            //forward();
            direction(0);
            setSpeed(abs(speed));
            break;
        case CMD_REV:
            //reverse();
            direction(0);
            setSpeed(abs(speed)*-1);
            break;
        case CMD_BRK:
            stop();
            break;
        case CMD_LFT:
            left();
            break;
        case CMD_RGT:
            right();
            break;
        case CMD_SUP:
            speedUp();
            break;
        case CMD_SDN:
            slowDown();
            break;
        default:
            // Not one of our commands
            foundCommand = false;
            break;
    }

    // Clear the command store if we handled the command
    if(foundCommand) {
        *commandStore = CMD_ZZZ;
#ifdef BT_FEEDBACK
        SSerial << F("Speed: ") << speed << F("  Dir: ") << dir << endl;
#endif //BT_FEEDBACK
        D(F("Speed: ") << speed << F("  Dir: ") << dir << endl);
    }
}

void DriveTrain::updateWheels() {
    // Left and right relative speeds
    int8_t leftRel, rightRel;

    // See the MovementControl docs for more info.
    // For a positive direction (forward or turning right), the left wheel
    // relative speed is at 100%, while the right wheel needs to be mapped
    // between 100 and -100 %
    if (dir>=0) {
        leftRel = 100;
        rightRel = map(dir, 0, 100, 100, -100);
    } else {
        // For turning to the left, the right wheel rel speed is 100% while
        // the left wheel needs to be mapped between 100 and -100 %
        rightRel = 100;
        leftRel = map(dir, 0, -100, 100, -100);
    }

    // The left and right wheel speeds are now the relative percentages of the
    // current speed setting. Calculate the actual speed per wheel relative to
    // the current speed.
    leftRel = ((int16_t)speed*leftRel)/100;
    rightRel = ((int16_t)speed*rightRel)/100;

    // Update the wheels
	wheel[LEFT]->setSpeed(leftRel);
	wheel[RIGHT]->setSpeed(rightRel);

    // Update the global shared dir and speed indicators
    shared.speed = speed;
    shared.dir = dir;
};

void DriveTrain::forward() {
    // Set speed to 100% and direction to 0
    speed = 100;
    dir = 0;
    // Update
    updateWheels();
}

void DriveTrain::reverse() {
    // Set speed to -100% and direction to 0
    speed = -100;
    dir = 0;
    // Update
    updateWheels();
}

void DriveTrain::stop() {
    // Set speed to 0, but leave current direction
    speed = 0;
    // Update
    updateWheels();
}

void DriveTrain::left() {
    // Adjust direction
    dir -= TURN_STEP;
    // Stick to limits
    if (dir < MAX_LEFT) dir = MAX_LEFT;
    // Update
    updateWheels();
}

void DriveTrain::right() {
    // Adjust direction
    dir += TURN_STEP;
    // Stick to limits
    if (dir > MAX_RIGHT) dir = MAX_RIGHT;
    // Update
    updateWheels();
}

void DriveTrain::direction(int8_t direct) {
    // Ignore an invalid direction
	if(dir<MAX_LEFT || dir>MAX_RIGHT) return;
	// Set the new direction.
    dir = direct;
    // Update
    updateWheels();
}

void DriveTrain::speedUp() {
    // Adjust speed
    if(speed==0) {
#ifdef HBRIDGE_DRV_EN
        // For HBridge drivers, we do a power start to 50% power from standstill
        powerStart(millis(), 50);
        return;
#else
        // For Servos, we simply go to 50%
        speed = 50;
#endif // HBRIDGE_DRV_EN
    } else {
        speed += SPEED_STEP;
    }
    // Stick to limits
    if (speed > MAX_SPEED) speed = MAX_SPEED;
    // Update
    updateWheels();
}

void DriveTrain::slowDown() {
    // Adjust speed
    if(speed==0) {
#ifdef HBRIDGE_DRV_EN
        // For HBridge drivers, we do a power start to 50% power from standstill
        powerStart(millis(), -50);
        return;
#else
        // For Servos, we simply go to 50%
        speed = -50;
#endif // HBRIDGE_DRV_EN
    } else {
        speed -= SPEED_STEP;
    }
    // Stick to limits
    if (speed < MIN_SPEED) speed = MIN_SPEED;
    // Update
    updateWheels();
}

/**
 * Set the speed.
 *
 * @param speed A speed value as percentage of full speed.
 **/

void DriveTrain::setSpeed(int8_t s) {

    // Validate
    if(s<MIN_SPEED || s>MAX_SPEED)
        return;

	// Set the speed
    speed = s;
    // Update
    updateWheels();
}
