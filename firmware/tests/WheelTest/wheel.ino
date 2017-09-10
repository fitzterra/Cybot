#include "driveTrain.h"
#include "utils.h"


int8_t speed = 0;
#ifdef SERVO_DRV_EN
Wheel_CRS *wheel;
#else
Wheel_HB *wheel;
#endif

void setup() {
    OpenSerial();

    Serial << "Starting...\n";
#ifdef SERVO_DRV_EN
    wheel = new Wheel_CRS(SERVO_LEFT, LEFT);
#else
    wheel = new Wheel_HB(HBRIDGE_LEFT_FWD, HBRIDGE_LEFT_REV, LEFT);
#endif
}

void speedup() {
    if (speed>=100)
        return;
    speed = speed + 10;
    wheel->setSpeed(speed);
}

void slowdown() {
    if (speed<=-100)
        return;
    speed = speed - 10;
    wheel->setSpeed(speed);
}

void changeDirection() {
    speed = speed * -1;
    wheel->setSpeed(speed);
}

void loop() {
    while(!Serial.available()) {};

    switch(Serial.read()) {
        case 'w':
            speedup();
            break;
        case 'x':
            slowdown();
            break;
        case 's':
            changeDirection();
    }
}

