#include "include/Renderer/Utils.hpp"

Vertex::Vertex() {
    position = {0,0,0};
    normal = {0,0,0};
    uv = {0,0};
}

Vertex::Vertex(const vec3& position, const vec3& normal, const vec2& uv) {
    this->position = position;
    this->normal = normal;
    this->uv = uv;
}

vec3 GLMVec(const aiVector3D& vec) {
    return {vec.x,vec.y,vec.z};
};

vec2 GLMVec(const aiVector2D& vec) {
    return {vec.x,vec.y};
};

GLuint TextureFromFile(const char* name, const char* dir) {
    if(!name) {
        throw invalid_argument("Name is null");
    }
    if(!dir) {
        throw invalid_argument("Directory is null");
    }
    string path = string(dir) + "/" + string(name);
    int w, h, c;
    unsigned char* data = stbi_load(path.c_str(),&w,&h,&c,0);
    if(!data) {
        throw runtime_error("Can't read texture file");
    }
    GLenum format = GL_RGB;
    switch (c) {
        case 1: format = GL_RED; break;
        case 3: format = GL_RGB; break;
        case 4: format = GL_RGBA; break;
        default: throw runtime_error("");
    }
    GLuint texID = 0;
    glGenTextures(1,&texID);
    glBindTexture(GL_TEXTURE_2D,texID);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texID;
}

tuple<GLuint, GLuint, GLuint> CreateQuad(const float& w, const float& h) {
    GLuint VAO, VBO, EBO;
    Vertex vertices[4] = {
        Vertex(vec3(0.0f),vec3(0.0f),vec2(0.0f)),
        Vertex(vec3(w,0.0f,0.0f),vec3(0.0f),vec2(1.0f,0.0f)),
        Vertex(vec3(w,h,0.0f),vec3(0.0f),vec2(1.0f,1.0f)),
        Vertex(vec3(0.0f,h,0.0f),vec3(0.0f),vec2(0.0f,1.0f)),
    };
    uint8_t indices[6] = { 0, 1, 2, 2, 3, 0 };

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, 4 * sizeof(Vertex2D), &vertices[0], GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(GLuint), &indices[0], GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)0);

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex2D), (void*)offsetof(Vertex2D, uv));

    glBindVertexArray(0);
    return make_tuple(VAO,VBO,EBO);
}