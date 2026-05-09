#include "include/Renderer/Mesh.hpp"

void Mesh::SetupMesh(const bool& drawFlag) {
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);
  
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex), &vertices[0], drawFlag ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);  

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(unsigned int), &indices[0], drawFlag ? GL_DYNAMIC_DRAW : GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);	
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)0);
    glEnableVertexAttribArray(1);	
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, normal));
    glEnableVertexAttribArray(2);	
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)offsetof(Vertex, uv));
    glBindVertexArray(0);
}

void Mesh::Draw(Shader& shader) {
    for(unsigned int i = 0; i < textures.size(); i++) {
        string name = textures[i].type;
        GLuint slot = 0;
        if (name == "texture_diffuse") {
            slot = TEXTURES_SLOT_DIFFUSE;
        }
        else if (name == "texture_specular") {
            slot = TEXTURES_SLOT_SPECULAR;
        }
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader.SetInt((name), slot);
    }
    PROFILER_ADD_DRAW_CALL(vertices.size()/3);
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindVertexArray(0);
}

void Mesh::DrawInstanced(Shader& shader, vector<mat4>& matrices) {
    for(unsigned int i = 0; i < textures.size(); i++) {
        string name = textures[i].type;
        GLuint slot = 0;
        if (name == "texture_diffuse") {
            slot = TEXTURES_SLOT_DIFFUSE;
        }
        else if (name == "texture_specular") {
            slot = TEXTURES_SLOT_SPECULAR;
        }
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(GL_TEXTURE_2D, textures[i].id);
        shader.SetInt((name), slot);
    }
    
    glGenBuffers(1, &transformBuffer);
    glBindBuffer(GL_ARRAY_BUFFER, transformBuffer);
    glBufferData(GL_ARRAY_BUFFER, matrices.size() * sizeof(glm::mat4), &matrices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(3);
    glVertexAttribPointer(3, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)0);
    glEnableVertexAttribArray(4); 
    glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(1 * vec4size));
    glEnableVertexAttribArray(5); 
    glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(2 * vec4size));
    glEnableVertexAttribArray(6); 
    glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, 4 * vec4size, (void*)(3 * vec4size));

    glVertexAttribDivisor(3, 1);
    glVertexAttribDivisor(4, 1);
    glVertexAttribDivisor(5, 1);
    glVertexAttribDivisor(6, 1);

    glDrawElementsInstanced(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0, matrices.size());
    glBindVertexArray(0);
    glDeleteBuffers(1,&transformBuffer);
    PROFILER_ADD_DRAW_CALL(vertices.size()/3);
}
       
Mesh::Mesh(vector<Vertex> vertices, vector<GLuint> indices, vector<Texture> textures, const bool& drawFlag) {
    this->vertices = vertices;
    this->indices = indices;
    this->textures = textures;
    SetupMesh(drawFlag);
}

Mesh::~Mesh() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}