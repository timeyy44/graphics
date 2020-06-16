

#version 330 core
out vec4 FragColor;

in vec2 TexCoord;
in vec3 normal;
in vec3 fragPos;

//texture samplers
//uniform sampler2D texture1;
//uniform sampler2D texture2;

//uniform vec3 objectColor;
//uniform vec3 lightColor;
//uniform vec3 lightPos;
uniform vec3 viewerPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    //sampler2D emission;
    float     shininess;
};

uniform Material material;

struct Light {
    vec3 position;
    vec3 direction;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    
    float k0;
    float k1;
    float k2;
    
    float angle;
    float angleOut;
};

uniform Light light;

void main()
{
    // linearly interpolate between both textures (80% container, 20% awesomeface)
    //FragColor = mix(texture(texture1, TexCoord), texture(texture2, TexCoord), 0.2);
    
    
    //计算片段是否位于聚光灯照射范围内
    vec3 lightDir = normalize(light.position-fragPos);
    vec3 lightSpotDir = normalize(light.direction);
    float fragAngle = dot(-lightDir, lightSpotDir);
    float percent = clamp((fragAngle - light.angleOut) / (light.angle - light.angleOut), 0.01, 1.0);
    
    //计算光照强度随距离衰减
    float dis = length(light.position-fragPos);
    float power = 1.0f / (light.k0 + light.k1 * dis + light.k2 * dis * dis);
    
    //环境光照
    //float ambi = 0.1f;
    vec3 ambient =  light.ambient * vec3(texture(material.diffuse, TexCoord));
    
    //漫反射光照
    vec3 norm = normalize(normal);
    
    float diff = max(dot(norm, lightDir), 0.0f);
    vec3 diffuse = light.diffuse * (diff * vec3(texture(material.diffuse, TexCoord)));
    
    //镜面光照
    float specularStrenth = 0.5f;
    vec3 viewDir = normalize(viewerPos - fragPos);
    vec3 flectDir = reflect(-lightDir, norm);
    
    float spec = pow(max(dot(viewDir, flectDir), 0.f), material.shininess);
    spec *= specularStrenth;
    vec3 specLight = light.specular * (spec * vec3(texture(material.specular, TexCoord)));
    
    //自发光
    //vec3 emissionLight = vec3(1.0f) * vec3(texture(material.emission, TexCoord));
    
    vec3 result = (ambient + diffuse + specLight) * power * percent;
    
    //光照的片段着色器
    FragColor = vec4(result, 1.0f);
}

