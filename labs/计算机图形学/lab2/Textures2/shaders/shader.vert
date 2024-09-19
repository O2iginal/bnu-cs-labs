#version 400 core

layout(location = 0) in vec2 position; // 位置坐标 //
layout(location = 1) in vec2 texCoordinate; // 纹理坐标 //


uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;


out vec2 fragTexCoordinate;

void main(){
    gl_Position = proj * view * model * vec4(position, 0.0f, 1.0f);
    fragTexCoordinate = vec2(texCoordinate.x, 1.0-texCoordinate.y);
}
