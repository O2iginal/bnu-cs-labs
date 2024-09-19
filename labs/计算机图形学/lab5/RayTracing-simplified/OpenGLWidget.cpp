/*!
*	File: OpenGLWidget.cpp
*	Date: 2018/03/01 19:42
*
*	Author: Zhang Yu
*	Contact: YuCrazing@mail.bnu.edu.cn
*
*	Description:
*
*
*
*/


#include "OpenGLWidget.h"

#include <QFile>
#include <QTime>

OpenGLWidget::OpenGLWidget(QWidget * parent) :QOpenGLWidget(parent) {

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    format.setSwapBehavior(QSurfaceFormat::DoubleBuffer);
    format.setSwapInterval(0);
    QSurfaceFormat::setDefaultFormat(format);
    this->setFormat(format);

    this->setUpdateBehavior(QOpenGLWidget::PartialUpdate); //

    this->setFocusPolicy(Qt::ClickFocus);

    Q_INIT_RESOURCE(Resources);
}

void OpenGLWidget::render()
{
    int lastPercent = -1;


    // 每个像素的采样数
    int sampleNum = 100;  // modify, 多次采样

    img = QImage(nx, ny, QImage::Format_RGB888);
    img.fill(0);

    qDebug() << "Rendering started...";

    QTime timer;
    timer.start();

    for(int j = ny-1; j >= 0; --j){
        for(int i = 0; i < nx; ++i){
            QVector3D color;

            // 随机多次采样 (抗锯齿)
            for(int sample = 0; sample < sampleNum; ++sample){
                float u = (i+Functions::random()) * 1.0f / nx;
                float v = (j+Functions::random()) * 1.0f / ny;
                color += scene.getColor(camera.get_ray(u, v));
            }
            color /= sampleNum;

            // gamma correction
            color = QVector3D(sqrt(color.x()), sqrt(color.y()), sqrt(color.z()));  // 开平方处理

            // modify, 限制上下界
            int c1 = Functions::clamp(255.99*color.x(), 0, 255);
            int c2 = Functions::clamp(255.99*color.y(), 0, 255);
            int c3 = Functions::clamp(255.99*color.z(), 0, 255);

            img.setPixelColor(i, ny-1-j, {c1, c2, c3});
        }

        // 显示渲染进度
        int percent = (1.0f - j*1.0f/(ny-1))*100;
        if(percent != lastPercent){
            lastPercent = percent;
            qDebug() << percent << "%";
        }
    }

    int nMilliseconds = timer.elapsed();
    qDebug() << "Rendering time:" << nMilliseconds / 1000 << "s";
    qDebug() << "Maximum recursion depth:" << scene.maxDep;

    // 保存图片
    img.save("scene.png");
}

void OpenGLWidget::initializeGL() {

    // 在任何类中使用 OpenGL 函数，需要先调用 initializeOpenGLFunctions();
    // 除了 initializeGL() 与 paintGL() 之外，其他函数中如需要使用 OpenGL 函数，需要先调用 makeCurrent()

    if (!initializeOpenGLFunctions()) {
        qFatal("OpenGLWidget::initializeOpenGLFunctions() failed.\n");
        return;
    }

    // 屏幕宽高
    nx = this->width();
    ny = this->height();

    // 随机数种子
    srand(time(0));
    scene.maxDep = 0;


    ////////////////////
    Sphere sphere[10], ground;

    // 2个漫反射
    sphere[0].center = {0.0f, 0.0f, 0.0f};
    sphere[0].radius = 1.0f;
    sphere[0].mat_ptr = make_shared<lambertian>(QVector3D(0.7, 0.3, 0.3));  // modify

    sphere[1].center = {-2.5f, 0.5f, 0.0f};
    sphere[1].radius = 1.5f;
    sphere[1].mat_ptr = make_shared<lambertian>(QVector3D(0.8, 0.8, 0.0));  // modify

    // 2个金属
    sphere[2].center = {3.0f, 1.0f, 0.0f};
    sphere[2].radius = 2.0f;
    sphere[2].mat_ptr = make_shared<metal>(QVector3D(0.8, 0.6, 0.2));  // modify

    sphere[3].center = {-1.0f, 0.0f, 2.5f};
    sphere[3].radius = 1.0f;
    sphere[3].mat_ptr = make_shared<metal>(QVector3D(0.8, 0.8, 0.8));  // modify

    // 2个玻璃
    sphere[4].center = {0.2f, -0.4f, 4.0f};
    sphere[4].radius = -0.6f;
    sphere[4].mat_ptr = make_shared<dielectric>(1.5);  // modify

    sphere[5].center = {2.0f, 0.0f, 2.5f};
    sphere[5].radius = 1.0f;
    sphere[5].mat_ptr = make_shared<dielectric>(2.0);  // modify

    // 地面设置为漫反射
    ground.center = {0.0f, -201.0f, 0.0f};
    ground.radius = 200.0f;
    ground.mat_ptr = make_shared<lambertian>(QVector3D(0, 0.7, 0.7));  // modify

    // modify 去掉注释
    scene.objects.push_back(sphere[0]);
    scene.objects.push_back(sphere[1]);
    scene.objects.push_back(sphere[2]);
    scene.objects.push_back(sphere[3]);
    scene.objects.push_back(sphere[4]);
    scene.objects.push_back(sphere[5]);
    scene.objects.push_back(ground);

    QVector3D worldup = {0.0f, 1.0f, 0.0f};
    QVector3D lookfrom = {0.0f, 4.0f, 12.0f};
    QVector3D lookat = {0.0f, 0.0f, 0.0f};
    float dist_to_focus = (lookfrom-lookat).length();
    float aperture = 0.0f;

    camera = {lookfrom, lookat, worldup, 32, 1.0f*nx/ny, aperture, dist_to_focus};
    ////////////////////



    render();
}

void OpenGLWidget::paintGL() {

    update(); // necessary

    QPainter painter;
    painter.begin(this);
    painter.drawImage(0, 0, img.scaledToWidth(this->width()));
    painter.end();
}

void OpenGLWidget::resizeGL(int w, int h) {
    makeCurrent(); // necessary
}


