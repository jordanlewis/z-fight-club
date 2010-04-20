#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "../Engine/world.h"
#include "../Agents/agent.h"
#include "../Utilities/vec3f.h"
#include "color.h"
extern "C" {
#include "Parser/track-parser.h"
}

class Graphics
{
  private:
    Graphics();
    ~Graphics();
    Graphics(const Graphics&);
    Graphics &operator=(const Graphics &);
    static Graphics _instance;

    bool initialized;    /* !<is opengl ready to go */
    void arrow(Vec3f, Vec3f);
        
  public:
    void initGraphics();
    void render(World * world);	
    void render(Agent * agent);
    void render(TrackData_t *track);
    int sphere(Vec3f, float, Color);

    static Graphics &getInstance();
};
    
#endif
