#include "utils.h"

void setup() {
    OpenSerial();
}

void loop() {
    D("Done\n");
    delay(2000);
    OpenSerial();
    delay(2000);
    OpenSerial(57600);
    while(1) {};
}
