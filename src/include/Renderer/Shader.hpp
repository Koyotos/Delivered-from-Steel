#ifndef FE_SHADER
#define FE_SHADER

#include <glad/glad.h>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/ext/vector_float4.hpp>
#include <string>
#include <vector>

using namespace std;
using namespace glm;

/*
Shader class is a wrapper aroung openGL shader. It is loaded from several shader codes.
It allows to set variables through it's setters and simple usage in rendering. Every Shader
should be uniqe (one shader per one GPU code).
*/
class Shader {
    private:
    GLuint id;
    string name;

    inline void Compile(string[4]);

    public:
    /*
    @brief Use shader program. This should be called before rendering with shader.
    @return void
    */
    void Use();

    /*
    @brief Sets shader's name. Name is used to identify shader, should be unique for every shader.
    @param1 const string& - name to be set
    @return void
    */
    void SetName(const string&);

    /*
    @brief Gets name from shader.
    @return const string& - shader's name
    */
    const string& GetName() const noexcept;

    /*
    @brief Sets integer value for the given field inside shader program.
    @param1 const string& - field name
    @param2 const int32_t& - value
    @return void
    */
    void SetInt(const string&, const int32_t&);

    /*
    @brief Sets floating point value for the given field inside shader program.
    @param1 const string& - field name
    @param2 const float& - value
    @return void
    */
    void SetFloat(const string&, const float&);

    /*
    @brief Sets boolean value for the given field inside shader program.
    @param1 const string& - field name
    @param2 const bool& - value
    @return void
    */
    void SetBool(const string&, const bool&);

    /*
    @brief Sets vector of 2 floats for the given field inside shader program.
    @param1 const string& - field name
    @param2 const vec2& - value
    @return void
    */
    void SetVec2(const string&, const vec2&);

    /*
    @brief Sets vector of 3 floats for the given field inside shader program.
    @param1 const string& - field name
    @param2 const vec3& - value
    @return void
    */
    void SetVec3(const string&, const vec3&);

    /*
    @brief Sets vector of 4 floats value for the given field inside shader program.
    @param1 const string& - field name
    @param2 const vec4& - value
    @return void
    */
    void SetVec4(const string&, const vec4&);

    /*
    @brief Sets matrix 4x4 value for the given field inside shader program.
    @param1 const string& - field name
    @param2 const mat4& - value
    @return void
    */
    void SetMat4(const string&, const mat4&);

    /*
    @brief Constructor that takes an array of shader source codes and it's name. Shader requires values 0 and 3
    corresponding to vertex shader and fragment shader.
    @param1 string[4] - array of shader codes, order is : vertex, tesselation, geometry, fragment
    @param2 const string& - name
    @return Shader
    */
    Shader(string[4], const string&);

    /*
    @brief Constructor that takes shader source codes and it's name. Shader requires codes 0 and 3
    corresponding to vertex shader and fragment shader.
    @param1 string - vertex shader code
    @param2 string - tesselation shader code
    @param3 string - goemetry shader code
    @param4 string - fragment shader code
    @param5 const string& - name
    @return Shader
    */
    Shader(string, string, string, string, const string&);

    /*
    @brief Destructor, performs cleanup. THIS WILL DELETE SHADER FROM GPU.
    If two or more shader use the same GPU program calling this will result in memory errors.
    */
    ~Shader();
};

#endif