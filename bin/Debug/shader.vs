#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoord;

out vec2 tCoord;
uniform mat4 uProjectionMatrix;
uniform mat4 uModelTranslate;



void main()
{
    tCoord = texCoord;
    gl_Position = uModelTranslate * vec4(position.xyz, 1.0f);

}
