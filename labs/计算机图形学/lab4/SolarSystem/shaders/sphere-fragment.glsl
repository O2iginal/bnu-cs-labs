#version 400 core

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

in vec3 fragPosition;
in vec3 fragNormal;
in vec2 fragTexCoordinate;

uniform bool enable_light;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform sampler2D diffuse_map;
uniform sampler2D specular_map;

out vec4 outColor;

void main()
{
    if(enable_light){
        Light light;
        Material material;

        light.ambient = vec3(1.0f);
        light.diffuse = vec3(1.0f);
        light.specular = vec3(1.0f);

        vec3 Kd = texture(diffuse_map, fragTexCoordinate).rgb;
        vec3 Ks = vec3(0.5f);

        material.ambient = 0.3f * Kd;
        material.diffuse = Kd;
        material.specular = Ks;
        material.shininess = 32;

        // ambient
        vec3 ambient = material.ambient * light.ambient;

        // diffuse
        vec3 normal = normalize(fragNormal);
    //    if(gl_FrontFacing) normal = -normal;
        vec3 lightDirection = normalize(fragPosition - lightPosition);
        float diff = max(0, dot(normal, -lightDirection));
        vec3 diffuse = diff * material.diffuse * light.diffuse;

        // specular
        vec3 viewDirection = normalize(fragPosition - cameraPosition);
        vec3 reflectDirection = reflect(lightDirection, normal);
        float spec = pow(max(0, dot(reflectDirection, -viewDirection)), material.shininess);
        vec3 specular = spec * material.specular * light.specular;

        vec3 result = ambient + diffuse + specular;

        outColor = vec4(result, 1.0f);

    }else{
        outColor = texture(diffuse_map, fragTexCoordinate);
    }

//    outColor = pow(outColor, vec4(1.0f/2.2));
}
