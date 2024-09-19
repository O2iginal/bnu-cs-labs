/*!
*	File: OpenGLWidget.h
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

#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H

#include "Camera.h"

#include<QOpenGLWidget>
#include<QOpenGLFunctions_4_0_Core>

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_0_Core
{

    Q_OBJECT

    QVector3D lightPos;

    Camera * camera;

    GLuint VAO, VBO, EBO, program;

    QMatrix4x4 model, view, projection;

    // 位置
    std::vector<QVector3D> vertices;
    // 顶点顺序
    std::vector<unsigned int> indices;

public:

    OpenGLWidget(QWidget * parent = 0);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

    void mousePressEvent(QMouseEvent * event);
    void mouseReleaseEvent(QMouseEvent * event);
    void mouseMoveEvent(QMouseEvent * event);

    void keyPressEvent(QKeyEvent * event);
    void keyReleaseEvent(QKeyEvent * event);

};

#endif // OPENGLWIDGET_H
