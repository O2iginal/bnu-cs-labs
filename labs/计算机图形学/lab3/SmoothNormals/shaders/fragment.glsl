#version 400 core
// 实验备注 : 此处与flat版本的fragment完全相同, 主要是修改vertex接受传入的法向信息, 然后传给fragment的vNormal

struct Light{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Material{
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float shininess;
};

// 片元在世界坐标系下的位置 //
in vec3 vPosition;

// 该片元在世界坐标系下的法向 //
in vec3 vNormal;


// 点光源在世界坐标系下的位置 //
uniform vec3 lightPosition;

// 相机在世界坐标系下的位置 //
uniform vec3 cameraPosition;

out vec4 outColor;

void main()
{
    Light light;
    Material material;

    light.ambient = vec3(0.3f);
    light.diffuse = vec3(1.0f);
    light.specular = vec3(0.5f);

    material.ambient = vec3(0.3f);
    material.diffuse = vec3(0.5f);
    material.specular = vec3(0.7f);
    material.shininess = 0.078125 * 128;

    // 环境光
    vec3 ambient = light.ambient * material.ambient;

    // 漫反射
//    vec3 lightDir =  lightPosition - vPosition;
//    vec3 diffuse = light.ambient * material.ambient * ( normalize(lightDir) * normalize(vNormal) );
    vec3 norm = normalize(vNormal);
    vec3 lightDir = normalize(lightPosition - vPosition);
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * material.diffuse);


    // 镜面反射
    vec3 viewDir = normalize(cameraPosition - vPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * material.specular * spec;

    vec3 I = ambient + diffuse + specular;
    outColor = vec4(I, 1.0f);
}

