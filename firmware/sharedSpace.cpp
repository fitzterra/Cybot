/**
 * The shared space structure external definition.
 **/

#include "sharedSpace.h"

// Define the shared space.
// Since sharedSpace.h is included by config.h, the shared space is
// automatically available to almost all modules in the system. 
// An issue with this is that nothing defined in config.h is available here so
// initializing any of the shared members using config values can not be done
// here.
// The best place to initialize the members to default values would be in the
// main source file.
shared_t shared;
