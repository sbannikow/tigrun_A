#version 330 core
layout (location = 0) in vec3 position;
layout (location = 1) in vec3 color;

uniform mat4 uProjectionMatrix;
uniform mat4 uModelTranslate;
uniform vec4 vPos;

out vec3 ourColor;


void main()
{
    //cColor = color;
    gl_Position = uModelTranslate * vec4(position.xyz, 1.0f);
    ourColor = color;
 
}