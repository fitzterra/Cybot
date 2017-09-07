#include "driveTrain.h"
#include "utils.h"


int8_t speed = 0;
Wheel_HB *wheel;

void setup() {
    OpenSerial();

    Serial << "Starting...\n";
    wheel = new Wheel_HB(HBRIDGE_LEFT_FWD, HBRIDGE_LEFT_REV, LEFT);
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

