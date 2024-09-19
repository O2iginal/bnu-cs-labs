#version 400 core

layout(location = 0) in vec3 position; // 位置 //
layout(location = 1) in vec3 vertexNormal;  // +++传入顶点平均法向

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;

out vec3 vPosition;
out vec3 vNormal;  // +++传给fragment法向信息

void main(){

    vNormal = vertexNormal;  // +++赋值

    // 在世界坐标系中进行计算 //
    vec4 worldPosition = model * vec4(position, 1.0f);

    gl_Position = proj * view * worldPosition;

    vPosition = vec3(worldPosition) / worldPosition.w;
}
