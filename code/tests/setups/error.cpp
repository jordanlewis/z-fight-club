#include "Utilities/error.h"

void testSetup()
{
    Error &error = Error::getInstance();

    error.log(UTILITIES, CRITICAL, "Welcome to the error system\n");
    error.log(UTILITIES, CRITICAL, "Right now all module specific messages are disabled... ");
    error.log(UTILITIES, CRITICAL, "However as you can see these messages are still popping up ");
    error.log(UTILITIES, CRITICAL, "that's because they're CRITICAL. By default CRITICAL messages print ");
    error.log(UTILITIES, CRITICAL, "no matter what\n");
    error.log(UTILITIES, IMPORTANT, "this message won't print because it's only IMPORTANT\n");
    error.log(UTILITIES, TRIVIAL, "and this one really won't, it's TRIVIAL\n");

    /* we can change how verbose we are with error messages */
    error.setVerbosity(TRIVIAL);

    error.log(UTILITIES, IMPORTANT, "Everything >= the verbosity setting will print, so in TRIVIAL mode we print everything.\n");

    error.setVerbosity(IMPORTANT);
    error.log(UTILITIES, TRIVIAL, "So a trivial message won't print in IMPORTANT mode\n");



    /* We can also filter messages by module, turning a module "on" will give you
     * every message from the module, no matter how trivial */

    error.on(AGENT);

    error.log(AGENT, TRIVIAL, "We'll now see even trivial messages about agents\n");

    error.off(AGENT);
    error.log(AGENT, TRIVIAL, "Now we won't\n");

    /* error logging also works on ints and floats */
    error.log(UTILITIES, CRITICAL, 1);
    error.log(UTILITIES, CRITICAL, "\n");
    error.log(UTILITIES, CRITICAL, 3.14f);
    error.log(UTILITIES, CRITICAL, "\n");

    error.log(ENGINE, CRITICAL, "Now the engine is going to segfault because this testSetup doesn't implement any agents. But hey this is the error setup, segfaulting is a feature, not a bug.\n");
}
