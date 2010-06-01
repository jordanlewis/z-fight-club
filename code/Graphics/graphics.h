#ifndef GRAPHICS_H
#define GRAPHICS_H

#include "allclasses.h"
#include "Engine/component.h"
#include "color.h"
#include <vector>
#include <deque>

#if defined(__APPLE__) && defined(__MACH__)
#  include <OpenGL/gl.h>
#else
#  define GL_GLEXT_PROTOTYPES
#  include <GL/gl.h>
#endif

extern "C" {
#include "Parser/track-parser.h"
#include "shader.h"
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

    GLuint glowTexId;
    bool glowEnabled;
    static const int glowTexWidth = 512,
                     glowTexHeight = 512;
    ShaderProgram_t *glowShader;
    void copyBufferToTexture(); // Should replace this with a FBO
    void renderGlowTexture();
    void renderGlowLayer();
    void renderColorLayer();
    void initGlow();

  public:
    void initGraphics();
    void render();
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
