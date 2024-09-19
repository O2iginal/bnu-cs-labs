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

#include <QDebug>

#include <cmath>

#ifndef PI
#define PI 3.141592653589793238462643383279502884197
#endif


OpenGLWidget::OpenGLWidget(QWidget * parent) :QOpenGLWidget(parent) {

    QSurfaceFormat format = QSurfaceFormat::defaultFormat();
    format.setVersion(4, 0);
    format.setProfile(QSurfaceFormat::OpenGLContextProfile::CoreProfile);
    QSurfaceFormat::setDefaultFormat(format);
    this->setFormat(format);

    this->setFocusPolicy(Qt::ClickFocus);
    Q_INIT_RESOURCE(Resources);
}


// cubemap texture
void OpenGLWidget::loadCube()
{
    makeCurrent(); // necessary!!

    boxProgram.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/skybox-vertex.glsl");
    boxProgram.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/skybox-fragment.glsl");
    boxProgram.link();


    std::vector<QVector3D> boxVertices={  // box的八个顶点
        {-1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, 1.0f},
        {1.0f, -1.0f, -1.0f},
        {-1.0f, -1.0f, -1.0f},
        {-1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, 1.0f},
        {1.0f, 1.0f, -1.0f},
        {-1.0f, 1.0f, -1.0f}
    };

    std::vector<unsigned int> boxIndices={  // 十二个三角形绘制box的六个面
      0, 3, 1,
      1, 3, 2,
      3, 0, 4,
      3, 4, 7,
      0, 1, 5,
      0, 5, 4,
      1, 2, 6,
      1, 6, 5,
      2, 7, 6,
      2, 3, 7,
      4, 5, 7,
      5, 6, 7
    };


    glGenVertexArrays(1, &boxVAO);
    glGenBuffers(1, &boxVBO);
    glGenBuffers(1, &boxEBO);

    glBindVertexArray(boxVAO);

    glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
    glBufferData(GL_ARRAY_BUFFER, boxVertices.size() * sizeof(QVector3D), boxVertices.size()?&boxVertices[0]:NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, boxIndices.size() * sizeof(unsigned int), boxIndices.size()?&boxIndices[0]:NULL, GL_STATIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    //////
    boxTexUnit = 15;
    glGenTextures(1, &boxTexture);

    glActiveTexture(GL_TEXTURE0 + boxTexUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, boxTexture);

//    std::vector<QString> fileNames={
//        ":/images/skybox/rt.jpg",
//        ":/images/skybox/lf.jpg",
//        ":/images/skybox/up.jpg",
//        ":/images/skybox/dn.jpg",
//        ":/images/skybox/ft.jpg",
//        ":/images/skybox/bk.jpg"
//    };

    std::vector<QString> fileNames={  // box六个面的图片
        ":/images/universe/right.png",
        ":/images/universe/left.png",
        ":/images/universe/top.png",
        ":/images/universe/bottom.png",
        ":/images/universe/front.png",
        ":/images/universe/back.png"
    };

    // 6张图片的载入与格式转换
    for (unsigned int i = 0; i < fileNames.size(); i++)
    {
        QImage image;
        image.load(fileNames[i]);
        image = image.convertToFormat(QImage::Format_RGBA8888);

        if(!image.isNull()){
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i,
                         0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits()
                         );
        }else{
            qDebug() << "Cubemap loading failed." << endl;
        }
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    boxProgram.bind();
    boxProgram.setUniformValue(boxProgram.uniformLocation("cubemap"), boxTexUnit);
}

void OpenGLWidget::initData()
{
    double plantData[] = {
        // 半径，    轨道半径，   公转周期（天），自转周期（天）
        696000,     0,          0,          0,                      // sun
        2440,       0.3871,     87.93,      59,
        6052,       0.7233,     225,        243,
        6378,       1.0000,     365.26,     (23+56.0/60.0)/24.0,    // earth
        3397,       1.5237,     687,        (24+37.0/60)/24,
        71492,      5.2026,     11.86*365,  (9+50.0/60)/24,
        60268,      9.5549,     29.46*365,  (10+39.0/60)/24,
        25559,      19.2184,    84.01*365,  (17+14.0/60)/24,          // 但是他们的公转周期仍非常长
        24674,      30.1104,    164.82*365, (16+6.0/60)/24,
        2376,       36,         284*365,    6.387,                  // 冥王星 (轨道半径为错误数据）

//        1737,       0.002569,   27.3,       27.3                    // 月球
        600,       0.008000,   27.3,       27.3                    // 月球(将绕地球公转半径调大,避免与地球接触;将月球半径调小)
    };

    for (int i = 1; i <= 10; i ++)
    {

        double r = plantData[i*4], dis = plantData[i*4+1], orbitalT = plantData[i*4+2], rotateT = plantData[i*4+3];
        // 为了显示效果， 避免差距过大, 这里使用平方根比例 (对于行星半径， 使用4次根号比例！）
        radius[i] = sqrt(sqrt( r / (plantData[0] / 10) ));   // 太阳过大， 这里将其按一定比例缩小
        distance[i] = sqrt(dis);
        orbitalV[i] = 1/sqrt(orbitalT);
        rotateV[i] = 1/sqrt(rotateT);
    }
}

void OpenGLWidget::initializeGL() {
    initData();

    // 在任何类中使用 OpenGL 函数，需要先调用 initializeOpenGLFunctions();
    // 除了 initializeGL() 与 paintGL() 之外，其他函数中如需要使用 OpenGL 函数，需要先调用 makeCurrent()

    if (!initializeOpenGLFunctions()) {
        qFatal("OpenGLWidget::initializeOpenGLFunctions() failed.\n");
        return;
    }

    program.addShaderFromSourceFile(QOpenGLShader::Vertex, ":/shaders/sphere-vertex.glsl");
    program.addShaderFromSourceFile(QOpenGLShader::Fragment, ":/shaders/sphere-fragment.glsl");
    program.link();


    // 生成单位球面 Mesh
    int n = 100, m = 100;  // 将球体用100*100的单位mesh表示
    for(int i = 0; i < n; ++i){
        for(int j = 0; j < m; ++j){

            float u = (i*1.0f/(n-1))*(2*PI);  // 经度方向2pi
            float v = (j*1.0f/(m-1))*(PI);    // 维度方向 pi

            QVector3D position = QVector3D(cos(u)*sin(v), cos(v), -sin(u)*sin(v));
            QVector3D normal = position;
            QVector2D textureCoordinate = QVector2D(i*1.0f/n, j*1.0f/m);  // 纹理坐标

            vertices.push_back(position);  // 顶点位置坐标
            normals.push_back(normal);     // 法向方向与位矢相同
            textureCoordinates.push_back(textureCoordinate);

            if(j>0){  // 三角片索引
                indices.push_back(i*m+j);
                indices.push_back(i*m+j-1);
                indices.push_back(((i-1+n)%n)*m+j);

                indices.push_back(i*m+j-1);
                indices.push_back(((i-1+n)%n)*m+j-1);
                indices.push_back(((i-1+n)%n)*m+j);
            }
        }
    }


    glGenVertexArrays(1, &VAO);
    glGenBuffers(3, VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // 位置
    glBindBuffer(GL_ARRAY_BUFFER, VBO[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(QVector3D), vertices.size()?&vertices[0]:NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(0);

    // 法向
    glBindBuffer(GL_ARRAY_BUFFER, VBO[1]);
    glBufferData(GL_ARRAY_BUFFER, normals.size() * sizeof(QVector3D), normals.size()?&normals[0]:NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(1);

    // 纹理坐标
    glBindBuffer(GL_ARRAY_BUFFER, VBO[2]);
    glBufferData(GL_ARRAY_BUFFER, textureCoordinates.size() * sizeof(QVector2D), textureCoordinates.size()?&textureCoordinates[0]:NULL, GL_STATIC_DRAW);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GL_FLOAT), (const void*)0);
    glEnableVertexAttribArray(2);
    
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), indices.size()?&indices[0]:NULL, GL_STATIC_DRAW);


    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);



    // load cubemap as skybox
    loadCube();


    // load textures
    glGenTextures(11, texture);

    std::vector<QString> fileNames={
        ":/images/planets/Sun.jpg",


        ":/images/planets/Mercury.jpg",
        ":/images/planets/Venus.jpg",

        ":/images/planets/Earth.jpg",

        ":/images/planets/Mars.jpg",
        ":/images/planets/Jupiter.jpg",
        ":/images/planets/Saturn.jpg",
        ":/images/planets/Uranus.jpg",
        ":/images/planets/Neptune.jpg",
        ":/images/planets/Pluto.jpg",

        ":/images/planets/Moon.jpg"
    };


    // 载入星体纹理图片
    for(int i = 0; i < 11; ++i){

        QImage image;
        image.load(fileNames[i]);
        image = image.convertToFormat(QImage::Format_RGBA8888);

        texUnit[i] = i;
        glActiveTexture(GL_TEXTURE0 + texUnit[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, image.width(), image.height(), 0, GL_RGBA, GL_UNSIGNED_BYTE, image.constBits());

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glGenerateMipmap(GL_TEXTURE_2D);
    }



    glEnable(GL_DEPTH_TEST);

    lightPosition = {0.0f, 0.0f, 0.0f};


    camera = new Camera();
    camera->setPosition({0.0f, 0.0f, 10.0f});
    camera->lookTo({0.0f, 0.0f, -1.0f});

    frame = 0;

}

void OpenGLWidget::paintGL() {

    ++frame;

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


    boxProgram.bind();
    boxProgram.setUniformValue(boxProgram.uniformLocation("view"), view);
    boxProgram.setUniformValue(boxProgram.uniformLocation("proj"), projection);

    program.bind();
    program.setUniformValue(program.uniformLocation("model"), model);
    program.setUniformValue(program.uniformLocation("view"), view);
    program.setUniformValue(program.uniformLocation("proj"), projection);

    program.setUniformValue(program.uniformLocation("lightPosition"), lightPosition);
    program.setUniformValue(program.uniformLocation("cameraPosition"), camera->getPosition());

    glViewport(0, 0, this->width(), this->height());

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    // render background
    glDisable(GL_DEPTH_TEST);
    boxProgram.bind();
    glBindVertexArray(boxVAO);
    glActiveTexture(GL_TEXTURE0 + boxTexUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, boxTexture);
    glDrawElements(GL_TRIANGLES, 12*3, GL_UNSIGNED_INT, 0);
    glEnable(GL_DEPTH_TEST);


    program.bind();
    glBindVertexArray(VAO);



    // sun
    model = QMatrix4x4();
    model.scale(2.0f);
    program.setUniformValue(program.uniformLocation("model"), model);

    program.setUniformValue(program.uniformLocation("enable_light"), false);
    program.setUniformValue(program.uniformLocation("diffuse_map"), 0);
    glActiveTexture(GL_TEXTURE0 + texUnit[0]);
    glBindTexture(GL_TEXTURE_2D, texture[0]);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


    program.setUniformValue(program.uniformLocation("enable_light"), true);

//    // earth
//    model = QMatrix4x4();
//    model.rotate(frame*0.2, {0.0f, 1.0f, 0.0f});
//    model.translate({10.0f, 0.0f, 0.0f});
//    program.setUniformValue(program.uniformLocation("model"), model);
//    program.setUniformValue(program.uniformLocation("normalMatrix"), model.normalMatrix());

//    program.setUniformValue(program.uniformLocation("diffuse_map"), 1);
//    glActiveTexture(GL_TEXTURE0 + texUnit[1]);
//    glBindTexture(GL_TEXTURE_2D, texture[1]);
//    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);


    for (int i = 1; i <= 10; i ++)
    {
        double dis = distance[i] * 20;                      // 轨道半径
        double r = radius[i] * 2;                               // 行星半径
        double orbitalv = orbitalV[i] * frame * 10;         // 公转速度
        double rotatev = rotateV[i]  * frame * 3;                 // 自转速度

        model = QMatrix4x4();

        if (i == 10) // moon
        {
            model.rotate(orbitalV[3] * frame * 10, {0.0f, 1.0f, 0.0f});
            model.translate({distance[3]*20, 0.0f, 0.0f});
        }
        model.rotate(orbitalv, {0.0f, 1.0f, 0.0f});         // 公转
        model.translate({dis , 0.0f, 0.0f});                // 轨道半径
        model.scale(r);                                     // 行星半径
        model.rotate(rotatev, {0.0f, 1.0f, 0.0f});          // 自转

        program.setUniformValue(program.uniformLocation("model"), model);
        program.setUniformValue(program.uniformLocation("normalMatrix"), model.normalMatrix());

        program.setUniformValue(program.uniformLocation("diffuse_map"), i);
        glActiveTexture(GL_TEXTURE0 + texUnit[i]);
        glBindTexture(GL_TEXTURE_2D, texture[i]);
        glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    }
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


