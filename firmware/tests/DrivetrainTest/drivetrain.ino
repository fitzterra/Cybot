#include "driveTrain.h"
#include "utils.h"

DriveTrain *dt;


void setup() {
    OpenSerial();

    Serial << "Starting...\n";
    dt = new DriveTrain(HBRIDGE_LEFT_FWD, HBRIDGE_LEFT_REV,
                        HBRIDGE_RIGHT_FWD, HBRIDGE_RIGHT_REV);
}

void loop() {
    while(!Serial.available()) {};

    switch(Serial.read()) {
        case 'w':
            dt->speedUp();
            break;
        case 'x':
            dt->slowDown();
            break;
        case 's':
            dt->stop();
            break;
        case 'a':
            dt->left();
            break;
        case 'd':
            dt->right();
            break;
    }
}

