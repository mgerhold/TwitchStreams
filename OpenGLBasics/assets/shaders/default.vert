#version 430 core

layout (location = 0) in vec2 aPos;
layout (location = 1) in vec3 aColor;

out vec3 fragmentColor;

uniform mat4 projectionMatrix;

void main()
{
   gl_Position = projectionMatrix * vec4(aPos.x, aPos.y, 0.0, 1.0);
   fragmentColor = aColor;
}