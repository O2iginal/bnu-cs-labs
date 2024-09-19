#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 color;

out vec3 fragColor;

// uniform 变量用于存储从 CPU 传递到 GPU 的变量，uniform 变量的值不能从 shader 中修改 //
uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

void main(){
    gl_Position = proj * view * model * vec4(position, 1.0f);
    fragColor = color;
}
