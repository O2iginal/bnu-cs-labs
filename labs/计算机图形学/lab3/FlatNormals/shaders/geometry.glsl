#version 400 core

// 输入的图元(Primitive) //
layout (triangles) in;

// 输出的图元 //
layout (triangle_strip, max_vertices = 3) out;

// 图元的每个顶点 //
in vec3 vertexPosition[];

// 输出图元的法向 //
out vec3 vNormal;
out vec3 vPosition;

void main() {

    vec3 normal = cross(vertexPosition[1] - vertexPosition[0], vertexPosition[2] - vertexPosition[0]);

    // 计算法向 //
    vNormal = normal;
    vPosition = vertexPosition[0];
    // gl_in 是 内置数组 gl_in[0] 表示图元的第一个顶点 //
    gl_Position = gl_in[0].gl_Position;
    EmitVertex();

    vNormal = normal;
    vPosition = vertexPosition[1];
    gl_Position = gl_in[1].gl_Position;
    EmitVertex();

    vNormal = normal;
    vPosition = vertexPosition[2];
    gl_Position = gl_in[2].gl_Position;
    EmitVertex();

    EndPrimitive();
}
