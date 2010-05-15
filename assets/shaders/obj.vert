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

uniform int	lightMode;	//!< the current value of View->lighting.
varying vec3 color;

const int NO_LIGHT = 0;

void main (void)
{
	gl_Position = ftransform();
	gl_TexCoord[0] = gl_MultiTexCoord0;


    if (lightMode != NO_LIGHT) {
	vec4 ecPos4 = gl_ModelViewMatrix * gl_Vertex;
	vec3 ecPos = ecPos4.xyz / ecPos4.w;
	vec3 norm = normalize(gl_NormalMatrix * gl_Normal);
      // compute the light intensity based on its type
	float intensity;
	if (gl_LightSource[0].position.w == 0.0) {
	  // directional light
	    vec3 lightVec = normalize(gl_LightSource[0].position.xyz);
	    intensity = max(dot(lightVec, norm), 0.0);
	}
	else { // point light
	  // compute vector from fragment to light
	    vec3 lightVec = gl_LightSource[0].position.xyz - ecPos;
	  // distance to light
	    float dist = length(lightVec);
	  // normalize light vector
	    lightVec = normalize(lightVec);
	  // compute attenuation
	    float attenuation = 1.0 / (
		gl_LightSource[0].constantAttenuation +
		gl_LightSource[0].linearAttenuation * dist +
		gl_LightSource[0].quadraticAttenuation * dist * dist);
	    if (gl_LightSource[0].spotCutoff == 180.0) {
		intensity = attenuation * max(dot(lightVec, norm), 0.0);
	    }
	    else {
	      // See if point on surface is inside cone of illumination
		float spotDot = dot(-lightVec, normalize(gl_LightSource[0].spotDirection));
		if (spotDot < gl_LightSource[0].spotCosCutoff) {
		    attenuation = 0.0; // light adds no contribution
		}
		else {
		  // Combine the spotlight and distance attenuation.
		    attenuation *= pow(spotDot, gl_LightSource[0].spotExponent);	
		}
		intensity = attenuation * max(dot(lightVec, norm), 0.0);
	    }
	}

      // the fragment color is the ambient color plus the lighting
	color = (gl_LightModel.ambient.rgb
		+ intensity * gl_LightSource[0].diffuse.rgb) * gl_Color.rgb;
	gl_FrontColor = vec4(clamp(color, 0.0, 1.0), 1.0);
    }
    else {
		color = vec3(gl_Color);
    }

}
