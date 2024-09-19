/*!
*	File: OpenGLWidget.h
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

#ifndef OPENGLWIDGET_H
#define OPENGLWIDGET_H


#include <QDebug>
#include <QVector3D>
#include <QOpenGLWidget>
#include <QOpenGLFunctions_4_0_Core>
#include <QPainter>

#include <ctime>
#include <cmath>
#include <memory>
using namespace std;

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

// 辅助函数
class Functions{
public:

    // 限制上下界  modify, 新增
    static double clamp(double x, double min, double max) {
        if (x < min) return min;
        if (x > max) return max;
        return x;
    }

    // 返回较小值        modify, (新增)
    static double ffmin(double a, double b) { return a <= b ? a : b; }
    static double ffmax(double a, double b) { return a >= b ? a : b; }

    // 生成 [0, 1] 范围的随机数
    static float random() {
        return rand()*1.0f/RAND_MAX;
    }

    // 反射
    // n 是一个单位向量
    static QVector3D reflect(const QVector3D & v, const QVector3D & n){
        return v - 2.0f*(QVector3D::dotProduct(v,n))*n; // v*u is not dot(v, u) !!!
    }

    // 折射
    // n 是一个单位向量
    static bool refract(const QVector3D & v, const QVector3D & n, float ni_over_nt, QVector3D & refracted){
        QVector3D uv = v.normalized();
        float dt = QVector3D::dotProduct(uv, n);
        float discriminant = 1.0f - ni_over_nt * ni_over_nt * (1 - dt*dt);
        if(discriminant > 0){
            refracted = ni_over_nt * (uv - n*dt) - n * sqrt(discriminant);
            return true;
        }
        return false;
    }

    // modify, (新增), 重载refract
    static QVector3D refract(const QVector3D& uv, const QVector3D& n, double etai_over_etat) {
        auto cos_theta = QVector3D::dotProduct(-uv, n);
        QVector3D r_out_parallel =  etai_over_etat * (uv + cos_theta*n);
        QVector3D r_out_perp = -sqrt(1.0 - r_out_parallel.lengthSquared()) * n;
        return r_out_parallel + r_out_perp;
    }

    // Christophe Schlick polynomial approximation of glass reflection
    static float schlick(float cosine, float ref_idx){
        float r0 = (1-ref_idx) / (1+ref_idx);
        r0 = r0*r0;
        return r0 + (1-r0)*pow((1-cosine), 5);
    }

    // 随机生成单位圆中的一点
    static QVector3D randomPointInUnitDisk(){
        QVector3D p;
        do{
            p = 2.0f * QVector3D(random(), random(), 0.0f) - QVector3D(1.0f, 1.0f, 0.0f);
        }while(QVector3D::dotProduct(p, p) >= 1.0f);
        return p;
    }

    // 随机生成单位球中的一点
    static QVector3D randomPointInUnitSphere(){
            QVector3D p;
            do{
                p = 2.0*QVector3D(random(), random(), random()) - QVector3D(1.0f, 1.0f, 1.0f);
            }while(QVector3D::dotProduct(p, p) >= 1.0f);
            return p;
    }
};

// 射线
class Ray{
    QVector3D ori, dir;

public:

    Ray(const QVector3D & _ori = QVector3D(), const QVector3D & _dir = QVector3D(1.0f, 0.0f, 0.0f)):
       ori(_ori), dir(_dir)
    {}

    QVector3D origin() const {return ori;}

    // normalized direction
    QVector3D direction() const {return dir.normalized();}

    // 作用 : 通过hit判断得到交点参数u, 从而记录交点位置
    QVector3D point(float t) const {return ori + t * direction();}

};



class material;  // modify, 前向声明

// modify, 存储交点信息
class hitPointPara{
public:
    // 法向
    QVector3D normal;
    // 交点参数
    float u;
    // 交点位置
    QVector3D p;
    // 材质指针
    shared_ptr<material> mat_ptr;
    // 朝向
    bool front_face;
    // 计算法向
    inline void set_face_normal(const Ray& r, const QVector3D& outward_normal) {
        front_face = QVector3D::dotProduct(r.direction(), outward_normal) < 0;
        normal = front_face ? outward_normal :-outward_normal;
    }
};


// modify , (新增部分),===================== my code begin ==========================

// 材质虚基类
class material {
    public:
        virtual bool scatter(
            const Ray& r_in, const hitPointPara& rec, QVector3D& attenuation, Ray& scattered
        ) const = 0;
};

// 漫反射材质
class lambertian : public material {
    public:
        lambertian(const QVector3D& a) : albedo(a) {}

        virtual bool scatter(
            const Ray& r_in, const hitPointPara& para, QVector3D& attenuation, Ray& scattered
        ) const {
            QVector3D scatter_direction = para.normal + Functions::randomPointInUnitSphere();
            scattered = Ray(para.p, scatter_direction);
            attenuation = albedo;
            return true;
        }

    public:
        QVector3D albedo;
};

// 金属材质
class metal : public material {
    public:
        metal(const QVector3D& a) : albedo(a) {}

        virtual bool scatter(
            const Ray& r_in, const hitPointPara& para, QVector3D& attenuation, Ray& scattered
        ) const {
            QVector3D reflected = Functions::reflect(r_in.direction(), para.normal);
            scattered = Ray(para.p, reflected);
            attenuation = albedo;
            return (QVector3D::dotProduct(scattered.direction(), para.normal) > 0);
        }

    public:
        QVector3D albedo;

};

// 电介质材质 (可折射)
class dielectric : public material {
    public:
        dielectric(double ri) : ref_idx(ri) {}

        virtual bool scatter(
            const Ray& r_in, const hitPointPara& para, QVector3D& attenuation, Ray& scattered
        ) const {
            attenuation = QVector3D(1.0, 1.0, 1.0);
            double etai_over_etat = (para.front_face) ? (1.0 / ref_idx) : (ref_idx);

            QVector3D unit_direction = r_in.direction();
            double cos_theta = Functions::ffmin(QVector3D::dotProduct(-unit_direction, para.normal), 1.0);

            double sin_theta = sqrt(1.0 - cos_theta*cos_theta);
            if (etai_over_etat * sin_theta > 1.0 ) {
                QVector3D reflected = Functions::reflect(unit_direction, para.normal);
                scattered = Ray(para.p, reflected);
                return true;
            }
            double reflect_prob = Functions::schlick(cos_theta, etai_over_etat);
            if (Functions::random() < reflect_prob)
            {
                QVector3D reflected = Functions::reflect(unit_direction, para.normal);
                scattered = Ray(para.p, reflected);
                return true;
            }
            QVector3D refracted = Functions::refract(unit_direction, para.normal, etai_over_etat);
            scattered = Ray(para.p, refracted);
            return true;
        }

    public:
        double ref_idx;
};

// (新增部分) ======================= my code end ==========================


// 球体
class Sphere{
public:
    QVector3D center;
    float radius;
    shared_ptr<material> mat_ptr;   // modify, 新增, 球体表面材质指针 (使用cpp共享指针)

    Sphere(){}
    Sphere(const QVector3D & c, float r, shared_ptr<material> m=nullptr): center(c), radius(r), mat_ptr(m) {}

    // 判断球体与射线是否相交，相交则返回参数 u (u >= u_min && u <= u_max) 和 交点法向
    // 思考为什么需要 u 要属于 [u_min, u_max] 范围内 ? 答:最小值避免自交,最大值实现遮挡
    bool hit(const Ray & r, hitPointPara & para, float u_min, float u_max) const {
        QVector3D oc = r.origin() - center;
        float a = QVector3D::dotProduct(r.direction(), r.direction());
        float b = QVector3D::dotProduct(oc, r.direction());
        float c = QVector3D::dotProduct(oc, oc) - radius*radius;

        float discriminant = b*b - a*c;

        // modify, 以下if else 有修改, 因为对hitPointPara类定义中添加了一些成员
        if(discriminant > 0){
            auto root = sqrt(discriminant);
            float u = (-b - sqrt(discriminant))/a;
            // 交点 1
            if(u < u_max && u > u_min){
                para.u = u;                                 // 交点参数
                para.p = r.point(para.u);                   // 交点坐标
                QVector3D outward_normal = (para.p - center) / radius;
                para.set_face_normal(r, outward_normal);    // 交点法向
                para.mat_ptr = mat_ptr;                     // 材质
                return true;
            }

            // 交点 2
            u = (-b + root)/a;
            if(u < u_max && u > u_min){
                para.u = u;                                 // 交点参数
                para.p = r.point(para.u);                   // 交点坐标
                QVector3D outward_normal = (para.p - center) / radius;
                para.set_face_normal(r, outward_normal);    // 交点法向
                para.mat_ptr = mat_ptr;                     // 材质
                return true;
            }
        }
        return false;
    }
};

class Scene{
public:

    // 记录最大递归深度，用于调试
    int maxDep;

    // 所有物体（仅含球体）
    std::vector<Sphere> objects;

    // 射线与场景相交，返回颜色
    QVector3D getColor(const Ray & r, int dep=0){

        maxDep = maxDep<dep?dep:maxDep;

        // modify , 设置递归上限
        int depLim = 50;
        if (dep >= depLim)
        {
            // 当递归深度达到上界, 终止递归, 返回黑色
            return QVector3D({0.0f, 0.0f, 0.0f});
        }


        hitPointPara tmpPara, para;

        bool hit = false;
        float closestU = 1e20f;
        float eps = 1e-4f; // 防止浮点误差导致的自交

        // 遍历所有物体，判断射线是否与场景相交
        for(auto object : objects){
            if(object.hit(r, tmpPara, eps, closestU)){
                hit = true;
                closestU = tmpPara.u;  // 维护最远点
                para = tmpPara;        // 相交参数 (射线交点参数和交点法向)
            }
        }

        // modify, 将原来此部分代码的工作封装如material的方法, 因而大大简化
        if(hit) {
            Ray scattered;
            QVector3D attenuation;
            if (para.mat_ptr->scatter(r, para, attenuation, scattered)) {  // 这里调用材质中设定的光线计算函数
                return attenuation * getColor(scattered, dep + 1);  // 递归
            }
        }
        else{
            // 不相交，则返回背景颜色
            float t = 0.5f * (r.direction().y() + 1.0f);
            return (1.0f - t) * QVector3D(1.0f, 1.0f, 1.0f) + t * QVector3D(0.5f, 0.7f, 1.0f);
        }

    }
};

// 相机
class Camera{
public:
    QVector3D origin;
    QVector3D lower_left_corner;
    QVector3D horizontal;
    QVector3D vertical;
    QVector3D u, v, w; // right, up, behind
    float lens_radius;
    Camera(){}
    Camera(QVector3D lookfrom, QVector3D lookat, QVector3D vup, float vfov, float aspect, float aperture, float focus_dist){
        lens_radius = aperture / 2.0f;
        float theta = vfov * M_PI / 180.0f;
        float half_height = tan(theta/2.0f);
        float half_width = aspect * half_height;
        origin = lookfrom;
        w = (lookfrom - lookat).normalized();
        u = QVector3D::crossProduct(vup, w).normalized();
        v = QVector3D::crossProduct(w, u);
        lower_left_corner = origin - half_width*focus_dist*u-half_height*focus_dist*v-focus_dist*w;
        horizontal = 2.0f * half_width*focus_dist*u;
        vertical = 2.0f * half_height*focus_dist*v;
    }
    Ray get_ray(float s, float t){
        QVector3D rd = lens_radius * Functions::randomPointInUnitDisk();
        QVector3D offset = u * rd.x() + v * rd.y();
        return Ray(origin + offset, lower_left_corner + s*horizontal + t*vertical - origin - offset);
    }
};




class OpenGLWidget : public QOpenGLWidget, public QOpenGLFunctions_4_0_Core
{

    Q_OBJECT

    Scene scene;

    Camera camera;

    QImage img;

    QPainter painter;

    int nx, ny;

public:

    OpenGLWidget(QWidget * parent = 0);

protected:

    void render();

    void initializeGL() override;
    void paintGL() override;
    void resizeGL(int w, int h) override;
};

#endif // OPENGLWIDGET_H
