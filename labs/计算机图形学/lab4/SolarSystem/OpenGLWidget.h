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
#include<QOpenGLShaderProgram>
#include<QOpenGLFunctions_4_0_Core>

class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_0_Core
{

    Q_OBJECT

    Camera * camera;
    QMatrix4x4 model, view, projection;

    QOpenGLShaderProgram program, boxProgram;

    GLuint VAO, VBO[3], EBO, texUnit[12], texture[12];
    GLuint boxVAO, boxVBO, boxEBO, boxTexUnit, boxTexture;

    QVector3D lightPosition;

    std::vector<QVector3D> vertices, normals;
    std::vector<QVector2D> textureCoordinates;
    std::vector<unsigned int> indices;
    double radius[11], distance[11], orbitalV[11], rotateV[11];


    int frame;

public:

    OpenGLWidget(QWidget * parent = 0);

protected:

    void loadCube();        // modify +
    void initData();

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
