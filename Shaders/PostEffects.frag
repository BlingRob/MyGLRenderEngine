#version 450 core 

in vec2 TexCoords; 
layout (binding = 10) uniform sampler2D scene;
subroutine vec4 TypeAffect();
subroutine uniform TypeAffect affect; 
uniform bool conv;
uniform bool invert;
uniform mat3 kernel;

out vec4 color;

subroutine(TypeAffect)
vec4 Invertion()
{
    return vec4(vec3(1.0f - texture(scene, TexCoords)),1.0f);
}
subroutine(TypeAffect)
vec4 Convolution()
{
        vec3 sampleTex[9];
        ivec2 CurPix = ivec2(gl_FragCoord.xy);
        sampleTex[0] = texelFetchOffset(scene, CurPix, 0, ivec2(-1.0f, 0.0f)).rgb;
        sampleTex[1] = texelFetchOffset(scene, CurPix, 0, ivec2( 0.0f, 1.0f)).rgb;
        sampleTex[2] = texelFetchOffset(scene, CurPix, 0, ivec2( 1.0f, 1.0f)).rgb;
        sampleTex[3] = texelFetchOffset(scene, CurPix, 0, ivec2(-1.0f, 0.0f)).rgb;
        sampleTex[4] = texelFetchOffset(scene, CurPix, 0, ivec2( 0.0f, 0.0f)).rgb;
        sampleTex[5] = texelFetchOffset(scene, CurPix, 0, ivec2( 1.0f, 0.0f)).rgb;
        sampleTex[6] = texelFetchOffset(scene, CurPix, 0, ivec2(-1.0f,-1.0f)).rgb;
        sampleTex[7] = texelFetchOffset(scene, CurPix, 0, ivec2( 0.0f,-1.0f)).rgb;
        sampleTex[8] = texelFetchOffset(scene, CurPix, 0, ivec2( 1.0f,-1.0f)).rgb;

        vec3 col = vec3(0.0);
        for(int i = 0; i < 9; i++)
            col += sampleTex[i] * kernel[i / 3][i % 3];   
        return vec4(col, 1.0);
}
subroutine(TypeAffect)
vec4 Default()
{
    return texture(scene, TexCoords);
}

void main()
{
	color = affect();
}
