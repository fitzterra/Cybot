#include <SoftwareSerial.h>
#include <SoftPWM.h>
#include <Streaming.h>
#include <stdint.h>


class HBridge {
    private:
        uint8_t _pinF;  // Forward pin
        uint8_t _pinR;  // Reverse pin
        // Speed as a percentage of full power. Positive if forward, negative backwards.
        int8_t _speed;
        // The minimum percentage of full speed at which the motor will stall.
        // This will be dependant on factors such as the motor itself, the
        // load, the state of the batteries if this is the power source, etc.
        uint8_t _stallSpeed;
        // The minimum percentage of full speed needed to start the motor
        // moving from stand still. As for _stallSpeed, this value also depends
        // on the motor, load and battery state. When starting the motor from
        // stand still, and the desired speed is less than _startSpeed, the
        // speed will initially be set to this speed to get the motor going,
        // and after _startTime, will be reset to the desired speed.
        uint8_t _startSpeed;
        // The amount of time to allow the motor to run at _startSpeed before
        // switching to a lower speed if it was started from stand still to a
        // speed less than _startSpeed.
        uint8_t _startTime;
    public:
        HBridge(uint8_t pFwd, uint8_t pRev) : _pinF(pFwd), _pinR(pRev), _speed(0) {stop();};
        void setSpeed(int8_t s);
        int8_t getSpeed() { return _speed; };
        void stop() {setSpeed(0); };
};

/**
 * Set the rotation speed as a percentage of full speed, with 100% being full
 * speed forward, and -100% being full speed reverse.
 *
 * @param s: speed percentage of 100%
 */
void HBridge::setSpeed(int8_t s) {
    // Validate the speed
    if(s<-100 || s>100) {
        Serial << "Invalid speed: " << s << endl;
        // We ignore it for now.
        return;
    }
    // Save the speed
    _speed = s;
    Serial << "Setting speed: " << s << endl;

    // One of the pins needs to turned off (set to zero) depending on the direction
    uint8_t zeroPin = _speed<0 ? _pinF : _pinR;
    // The other pin will be the control pin
    uint8_t ctrlPin = _speed<0 ? _pinR : _pinF;
    Serial << "zeroPin: " << zeroPin << "  ctrlPin: " << ctrlPin << endl;

    // Set the pins, making sure to ALWAYS set the zero pin first to make
    // *SURE* we never have a situation where both forward and reverse pins are
    // high, which would destroy the H-Bridge!
    SoftPWMSet(zeroPin, 0);
    // Now we can set the PWM duty cycle on the control pin
    SoftPWMSetPercent(ctrlPin, abs(s));
}

#define C_FWD 'w'
#define C_REV 'z'
#define C_LFT 'a'
#define C_RGT 's'
#define C_STP 'h'
#define C_SUP '1'   // Speed up
#define C_SDN '6'   // Slow down

SoftwareSerial bt(12, 11); //Bluetooth pins RX and TX connected to 0 and 1 on arduino nano
HBridge leftWheel(3, 4);
HBridge rightWheel(5, 6);

char checkControl() {
    if(Serial.available()) {
        return Serial.read();
    }
    if(bt.available()) {
        return bt.read();
    }

    return 0;
}

void setup() {
    SoftPWMBegin();
    Serial.begin(115200);
    bt.begin(9600);

    Serial << "All set up...\n";
}


void loop() {
    char c = checkControl();
    int8_t currSpeed = leftWheel.getSpeed();
    int8_t dir = currSpeed<0 ? -1 : 1;
    int8_t absSpeed = abs(currSpeed);
    int8_t newSpeed = currSpeed;

    if(c) {
        Serial << "cmd: " << c << " - " << _HEX(c) << endl;
        //Serial << "cmd: " << ord(c) << endl;
        //Serial.println(c, HEX); 

    }

    switch(c) {
        case C_FWD:
            newSpeed = absSpeed;
            break;
        case C_REV:
            newSpeed = -absSpeed;
            break;
        case C_LFT:
        case C_RGT:
        case C_STP:
            newSpeed = 0;
            break;
        case C_SUP:
            newSpeed = (absSpeed+10)*dir;
            break;
        case C_SDN:
            newSpeed = (absSpeed-10)*dir;
            break;
    }

    if(newSpeed != currSpeed) {
        Serial << "Speed: " << newSpeed << endl;
        leftWheel.setSpeed(newSpeed);
        rightWheel.setSpeed(newSpeed);
    }
            
}
