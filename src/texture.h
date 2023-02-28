#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include "shader.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>
using namespace std;

// enum class TextureType{
//     TEXTURE_1D,
//     TEXTURE_2D,
//     TEXTURE_3D
// };

class Texture  //! only support 2D texture now.
{
private:
    unsigned int textureID;
    std::string m_texturePath;
    std::string m_typename;
    
public:
    Texture(const string& name, const string& texturePath);
    unsigned int getTextureID() { return textureID; }
    std::string getTexturePath() { return m_texturePath; }
    std::string getTextureTypeName() { return m_typename; }
    void bindTexture() { glBindTexture(GL_TEXTURE_2D, textureID); }
};

//- 把图片读取到纹理里面，绑定到textureID.
Texture::Texture(const string& name, const string& texturePath)
{   
    m_typename = name;
    m_texturePath = texturePath;

    // cout << texturePath << endl;

    // unsigned int textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);   
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(texturePath.c_str(), &width, &height, &nrChannels, 0);
    if (data)
    {
        GLenum format;
        if (nrChannels == 1)
            format = GL_RED;
        else if (nrChannels == 3)
            format = GL_RGB;
        else if (nrChannels == 4)
            format = GL_RGBA;

        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        std::cout << "Failed to load texture" << std::endl;
    }
    stbi_image_free(data);
}
