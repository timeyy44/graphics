#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 normal;
in vec3 fragPos;

uniform vec3 viewerPos;

struct K {
    float k0;
    float k1;
    float k2;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct Flash {
    float angle;
    float angleOut;
};

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float     shininess;
};

uniform Material material;

struct DirectLight {
    vec3 lightDir;
    
    Light light;
};

uniform DirectLight directLight;

struct PointLight {
    vec3 lightPos;
    
    Light light;
    K k;
};

uniform PointLight pointLight;

struct FlashLight {
    vec3 lightPos;
    vec3 lightDir;
    
    Light light;
    K k;
    Flash flash;
};

uniform FlashLight flashLight;

vec3 directCal(DirectLight directLight);
vec3 pointCal(PointLight pointLight);
vec3 flashCal(FlashLight flashLight);

void main() {
    
    //每个片段上的光照强度总和
    vec3 result = vec3(0.0);
    
    result += flashCal(flashLight);
    result += directCal(directLight);
    result += pointCal(pointLight);
    
    //光照的片段着色器
    FragColor = vec4(result, 1.0f);
}

vec3 directCal(DirectLight directLight) {
    
    vec3 lightDir = normalize(-directLight.lightDir);
    //环境光照
    vec3 ambient =  directLight.light.ambient * vec3(texture(material.diffuse, TexCoord));
    
    //漫反射光照
    vec3 norm = normalize(normal);
    
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = directLight.light.diffuse * (diff * vec3(texture(material.diffuse, TexCoord)));
    
    //镜面光照
    //phong模型
    vec3 viewDir = normalize(viewerPos - fragPos);
    /*vec3 flectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, flectDir), 0.f), material.shininess);*/
    
    //blinn-phong模型
    vec3 half_f = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, half_f), 0.f), material.shininess);
    
    vec3 specLight = directLight.light.specular * (spec * vec3(texture(material.specular, TexCoord)));
    
    return (ambient + diffuse + specLight);
}

vec3 pointCal(PointLight pointLight) {
    
    vec3 lightDir = normalize(pointLight.lightPos-fragPos);
    
    //计算光照强度随距离衰减
    float dis = length(pointLight.lightPos-fragPos);
    float power = 1.0f / (pointLight.k.k0 + pointLight.k.k1 * dis + pointLight.k.k2 * dis * dis);
    
    //环境光照
    vec3 ambient =  pointLight.light.ambient * vec3(texture(material.diffuse, TexCoord));
    
    //漫反射光照
    vec3 norm = normalize(normal);
    
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = pointLight.light.diffuse * (diff * vec3(texture(material.diffuse, TexCoord)));
    
    //镜面光照
    float specularStrenth = 0.5f;
    vec3 viewDir = normalize(viewerPos - fragPos);
    /*vec3 flectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, flectDir), 0.f), material.shininess);*/
    
    //blinn-phong模型
    vec3 half_f = normalize(lightDir+viewDir);
    float spec = pow(max(dot(norm, half_f), 0.f), material.shininess);
    
    vec3 specLight = pointLight.light.specular * (spec * vec3(texture(material.specular, TexCoord)));
    
    return (ambient + diffuse + specLight) * power;
}
vec3 flashCal(FlashLight flashLight) {
    
    //计算片段是否位于聚光灯照射范围内
    vec3 lightDir = normalize(flashLight.lightPos-fragPos);
    vec3 lightSpotDir = normalize(flashLight.lightDir);
    float fragAngle = dot(-lightDir, lightSpotDir);
    float percent = clamp((fragAngle - flashLight.flash.angleOut) / (flashLight.flash.angle - flashLight.flash.angleOut), 0.01, 1.0);
    
    //计算光照强度随距离衰减
    float dis = length(flashLight.lightPos-fragPos);
    float power = 1.0f / (flashLight.k.k0 + flashLight.k.k1 * dis + flashLight.k.k2 * dis * dis);
    
    //环境光照
    vec3 ambient =  flashLight.light.ambient * vec3(texture(material.diffuse, TexCoord));
    
    //漫反射光照
    vec3 norm = normalize(normal);
    
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = flashLight.light.diffuse * (diff * vec3(texture(material.diffuse, TexCoord)));
    
    //镜面光照
    vec3 viewDir = normalize(viewerPos - fragPos);
    /*vec3 flectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, flectDir), 0.f), material.shininess);*/
    
    //blinn-phong模型
    vec3 half_f = normalize(lightDir + viewDir);
    float spec = pow(max(dot(norm, half_f), 0.f), material.shininess);
    
    vec3 specLight = flashLight.light.specular * (spec * vec3(texture(material.specular, TexCoord)));
    
    return (ambient + diffuse + specLight) * power * percent;
}
