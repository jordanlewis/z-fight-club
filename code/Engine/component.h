#ifndef COMPONENT_H
#define COMPONENT_H

class Component
{
    float avgTime;
    double lastRun;
  protected:
    bool start();
    void finish();
  public:
    float frequency;
    float fps();
    Component();
};

#endif
