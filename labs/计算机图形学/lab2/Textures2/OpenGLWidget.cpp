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
    QImage image[2];
    image[0].load(":/images/face.png");
    image[1].load(":/images/brick.jpg");    // modify8: 载入第二张纹理图片


    // 创建 texture object
    glGenTextures(2, texture);      // 这里的第一个参数"2"表示生成两个纹理

    for(int i = 0; i < 2; ++i){     // modify8 : 将循环个数1改为2, 处理两个纹理
        if(image[i].isNull()){
            qFatal("Image loading failed.\n");
            return ;
        }
        // 将图片的数据存储格式转换为 RGBA 格式
        image[i] = image[i].convertToFormat(QImage::Format_RGBA8888);

        // 激活 texture unit
        texUnit[i] = i;
        glActiveTexture(GL_TEXTURE0 + texUnit[i]);

        // 绑定纹理 (texture object)
        glBindTexture(GL_TEXTURE_2D, texture[i]);

        // 设置纹理属性
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        // 传递图片像素数据到绑定的纹理
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image[i].width(), image[i].height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image[i].constBits());

        // 生成 Mipmap
        glGenerateMipmap(GL_TEXTURE_2D);
    }

    // 传递变量到 shader
    glUseProgram(program);
    GLint texUniformLocation = glGetUniformLocation(program, "tex");
    glUniform1iv(texUniformLocation, 2, texUnit);   //  modify8 : 将第二个参数(个数) 从1修改为2

    /***********************************************************************/


    float vertices[] = {
        //  顶点坐标       纹理坐标
           -1.0f, -0.5f, 0.0, 0.0,  // 左下
           1.0f, -0.5f,  1.0, 0.0,  // 右下
           1.0f, 0.5f,   1.0, 1.0,  // 右上

//          -1.0f, -0.5f,  0.0, 0.0,  // 左下     // modify7 : 去除两个重复顶点数据
//           1.0f, 0.5f,   1.0, 1.0,  // 右上

          -1.0f, 0.5f,   0.0, 1.0   // 左上
       };

    // 创建EBO前必须设置顶点的索引顺序
    unsigned int indices[] = {      // modify7 : 索引6个顶点(两个三角形绘制矩形)
        0, 1, 2,
        0, 2, 3,
    };

    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    // 指定 位置坐标 属性的存储位置与格式
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);

    // 指定 纹理坐标 的存储位置与格式
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GL_FLOAT), (const void*)(2 * sizeof(GL_FLOAT)));
    glEnableVertexAttribArray(1);
    
    /********************* 创建和使用 EBO *********************/  // modify7
    // 创建EBO对象
    glGenBuffers(1, &EBO);
    // 绑定EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // 把顶点的索引顺序复制到EBO缓冲中
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    /********************************************************/

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    // 必须先解绑 VAO, 之后才能解绑 EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    glEnable(GL_DEPTH_TEST);


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


    for(int i = 0; i < 2; ++i){         // modify8 : 修改循环1为2
        // 激活 texture unit
        glActiveTexture(GL_TEXTURE0 + texUnit[i]);
        // 绑定 texture
        glBindTexture(GL_TEXTURE_2D, texture[i]);
    }


    glUseProgram(program);
    glBindVertexArray(VAO);

    // 此处修改为按照 EBO 中的顶点顺序绘制  // Modify7 : 用glDrawElements来替换glDrawArrays函数，表示从索引缓冲区渲染三角形
//    glDrawArrays(GL_TRIANGLES, 0, 6);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

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


