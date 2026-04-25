#include "include/Renderer/Sprite.hpp"
#include "include/Renderer/Mesh.hpp"
#include "include/Profiler/Profiler.hpp"

const string& Sprite::GetDir() const noexcept {
    return directory;
}

void Sprite::SetActiveTexture(const GLuint& index) {
    if (index < textures.size())
        active = index;
}

void Sprite::SetActiveTexture(const string& name) {
    for(uint8_t i = 0; i < textures.size(); i++) {
        if(textures[i].type == name) {
            active = i;
            return;
        }
    }
}

Texture& Sprite::GetActiveTexture() {
    return textures[active];
}

vec2 Sprite::GetSize() {
    return size;
}

void Sprite::Draw(Shader& shader) {
    shader.Use();
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[active].id);
    glBindVertexArray(VAO);
    PROFILER_ADD_DRAW_CALL(indices.size() / 3);
    glDrawElements(GL_TRIANGLES, indices.size(), GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindVertexArray(0);
}

void Sprite::SetupSprite() {
    GLint w,h;
    glBindTexture(GL_TEXTURE_2D,textures[0].id);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_WIDTH,&w);
    glGetTexLevelParameteriv(GL_TEXTURE_2D,0,GL_TEXTURE_HEIGHT,&h);
    glBindTexture(GL_TEXTURE_2D,0);

    size = vec2(w,h);
    vertices = {
        {{0.0f, 0.0f}, {0.0f, 0.0f}},
        {{float(w), 0.0f}, {1.0f, 0.0f}},
        {{float(w), float(h)}, {1.0f, 1.0f}},
        {{0.0f, float(h)}, {0.0f, 1.0f}}
    };
    indices = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(Vertex2D), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indices.size() * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));

    glBindVertexArray(0);
}

void Sprite::LoadTextures() {
    for(const directory_entry& entry : directory_iterator(directory)) {
        Texture text;
        text.id = TextureFromFile(entry.path().filename().string().c_str(), directory.c_str());
        text.path = entry.path().string();
        text.type = entry.path().stem().string();
        textures.push_back(text);
    }
}
    
Sprite::Sprite(const string& path) {
    directory = path;
	active = 0;
    LoadTextures();
    SetupSprite();
}

Sprite::~Sprite() {
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
}
