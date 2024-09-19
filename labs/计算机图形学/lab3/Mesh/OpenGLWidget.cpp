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
#include <iostream>

OpenGLWidget::OpenGLWidget(QWidget * parent) :QOpenGLWidget(parent) {

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
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



/************************** 从 obj 文件中读入数据 **************************/
    QFile modelFile(":/models/StanfordDragon/dragon.obj");
    if (!modelFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Model file open failed." << endl;
        return;
    }

    QTextStream modelfileTextStream(&modelFile);
    while(!modelfileTextStream.atEnd()){
        // 读入每一行 (这里没有使用split,  因为obj文件中空格数不一致)
        QString line = modelfileTextStream.readLine();
        QTextStream in(&line);
        QString type;
        in>>type;
        if(type=="v")
        {
            float x, y, z;
            in>>x>>y>>z;
            vertices.push_back(QVector3D(x,y,z));
        }
        else if(type=="f")
        {
            unsigned int a, b, c;
            in>>a>>b>>c;
            indices.push_back(a-1);
            indices.push_back(b-1);
            indices.push_back(c-1);
        }
    }

    modelFile.close();
/************************** 从 obj 文件中读入数据 **************************/

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), vertices.size()?&vertices[0]:NULL, GL_STATIC_DRAW);


    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);


    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.size()?&indices[0]:NULL, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 解除绑定的 EBO ，必须在 VAO 之后解除绑定！
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);

    // 启用 depth test
    glEnable(GL_DEPTH_TEST);

    camera = new Camera();
    camera->setPosition({0.0f, 0.0f, 5.0f});
    camera->lookTo({0.0f, 0.0f, -1.0f});

}

void OpenGLWidget::paintGL() {

    update(); // necessary
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


    glViewport(0, 0, viewportWidth, viewportHeight);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    // 清除 color buffer 和 depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // 绑定 VAO
    glBindVertexArray(VAO);
    // 使用线框模式
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    // 绘制图像
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
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
