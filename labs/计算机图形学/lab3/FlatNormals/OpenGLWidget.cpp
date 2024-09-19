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


    QFile vertexShaderFile, fragmentShaderFile, geometryShaderFile;
    vertexShaderFile.setFileName(":/shaders/vertex.glsl");
    fragmentShaderFile.setFileName(":/shaders/fragment.glsl");
    geometryShaderFile.setFileName(":/shaders/geometry.glsl");
    if (!vertexShaderFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Vertex shader file open failed." << endl;
        return;
    }
    if (!fragmentShaderFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Fragment shader file open failed." << endl;
        return;
    }
    if (!geometryShaderFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Geometry shader file open failed." << endl;
        return;
    }

    QTextStream vertexShaderTextStream(&vertexShaderFile);
    QTextStream fragmentShaderTextStream(&fragmentShaderFile);
    QTextStream geometryShaderTextStream(&geometryShaderFile);
    QByteArray vertexShaderSourceTemp = vertexShaderTextStream.readAll().toLocal8Bit();
    const char * vertexShaderSource = vertexShaderSourceTemp.data();
    QByteArray fragmentShaderSourceTemp = fragmentShaderTextStream.readAll().toLocal8Bit();
    const char * fragmentShaderSource = fragmentShaderSourceTemp.data();
    QByteArray GeometryShaderSourceTemp = geometryShaderTextStream.readAll().toLocal8Bit();
    const char * geometryShaderSource = GeometryShaderSourceTemp.data();

    vertexShaderFile.close();
    fragmentShaderFile.close();
    geometryShaderFile.close();


    const int bufSize = 1 << 10;
    GLint success;
    GLchar infoLog[bufSize];

    GLuint vertexShader, fragmentShader, geometryShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    geometryShader = glCreateShader(GL_GEOMETRY_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glShaderSource(geometryShader, 1, &geometryShaderSource, NULL);

    glCompileShader(vertexShader);
    glCompileShader(fragmentShader);
    glCompileShader(geometryShader);
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
    glGetShaderiv(geometryShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(geometryShader, bufSize, NULL, infoLog);
        qDebug("Geometry shader compiled error: %s\n", infoLog);
    }

    program = glCreateProgram();


    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glAttachShader(program, geometryShader);

    glLinkProgram(program);
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(program, bufSize, NULL, infoLog);
        qDebug("Shader program linked error: %s\n", infoLog);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
    glDeleteShader(geometryShader);



    /******************** 从 obj 文件中读入数据 *********************/
    QFile modelFile(":/models/StanfordDragon/dragon.obj");
//     QFile modelFile(":/models/paimeng/paimeng.obj");
    if (!modelFile.open(QFile::ReadOnly | QFile::Text)){
        qDebug() << "Model file open failed." << endl;
        return;
    }

    QTextStream modelfileTextStream(&modelFile);
    while(!modelfileTextStream.atEnd()){
        // 读取obj文件
        QString line = modelfileTextStream.readLine();  // 读取一行
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
    /***************************************************************/

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glGenBuffers(1, &VBO);


    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), &vertices[0], GL_STATIC_DRAW);
    // 位置
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);

    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    // 启用 depth test
    glEnable(GL_DEPTH_TEST);
//    glShadeModel(GL_SMOOTH);


    camera = new Camera();
    camera->setPosition({0.0f, 0.0f, 5.0f});
    camera->lookTo({0.0f, 0.0f, -1.0f});

    //
    lightPos = QVector3D(100.0f, 100.0f, 100.0f);

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

    //
    GLint lightUniformLocation = glGetUniformLocation(program, "lightPosition");
    GLint cameraUniformLocation = glGetUniformLocation(program, "cameraPosition");

    glUniform3f(lightUniformLocation, lightPos.x(), lightPos.y(), lightPos.z());
    glUniform3f(cameraUniformLocation, camera->getPosition().x(), camera->getPosition().y(), camera->getPosition().z());


    glViewport(0, 0, viewportWidth, viewportHeight);

    glClearColor(0.0, 0.0, 0.0, 1.0);

    // 清除 color buffer 和 depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

//    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    glBindVertexArray(VAO);
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
