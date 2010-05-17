/*! \file shader.vert
 *
 * \brief The vertex shader for Project 1.
 *
 * \author John Reppy
 *
 */

/* COPYRIGHT (c) 2008 John Reppy (http://www.cs.uchicago.edu/~jhr)
 * All rights reserved.
 */

#version 120

varying float attenuation;
varying vec3 normal;
varying vec3 color;
varying vec3 lightVec;

void main (void)
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;


        vec4 ecPos4 = gl_ModelViewMatrix * gl_Vertex;
        vec3 ecPos = ecPos4.xyz / ecPos4.w;
        normal = normalize(gl_NormalMatrix * gl_Normal);
        // compute the light intensity based on its type

        // compute vector from fragment to light
        lightVec = gl_LightSource[0].position.xyz - ecPos;
        lightVec = normalize(lightVec);
        // distance to light
        float dist = length(lightVec);

        // compute attenuation
        attenuation = 1.0 / (
                gl_LightSource[0].constantAttenuation +
                gl_LightSource[0].linearAttenuation * dist +
                gl_LightSource[0].quadraticAttenuation * dist * dist);
}
