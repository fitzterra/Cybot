/**
 * Cybot Resurrected.
 *
 **/

#include "config.h"
#include "debug.h"
#include "control.h"
#include "driveTrain.h"

// Storage for next command to execute. A value of CMD_ZZZ means that there are
// no command to execute currently. Every command consumer is responsible for
// setting this to CMD_ZZZ after consuming any command held here.
uint8_t nxtCmd = CMD_ZZZ;

// Set up pointer for each enabled input method.
#ifdef INP_SERIAL_EN
SerialIn *sInput;
#endif // INP_SERIAL_EN
#ifdef INP_BT_EN
BTIn *btInput;
#endif // INP_BT_EN
#ifdef INP_IR_EN
IRIn *irInput;
#endif // INP_IR_EN
// The drive train object pointer
DriveTrain *drvTrn;


void setup() {
    OpenSerial();
    D(F("Starting...\n"));

    // Load command maps from EEPROM
    loadCmdMaps();

    // Create required input method instances
#ifdef INP_SERIAL_EN
    sInput = new SerialIn(commandMaps.serialMap, &nxtCmd,
                          SI_MIN_DELAY, SI_REPEAT_MAX);
#endif // INP_SERIAL_EN
#ifdef INP_BT_EN
    btInput = new BTIn(commandMaps.btMap, &nxtCmd,
                       BT_MIN_DELAY, BT_REPEAT_MAX);
#endif // INP_BT_EN
#ifdef INP_IR_EN
    irInput = new IRIn(IR_PIN, commandMaps.irMap, &nxtCmd,
                       IR_MIN_DELAY, IR_REPEAT_MAX);
#endif // INP_IR_EN

    // Create the drive train object based on the selected drive option
#ifdef HBRIDGE_DRV_EN
    drvTrn = new DriveTrain(&nxtCmd, HBRIDGE_LEFT_FWD, HBRIDGE_LEFT_REV,
                            HBRIDGE_RIGHT_FWD, HBRIDGE_RIGHT_REV);
#else
    drvTrn = new DriveTrain(&nxtCmd, SERVO_LEFT, SERVO_RIGHT);
#endif // HBRIDGE_DRV_EN
}

void loop() {
    // Set up the tasks
    Task *tasks[] = {
#ifdef INP_SERIAL_EN
            sInput,
#endif // INP_SERIAL_EN
#ifdef INP_BT_EN
            btInput,
#endif // INP_BT_EN
#ifdef INP_IR_EN
            irInput,
#endif // INP_IR_EN
            drvTrn,
        };

    // The scheduler
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // and go...
    sched.run();
}
