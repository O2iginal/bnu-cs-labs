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

    Camera * camera;

    GLuint VAO, VBO, program;

    QMatrix4x4 model, view, projection;

    // 使用 c++ STL 中的动态数组 vector 存储所有顶点属性（位置）的数据
    std::vector<QVector3D> vertices;

    float nearPlane, farPlane;
    int viewportWidth, viewportHeight;



public:

    OpenGLWidget(QWidget * parent = 0);

signals:
   void cursorPosition(QString s);

protected:

    // 判断是否被选取
    bool pick(int winPosX, int winPosY);
    // 输入模型中三维坐标点v, 返回窗口二维坐标点 res, 被pick调用
    QVector2D pointTransform(QVector3D v);

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
