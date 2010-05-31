#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "allclasses.h"
#include "Engine/component.h"
#include "color.h"
#include <vector>
#include <deque>

extern "C" {
#include "Parser/track-parser.h"
}

typedef enum
{
    COLOR,
    GLOW
} Layer_t;

class Graphics : public Component
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
    World *world;
    Error *error;

  public:
    void initGraphics();
    void render();
    void renderGlowLayer();
    void renderColorLayer();
    void renderHUD();
    void render(Agent * agent);
    void render(TrackData_t *track);
    void render(AIController *);
    void render(Hud *);
    void renderBoxPGeoms();
    void render(std::deque<Vec3f>);
    void render(std::vector<Vec3f>);

    static Graphics &getInstance();
};

#endif
