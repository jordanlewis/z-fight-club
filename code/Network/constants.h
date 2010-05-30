#ifndef NET_CONSTANTS_H
#define NET_CONSTANTS_H

#define DEFAULT_NETWORK_PORT 6888
#define DEFAULT_MAX_SERVER_CONNECTIONS 16

//Used by client.h
#define NET_RTT_MIX_FACTOR .2 //newRTT=mix_factor*oldRTT+(1-mix_factor)freshRTT
#define NET_RANGE_FUDGE 5 /* Allow a fudge factor in our calculations of 
                             * acceptable error in the client's dead reckoning.
                             * This helps account for acceleration, as the 
                             * acceptable error only takes velocity into
                             * account. */

#endif
