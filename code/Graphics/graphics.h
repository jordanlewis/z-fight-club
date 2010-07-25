/*! \file graphics.h
 * \brief Contains the declaration of the graphics singleton class
 *
 * This file contains the class declaration for the graphics singleton class,
 * which provides all of the game's rendering functions
 */

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

/*! \enum Layer_t
 * \brief The set of all render layer types: color and glow.
 *
 * This enum is used to indicate which layer is being rendered, the color layer
 * or the glow layer.
 */
typedef enum Layer_t
{
    COLOR,
    GLOW
} Layer_t;

/*! \class Graphics
 * \brief A singleton class which provides all of the game's rendering functions.
 *
 * This class is a singleton (guarantees that there is only one instance of it
 * in existence) and provides rendering functions to the rest of the game.
 * In so doing, it also manages the state of opengl.
 */
class Graphics : public Component
{
  private:
    Graphics(); //!< Private default constructor, to prevent accidental initialization of the class
    ~Graphics(); //!< Private default destructor, to prevent accidental deletion of the class.
    Graphics(const Graphics&); //!< Private copy constructor, to prevent accidental initialization of the class.
    Graphics &operator=(const Graphics &); //!< Private override of the equals copy operator, to prevent accidental initialization of the class.
    static Graphics _instance; //!< The lone, static instance of this class, used by rest of the game to access rendering functions.

    /*! Describes whether the graphics class is ready to start rendering.
        This prevents other classes from calling rendering functions while
        openGL is still being set up
    */
    bool initialized;    

    /*! \brief Draws an arrow at "center" pointing in the direction "dir".
     *         This exists for debugging purposes, particularly problems with matrix transformations
     *  \param center the start point of the tail of the arrow
     *  \param dir the direction that the arrow will point (assuming \a center as the origin)
     */
    void DrawArrow(Vec3f, Vec3f);

    /*! \brief Draws a unit cube at "center" without using any matrix transformations.
     *         This exists for debugging purposes, particularly problems with matrix transformations
     *  \param center the location at which the cube is drawn.
     */
    void DrawCube(Vec3f);
    World *world; //!< A convenience pointer to the \a World singleton
    Error *error; //!< A convenience pointer to the \a Error logging singleton

    GLuint glowTexId; //!< The texture to which the glow layer is rendered, which will be superimposed on the color layer.
    bool glowEnabled; //!< Whether to render the glow layer, as determined by whether the user's computer supports glow.
    static const int glowTexWidth = 512, //!< The width of the texture to which the glow layer is rendered.
                     glowTexHeight = 512; //!< The height of the texture to which the glow layer is rendered.
    ShaderProgram_t *glowShader; //!< The shader that composites the glow layer with the color layer

    /*! \brief Once the glow layer has been rendered to the back buffer, this copies the back buffer to the glow texture
     */
    void copyBufferToTexture();

    /*! \brief Draws the glow texture onto a flat polygon, so that \a glowShader can composite it onto the color layer
     */
    void renderGlowTexture();

    /*! \brief Does the initial rendering of the glow layer into the back buffer */
    void renderGlowLayer();

    /*! \brief Renders the color layer into the back buffer */
    void renderColorLayer();
    
    /*! Initializes data structures associated with the glow effect */
    void initGlow();

  public:
    void initGraphics(); //!< Initializes openGL and prepares \a Graphics to start rendering */
    void render(); //!< Render the current scene
    void renderHUD(); //!< Render the heads up display

    /*! \brief Render an agent as an arrow for debugging.
     * \param agent the agent to render
     */
    void render(Agent * agent);

    /*! \brief Render the lanes of a track.
     * \param track The track whose lanes are rendered
     */
    void render(TrackData_t *track);

    /*! \brief Render an AI's direction and target, for debugging.
     * \param aiController the AI to draw
     */
    void render(AIController *);

    /*! \brief Render a path as represented by a deque of points
     * \param path The path to render
     */
    void render(std::deque<Vec3f>);

    /*! \brief Render a path as represented by a vector of points
     * \param path The path to render
     */
    void render(std::vector<Vec3f>);

    static Graphics &getInstance(); //!< Return a reference to the graphics singleton
};

#endif
