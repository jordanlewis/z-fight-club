#include "error.h"
#include <iostream>
#include <ostream>

Error Error::_instance;

Error::Error()
{
    int i;
    for (i = 0; i < NUM_ERRORS; i++)
	logging[i] = false;
}

Error::~Error()
{
}

Error &Error::getInstance()
{
    return _instance;
}

void Error::on(ErrorType_t error)
{
    logging[error] = true;
}

void Error::off(ErrorType_t error)
{
    logging[error] = false;
}

void Error::log(ErrorType_t error, const char * msg)
{
    if (logging[error])
	std::cerr << msg;
}
