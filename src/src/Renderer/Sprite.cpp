#include "include/Renderer/Sprite.hpp"
#include <algorithm>

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
    PROFILER_ADD_DRAW_CALL(2);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
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
    tuple<GLuint, GLuint, GLuint> buffers = CreateQuad(w,h);
    VAO = get<0>(buffers);
    VBO = get<1>(buffers);
    EBO = get<2>(buffers);
}

void Sprite::LoadTextures() {
    for (const directory_entry& entry : directory_iterator(directory)) {
        if (entry.is_directory()) {
            AnimationData anim;
            anim.name = entry.path().filename().string();

            vector<path> framePaths;
            for (const directory_entry& subEntry : directory_iterator(entry.path())) {
                if (subEntry.is_regular_file()) {
                    framePaths.push_back(subEntry.path());
                }
            }
            sort(framePaths.begin(), framePaths.end());

            for (const auto& p : framePaths) {
                Texture text;
                text.id = TextureFromFile(p.filename().string().c_str(), entry.path().string().c_str());
                text.path = p.string();
                text.type = anim.name;
                textures.push_back(text);
                anim.frames.push_back(textures.size() - 1);
            }
            animations[anim.name] = anim;
        }
        else if (entry.is_regular_file()) {
            Texture text;
            text.id = TextureFromFile(entry.path().filename().string().c_str(), directory.c_str());
            text.path = entry.path().string();
            text.type = entry.path().stem().string();
            textures.push_back(text);
        }
    }
}

const AnimationData* Sprite::GetAnimation(const string& name) const {
    auto it = animations.find(name);
    if (it != animations.end()) {
        return &it->second;
    }
    return nullptr;
}

bool Sprite::HasAnimation(const string& name) const {
    return animations.find(name) != animations.end();
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
