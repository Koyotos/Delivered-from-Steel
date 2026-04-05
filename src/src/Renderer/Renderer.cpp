#include "include/Renderer/Renderer.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Renderer/Light.hpp"

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
    lightsPos.clear();
    currentScene = scene;
    PrepareDraw(scene->root, Transform());
    PrepareLights();
    Draw(scene->root);
}

void Renderer::Draw(shared_ptr<Node> node) {
    node->Draw();
    for(auto& k : node->GetChildren()) {
        Draw(k);
    }
}

void Renderer::PrepareLights() {
    sort(lightsPos.begin(), lightsPos.end(), [](const pair<shared_ptr<Light>,float>& a, 
        const pair<shared_ptr<Light>,float>& b) {
            return a.second < b.second;
    });
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
    } else if(shared_ptr<Light> cast = dynamic_pointer_cast<Light>(node)) {
        if(cast->type == LIGHT_DIRECTIONAL) {
            lightsPos.push_back({cast,0});
        } else{
            vec2 campos = currentScene->sceneCam->GetPos();
            float dist = (campos.x-cast->data1.x)*(campos.x-cast->data1.x) + (campos.y-cast->data1.y)*(campos.y-cast->data1.y);
            lightsPos.push_back({cast,dist});
        }
    }
    for(auto& k : node->GetChildren()) {
        PrepareDraw(k, t);
    }
} 

void Renderer::SetLight(shared_ptr<Light> light, shared_ptr<Shader> shader, const int8_t& index) {
    if(light == nullptr) {
        return;
    }
    shader->SetInt("lightsNum", index);
    shader->SetInt("lights["+to_string(index)+"].type", light->type);
    shader->SetVec3("lights["+to_string(index)+"].data1", light->data1);
    shader->SetVec3("lights["+to_string(index)+"].data2", light->data2);
    shader->SetVec3("lights["+to_string(index)+"].data3", light->data3);
    shader->SetFloat("lights["+to_string(index)+"].data4", light->data4);
    shader->SetVec3("lights["+to_string(index)+"].colorAmbient", light->colorAmbient);
    shader->SetVec3("lights["+to_string(index)+"].colorDiffuse", light->colorDiffuse);
    shader->SetVec3("lights["+to_string(index)+"].colorSpecular", light->colorSpecular);
}

void Renderer::ConfigureShader(shared_ptr<VisualNode> node) {
    shared_ptr<Shader> shader = node->GetShader();
    if(const auto& cast2d = dynamic_pointer_cast<Object2D>(node)) { // Consider avoiding dynamic cast, possibly virtual type func
        shader->SetMat4("VP", currentScene->sceneCam->GetVP(0));
    } else {
        shader->SetMat4("VP", currentScene->sceneCam->GetVP(1));
        shader->SetVec3("viewPos",vec3(currentScene->sceneCam->GetPos(),0));
        for(int8_t i = 0; i < (lightsPos.size()>20 ? 20 : lightsPos.size()); i++) {
            SetLight(lightsPos[i].first, shader, i);
        }
    }   
}

void Renderer::EndFrame() {
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}