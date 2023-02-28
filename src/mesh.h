#pragma once
#include <glad/glad.h> // holds all OpenGL type declarations
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <string>
#include <vector>
#include "texture.h"
#include "shader.h"
#include "vertex.h"
using namespace std;

//class Texture;


// struct Texture {
//     unsigned int id;
//     string type;
//     string path;
// };

class Mesh
{
public:
    aiMesh*              m_mesh;
    const aiScene*       m_scene;
    aiMaterial*          m_material;
    vector<Texture>      m_textures;
    vector<Vertex>       m_vertices;
    vector<unsigned int> m_indices;
    string               m_meshName;
    string               m_directory;

public: 
    unsigned int VAO, VBO, EBO; 
    Mesh(aiMesh* mesh, const aiScene* scene, string directory)
        : m_mesh(mesh), m_scene(scene), m_directory(directory), m_meshName(mesh->mName.C_Str())
    {  
        m_material = scene->mMaterials[mesh->mMaterialIndex];
        getVerticesAndIndices(); // set m_vertices and m_indices
        setupMesh();
        loadTextureFromMaterialInfo();
    };

    void addTexture(Texture& tex);      // 为Mesh添加额外的纹理
    void Draw();
    void getVerticesAndIndices();
    void setupMesh();
    void loadTextureFromMaterialInfo(); // 从Mesh绑定的Material信息里面先读取纹理
    vector<Texture> Mesh::loadMaterialTextures(aiTextureType type, string typeName);
    unsigned int bindTexture(Shader& shader, unsigned int loc); // bind Texture and get next texture location.
};


void Mesh::addTexture(Texture& tex)
{
    m_textures.push_back(tex);
}

unsigned int Mesh::bindTexture(Shader& shader, unsigned int loc)
{   
    for (unsigned int i = 0; i < m_textures.size(); i++) //- 把该Mesh对应的纹理都读取进去
    {
        glActiveTexture(GL_TEXTURE0 + i + loc); // active proper texture unit before binding

        // now set the sampler to the correct texture unit
        glUniform1i(glGetUniformLocation(shader.ID, m_textures[i].getTextureTypeName().c_str()), i+loc);
        // and finally bind the texture
        glBindTexture(GL_TEXTURE_2D, m_textures[i].getTextureID());
    }
    
    return loc+1;
}

void Mesh::Draw()
{

    // draw mesh
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, static_cast<unsigned int>(m_indices.size()), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);

    // always good practice to set everything back to defaults once configured.
    glActiveTexture(GL_TEXTURE0);
}

void Mesh::getVerticesAndIndices()
{
    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < m_mesh->mNumVertices; i++)
    {
        Vertex vertex;
        glm::vec3 vector; 
        // positions
        vector.x = m_mesh->mVertices[i].x;
        vector.y = m_mesh->mVertices[i].y;
        vector.z = m_mesh->mVertices[i].z;
        vertex.Position = vector;
        // normals
        if (m_mesh->HasNormals())
        {
            vector.x = m_mesh->mNormals[i].x;
            vector.y = m_mesh->mNormals[i].y;
            vector.z = m_mesh->mNormals[i].z;
            vertex.Normal = vector;
        }
        // texture coordinates
        if (m_mesh->mTextureCoords[0]) // does the mesh contain texture coordinates?
        {
            glm::vec2 vec;
            vec.x = m_mesh->mTextureCoords[0][i].x;
            vec.y = m_mesh->mTextureCoords[0][i].y;
            vertex.TexCoords = vec;
            // tangent
            vector.x = m_mesh->mTangents[i].x;
            vector.y = m_mesh->mTangents[i].y;
            vector.z = m_mesh->mTangents[i].z;
            vertex.Tangent = vector;
            // bitangent
            vector.x = m_mesh->mBitangents[i].x;
            vector.y = m_mesh->mBitangents[i].y;
            vector.z = m_mesh->mBitangents[i].z;
            vertex.Bitangent = vector;
        }
        else
            vertex.TexCoords = glm::vec2(0.0f, 0.0f);

        m_vertices.push_back(vertex);
    }
    // indices.
    for (unsigned int i = 0; i < m_mesh->mNumFaces; i++)
    {
        aiFace face = m_mesh->mFaces[i];
        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++)
            m_indices.push_back(face.mIndices[j]);
    }
}

void Mesh::setupMesh()
{
    // create buffers/arrays
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);
    // 把顶点数据全部存到VBO去
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, m_vertices.size() * sizeof(Vertex), &m_vertices[0], GL_STATIC_DRAW);
    // 把顶点索引全部存到EBO去
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, m_indices.size() * sizeof(unsigned int), &m_indices[0], GL_STATIC_DRAW);

    //- set the vertex attribute pointers
    // vertex Positions
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    // vertex normals
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Normal));
    // vertex texture coords
    glEnableVertexAttribArray(2);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, TexCoords));
    // vertex tangent
    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Tangent));
    // vertex bitangent
    glEnableVertexAttribArray(4);
    glVertexAttribPointer(4, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, Bitangent));
    // ids
    glEnableVertexAttribArray(5);
    glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (void*)offsetof(Vertex, m_BoneIDs));
    // weights
    glEnableVertexAttribArray(6);
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, m_Weights));
    glBindVertexArray(0);
}

void Mesh::loadTextureFromMaterialInfo()
{   
    vector<Texture> diffuseMaps = loadMaterialTextures(aiTextureType_DIFFUSE, "texture_base_diffuse");
    m_textures.insert(m_textures.end(), diffuseMaps.begin(), diffuseMaps.end());
    // 2. specular maps
    vector<Texture> specularMaps = loadMaterialTextures(aiTextureType_SPECULAR, "texture_base_specular");
    m_textures.insert(m_textures.end(), specularMaps.begin(), specularMaps.end());
    // 3. normal maps
    std::vector<Texture> normalMaps = loadMaterialTextures(aiTextureType_HEIGHT, "texture_base_normal");
    m_textures.insert(m_textures.end(), normalMaps.begin(), normalMaps.end());
    // 4. height maps
    std::vector<Texture> heightMaps = loadMaterialTextures(aiTextureType_AMBIENT, "texture_base_height");
    m_textures.insert(m_textures.end(), heightMaps.begin(), heightMaps.end());
}

vector<Texture> Mesh::loadMaterialTextures(aiTextureType type, string typeName)
{
    vector<Texture> textures;
    for (unsigned int i = 0; i < m_material->GetTextureCount(type); i++)
    {
        aiString str;
        m_material->GetTexture(type, i, &str); // this str is the filename of texture without directory.
        string fileFullPath = m_directory + '/' + string(str.C_Str());
        Texture texture(typeName, fileFullPath);
        textures.push_back(texture);
    }
    return textures;
}
