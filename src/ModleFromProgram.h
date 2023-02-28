#pragma once
#include <glad/glad.h> 
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <string>
#include <iostream>
#include <vector>

#include "vertex.h"
#include "shader.h"

using namespace std;

class ModelFromProgram //! normally the vertices are always 3+3 format (pos3 + normal3)
{
public:
    vector<float> m_vertices;
    unsigned int pVAO, pVBO;
public:
    ModelFromProgram(vector<float>& vertices): m_vertices(vertices)
    {   
        //cout << "size of vertices: " << vertices.size() << endl;
        glGenVertexArrays(1, &pVAO);
        glGenBuffers(1, &pVBO);
        glBindVertexArray(pVAO);

        glBindBuffer(GL_ARRAY_BUFFER, pVBO);
        glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), &vertices[0], GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
        glEnableVertexAttribArray(1);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
        glBindVertexArray(0);
    }

    void Draw()
    {
		glBindVertexArray(pVAO);
        glDrawArrays(GL_TRIANGLES, 0, m_vertices.size()/6);

		glBindVertexArray(0); // release
    }
};

