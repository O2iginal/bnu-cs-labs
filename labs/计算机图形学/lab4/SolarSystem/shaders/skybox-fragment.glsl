#version 400 core

in vec3 fragTexCoordinate;

uniform samplerCube cubemap;

out vec4 outColor;

void main()
{
//    outColor = vec4(1.0f);
    outColor = texture(cubemap, fragTexCoordinate);
}
