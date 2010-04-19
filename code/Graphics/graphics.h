#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "../Engine/world.h"
#include "../Agents/agent.h"
#include "../Utilities/vec3f.h"
#include "color.h"

class Graphics
{
  private:
    Graphics();
    ~Graphics();
    Graphics(const Graphics&);
    Graphics &operator=(const Graphics &);
    static Graphics _instance;

    bool initialized;    /* !<is opengl ready to go */
        
  public:
    void initGraphics();
    void render(World * world);	
    void render(Agent * agent);
    int sphere(Vec3f, float, Color);
    int arrow(Vec3f, Vec3f);

    static Graphics &getInstance();
};
    
#endif
