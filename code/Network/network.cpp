#include "network.h"

int networkInit()
{
    if (enet_initialize() != 0)
	{
	    cerr << "ENet could not be initialized" << endl;
	    return -1;
	}
    
    return 0;
}
