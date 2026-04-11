#ifndef FE_CAMERA
#define FE_CAMERA

#include "include/Core/Node.hpp"
#include <glm/ext/matrix_clip_space.hpp>
#include <glm/ext/matrix_float4x4.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/ext/vector_float3.hpp>
#include <glm/trigonometric.hpp>
#include <glm/ext/matrix_transform.hpp>

using namespace glm;

class Camera : public Node {
    private:
    vec2 position;
    mat4 P = perspective(radians(45.0), 1920.0/1080.0 , 0.1, 200.0);
    mat4 O = ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f); 
    
    public:
    /*
    @brief Returns Vp matrix for perspective or orthogonal projection. If true is passed VP is perspective,
    otherwise orthogonal.
    @param1 const bool& - projection mode
    @return mat4 - VP matrix
    */
    mat4 GetVP(const bool&) const noexcept;

    mat4 GetUI() const noexcept { return O; }

    vec2 ToWorldCoords(const vec2&) const noexcept;

    vec2 GetPos();
    void SetPos(const vec2&);

    Camera();
    Camera(const vec2&);
    Camera(unordered_map<string,std::any>);
    ~Camera();
};

#endif