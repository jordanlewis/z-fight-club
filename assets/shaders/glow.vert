void main() {
    gl_Position = ftransform();
    //gl_FrontColor = gl_Color;
        
    // Give the automatically generated texture coordinates to the frag shader
    gl_TexCoord[0] = gl_MultiTexCoord0;
}

