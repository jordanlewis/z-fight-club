#ifndef ERROR_H
#define ERROR_H

#include <string>
#include <ostream>
using namespace std;

typedef enum {
    P_INPUT = 0,
    P_PHYSICS,
    P_AI,
    P_GRAPHICS,
    P_SOUND,
    P_CLIENT,
    P_SERVER,
    NUM_PROFILE
} Profile_t;

typedef enum {
    AGENT = 0,
    AI,
    ENGINE,
    NETWORK,
    PHYSICS,
    UTILITIES,
    GRAPHICS,
    PARSER,
    SOUND,
    NUM_MODULE
} ErrorModule_t;

typedef enum {
    TRIVIAL = 0,
    IMPORTANT,
    CRITICAL,
    NUM_VERBOSITY
} ErrorVerbosity_t;

class Error
{
  private:
    double last_start[NUM_PROFILE];
    double accumulated[NUM_PROFILE];

    static Error _instance;
    Error();
    Error(const Error&);
    Error &operator=(const Error &);
    ~Error();

  public:
    bool                module[NUM_MODULE];  /* !< which errors are we reporting */
    ErrorVerbosity_t    verbosity;           /* !< how trivial of an error do we print */
    static Error &getInstance();
    ostream nonestream;
    void on(ErrorModule_t);
    void off(ErrorModule_t);
    void setVerbosity(ErrorVerbosity_t);
    void log(ErrorModule_t, ErrorVerbosity_t, const string);
    void log(ErrorModule_t, ErrorVerbosity_t, const char *);
    void log(ErrorModule_t, ErrorVerbosity_t, const int);
    void log(ErrorModule_t, ErrorVerbosity_t, const float);
};

ostream &operator<<(ErrorModule_t,ErrorVerbosity_t);

#endif
