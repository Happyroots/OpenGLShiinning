#version 330 core

out vec4 FragColor;
in vec3 TexCoords; // ����3D��������ķ�������

uniform samplerCube skybox;

void main()
{    
    FragColor = texture(skybox, TexCoords);
    //FragColor = vec4(0.2, 0.5, 0.3, 1.0);
}