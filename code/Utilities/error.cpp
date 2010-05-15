#include "error.h"
#include "defs.h"
#include <iostream>
#include <ostream>

#define STR( foo ) # foo

const char* ProfileNames[] = 
{
    STR( P_INPUT ),
    STR( P_PHYSICS ),
    STR( P_AI ),
    STR( P_GRAPHICS ),
    STR( P_SOUND ),
    STR( P_CLIENT ),
    STR( P_SERVER )
};

Error Error::_instance;

Error::Error()
{
    int i;
    for (i = 0; i < NUM_MODULE; i++)
        module[i] = false;

    verbosity = CRITICAL;
}

Error::~Error()
{
}

Error &Error::getInstance()
{
    return _instance;
}

void Error::on(ErrorModule_t error)
{
    module[error] = true;
}

void Error::off(ErrorModule_t error)
{
    module[error] = false;
}

void Error::setVerbosity(ErrorVerbosity_t verbosity)
{
    this->verbosity = verbosity;
}

void Error::log(ErrorModule_t error, ErrorVerbosity_t verbosity, const string msg)
{
    if (module[error] || verbosity >= this->verbosity)
        std::cerr << msg;
}

void Error::log(ErrorModule_t error, ErrorVerbosity_t verbosity, const char * msg)
{
    if (module[error] || verbosity >= this->verbosity)
        std::cerr << msg;
}

void Error::log(ErrorModule_t error, ErrorVerbosity_t verbosity, const int msg)
{
    if (module[error] || verbosity >= this->verbosity)
        std::cerr << msg;
}

void Error::log(ErrorModule_t error, ErrorVerbosity_t verbosity, const float msg)
{
    if (module[error] || verbosity >= this->verbosity)
        std::cerr << msg;
}

void Error::pin(Profile_t p)
{
    last_start[p] = GetTime();
}

void Error::pout(Profile_t p)
{
    accumulated[p] += GetTime() - last_start[p];
}

void Error::pdisplay()
{
    static unsigned char headerclock = 0;
    if ((headerclock++ & 0x0F) == 0)
    {
        for (int i=0; i < NUM_PROFILE; i++)
        {
            printf("%11s", ProfileNames[i]);
        }
        printf("\n");
    }

    for (int i=0; i < NUM_PROFILE; i++)
    {
        printf("%11.4f", accumulated[i]);
    }
    printf("\n");
}
