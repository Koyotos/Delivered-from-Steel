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
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);

    stbi_image_free(data);
    return texID;
}