#include "commands.h"

void setup() {
    Serial.begin(115200);
    D(F("Serial is open!\n"));
}

void loop() {
    char c;

    // Wait for intput
    while(!Serial.available()) {};

    c = Serial.read();
    switch(c) {
        case 'l':
            if (loadCmdMaps())
                D(F("Successfully loaded\n"));
            else
                D(F("Load Failed\n"));
            break;
        case 's':
            saveCmdMaps();
            D(F("Saved maps\n"));
            break;
        case 'f':
            Serial << F("FWD now: ") << (int)commandMaps.serialMap[CMD_FWD] << endl;
            Serial << F("Press new FWD key: ");
            while(!Serial.available()) {};
            c = Serial.read();
            Serial << endl;
            if(Serial.available()) {
                while(Serial.available())
                    Serial.read();
                Serial << F("Multi character keys not acepted.\n");
                break;
            }
            commandMaps.serialMap[CMD_FWD] = c;
            saveCmdMaps();
            break;
        case 'F':
            Serial << F("FWD now: ") << (int)commandMaps.serialMap[CMD_FWD] << endl;
            break;
        default:
            D(F("Dunno what you want. 'l' or 's' only\n"));
    }
}
