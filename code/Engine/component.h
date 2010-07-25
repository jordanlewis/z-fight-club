#ifndef COMPONENT_H
#define COMPONENT_H

/*! Superclass for main game components.
 *
 *  This class tracks run time information for main game components. This
 *  enables "FPS" style tracking and frequency rate limiting for subclasses. To
 *  use this functionality in a subclass, set frequency to the maximum number
 *  of times per second the component should run, call start() at the beginning
 *  of a component's main run loop, and finish at the end of it. At that point,
 *  you can call fps() to determine the number of times this component would
 *  run per second if it were the only thing running based on a moving average
 *  of the runtime of the component. */
class Component
{
    float avgTime; /*!< Moving average of the runtime of the component */
    double lastRun; /*!< The last time that this component was run. */
  protected:
    bool start(); /*!< Call this at the start of your subclass's run loop */
    void finish(); /*!< Call this at the end of your subclass's run loop */
  public:
    float frequency; /*!< Set this to the desired FPS cap of your component */
    float fps(); /*!< Call this to find out how fast your component is running*/
    Component();
};

#endif
