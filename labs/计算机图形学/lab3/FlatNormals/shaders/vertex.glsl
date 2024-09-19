#version 400 core

layout(location = 0) in vec3 position; // 位置 //

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vertexPosition;

void main(){

    // 在世界坐标系中进行计算 //
    vec4 worldPosition = model * vec4(position, 1.0f);

    gl_Position = proj * view * worldPosition;

    vertexPosition = vec3(worldPosition) / worldPosition.w;
}
