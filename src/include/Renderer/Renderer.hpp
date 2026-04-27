#ifndef FE_RENDERER
#define FE_RENDERER

#include "include/Core/Scene.hpp"
#include "include/Core/Transform.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/ResourceManager/ResourceManager.hpp"

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
#define OBJECTS_NUMBER_PREDICT 15
#define UI_NUMBER_PREDICT 20
#define LIGHTS_SHADERS_PREDICT 5

#define CULL_RADIUS_ALWAYS_TRUE 0.000000001

enum NodeRenderType {
    NRT_NODE = 0,
    NRT_VISUALNODE = 1,
    NRT_PHYSICSNODE = 2,
    NRT_OBJECT2D = 3,
    NRT_OBJECT3D = 4,
    NRT_TEXTNODE = 5
};

class Renderer {
    private: 
    // General
    GLFWwindow* window;
    uint16_t windowW;
    uint16_t windowH;

    // Optimizations
    mat4 frameVP;
    mat4 frameVO;
    mat4 frameO;
    vector<Shader*> lightsUpdatedList;
 
    // Depth 
    GLuint FBO;
    GLuint depthMaps2DArray;
    GLuint depthCubeArray;
    shared_ptr<Shader> depthShaderLayered;
    shared_ptr<Shader> depthShaderNormal;
    int shadow2DUnit;
    int shadowCubeUnit;
    vector<shared_ptr<VisualNode>> potentialCasters; 

    static constexpr vec3 dirs[6] = {
                    {1,0,0},{-1,0,0},
                    {0,1,0},{0,-1,0},
                    {0,0,1},{0,0,-1}
                };
    static constexpr vec3 ups[6] = {
                    {0,-1,0},{0,-1,0},
                    {0,0,1},{0,0,-1},
                    {0,-1,0},{0,-1,0}
                };

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

    // Init 
    inline void GenShadowMaps();

    // Drawing pipeline
    inline void PrepareDraw(shared_ptr<Node>, Transform);
    inline void PrepareDrawNode(shared_ptr<VisualNode>, Transform&);
   
    inline void DepthPass();

    inline bool Cull(shared_ptr<VisualNode>);
    inline bool AffectsLight(const shared_ptr<VisualNode>& obj, const shared_ptr<Light>& light);

    inline void PrepareDrawLight(shared_ptr<Light>);
    inline void ComputeFrustum();
    inline void PrepareLights();
    inline void ResolveZ();
    inline void PrepareShaders();
    inline void ConfigureShader(shared_ptr<Node>);
    inline void SetLight(shared_ptr<Light>, shared_ptr<Shader>,const int8_t&);
    inline void BindShadowTextures();

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