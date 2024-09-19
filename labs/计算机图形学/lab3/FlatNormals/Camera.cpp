/*!
*	File: Camera.cpp
*	Date: 2018/03/01 19:43
*
*	Author: Zhang Yu
*	Contact: YuCrazing@mail.bnu.edu.cn
*
*	Description:
*	https://learnopengl.com/Getting-started/Camera
*
*
*/

#include "Camera.h"

#include <QtMath>


const float Camera::YAW = 270.0f; // 最好是在 [0, 360) 范围内，该范围不强制
const float Camera::PITCH = 0.0f; // (-90, 90)
const float Camera::SPEED = 20.0f;
const float Camera::SENSITIVIY = 0.3f;
const float Camera::ZOOM = 45.0f;
const float Camera::PITCH_UPPER_BOUND = 89.0f;
const float Camera::PITCH_LOWER_BOUND = -89.0f;

Camera::Camera(QVector3D position, QVector3D up, float yaw, float pitch) :
    movementSpeed(SPEED), mouseSensitivity(SENSITIVIY), zoom(ZOOM)
{
    lastFrame = lastMoveTime = -1.0f;
    drag = false;
    firstMouse = true;

    this->position = position;
    this->worldUp = up;
    this->yaw = yaw;
    this->pitch = pitch;

    this->updateCameraVectors();

    timer.start();
}

Camera::Camera(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch) :
    movementSpeed(SPEED), mouseSensitivity(SENSITIVIY), zoom(ZOOM)
{
    lastFrame = lastMoveTime = -1.0f;
    drag = false;
    firstMouse = true;

    this->position = QVector3D(posX, posY, posZ);
    this->worldUp = QVector3D(upX, upY, upZ);
    this->yaw = yaw;
    this->pitch = pitch;

    this->updateCameraVectors();

    timer.start();
}

Camera::~Camera()
{
}

QMatrix4x4 Camera::getViewMatrix()
{
    QMatrix4x4 mat;
    mat.lookAt(this->position, this->position + this->front, this->worldUp); // the world's upward direction or camera's?
    return mat;
}

void Camera::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        drag = true;
        firstMouse = true;
    }
}

void Camera::mouseReleaseEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton) {
        drag = false;
    }
}

void Camera::mouseMoveEvent(QMouseEvent *event)
{
    if (drag) {

        double xpos = event->x();
        double ypos = event->y();

        if (firstMouse) {
            lastX = xpos;
            lastY = ypos;
            firstMouse = false;
            return;
        }
        float sensitivity = 0.1f;
        float xoffset = (xpos - lastX) * sensitivity;
        float yoffset = (lastY - ypos) * sensitivity; //

        processMouseMovement(xoffset, yoffset);

        lastX = xpos;
        lastY = ypos;
    }
}

void Camera::processMouseMovement(float xoffset, float yoffset)
{
    xoffset *= this->mouseSensitivity;
    yoffset *= this->mouseSensitivity;

    this->yaw += xoffset;
    this->pitch += yoffset;

    if (this->pitch > PITCH_UPPER_BOUND) this->pitch = PITCH_UPPER_BOUND;
    if (this->pitch < PITCH_LOWER_BOUND) this->pitch = PITCH_LOWER_BOUND;

    this->updateCameraVectors();
}

void Camera::keyPressEvent(QKeyEvent *event)
{
    int key = event->key();
    keys[event->key()] = true;
    if (lastMoveTime < 0.0f) {
        if (
            key == Qt::Key_W ||
            key == Qt::Key_S ||
            key == Qt::Key_A ||
            key == Qt::Key_D ||
            key == Qt::Key_Space ||
            key == Qt::Key_Shift
            ) {
            lastMoveTime = timer.elapsed() * 1.0 / 1000;
        }
    }
}

void Camera::keyReleaseEvent(QKeyEvent *event)
{
    keys[event->key()] = false;
}

void Camera::processKeyboard()
{
    curFrame = timer.elapsed() * 1.0 / 1000;
    if (lastFrame >= 0.0f) deltaTime = curFrame - lastFrame;
    else deltaTime = 0.0f;
    lastFrame = curFrame;

    if (
        (keys.find(Qt::Key_W) == keys.end() || keys[Qt::Key_W] == false) &&
        (keys.find(Qt::Key_S) == keys.end() || keys[Qt::Key_S] == false) &&
        (keys.find(Qt::Key_A) == keys.end() || keys[Qt::Key_A] == false) &&
        (keys.find(Qt::Key_D) == keys.end() || keys[Qt::Key_D] == false) &&
        (keys.find(Qt::Key_Space) == keys.end() || keys[Qt::Key_Space] == false) &&
        (keys.find(Qt::Key_Shift) == keys.end() || keys[Qt::Key_Shift] == false)
        ) {
        lastMoveTime = -1.0f;
    }
    float velocity = this->movementSpeed * deltaTime;
    if (lastMoveTime > 0.0f) velocity *= (1.0 + curFrame - lastMoveTime);
    if (keys[Qt::Key_W]) this->position += this->front * velocity;
    if (keys[Qt::Key_S]) this->position -= this->front * velocity;
    if (keys[Qt::Key_A]) this->position -= this->right * velocity;
    if (keys[Qt::Key_D]) this->position += this->right * velocity;
    if (keys[Qt::Key_Space]) this->position += this->worldUp * velocity;
    if (keys[Qt::Key_Shift]) this->position -= this->worldUp * velocity;
}

//void Camera::scroll_callback(GLFWwindow* window, double xoffset, double yoffset){
//    processMouseScroll(yoffset);
//}

//void Camera::processMouseScroll(float yoffset)
//{
//    this->zoom -= yoffset;
//    if (this->zoom < 1.0f) this->zoom = 1.0f;
//    if (this->zoom > 89.0f) this->zoom = 89.0f;
//}

void Camera::updateCameraVectors()
{
    this->front.setX(qCos(qDegreesToRadians(pitch)) * qCos(qDegreesToRadians(yaw)));
    this->front.setY(qSin(qDegreesToRadians(pitch)));
    this->front.setZ(qCos(qDegreesToRadians(pitch)) * qSin(qDegreesToRadians(yaw)));

    this->front = this->front.normalized();
    this->right = QVector3D::crossProduct(this->front, this->worldUp).normalized();
    this->up = QVector3D::crossProduct(this->right, this->front).normalized();
}

float Camera::getZoom()
{
    return this->zoom;
}

void Camera::setPosition(QVector3D pos)
{
    this->position = pos;
}

QVector3D Camera::getPosition()
{
    return this->position;
}

QVector3D Camera::getFrontDirection()
{
    return this->front;
}

QVector3D Camera::getRightDirection()
{
    return this->right;
}

QVector3D Camera::getWorldUpDirection()
{
    return this->worldUp;
}

// 视线指向 direction
void Camera::lookTo(QVector3D direction)
{
    float eps = 1e-6;
    direction.normalize();
    // direction 不能与 worldUp 同向或反向
    if (1.0 - qFabs(QVector3D::dotProduct(direction, worldUp)) < eps) {
        //qDebug() << qFabs(QVector3D::dotProduct(direction, worldUp)) << endl;
        return;
    }

    pitch = qAsin(direction.y());
    float cosPitch = qSqrt(1 - direction.y() * direction.y());
    float cosYaw = direction.x() / cosPitch;
    float sinYaw = direction.z() / cosPitch;
    float t = qSqrt(cosYaw * cosYaw + sinYaw * sinYaw);
    cosYaw = cosYaw / t;
    sinYaw = sinYaw / t;
    if (cosYaw >= -eps) yaw = qAsin(sinYaw);
    else if (sinYaw >= -eps) yaw = qAcos(cosYaw);
    else yaw = qAtan(sinYaw / cosYaw) + M_PI;

    lookTo(qRadiansToDegrees(pitch), qRadiansToDegrees(yaw));

}

void Camera::lookTo(float _pitch, float _yaw)
{
    if (_pitch > PITCH_UPPER_BOUND || _pitch < PITCH_LOWER_BOUND) return;
    pitch = _pitch;
    yaw = _yaw;
    updateCameraVectors();
}
