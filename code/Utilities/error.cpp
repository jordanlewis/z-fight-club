#include "error.h"
#include <iostream>
#include <ostream>

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
