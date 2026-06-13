#ifndef FE_RENDERER
#define FE_RENDERER

#include "include/Core/Scene.hpp"
#include "include/Core/Transform.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Renderer/Shader.hpp"
#include "include/ResourceManager/ResourceManager.hpp"

#include "GLFW/glfw3.h"
#include "include/Renderer/Camera.hpp"
#include "include/Renderer/Light.hpp"

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 6
#define GLSL_VERSION "#version 460"

#define DEF_WIN_W 1920
#define DEF_WIN_H 1080

#define SHADOW_WIDTH 1024
#define SHADOW_HEIGHT 1024

#define MAX_LIGHTS_DIR_AND_SPOT 10
#define MAX_LIGHTS_POINT 5
#define OBJECTS_NUMBER_PREDICT 15
#define UI_NUMBER_PREDICT 20
#define LIGHTS_SHADERS_PREDICT 5
#define SHADER_NUMBER_PREDICT 5

#define CULL_RADIUS_ALWAYS_TRUE 0.000000001

enum NodeRenderType {
    NRT_NODE = 0,
    NRT_VISUALNODE = 1,
    NRT_PHYSICSNODE = 2,
    NRT_OBJECT2D = 3,
    NRT_OBJECT3D = 4,
    NRT_TEXTNODE = 5
};

enum RendererCommandValue {
    RCMDVAL_FALSE = 0, 
    RCMDVAL_TRUE = 1,
    RCMDVAL_SHADOWS_LOW = 2,
    RCMDVAL_SHADOWS_MEDIUM = 3,
    RCMDVAL_SHADOWS_HIGH = 4
};

enum RendererCommand {
    RCMD_RESIZE_W = 0,
    RCMD_RESIZE_H = 1,
    RCMD_REMAKE_WINDOW = 2,
    RCMD_SHADOW_QUALITY = 3,
    RCMD_BLOOM = 4,
    RCMD_GOD_RAYS = 5,
    RCMD_POINT_CULL_DIST = 6,
    RCMD_SPOT_CULL_DIST = 7,
    RCMD_DIR_DISTANCE = 8,
    RCMD_LIGHT_CULL_RADIUS = 9,
    RCMD_SATURATION_CONTROL = 10, 
    RCMD_SATURATION_VALUE = 11,
    RCMD_FULLSCREEN = 12
};

struct RenderData { 
    shared_ptr<Model> model;
    shared_ptr<Shader> shader;
    vector<mat4> matrices;
};

class Renderer {
    private: 
    // Window
    GLFWwindow* window;
    GLFWmonitor* monitor;
    uint16_t windowW;
    uint16_t windowH;

    // Buffers
    GLuint mainFBO;
    GLuint mainColorBuffer;
    GLuint depthColorBuffer;
    GLuint brightColorBuffer;
    GLuint depthFBO;
    GLuint blurFBOs[2];
    GLuint blurColorBuffers[2];

    // Optimizations
    mat4 frameVP;
    mat4 frameVO;
    mat4 frameO;
    mat4 frameV;
    mat4 frameP;
    vector<Shader*> updatedShaders;
    float lightCullRadius = 15.0f;
 
    // Depth Pass
    int16_t shadowW = SHADOW_WIDTH;
    int16_t shadowH = SHADOW_HEIGHT;
    int shadow2DUnit;
    int shadowCubeUnit;
    GLuint depthMaps2DArray;
    GLuint depthCubeArray;
    shared_ptr<Shader> depthShaderLayered;
    shared_ptr<Shader> depthShaderNormal;
    vector<RenderData> potentialCasters; 
    float pointCull = 40.0f;
    float spotCull = 20.0f;
    float dirDistance = 30.0f;

    // Post processing
    GLuint screenQuadVAO;
    GLuint screenQuadVBO;
    GLuint screenQuadEBO;
    shared_ptr<Shader> postProcessingShader;
    shared_ptr<Shader> blurShader;
    vec3 sunDir;
    mat4 sunMatrix;
    bool sunExists;

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
    vec4 frustumPlanes[6];

    // Drawing
    shared_ptr<Scene> currentScene;
    vector<RenderData> drawVector;
    vector<shared_ptr<VisualNode>> drawVector2D;
    vector<shared_ptr<VisualNode>> drawVectorUI;
    vector<pair<shared_ptr<Light>,float>> lightsPos;
    vector<pair<shared_ptr<Light>,float>> lightsPosPoint;
    vector<glm::mat4> lightSpaceMatrices;
    vector<float> farPlanes;

    // Init 
    inline void GenShadowMaps();
    inline void GenFramebuffers();

    // Drawing pipeline
    inline void PrepareDraw(shared_ptr<Node>, Transform);
    inline void PrepareDrawNode(shared_ptr<VisualNode>, Transform&);
    inline void PrepareDrawLight(shared_ptr<Light>);
    inline void CreateRenderData(shared_ptr<Object3D>, vector<RenderData>&);
   
    inline void DepthPass();
    inline void PostProcessingPass();
    inline void BlurBloomPass();

    inline void ComputeFrustum();
    inline bool Cull(const shared_ptr<VisualNode>&);
    inline void ResolveZ();

    inline void PrepareLights();
    inline void PrepareShaders();
    inline void ConfigureShader2D(shared_ptr<VisualNode>);
    inline void ConfigureShader(shared_ptr<Shader> shader, const NodeRenderType& type, const bool info2D);
    inline void SetLight(shared_ptr<Light>, shared_ptr<Shader>,const int8_t&);
    inline void BindShadowTextures();

    // Draws
    inline void Draw();

    inline void DestroyBuffers();

    public:
    void Reconfigure(const RendererCommand&, const int16_t& iv = 0, const float& fv = 0.0);
    void DrawScene(shared_ptr<Scene>);
    void EndFrame();
    void Init(ResourceManager&);
	GLFWwindow* GetWindow() const noexcept { return window; };

    Renderer(uint16_t windowW = DEF_WIN_W, uint16_t windowH = DEF_WIN_H);
    ~Renderer();
};

#endif