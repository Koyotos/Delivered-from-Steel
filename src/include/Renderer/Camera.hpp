#ifndef FE_CAMERA
#define FE_CAMERA

#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace glm;

enum CameraMode {
    CAM_MODE_PERSPECTIVE,
    CAM_MODE_ORTHO
};

class Camera {
    private:
    CameraMode mode = CAM_MODE_PERSPECTIVE;
    vec2 position;
    mat4 P = perspective(radians(45.0), 1920.0/1080.0 , 0.1, 200.0);
    
    public:
    mat4 GetVP() const noexcept;
    
};

#endif