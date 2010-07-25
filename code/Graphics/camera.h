/*! \file camera.h
 * \brief Contains the declataion of the Camera class
 *
 * This file contains the class declataion for the Camera class, and the
 * declaration of the CameraMode_t enum, which is used exclusively by the
 * Camera class
 */
#ifndef CAMERA_H
#define CAMERA_H

#include "allclasses.h"
#include "Utilities/vec3f.h"

/*! \enum CameraMode_t
 * \brief The types of view that a camera can take of the world, such as overhead or bird's-eyr
 */
typedef enum CameraMode_t {
    OVERHEAD = 0,       //!< overhead view on the world
    FIRSTPERSON,        //!< view from an agent first person
    THIRDPERSON,        //!< view from behind the agent
    BIRDSEYE,           //!< above the agent, but follow
    nModes
} CameraMode_t;

/*! \class Camera
 * \brief An openGL camera, with functions for positioning the camera and exporting relevant matrices.
 */ 
class Camera {
    CameraMode_t mode; //!< what perspective the camera has
    Agent *agent; //!< the agent this camera is following (can be null)
    float FOVY; //!< The field-of-view angle for this camera
    Vec3f smooth_orientation; //!< A weighted average of camera directions used to smooth camera movements

    Vec3f pos, //!< The position of thie camera
          up, //!< The up vector for this camera
          target; //!< The point in space at which this camera is looking

    float zNear, //!< The distance to the near z-clipping plane
          zFar; //!< The distance to the far z-clipping plane

    Error *error; //!< A pointer to the \a Error logging class, for convenience
  public:
    Camera(); //!< Constructor that just initializes all fields to default values
    
    /*! \brief A constructor that initializes this camera with view \a mode and follows the agent \a agent
     * \param mode The view that this camera will take
     * \param agent The agent that this camera will follow
     */
    Camera(CameraMode_t, Agent *);

    ~Camera(); //!< A default destructor

    void setup(); //!< Initializes the camera to a default, fixed position in the corner of the map

    int wres, //!< The horizontal resolution (width) of the render buffer
        hres; //!< The vertical resolution (height) of the render buffer

    /*! \brief Make this camera look at \a target.
     * \param target The point that this camera will look at
     */
    void setTarget(Vec3f target);
    void cycleView(); //!< cycle through the camera view, for debugging
    void setProjectionMatrix(); //!< Set the openGL projection matrix to the projection matrix corresponding to this camera.
    void setOrthoMatrix(); //!< Set the openGL projection matrix to an orthographic matrix for this camera, to render heads-up-display elements.
    const Vec3f &getTarget() const; //!< Return \a Camera::target
    const Vec3f &getPos() const; //!< Return \a Camera::pos.
    const Vec3f &getUp() const; //!< Return \a Camera::up
    const Agent *getAgent() const; //!< Return \a Camera::agent

    float getWres(); //!< Return \a Camera::wres
    float getHres(); //!< Return \a Camera::hres

    friend class Input;
    friend class Graphics;
};

#endif
