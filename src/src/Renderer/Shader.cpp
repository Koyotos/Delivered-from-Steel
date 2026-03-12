#include "include/Renderer/Shader.hpp"
#include <stdexcept>

void Shader::Use() {
    glUseProgram(id);
}

void Shader::SetName(const string& name) {
    this->name = name;
}

const string& Shader::GetName() const noexcept {
    return name;
}

void Shader::SetInt(const string& name, const int32_t& val) {
    glUseProgram(id);
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)val);
}   

void Shader::SetBool(const string& name, const bool& val) {
    glUseProgram(id);
    glUniform1i(glGetUniformLocation(id, name.c_str()), (int)val);
}

void Shader::SetFloat(const string& name, const float& val) {
    glUseProgram(id);
    glUniform1f(glGetUniformLocation(id, name.c_str()), (int)val);
}

void Shader::SetVec2(const string& name, const vec2& val) {
    glUseProgram(id);
    glUniform2fv(glGetUniformLocation(id,name.c_str()),1,&val[0]);
}

void Shader::SetVec3(const string& name, const vec3& val) {
    glUseProgram(id);
    glUniform3fv(glGetUniformLocation(id,name.c_str()),1,&val[0]);
}

void Shader::SetVec4(const string& name, const vec4& val) {
    glUseProgram(id);
    glUniform4fv(glGetUniformLocation(id,name.c_str()),1,&val[0]);
}

void Shader::SetMat4(const string& name, const mat4x4& val) {
    glUseProgram(id);
    glUniformMatrix4fv(glGetUniformLocation(id,name.c_str()),1,GL_FALSE,&val[0][0]);
}

void Shader::Compile(string sources[4]) {
    GLuint id = glCreateProgram();
    GLuint types[4] = {GL_VERTEX_SHADER, GL_TESS_CONTROL_SHADER, GL_GEOMETRY_SHADER, GL_FRAGMENT_SHADER};
    
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
        int status;
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