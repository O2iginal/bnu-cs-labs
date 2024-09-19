#version 400 core

layout(location = 0) in vec3 position;
layout(location = 1) in vec3 normal;
layout(location = 2) in vec2 texCoordinate;

uniform mat4 model;
uniform mat4 view;
uniform mat4 proj;
uniform mat3 normalMatrix;

out vec3 fragPosition;
out vec3 fragNormal;
out vec2 fragTexCoordinate;

void main(){
    vec4 worldPosition =  model * vec4(position, 1.0f);
    gl_Position = proj * view * worldPosition;

    fragPosition = worldPosition.xyz / worldPosition.w;
    fragNormal = normalMatrix * normal;
    fragTexCoordinate = texCoordinate;
//    fragTexCoordinate.y = 1.0f - fragTexCoordinate.y;
}
