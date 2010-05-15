uniform sampler2D color_tex, bump_tex, spec_tex;
uniform int lightMode;
uniform vec3 tangent, bitangent;
varying vec3 color;

void main()
{
    gl_FragColor = texture2D(color_tex , gl_TexCoord[0].st);
}
