#ifndef FE_RENDERER
#define FE_RENDERER

#include "include/Core/Scene.hpp"
#include "include/Core/Transform.hpp"
#include "include/Core/VisualNode.hpp"

#include "GLFW/glfw3.h"
#include "include/Renderer/Camera.hpp"
#include "include/Renderer/Light.hpp"

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 6
#define GLSL_VERSION "#version 460"

#define DEF_WIN_W 1920
#define DEF_WIN_H 1080

#define SHADOW_WIDTH 2048
#define SHADOW_HEIGHT 2048

#define MAX_LIGHTS 20

#define CULL_RADIUS_ALWAYS_TRUE 0.000000001

class Renderer {
    private: 
    // General
    GLFWwindow* window;
    uint16_t windowW;
    uint16_t windowH;

    // Depth 
    GLuint FBO;
    GLuint depthMaps2D[MAX_LIGHTS];
    GLuint depthCubemaps[MAX_LIGHTS];
    shared_ptr<Shader> depthShaderLayered;
    shared_ptr<Shader> depthShaderNormal;

    // Culling
    vec4 frustumLeft;
    vec4 frustumRight;
    vec4 frustumTop;
    vec4 frustumBottom;

    // Drawing
    shared_ptr<Scene> currentScene;
    vector<shared_ptr<VisualNode>> drawVector;
    vector<shared_ptr<VisualNode>> drawVectorUI;
    vector<pair<shared_ptr<Light>,float>> lightsPos;
    vector<glm::mat4> lightSpaceMatrices;
    vector<float> farPlanes;

    // Drawing pipeline
    inline void PrepareDraw(shared_ptr<Node>, Transform);
    inline void PrepareDrawNode(shared_ptr<VisualNode>, Transform&, bool&);
   
    inline void DepthPass();

    inline bool Cull(shared_ptr<VisualNode>);

    inline void PrepareDrawLight(shared_ptr<Light>);
    inline void ComputeFrustum();
    inline void PrepareLights();
    inline void ResolveZ();
    inline void PrepareShaders();
    inline void ConfigureShader(shared_ptr<Node>);
    inline void SetLight(shared_ptr<Light>, shared_ptr<Shader>,const int8_t&);

    // Draws
    inline void Draw();
    inline void DrawDebug();

    public:
    void DrawScene(shared_ptr<Scene>);
    void EndFrame();
    void Init(ResourceManager&);
	GLFWwindow* GetWindow() const noexcept { return window; };

    Renderer(uint16_t windowW = DEF_WIN_W, uint16_t windowH = DEF_WIN_H);
    ~Renderer();
};

#endif