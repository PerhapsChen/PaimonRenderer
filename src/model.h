#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "mesh.h"
#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

//unsigned int TextureFromFile(const char* path, const string& directory, bool gamma = false);

//! 一个Model对应多个Mesh和多个Material，
//! 模型文件应该指明每个Mesh使用那个Material，以及该Material的内容
//! 一个Material可能有多张Texture

//! 一个Scene有多张Mesh和多个Material，以array的形式存在，遍历的时候只能得到对应Mesh和Material的索引

class Model
{
public:
    vector<Mesh>       m_meshes; 
    aiScene*           m_scene;
    string directory;
    bool gammaCorrection;

    //- 构造函数接收一个模型地址，读取该模型，搜集一个aiMesh的数组，该数组可以用于求得mesh信息和mesh绑定的material信息
    Model(string const& path, bool gamma = false) : gammaCorrection(gamma)
    {
        Assimp::Importer importer;
        const aiScene* scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals | aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
        if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
        {
            cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << endl;
            return;
        }
        directory = path.substr(0, path.find_last_of('/'));
        processNode(scene->mRootNode, scene);
    }

private:
    void processNode(aiNode* node, const aiScene* scene)
    {
        for (unsigned int i = 0; i < node->mNumMeshes; i++)
        {
            aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
            m_meshes.push_back(Mesh(mesh, scene, directory)); // Mesh constructer
        }
        for (unsigned int i = 0; i < node->mNumChildren; i++)
        {
            processNode(node->mChildren[i], scene);
        }

    }
};

