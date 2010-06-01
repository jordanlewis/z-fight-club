uniform sampler2D glowTexture;
const float OFFSET = 0.005;
const float FALLOFF = 75.0;
void main() {   
    vec4 color = vec4(0.0,0.0,0.0,0.0);

    vec2 P[4];
    P[0] = vec2(-1,-1);
    P[1] = vec2(-1,1);
    P[2] = vec2(1,-1);
    P[3] = vec2(1,1);

    vec2 l = gl_TexCoord[0].st;
    for(int i=0;i<4;i++) color += texture2D(glowTexture,l+(2.0*P[i]*OFFSET))/FALLOFF;
    for(int i=0;i<4;i++) color += 4.0*texture2D(glowTexture,l+(vec2(1,2)*P[i]*OFFSET))/FALLOFF;
    for(int i=0;i<4;i++) color += 4.0*texture2D(glowTexture,l+(vec2(2,1)*P[i]*OFFSET))/FALLOFF;
    for(int i=0;i<4;i++) color += 7.0*texture2D(glowTexture,l+(vec2(2,0)*P[i]*OFFSET))/FALLOFF;
    for(int i=0;i<4;i++) color += 16.0*texture2D(glowTexture,l+(P[i]*OFFSET))/FALLOFF;
    for(int i=0;i<2;i++) color += 26.0*texture2D(glowTexture,l+(vec2(0,1)*P[i]*OFFSET))/FALLOFF;
    for(int i=0;i<4;i+=2) color += 26.0*texture2D(glowTexture,l+(vec2(1,0)*P[i]*OFFSET))/FALLOFF;
    color += 41.0*texture2D(glowTexture,l)/FALLOFF;
    gl_FragColor = color;
}

