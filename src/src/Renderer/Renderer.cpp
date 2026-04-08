#include "include/Renderer/Renderer.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/VisualNode.hpp"

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

    glEnable(GL_DEPTH_TEST);
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    currentScene = scene;
    PrepareDraw(scene->root, Transform());
    Draw(scene->root);

    #if defined(DEBUG)
    glDisable(GL_DEPTH_TEST);
    DrawDebug(scene->root);
    glEnable(GL_DEPTH_TEST);
    #endif
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
        ConfigureShader(nodeCast);
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

void Renderer::DrawDebug(shared_ptr<Node> node) {
	auto physicsNode = dynamic_pointer_cast<PhysicsNode>(node);
    if (physicsNode) {
        physicsNode->drawDebug();
    }
    for (auto& k : node->GetChildren()) {
        DrawDebug(k);
    }
}

void Renderer::ConfigureShader(shared_ptr<VisualNode> node) {
    shared_ptr<Shader> shader = node->GetShader();
    if(const auto& cast2d = dynamic_pointer_cast<Object2D>(node)) { // Consider avoiding dynamic cast, possibly virtual type func
        shader->SetMat4("VP", currentScene->sceneCam->GetVP(0));
    } else {
        shader->SetMat4("VP", currentScene->sceneCam->GetVP(1));
    }   
}

void Renderer::EndFrame() {
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}