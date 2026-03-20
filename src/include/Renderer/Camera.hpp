#ifndef FE_CAMERA
#define FE_CAMERA

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace glm;

class Camera {
    private:
    vec2 position;
    mat4 P = perspective(radians(45.0), 1920.0/1080.0 , 0.1, 200.0);
    mat4 O = ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f); 
    
    public:
    mat4 GetVP(const bool&) const noexcept;
};

#endif