#include "utils.h"

void setup() {
    OpenSerial();
    OpenSoftSerial();
}

void loop() {
    D("Done\n");
    delay(2000);
    OpenSerial();
    delay(2000);
    OpenSerial(57600);
    
    delay(3000);

    DSS("Done\n");
    delay(2000);
    OpenSoftSerial();
    delay(2000);
    OpenSoftSerial(57600);
    while(1) {};
}
