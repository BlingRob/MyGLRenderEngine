#version 450 core 

in vec2 TexCoords; 
uniform sampler2D scene;
uniform bool conv;
uniform bool invert;
uniform mat3 kernel;

out vec4 color;

const float offset = 1.0 / 1000.0;

void main()
{
	if(invert)
        color = vec4(vec3(1.0f - texture(scene, TexCoords)),1.0f);
    else if(conv)
    {
            vec2 offsets[9] = vec2[](
            vec2(-offset,  offset), // top-left
            vec2( 0.0f,    offset), // top-center
            vec2( offset,  offset), // top-right
            vec2(-offset,  0.0f),   // center-left
            vec2( 0.0f,    0.0f),   // center-center
            vec2( offset,  0.0f),   // center-right
            vec2(-offset, -offset), // bottom-left
            vec2( 0.0f,   -offset), // bottom-center
            vec2( offset, -offset)  // bottom-right    
        );
        /*float kernel[9] = float[](
            1.0 , 1.0, 1.0,
            1.0, -8.0 , 1.0,
            1.0, 1.0, 1.0  
        );*/
        
        vec3 sampleTex[9];
        for(int i = 0; i < 9; i++)
            sampleTex[i] = vec3(texture(scene, TexCoords.st + offsets[i]));
        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i / 3][i % 3];   
        color = vec4(col, 1.0);
     }
     else
	    color = texture(scene, TexCoords);
}