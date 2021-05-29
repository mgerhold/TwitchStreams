#version 430 core

in vec3 fragmentColor;
out vec4 FragColor;

void main()
{
    FragColor = vec4(fragmentColor.r, fragmentColor.g, fragmentColor.b, 1.0f);
}