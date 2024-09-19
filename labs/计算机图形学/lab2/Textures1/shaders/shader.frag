#version 400 core

in vec2 fragTexCoordinate;

// 纹理（实际是一个 int 类型的值，表示这个纹理的绑定位置是哪个 texture unit） //
uniform sampler2D tex;
in vec3 outColor;     // modify6 - 接收顶点颜色信息
out vec4 fragColor;

void main()
{
    // 混合两种颜色可以使用分量乘法 * : //
    // https://en.wikibooks.org/wiki/GLSL_Programming/Vector_and_Matrix_Operations#Operators //
    // 或者使用 mix 函数: //
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/mix.xhtml //


    // 内置函数 texture() //
//   fragColor = texture(tex, fragTexCoordinate) * vec4(outColor, 1);
    fragColor = mix(texture(tex, fragTexCoordinate), vec4(outColor, 1), 0.5);
//    fragColor = texture(tex, fragTexCoordinate);
}
