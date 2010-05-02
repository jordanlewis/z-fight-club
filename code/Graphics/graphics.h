#ifndef GRAPHICS_H
#define GRAPHICS_H
#include "Engine/world.h"
#include "Agents/agent.h"
#include "Agents/ai.h"
#include "Utilities/vec3f.h"
#include "color.h"
#include <vector>
#include <deque>
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

    /*! \brief Draws an arrow at "center" pointing in the direction "dir"
     *         This exists for debugging purposes, particularly problems with matrix transformations
     */
    void DrawArrow(Vec3f, Vec3f);
    /*! \brief Draws a unit cube at "center" without using any matrix transformations
     *         This exists for debugging purposes, particularly problems with matrix transformations
     */
    void DrawCube(Vec3f);
        
  public:
    void initGraphics();
    void render();	
    void render(Agent * agent);
    void render(TrackData_t *track);
    void render(AIController *);
    void renderBoxPGeoms();
    void render(std::deque<Vec3f>);
    void render(std::vector<Vec3f>);
    int sphere(Vec3f, float, Color);

    static Graphics &getInstance();
};
    
#endif
