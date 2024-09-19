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
    this->setMouseTracking(true);
    Q_INIT_RESOURCE(Resources);
}

// Determine whether point P in triangle ABC (重心法)
bool pointInTriangle(QVector2D A, QVector2D B, QVector2D C, QVector2D P)
{
    QVector2D v0 = C - A;
    QVector2D v1 = B - A;
    QVector2D v2 = P - A;

    float dot00 = QVector2D::dotProduct(v0, v0);
    float dot01 = QVector2D::dotProduct(v0, v1);
    float dot02 = QVector2D::dotProduct(v0, v2);
    float dot11 = QVector2D::dotProduct(v1, v1);
    float dot12 = QVector2D::dotProduct(v1, v2);

    float inverDeno = 1 / (dot00 * dot11 - dot01 * dot01);

    float u = (dot11 * dot02 - dot01 * dot12) * inverDeno ;
    if (u < 0 || u > 1) // if u out of range, return directly
    {
        return false;
    }

    float v = (dot00 * dot12 - dot01 * dot02) * inverDeno ;
    if (v < 0 || v > 1) // if v out of range, return directly
    {
        return false;
    }

    return u + v <= 1;
}

// 输入模型中三维坐标点v, 返回窗口二维坐标点 res (被pick调用)
QVector2D OpenGLWidget::pointTransform(QVector3D v)
{
    QVector4D v4 = {v, 1};                  // 三维坐标v -> 齐次坐标 v4
    v4 = projection * view * model * v4;    // 坐标变换
    QVector3D v3 = v4.toVector3DAffine();   // 齐次坐标变为三维坐标
    v3.QVector3D::normalize();              // 向量单位化
    float x1 = v3[0], y1 = v3[1];           // 屏幕上的单位化坐标
    float x2 = (x1 + 1) / 2 * this->width(), y2 = (1 - y1) / 2 * this->height();
    QVector2D res = {x2, y2};               // 相对于屏幕窗口大小的坐标
//    qDebug("res[0] = %f, res[1] = %f\n", res[0],res[1]);
    return res;
}




// 判断三角形是否被选取，被选中返回 true ，否则返回 false
// (winPosX, winPosY) 为鼠标右键单击时光标对应的窗口坐标
bool OpenGLWidget::pick(int winPosX, int winPosY)
{
    // 1.  不需要再次设置变换矩阵, 变换矩阵为成员, 在此处可见
//    qDebug("win_height = %d, win_width = %d", this->height(), this->width());
//    qDebug("winPosX = %d, winPosY = %d", winPosX, winPosY);


    QVector3D v3_0 = vertices[0], v3_1 = vertices[1], v3_2 = vertices[2];                             // 三角形原始坐标
    QVector2D v2_0 = pointTransform(v3_0), v2_1 = pointTransform(v3_1), v2_2 = pointTransform(v3_2);  // 三角形窗口坐标
    QVector2D v2_click = {winPosX, winPosY};                                                          // 鼠标点击坐标(窗口坐标)
    if (pointInTriangle(v2_0, v2_1, v2_2, v2_click))                                                  // 判断是否再三角形内
        return true;

    // 可能会用到的变量和函数：
    // 变换矩阵: model, view, projection;
    // 三角形的三个顶点坐标: vertices[0], vertices[1], vertices[2]
    // 绝对值: abs()
    // camera 当前位置: camera.getPosition()
    // 矩阵对应的逆矩阵: QMatrix4x4::inverted()
    // 四维齐次坐标变换为三维坐标: QVector4D::toVector3DAffine()
    // 向量叉乘: QVector3D::crossProduct()
    // 向量点乘: QVector3D::dotProduct()
    // 向量单位化: QVector3D::normalize() / QVector3D::normalized()

    return false;
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



    // 使用 c++ STL 中的动态数组 vector 存储所有顶点属性（位置）的数据
    vertices = {
        {-0.5f, -0.5f, 0.5f},
        {0.5f, -0.5f, 0.5f},
        {0.0f, 0.5f, 0.5f}
    };


    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), &vertices[0], GL_STATIC_DRAW);

    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);


    // 启用 depth test
    glEnable(GL_DEPTH_TEST);


    // 新建一个 camera 对象
    camera = new Camera();
    // 设置 camera 位置
    camera->setPosition({0.0f, 0.0f, 5.0f});
    // 设置 camera 的两个自由度（第三个自由度固定）
    camera->lookTo({0.0f, 0.0f, -1.0f});

}

void OpenGLWidget::paintGL() {

    update();
    camera->processKeyboard();

    // 视窗宽度与视窗高度
    viewportWidth = this->width();
    viewportHeight = this->height();

    // 近平面距离与远平面距离（棱台的顶部与底部）
    nearPlane = 0.1f;
    farPlane = 10000.0f;


    model = QMatrix4x4(); // 单位矩阵
    view = QMatrix4x4();
    projection = QMatrix4x4();

    // 若物体本身需要变换，可以修改 model 矩阵
    // model =

    // 计算 view 矩阵
     view.lookAt(camera->getPosition(), camera->getFrontDirection(), camera->getWorldUpDirection());

    // 计算 projection 矩阵
    projection.perspective(camera->getZoom(), viewportWidth/viewportHeight, nearPlane, farPlane);


    glUseProgram(program);

    // 将变量传递到 shader （从 CPU 传递到 GPU）
    GLint modelUniformLocation = glGetUniformLocation(program, "model");
    GLint viewUniformLocation = glGetUniformLocation(program, "view");
    GLint projUniformLocation = glGetUniformLocation(program, "proj");

    glUniformMatrix4fv(modelUniformLocation, 1, GL_FALSE, model.constData());
    glUniformMatrix4fv(viewUniformLocation, 1, GL_FALSE, view.constData());
    glUniformMatrix4fv(projUniformLocation, 1, GL_FALSE, projection.constData());




    // 设置视窗的位置与大小
    glViewport(0, 0, viewportWidth, viewportHeight);

    glClearColor(0.0, 0.0, 0.0, 1.0);
    // 清除 color buffer 和 depth buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glBindVertexArray(VAO);
    glDrawArrays(GL_TRIANGLES, 0, 3);

}

void OpenGLWidget::resizeGL(int w, int h) {
    makeCurrent(); // necessary
}

void OpenGLWidget::mousePressEvent(QMouseEvent * event)
{
    makeCurrent(); // necessary
    camera->mousePressEvent(event);

    if(event->button() == Qt::RightButton){
        glUseProgram(program);
        GLint pickedUniformLocation = glGetUniformLocation(program, "picked");
        glUniform1i(pickedUniformLocation, pick(event->x(), event->y()) ? 1 : 0);
    }
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

    emit cursorPosition("Cursor Position: (" + QString::number(event->x()) + ", " + QString::number(event->y()) + ")");
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
