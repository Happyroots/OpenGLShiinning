#pragma once
#ifndef MESH_H
#define MESH_H

//#include <glad/glad.h> // holds all OpenGL type declarations
#include <iostream>

//#define GLEW_STATIC
#include <glew.h> //"glew.h"
#include  <freeglut.h>//"freeglut.h"
//#include <../glad.h>
//#include <glfw3.h>
#include <../LoadShaders.h> //"../LoadShaders.h"

//#include "stb_image.h"
#include <stb_image.h>

#include  "../vmath.h"//<../vmath.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

//#include <learnopengl/shader.h>

#include <string>
#include <vector>
using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex {
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

struct Texture {
    unsigned int id;
    string type;
    //string path;
    aiString path;  // 我们储存纹理的路径用于与其它纹理进行比较
};

class Mesh {
public:
    // mesh Data
    vector<Vertex>       vertices;
    vector<unsigned int> indices;
    vector<Texture>      textures;

    unsigned int VAO;

    // constructor 构造函数
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, vector<Texture> textures);
    // render the mesh
    void Draw(GLuint program);//Shader& shader

private:
    // render data 
    unsigned int VBO, EBO;
    // initializes all the buffer objects/arrays
    void setupMesh();

};

class Model
{
public:
    /*  函数   */
    Model(const char* path)
    {
        loadModel(path);
    }
    void Draw(GLuint program); //Shader shader //需要着色器标识

private:
    /*  模型数据  */
    vector<Mesh> meshes;
    string directory; //路径？
    vector<Texture> textures_loaded; //已经加载过的纹理
    /*  函数   */
    void loadModel(string path);
    void processNode(aiNode* node, const aiScene* scene);
    Mesh processMesh(aiMesh* mesh, const aiScene* scene);
    vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
        string typeName);

};


#endif
