#version 120

uniform sampler2D color_tex, bump_tex, spec_tex;
//uniform vec3 tangent, bitangent;
varying vec3 normal;
varying vec3 color;
varying vec3 lightVec;
varying float attenuation;

void main()
{
		/* grab the bump map information*/
		//vec3 BumpNorm = vec3(texture2D(bump_tex, gl_TexCoord[0].st));
		//BumpNorm = BumpNorm - 0.5;
		//BumpNorm = normalize(BumpNorm);

		/* grab the specular map information */
		//vec4 spec = texture2D(spec_tex, gl_TexCoord[0].st);
		//vec3 spec_color = vec3(spec);
		//float phong_exp = spec[3] * 128;
		
		//vec3 lightVec_tangent_space = vec3(dot(lightVec, tangent), dot(lightVec, bitangent), dot(lightVec, normal));

		float intensity;
		//intensity = attenuation * max(dot(lightVec_tangent_space, BumpNorm), 0.0);
		intensity = attenuation * max(dot(lightVec, normal), 0.0);
	
        gl_FragColor = 
            vec4(
                intensity
                vec3(texture2D(color_tex, gl_TexCoord[0].st)), 1.0); 
			//vec4(
			//		(pow(intensity, phong_exp) * gl_LightSource[0].specular.rgb) * 
			//		vec3(texture2D(spec_tex, gl_TexCoord[0].st)), 1.0);
}
