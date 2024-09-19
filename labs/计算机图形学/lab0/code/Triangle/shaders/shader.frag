// fragment shader 在 vertex shader 之后执行 //
// 物体每个表面的每个像素都会执行该 fragment shader //

#version 330 core   //版本号
out vec4 fragColor;     // fragment shader 中必须输出一个 vec4 类型变量，表示物体表面每个像素的颜色，格式为 rgba //
in vec3 vertColor;    //输入颜色 为上一步输出(从vert同名out变量传入)

void main()
{
    fragColor = vec4(vertColor, 1.0f); //输出
}
