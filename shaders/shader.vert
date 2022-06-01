#version 330 core

layout(location = 0) in vec3 position;
layout(location = 5) in vec2 inUV;

//out vec2 inUV;

void main() {
//    inUV = iUV;
    gl_Position = vec4(position, 1.0);
}
