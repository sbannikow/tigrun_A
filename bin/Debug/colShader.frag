#version 330 core
out vec4 color;
uniform vec4 cColor;




void main()
{
    color = vec4(cColor);
}