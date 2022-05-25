#version 330 core

out vec4 FragColor;
in vec2 TexCoords;
in vec3 Normal; //��ķ�������
in vec3 FragPos;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_diffuse2;
uniform sampler2D texture_diffuse3;
uniform sampler2D texture_specular1;
uniform sampler2D texture_specular2;

uniform vec3 viewPos;
struct Material { //����
    //vec3 ambient;
    //vec3 diffuse;
    //vec3 specular;    
    sampler2D diffuse;
    sampler2D specular;  
    float shininess;
};
uniform Material material; 

//----------------------------------
//�ӹ�Դ
struct SpotLight{ //�۹�
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

struct PointLight { //���Դ
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

struct DirLight { //�����
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
    // ����
    vec3 norm = normalize(Normal);
    vec3 viewDir = normalize(viewPos - FragPos);

    vec3 result = CalcSpotLight(spotLight, norm, FragPos, viewDir);    
    for(int i = 3; i < NR_POINT_LIGHTS; i++) 
        result += CalcPointLight(pointLights[i], norm, FragPos, viewDir);    
    result += CalcDirLight(dirLight, norm, viewDir);    

	FragColor =  vec4(result, 1.0);//vec4(texture(texture_diffuse1, TexCoords).rgb, 1.0);
}

//�����
vec3 CalcDirLight(DirLight light, vec3 normal, vec3 viewDir)
{
    vec3 lightDir = normalize(-light.direction);
    // ��������ɫ
    float diff = max(dot(normal, lightDir), 0.0);
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    vec3 ambient  = light.ambient  * vec3(texture(material.diffuse, TexCoords));
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(material.diffuse, TexCoords));
    vec3 specular = light.specular * spec * vec3(texture(material.diffuse, TexCoords)); //specular
    return (ambient + diffuse + specular);
}

//���Դ
vec3 CalcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{
    vec3 norm = normalize(normal); //����Ƕ���Ҫ��׼��

    // ��������ɫ
    vec3 lightDir = normalize(light.position - fragPos);
    float diff = max(dot(normal, lightDir), 0.0); //��Χ��0-1
    // �������ɫ
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    // �ϲ����
    //������
    vec3 ambient  = light.ambient  * vec3(texture(texture_diffuse1, TexCoords)); //material.diffuse
    vec3 diffuse  = light.diffuse  * diff * vec3(texture(texture_diffuse1, TexCoords)); //material.diffuse
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords)); //material.specular
    // ˥��
    float distance    = length(light.position - fragPos); //�����Ǹ�ֵ��
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    //˥����̫���ˣ� Ϊʲô����ˣ�
    //ambient  *= attenuation;
    //diffuse  *= attenuation;
    //specular *= attenuation;
    return (ambient + diffuse + specular);
}

//�۹�    
vec3 CalcSpotLight(SpotLight light, vec3 normal, vec3 fragPos,  vec3 viewDir){
    vec3 lightDir = normalize(light.position - FragPos); //����Ƕ�Ҫ��׼��

    //������
    //������
    vec3 norm = normalize(normal); 
    float diff = max(dot(norm, lightDir), 0.0); 
    //���淴��
    vec3 reflectDir = reflect(-lightDir, norm); 
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess); 
    
    vec3 ambient = light.ambient * vec3(texture(texture_diffuse1, TexCoords)).rgb; //material.diffuse
    vec3 diffuse = light.diffuse * diff  * vec3(texture(texture_diffuse1, TexCoords)).rgb; //material.diffuse
    vec3 specular = light.specular * spec * vec3(texture(texture_diffuse1, TexCoords)).rgb; //material.specular

    // attenuation ˥��
    float distance    = length(light.position - FragPos);
    float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));    
    //ambient  *= attenuation; // remove attenuation from ambient, as otherwise at large distances the light would be darker inside than outside the spotlight due the ambient term in the else branche
    //diffuse   *= attenuation;
    //specular *= attenuation;   

    //�۹�
    float epsilon   = light.cutOff - light.outerCutOff;
    float theta = dot(lightDir, normalize(-light.direction)); 
    // check if lighting is inside the spotlight cone
    //clamp���������ѵ�һ������Լ��(Clamp)����0.0��1.0֮�䡣�Ᵽ֤ǿ��ֵ������[0, 1]����֮��
    float intensity = clamp((theta - light.outerCutOff) / epsilon, 0.0, 1.0);   
    ambient  *= intensity;    // ������Դ: �����Ի���������Ӱ�죬������������һ���
    diffuse  *= intensity;
    specular *= intensity;
    
    return (ambient + diffuse + specular); // 
}
