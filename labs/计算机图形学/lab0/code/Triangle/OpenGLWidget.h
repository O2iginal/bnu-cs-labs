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

#include<QOpenGLWidget>
#include<QOpenGLFunctions_4_0_Core>

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_0_Core
{

    Q_OBJECT

    GLuint VAO, VBO, program;

public:

    OpenGLWidget(QWidget * parent = 0);

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);

};

#endif // OPENGLWIDGET_H
