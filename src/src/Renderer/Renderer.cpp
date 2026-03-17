#include "include/Renderer/Renderer.hpp"

void Renderer::Init() {
    if(!glfwInit()) {
        throw runtime_error("Can't initilize GLFW");
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);    
    
    window = glfwCreateWindow(DEF_WIN_W,DEF_WIN_H,"Game",nullptr,nullptr);
    if(!window) {
        throw runtime_error("Can't create window");
    }

    glfwMakeContextCurrent(window);
    // glfwSwapInterval(1); VSYNC don't know if it will be useful
    
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        throw runtime_error("GL loader eror");
    }

    glEnable(GL_AUTO_NORMAL);
    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT,GL_FILL);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    PrepareDraw(scene->root, Transform());
    Draw(scene->root);
}

void Renderer::Draw(shared_ptr<Node> node) {
    node->Draw();
    for(auto& k : node->GetChildren()) {
        Draw(k);
    }
}

void Renderer::PrepareDraw(shared_ptr<Node> node, Transform t) {
    if(node->TestDraw()) {
        shared_ptr<VisualNode> nodeCast = static_pointer_cast<VisualNode>(node);
        ConfigureShader(nodeCast->GetShader());
        if(!nodeCast->TestIgnoreParent()) {
            if(nodeCast->TestTransformChanged())
                nodeCast->ApplyParentTransform(t);
        } else {
            nodeCast->ResetGlobal();
        }
        t = nodeCast->GetTransform();
    } 
    for(auto& k : node->GetChildren()) {
        PrepareDraw(k, t);
    }
} 

void Renderer::ConfigureShader(shared_ptr<Shader> shader) {
    shader->SetMat4("VP", camera.GetVP());
}

void Renderer::EndFrame() {
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}