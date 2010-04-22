#ifndef ERROR_H
#define ERROR_H

#include <iostream>

typedef enum {
    AGENT = 0,
    ENGINE,
    NETWORK,
    PHYSICS,
    UTILITIES,
    GRAPHICS,
    PARSER,
    SOUND,
    NUM_ERRORS
} ErrorType_t;

class Error
{
    private:
	Error();
	~Error();
	static Error _instance;

    public:
	bool logging[NUM_ERRORS];	/* !< which errors are we reporting */
	static Error &getInstance();
	void on(ErrorType_t);
	void off(ErrorType_t);
	void log(ErrorType_t, const char *);
};
#endif
