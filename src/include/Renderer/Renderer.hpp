#ifndef FE_RENDERER
#define FE_RENDERER

#include "include/Core/Scene.hpp"
#include "include/Core/Transform.hpp"
#include "include/Core/VisualNode.hpp"

class Renderer {
    private: 
    inline void PrepareDraw(shared_ptr<Node>, Transform);
    inline void Draw(shared_ptr<Node>);
    inline void ConfigureShader(shared_ptr<Shader>);

    public:
    void DrawScene(shared_ptr<Scene>);
};

#endif