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

class Renderer {
    private: 
    // Window
    GLFWwindow* window;
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
 
    // Depth Pass
    int shadow2DUnit;
    int shadowCubeUnit;
    GLuint depthMaps2DArray;
    GLuint depthCubeArray;
    shared_ptr<Shader> depthShaderLayered;
    shared_ptr<Shader> depthShaderNormal;
    vector<shared_ptr<VisualNode>> potentialCasters; 

    // Post processing
    GLuint screenQuadVAO;
    GLuint screenQuadVBO;
    GLuint screenQuadEBO;
    shared_ptr<Shader> postProcessingShader;
    shared_ptr<Shader> blurShader;
    vec3 sunDir;
    mat4 sunMatrix;

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
    vector<shared_ptr<VisualNode>> drawVector;
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
   
    inline void DepthPass();
    inline void PostProcessingPass();
    inline void BlurBloomPass();

    inline void ComputeFrustum();
    inline bool Cull(const shared_ptr<VisualNode>&);
    inline void ResolveZ();
    inline bool AffectsLight(const shared_ptr<VisualNode>& obj, const shared_ptr<Light>& light);

    inline void PrepareLights();
    inline void PrepareShaders();
    inline void ConfigureShader(shared_ptr<VisualNode>);
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