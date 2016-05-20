//## you can use constant decls for symbolic constants
//## for example
//##	const int NO_LIGHT = 0;

#version 120
void main (void) {
    vec4 vertPos, ambient, diffuse, lightAmbient, color;
    vec3 norm, delta, lightVec, halfVec;
    float intensity, halfIntensity, spotAngle, dist, attenuation, spotPower;

    norm = normalize(gl_NormalMatrix * gl_Normal);
    vertPos = gl_ModelViewMatrix * gl_Vertex;
    delta = vec3(gl_LightSource[0].position - vertPos);
    dist = length(delta);

    // If it's directional, then our vector is just the normalized position
    // of the light. If not, we normalize the vector between the current vertex
    // and the light's position and use that as the vector.
    lightVec = normalize(delta);

    // Use this nice derived variable that's the emissive color of the material
    // plus the ambient color of the material times the global ambient color
    ambient = gl_FrontLightModelProduct.sceneColor;

    color = ambient;

    // how strong is the light, based on its angle from the surface?
    intensity = max(0.0, dot(norm, lightVec));

    if (intensity > 0.0)
    {
        diffuse = gl_FrontLightProduct[0].diffuse;
        lightAmbient = gl_FrontLightProduct[0].ambient;
        attenuation = 1.0;
        color += attenuation * (lightAmbient +
                                diffuse * intensity);
        // simple specular calculation
        halfVec = normalize(gl_LightSource[0].halfVector.xyz);
        halfIntensity = max(0.0, dot(norm, halfVec));
        color += gl_FrontLightProduct[0].specular *
                    pow(halfIntensity, gl_FrontMaterial.shininess);

    }
    gl_FrontColor = color;
    gl_Position = ftransform();
}
