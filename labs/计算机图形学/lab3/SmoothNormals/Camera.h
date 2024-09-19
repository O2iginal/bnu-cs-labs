/*!
*	File: Camera.h
*	Date: 2018/03/01 19:40
*
*	Author: Zhang Yu
*	Contact: YuCrazing@mail.bnu.edu.cn
*
*	Description:
*	https://learnopengl.com/Getting-started/Camera
*
*
*/

#ifndef CAMERA_H
#define CAMERA_H

#include <QTime>
#include <QVector3D>
#include <QMatrix4x4>
#include <QMouseEvent>
#include <QOpenGLFunctions_4_4_Core>

#include <cstdio>
#include <cstring>

class Camera : public QOpenGLFunctions_4_4_Core
{
    static const float YAW; // 最好是在 [0, 360) 范围内，该范围不强制
    static const float PITCH; // (-90, 90)
    static const float SPEED;
    static const float SENSITIVIY;
    static const float ZOOM;
    static const float PITCH_UPPER_BOUND;
    static const float PITCH_LOWER_BOUND;

    float lastMoveTime;
    QTime timer;


    std::map<int, bool> keys;
    bool drag;
    bool firstMouse;
    float lastX, lastY;
    float curFrame, lastFrame, deltaTime;


    QVector3D position;
    QVector3D front;
    QVector3D up;
    QVector3D right;
    QVector3D worldUp;

    float yaw;
    float pitch;

    float movementSpeed;
    float mouseSensitivity;
    float zoom;

    void updateCameraVectors();
    void processMouseMovement(float xoffset, float yoffset);

public:
    Camera(QVector3D position = QVector3D(0.0f, 0.0f, 5.0f), QVector3D up = QVector3D(0.0f, 1.0f, 0.0f), float yaw = YAW, float pitch = PITCH);
    Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
    ~Camera();

    QMatrix4x4 getViewMatrix();

    void mousePressEvent(QMouseEvent *event);
    void mouseReleaseEvent(QMouseEvent *event);
    void mouseMoveEvent(QMouseEvent *event);

    void keyPressEvent(QKeyEvent *event);
    void keyReleaseEvent(QKeyEvent *event);
    void processKeyboard();
    //    void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    //    void processMouseScroll(float yoffset);


    float getZoom();
    void setPosition(QVector3D pos);
    QVector3D getPosition();
    QVector3D getFrontDirection();
    QVector3D getRightDirection();
    QVector3D getWorldUpDirection();


    void lookTo(QVector3D direction);
    void lookTo(float _pitch, float _yaw);
};


#endif // CAMERA_H
