#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal; //面的法向量？
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform vec3 viewPos;
struct Material { //材质
    //vec3 ambient;
    //vec3 diffuse;
    //vec3 specular;    
    sampler2D diffuse;
    sampler2D specular;  
    float shininess;
};
uniform Material material; 

//----------------------------------
//加光源
struct SpotLight{ //聚光
    vec3 position; 
    vec3 direction; 
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float constant;
    float linear;
    float quadratic;

    float cutOff; 
    float outerCutOff;
};
uniform SpotLight spotLight;

struct PointLight { //点光源
    vec3 position;

    float constant;
    float linear;
    float quadratic;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
#define NR_POINT_LIGHTS 4
uniform PointLight pointLights[NR_POINT_LIGHTS];

struct DirLight { //定向光
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};  
uniform DirLight dirLight;

vec3 CalcSpotLight(SpotLight spotLight, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir);
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir);
void main()
{
    // 属性
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    for(int i = 3; i < NR_POINT_LIGHTS; i++) 
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    result += CalcDirLight(dirLight, norm, viewDir);    

	FragColor =  vec4(result, 1.0);//vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0);
}

//定向光
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // 漫反射着色
    float diff = max(dot(normal, lightDir), 0.0);
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoords)); //specular
    return (ambient + diffuse + specular);
}

//点光源
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 norm = normalize(normal); //计算角度需要标准化

    // 漫反射着色
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0); //范围是0-1
    // 镜面光着色
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // 合并结果
    //环境光
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords)); //material.diffuse
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords)); //material.diffuse
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords)); //material.specular
    // 衰减
    float distance    = length(light.position - fragPos); //可能是负值？
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    //衰减的太多了？ 为什么变黑了？
    //ambient  *= attenuation;
    //diffuse  *= attenuation;
    //specular *= attenuation;
    return (ambient + diffuse + specular);
}

//聚光    
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,  vec3 viewDir){
    vec3 lightDir = normalize(light.position - FragPos); //计算角度要标准化

    //环境光
    //漫反射
    vec3 norm = normalize(normal); 
    float diff = max(dot(norm, lightDir), 0.0); 
    //镜面反射
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 
    
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords)).rgb; //material.diffuse
    vec3 diffuse = light.diffuse * diff  * vec3(texture(texture_diffuse1, TexCoords)).rgb; //material.diffuse
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords)).rgb; //material.specular

    // attenuation 衰减
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    //ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
    //diffuse   *= attenuation;
    //specular *= attenuation;   

    //聚光
    float epsilon   = light.cutOff - light.outerCutOff;
    float theta = dot(lightDir, normalize(-light.direction)); 
    // check if lighting is inside the spotlight cone
    //clamp函数，它把第一个参数约束(Clamp)在了0.0到1.0之间。这保证强度值不会在[0, 1]区间之外
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   
    ambient  *= intensity;    // 单个光源: 将不对环境光做出影响，让它总是能有一点光
    diffuse  *= intensity;
    specular *= intensity;
    
    return (ambient + diffuse + specular); // 
}
