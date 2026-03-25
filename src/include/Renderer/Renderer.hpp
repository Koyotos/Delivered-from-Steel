#ifndef FE_RENDERER
#define FE_RENDERER

#include "include/Core/Scene.hpp"
#include "include/Core/Transform.hpp"
#include "include/Core/VisualNode.hpp"

#include "GLFW/glfw3.h"
#include "include/Renderer/Camera.hpp"

#define GL_VERSION_MAJOR 4
#define GL_VERSION_MINOR 6
#define GLSL_VERSION "#version 460"

#define DEF_WIN_W 1920
#define DEF_WIN_H 1080

class Renderer {
    private: 
    GLFWwindow* window;
    uint16_t windowW;
    uint16_t windowH;

    Camera camera;

    inline void PrepareDraw(shared_ptr<Node>, Transform);
    inline void Draw(shared_ptr<Node>);
    inline void ConfigureShader(shared_ptr<VisualNode>);

    public:
    void DrawScene(shared_ptr<Scene>);
    void EndFrame();
    void Init();
};

#endif