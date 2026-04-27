#include "include/Renderer/Shader.hpp"
#include "include/Profiler/Profiler.hpp"
#include <stdexcept>

GLuint Shader::currentlyBoundShader = 0;

void Shader::Use() {
    if (Shader::currentlyBoundShader == this->id) {
        return;
    }

    PROFILER_ADD_STATE_CHANGE();
    glUseProgram(this->id);
    Shader::currentlyBoundShader = this->id;
}

void Shader::SetName(const string& name) {
    this->name = name;
}

const string& Shader::GetName() const noexcept {
    return name;
}

void Shader::SetInt(const string& name, const int32_t& val) {
    Use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)val);
}   

void Shader::SetBool(const string& name, const bool& val) {
    Use();
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)val);
}

void Shader::SetFloat(const string& name, const float& val) {
    Use();
    glUniform1f(glGetUniformLocation(id, name.c_str()), val);
}

void Shader::SetVec2(const string& name, const vec2& val) {
    Use();
    glUniform2fv(glGetUniformLocation(id,name.c_str()),1,&val[0]);
}

void Shader::SetVec3(const string& name, const vec3& val) {
    Use();
    glUniform3fv(glGetUniformLocation(id,name.c_str()),1,&val[0]);
}

void Shader::SetVec4(const string& name, const vec4& val) {
    Use();
    glUniform4fv(glGetUniformLocation(id,name.c_str()),1,&val[0]);
}

void Shader::SetMat4(const string& name, const mat4x4& val) {
    Use();
    glUniformMatrix4fv(glGetUniformLocation(id,name.c_str()),1,GL_FALSE,&val[0][0]);
}

void Shader::Compile(string sources[4]) {
    id = glCreateProgram();
    GLuint types[4] = {GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER};
      int status;
    for(uint8_t i = 0; i < 4; i++) {
        if(sources[i]=="") {
            if(i==0 || i==3) {
                throw invalid_argument("Missing vertex or fragment code");
            } else {
                continue;
            }
        }
        GLuint part = glCreateShader(types[i]);
        const char* src = sources[i].c_str();
        glShaderSource(part,1,&src,nullptr);
        glCompileShader(part);
        glGetShaderiv(part, GL_COMPILE_STATUS,&status);
        if(!status) {
            char buffer[512];
            glGetShaderInfoLog(part,512,nullptr,buffer);
            throw runtime_error("Failed to compile shader : " + string(buffer));
        }
        glAttachShader(id,part);
        glDeleteShader(part);
    }
    glLinkProgram(id);
    glGetShaderiv(id, GL_LINK_STATUS,&status);
    if(!status) {
        char buffer[512];
        glGetShaderInfoLog(id,512,nullptr,buffer);
        throw runtime_error("Failed to link shader : " + string(buffer));
    }
}

Shader::Shader(string sources[4], const string& name) {
    this->name = name;
    Compile(sources);
}   

Shader::Shader(string vertex, string tesselation, string geometry, string fragment, const string& name) {
    this->name = name;
    string sources[4] = {vertex,tesselation,geometry,fragment};
    Compile(sources);
}

Shader::~Shader() {

}