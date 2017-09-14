#include "config.h"
#include "debug.h"
#include "control.h"

class InpCons : public TimedTask {
    private:
        uint32_t rate;
        uint8_t *cmd;

    public:
    InpCons(uint32_t r, uint8_t *c) : 
        TimedTask(millis()), rate(r), cmd(c)  {
        D(F("Consumer initialized.\n"));
    };

    void run(uint32_t now) {
        if(*cmd < CMD_ZZZ) {
            D(F("New command: ") << cmdName[*cmd] << endl);
            DSS(F("New command: ") << cmdName[*cmd] << endl);
            *cmd = CMD_ZZZ;
        }
        incRunTime(rate);
    }
};


void setup() {
    OpenSerial();
    D(F("We're open.\n"));

    OpenSoftSerial();
    DSS(F("Soft serial is open.\n"));
}

void loop() {
    uint8_t cmd = CMD_ZZZ;

    SerialIn sIn(commandMaps.serialMap, &cmd, SI_MIN_DELAY, SI_REPEAT_MAX);
    BTIn bIn(commandMaps.btMap, &cmd, BT_MIN_DELAY, BT_REPEAT_MAX);
    IRIn iIn(IR_PIN, commandMaps.irMap, &cmd, IR_MIN_DELAY, IR_REPEAT_MAX);
    InpCons consumer(100, &cmd);
    Task *tasks[] = {&sIn, &bIn, &iIn, &consumer};
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    sched.run();
}
