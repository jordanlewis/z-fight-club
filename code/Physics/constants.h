#ifndef PHYSICS_CONSTANTS
#define PHYSICS_CONSTANTS
/* Holds default values for all physics constants.  All constants should be
 * named with the prefix PH in order to avoid name collisions. */

//Internal physics engine parameters
#define PH_MAX_CONTACTS 8
#define PH_TIMESTEP .01

//General physics world parameters
#define PH_GRAVITY -9.8
#define PH_LINDAMP .005
#define PH_ANGDAMP .1
#define PH_MAXACC 100

//Weapon parameters
#define PH_SMACKFORCE 100000

#endif
