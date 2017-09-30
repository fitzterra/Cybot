/**
 * Shared data structures available to all modules.
 *
 * The shared data structure will be used as global shared variable area that
 * all modules/tasks will have access to. This makes it easy for the each
 * module/task to feed current state information back that can be used by other
 * modules or tasks.
 **/

#include <stdint.h>

#ifndef __SHARED_SPACE__
#define __SHARED_SPACE__

struct shared_t {
    // Storage for next command to execute. A value of CMD_ZZZ means that there are
    // no command to execute currently. Every command consumer is responsible for
    // setting this to CMD_ZZZ after consuming any command held here.
    uint8_t nxtCmd;
    int8_t speed;       // Current speed
    int8_t dir;         // Current direction
};

extern shared_t shared;
    
#endif //__SHARED_SPACE__
