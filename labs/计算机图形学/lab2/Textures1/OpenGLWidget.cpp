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
#include <QOpenGLTexture>


OpenGLWidget::OpenGLWidget(QWidget * parent) :QOpenGLWidget(parent) {

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);

    format.setSamples(4);

    QSurfaceFormat::setDefaultFormat(format);
    this->setFormat(format);

    this->setFocusPolicy(Qt::ClickFocus);
    Q_INIT_RESOURCE(Resources);
}

void OpenGLWidget::initializeGL() {

    // 在任何类中使用 OpenGL 函数，需要先调用 initializeOpenGLFunctions();
    // 除了 initializeGL() 与 paintGL() 之外，其他函数中如需要使用 OpenGL 函数，需要先调用 makeCurrent()

    if (!initializeOpenGLFunctions()) {
        qFatal("OpenGLWidget::initializeOpenGLFunctions() failed.\n");
        return;
    }


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
    QByteArray vertexShaderSourceTemp = vertexShaderTextStream.readAll().toLocal8Bit();
    const char * vertexShaderSource = vertexShaderSourceTemp.data();
    QByteArray fragmentShaderSourceTemp = fragmentShaderTextStream.readAll().toLocal8Bit();
    const char * fragmentShaderSource = fragmentShaderSourceTemp.data();

    vertexShaderFile.close();
    fragmentShaderFile.close();


    const int bufSize = 1 << 10;
    GLint success;
    GLchar infoLog[bufSize];

    GLuint vertexShader, fragmentShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
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

    program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, bufSize, NULL, infoLog);
        qDebug("Shader program linked error: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /**************************** 创建 texture *****************************/
    // 读入图片文件
    QImage image(":/images/face.png");
    if(image.isNull()){
        qFatal("Image loading failed.\n");
        return ;
    }
    // 将图片的数据存储格式转换为 RGBA 格式
    image = image.convertToFormat(QImage::Format_RGBA8888);


    // 创建 texture object
    glGenTextures(1, &texture);

    texUnit = 0;
    // 激活纹理绑定位置 0 (texture unit 0)
    glActiveTexture(GL_TEXTURE0 + texUnit);

    // 绑定纹理 (texture object)
    glBindTexture(GL_TEXTURE_2D, texture);

    // 设置纹理属性
    // 设置环绕属性的参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);      // modify5
    // 设置过滤属性的参数
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    // 传递图片像素数据到绑定的纹理
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());

    // 生成 Mipmap
    glGenerateMipmap(GL_TEXTURE_2D);

    // 传递变量到 shader
    glUseProgram(program);
    GLint texUniformLocation = glGetUniformLocation(program, "tex");
    glUniform1i(texUniformLocation, texUnit);
    /***********************************************************************/


    float vertices[] = {
            //顶点坐标       纹理坐标       颜色
           -1.0f, -0.5f,   0.0, 0.0,    0.0f, 0.0f, 1.0f,   // modify6 : 添加颜色信息
            1.0f, -0.5f,   4.0, 0.0,    1.0f, 0.0f, 0.0f,   // modify5 : 修改纹理坐标1->4.0
            1.0f,  0.5f,   4.0, 2.0,    0.0f, 1.0f, 0.0f,

           -1.0f, -0.5f,   0.0, 0.0,    1.0f, 0.0f, 0.0f,
            1.0f,  0.5f,   4.0, 2.0,    0.0f, 0.0f, 1.0f,
           -1.0f,  0.5f,   0.0, 2.0,    0.0f, 1.0f, 0.0f
       };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 指定 位置坐标 属性的存储位置与格式
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (const void*)0);  // modify6 : 4->7
    glEnableVertexAttribArray(0);

    // 指定 纹理坐标 的存储位置与格式
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (const void*)(2 * sizeof(GL_FLOAT))); // modify6 : 4->7
    glEnableVertexAttribArray(1);

    // 指定 颜色 的存储位置与格式
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 7 * sizeof(GL_FLOAT), (const void*)(4 * sizeof(GL_FLOAT))); // modify6 : 4->7
    glEnableVertexAttribArray(2);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 必须先解绑 VAO, 之后才能解绑 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 启用 depth test
    glEnable(GL_DEPTH_TEST);
    // 启用 multi sample
    glEnable(GL_MULTISAMPLE);

    camera = new Camera();
    camera->setPosition({0.0f, 0.0f, 5.0f});
    camera->lookTo({0.0f, 0.0f, -1.0f});

}

void OpenGLWidget::paintGL() {

    update();
    camera->processKeyboard();

    int viewportWidth = this->width();
    int viewportHeight = this->height();
    float nearPlane = 0.1f;
    float farPlane = 10000.0f;

    model = QMatrix4x4();
    view = QMatrix4x4();
    projection = QMatrix4x4();
    view.lookAt(camera->getPosition(), camera->getPosition() + camera->getFrontDirection(), camera->getWorldUpDirection());
    projection.perspective(camera->getZoom(), viewportWidth * 1.0f / viewportHeight, nearPlane, farPlane);

    glUseProgram(program);
    GLint modelUniformLocation = glGetUniformLocation(program, "model");
    GLint viewUniformLocation = glGetUniformLocation(program, "view");
    GLint projUniformLocation = glGetUniformLocation(program, "proj");
    glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, model.constData());
    glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, view.constData());
    glUniformMatrix4fv(projUniformLocation, 1, GL_FALSE, projection.constData());


    glViewport(0, 0, this->width(), this->height());

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);


    // 激活 texture unit
    glActiveTexture(GL_TEXTURE0 + texUnit);
    // 绑定 texture
    glBindTexture(GL_TEXTURE_2D, texture);

    glUseProgram(program);
    glBindVertexArray(VAO);

    // 按照 EBO 中的顶点顺序绘制
    glDrawArrays(GL_TRIANGLES, 0 , 6);
}

void OpenGLWidget::resizeGL(int w, int h) {
    makeCurrent(); // necessary

}

void OpenGLWidget::mousePressEvent(QMouseEvent * event)
{
    makeCurrent(); // necessary
    camera->mousePressEvent(event);
}

void OpenGLWidget::mouseReleaseEvent(QMouseEvent * event)
{
    makeCurrent(); // necessary
    camera->mouseReleaseEvent(event);
}

void OpenGLWidget::mouseMoveEvent(QMouseEvent * event)
{
    makeCurrent(); // necessary
    camera->mouseMoveEvent(event);
}

void OpenGLWidget::keyPressEvent(QKeyEvent * event)
{
    makeCurrent(); // necessary
    camera->keyPressEvent(event);
}

void OpenGLWidget::keyReleaseEvent(QKeyEvent * event)
{
    makeCurrent(); // necessary
    camera->keyReleaseEvent(event);
}


