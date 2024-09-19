#version 400 core

layout(location = 0) in vec3 position;

uniform mat4 view;
uniform mat4 proj;

out vec3 fragTexCoordinate;

void main(){
    gl_Position = proj * mat4(mat3(view)) * vec4(position, 1.0f);

    fragTexCoordinate = position;
}
