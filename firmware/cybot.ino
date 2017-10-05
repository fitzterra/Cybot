/**
 * Cybot Resurrected.
 *
 **/

#include "config.h"
#include "debug.h"
#include "control.h"
#include "driveTrain.h"

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

// Display pointer
#ifdef DISPLAY_EN
#include "display.h"
Display *display;
#endif // DISPLAY_EN

// The drive train object pointer
DriveTrain *drvTrn;

// Function to initialize the shared space to default values where required.
// For some reason the members can not be initialized individually outside of a
// function at this level - not sure why that is.
void initShared() {
    shared.nxtCmd = CMD_ZZZ;
    shared.speed = 0;
    shared.dir = 0;
}

void setup() {
    initShared();

    OpenSerial();
    D(F("Starting...\n"));

    // Load command maps from EEPROM
    loadCmdMaps();

    // Create required input method instances
#ifdef INP_SERIAL_EN
    sInput = new SerialIn(commandMaps.serialMap, &shared.nxtCmd,
                          SI_MIN_DELAY, SI_REPEAT_MAX);
#endif // INP_SERIAL_EN
#ifdef INP_BT_EN
    btInput = new BTIn(commandMaps.btMap, &shared.nxtCmd,
                       BT_MIN_DELAY, BT_REPEAT_MAX);
#endif // INP_BT_EN
#ifdef INP_IR_EN
    irInput = new IRIn(IR_PIN, commandMaps.irMap, &shared.nxtCmd,
                       IR_MIN_DELAY, IR_REPEAT_MAX);
#endif // INP_IR_EN
#ifdef DISPLAY_EN
    display = new Display(DISP_UPD_FREQ);
#endif // DISPLAY_EN

    // Create the drive train object based on the selected drive option
#ifdef HBRIDGE_DRV_EN
    drvTrn = new DriveTrain(&shared.nxtCmd, HBRIDGE_LEFT_FWD, HBRIDGE_LEFT_REV,
                            HBRIDGE_RIGHT_FWD, HBRIDGE_RIGHT_REV);
#else
    drvTrn = new DriveTrain(&shared.nxtCmd, SERVO_LEFT, SERVO_RIGHT);
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
#ifdef DISPLAY_EN
            display,
#endif // DISPLAY_EN
            drvTrn,
        };

    // The scheduler
    TaskScheduler sched(tasks, NUM_TASKS(tasks));

    // and go...
    sched.run();
}
