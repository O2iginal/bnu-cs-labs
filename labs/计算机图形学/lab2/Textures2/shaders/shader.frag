#version 400 core

in vec2 fragTexCoordinate;

// 纹理（实际是一个 int 类型的值，表示这个纹理的绑定位置是哪个 texture unit） //
uniform sampler2D tex[2];
//uniform sampler2D tex2;

out vec4 outColor;

void main()
{
    // 混合两种颜色可以使用分量乘法 * : //
    // https://en.wikibooks.org/wiki/GLSL_Programming/Vector_and_Matrix_Operations#Operators //
    // 或者使用 mix 函数: //
    // https://www.khronos.org/registry/OpenGL-Refpages/gl4/html/mix.xhtml //


    // 内置函数 texture() //
//   outColor = texture(tex[0], fragTexCoordinate);    // modify8 : 改用使用两个纹理
   outColor = mix(texture(tex[0], fragTexCoordinate), texture(tex[1], fragTexCoordinate), 0.5);
}
