/*!
*	File: OpenGLWidget.cpp
*	Date: 2018/03/01 19:42
*
*	Author: Zhang Yu
*	Contact: YuCrazing@mail.bnu.edu.cn
*
*	Description:
*	https://learnopengl.com/
*
*
*/


#include "OpenGLWidget.h"

#include <QFile>
#include <QDebug>


OpenGLWidget::OpenGLWidget(QWidget * parent) :QOpenGLWidget(parent) {
    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    this->setFormat(format);

    Q_INIT_RESOURCE(Resources);
}

void OpenGLWidget::initializeGL() {

    // 在任何类中使用 OpenGL 函数，需要先调用 initializeOpenGLFunctions();
    // 除了 initializeGL() 与 paintGL() 之外，其他函数中如需要使用 OpenGL 函数，需要先调用 makeCurrent()

    if (!initializeOpenGLFunctions()) {
        qFatal("OpenGLWidget::initializeOpenGLFunctions() failed.\n");
        return;
    }


    /* 以下代码演示如何使用 OpenGL 创建和使用 shader object 与 program object */

    // 从 shader.vert 和 shader.frag 文件中读取 shader 源代码
    QFile vertexShaderFile, fragmentShaderFile;

    vertexShaderFile.setFileName(":/shaders/shader.vert");
    fragmentShaderFile.setFileName(":/shaders/shader.frag");

    if (!vertexShaderFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Vertex shader file open failed." << endl;
        return;
    }
    if (!fragmentShaderFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Fragment shader file open failed." << endl;
        return;
    }
    QTextStream vertexShaderTextStream(&vertexShaderFile);
    QTextStream fragmentShaderTextStream(&fragmentShaderFile);

    // 使用字符数组存储 shader 源代码
    // https://wiki.qt.io/Technical_FAQ#How_can_I_convert_a_QString_to_char.2A_and_vice_versa.3F
    QByteArray vertexShaderSourceTemp = vertexShaderTextStream.readAll().toLocal8Bit();
    const char * vertexShaderSource = vertexShaderSourceTemp.data();
    QByteArray fragmentShaderSourceTemp = fragmentShaderTextStream.readAll().toLocal8Bit();
    const char * fragmentShaderSource = fragmentShaderSourceTemp.data();

    vertexShaderFile.close();
    fragmentShaderFile.close();

//    qDebug("%s\n", vertexShaderSource);
//    qDebug("%s\n", fragmentShaderSource);

    const int bufSize = 1 << 10;
    GLint success;
    GLchar infoLog[bufSize];

    GLuint vertexShader, fragmentShader;

    // 创建 shader object
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

    // 写入 shader 的源代码
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    // 编译 shader
    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);

    // 检查是否编译成功，若编译失败则输出错误信息
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, bufSize, NULL, infoLog);
        qDebug("Vertex shader compiled error: %s\n", infoLog);
    }
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, bufSize, NULL, infoLog);
        qDebug("Fragment shader compiled error: %s\n", infoLog);
    }


    // 创建 program object，一个 program object 中可能包含多个 shader
    program = glCreateProgram();

    // 将 shader 添加到 program
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    // 链接 program
    glLinkProgram(program);

    // 检查是否链接成功，若链接失败则输出错误信息
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, bufSize, NULL, infoLog);
        qDebug("Shader program linked error: %s\n", infoLog);
    }

    // 删除 shader object
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);




    /* 以下代码演示如何使用 OpenGL 绘制一个三角形 */

    // 使用数组存储所有顶点的属性（位置）数据
    float vertices[] = {
           -1.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // left     (-1.0, -0.5)    red
            0.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,    // right    (0.0,  -0.5)    green
            -0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // top      (-0.5,  0.5)    blue

            0.0f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,    // left     (0.0,  -0.5)    red
             1.0f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // right    (1.0,  -0.5)    green
             0.5f,  0.5f, 0.0f,  0.0f, 0.0f, 1.0f,  // top      (0.5,   0.5)    blue
       };


    // 创建 vertex buffer object，vertex buffer object 本质上是一个显存数组，用于存储数据
    glGenBuffers(1, &VBO);

    // 绑定 vertex buffer object，这是 OpenGL 的特性，任何对象在使用之前都要绑定
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 将顶点数据从 内存数组vertices 复制到 显存数组VBO
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);


    // 创建 vertex array object，vertex array object 用来指定顶点属性数据格式，不存储顶点数据，需要与 VBO 配合使用
    glGenVertexArrays(1, &VAO);

    // 绑定 vertex array object
    glBindVertexArray(VAO);

    // vertex buffer object 已绑定，此处不用再次绑定
    // glBindBuffer(GL_ARRAY_BUFFER, VBO);

    // 用 VAO 指定每个顶点属性数据的存储位置与格式 ( 只有每条数据(一个顶点的6个参数) 的前3个位置坐标数据  )
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);   // 启用编号为 0 的属性

    // +++++++++++++++++++++++ 如上两行, 同样的方法, 指定顶点的颜色属性数据和格式
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);   // 启用编号为 1 的属性

    // 解除绑定的 VBO 与 VAO
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

void OpenGLWidget::paintGL() {

    // Qt 用来更新窗口的函数，每一帧必须要调用这个函数
    update(); // necessary


    // 设置视窗的位置与大小
    // multiply devicePixelRatio() on retina display
    // retina 显示屏需要把 viewportWidth 和 viewportHeight 乘以 2
    glViewport(0, 0, this->width(), this->height());

    // 设置清除颜色
    glClearColor(0.0, 0.0, 0.0, 1.0);
    // 使用清除颜色来清除画布（作为背景色）
    glClear(GL_COLOR_BUFFER_BIT);


    // 选择使用哪个 program （其中包括 vertex shader 和 fragment shader）
    glUseProgram(program);

    // 绑定 VAO
    glBindVertexArray(VAO);
    // 绘制图像
    glDrawArrays(GL_TRIANGLES, 0, 6);
//    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void OpenGLWidget::resizeGL(int w, int h) {
    makeCurrent(); // necessary

}


