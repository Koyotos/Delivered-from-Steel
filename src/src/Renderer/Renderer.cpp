#include "include/Renderer/Renderer.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/Object3D.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Renderer/TextNode.hpp"
#include "include/Profiler/Profiler.hpp"
#include <iostream>

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
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
    glDepthFunc(GL_LESS);
    glPolygonMode(GL_FRONT_AND_BACK,GL_FILL);
}

void Renderer::ComputeFrustum() {
    mat4 vp = currentScene->sceneCam->GetVP(1);
    frustumLeft = vec4(vp[0][3] + vp[0][0], vp[1][3] + vp[1][0], vp[2][3] + vp[2][0], vp[3][3] + vp[3][0]);
    frustumRight = vec4(vp[0][3] - vp[0][0], vp[1][3] - vp[1][0], vp[2][3] - vp[2][0], vp[3][3] - vp[3][0]);
    frustumBottom = vec4(vp[0][3] + vp[0][1], vp[1][3] + vp[1][1], vp[2][3] + vp[2][1], vp[3][3] + vp[3][1]);
    frustumTop = vec4(vp[0][3] - vp[0][1], vp[1][3] - vp[1][1], vp[2][3] - vp[2][1], vp[3][3] - vp[3][1]);
    auto normalizePlane = [](vec4& plane) {
        float length = glm::length(vec3(plane));
        plane /= length;
    };

    normalizePlane(frustumLeft);
    normalizePlane(frustumRight);
    normalizePlane(frustumBottom);
    normalizePlane(frustumTop);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightsPos.clear();
    drawVector.clear();
    drawVectorUI.clear();

    // Prepare culling
    currentScene = scene;
    ComputeFrustum();

    // Vectorize scene
    PrepareDraw(scene->root, Transform());
    ResolveZ();

    // Setup shaders
    PrepareLights();
    PrepareShaders();

    // Draw
    Draw();

    #if defined(DEBUG)
    glDisable(GL_DEPTH_TEST);
    DrawDebug();
    glEnable(GL_DEPTH_TEST);
    #endif
}

void Renderer::Draw() {
    for(auto& node : drawVector) {
        PROFILER_ADD_OBJECT();
        node->Draw();
    }
    for(auto& node : drawVectorUI) {
        PROFILER_ADD_OBJECT();
        node->Draw();
    }
}

void Renderer::PrepareLights() {
    sort(lightsPos.begin(), lightsPos.end(), [](const pair<shared_ptr<Light>,float>& a, 
        const pair<shared_ptr<Light>,float>& b) {
            return a.second < b.second;
    });
}

void Renderer::PrepareShaders() {
    for(auto& node : drawVector) {
        ConfigureShader(node);
    }
    for(auto& node : drawVectorUI) {
        ConfigureShader(node);
    }
}

bool Renderer::Cull(shared_ptr<VisualNode> node) {
    float radius = node->GetCullRadius();
    if(radius < CULL_RADIUS_ALWAYS_TRUE) {
        return true;
    }
    vec3 pos = node->GetTransform().GetGlobal()[3];
    if(glm::dot(vec3(frustumLeft), pos) + frustumLeft.w < -radius) return false;
    if(glm::dot(vec3(frustumRight), pos) + frustumRight.w < -radius) return false;
    if(glm::dot(vec3(frustumBottom), pos) + frustumBottom.w < -radius) return false;
    if(glm::dot(vec3(frustumTop), pos) + frustumTop.w < -radius) return false;
    return true; 
}

void Renderer::PrepareDraw(shared_ptr<Node> node, Transform t) {
    bool childTransformState = false;
    if(node->Type() == "TextNode" || node->RenderType() == "Object2D" 
        || node->RenderType() == "Object3D") {
        PrepareDrawNode(static_pointer_cast<VisualNode>(node), t, childTransformState);
        
    } else if(node->Type() == "Light") {
        PrepareDrawLight(static_pointer_cast<Light>(node));
    }
    for(auto& k : node->GetChildren()) {
        k->SetTransformChanged(childTransformState);
        PrepareDraw(k, t);
    }
} 

void Renderer::DrawDebug() {
    for(auto& node : drawVector) {
        auto physicsNode = static_pointer_cast<PhysicsNode>(node);
        if (physicsNode) {
            physicsNode->drawDebug();
        } else {
            node->Draw();
        }
    }
}

void Renderer::ResolveZ() {
    sort(drawVectorUI.begin(), drawVectorUI.end(),[](const shared_ptr<VisualNode>& a, 
        const shared_ptr<VisualNode>& b) {
            return a->GetZIndex() > b->GetZIndex();
    });
}

void Renderer::PrepareDrawNode(shared_ptr<VisualNode> visualCast, Transform& t, bool& flag) {
    if(!visualCast->TestIgnoreParent()) {
        if(visualCast->TestTransformChanged()) {
            visualCast->ApplyParentTransform(t);
            flag = true;
        }
    } else {
        visualCast->ResetGlobal();
    }

    t = visualCast->GetTransform();
    if(Cull(visualCast)) {
        if(visualCast->Type() == "TextNode" || visualCast->RenderType() == "Object2D") {
            drawVectorUI.push_back(visualCast);
        } else {
            drawVector.push_back(visualCast);
        }
    }
}

void Renderer::PrepareDrawLight(shared_ptr<Light> light) {
    if(light->type == LIGHT_DIRECTIONAL) {
            lightsPos.push_back({light,0});
    } else {
        vec2 campos = currentScene->sceneCam->GetPos();
        float dist = (campos.x-light->data1.x)*(campos.x-light->data1.x) + (campos.y-light->data1.y)*(campos.y-light->data1.y);
        lightsPos.push_back({light,dist});
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

void Renderer::ConfigureShader(shared_ptr<Node> node) {
    if(node->Type() == "Object2D") {
        shared_ptr<Object2D> obj2d = static_pointer_cast<Object2D>(node);
        shared_ptr<Shader> shader = obj2d->GetShader();
        if(obj2d->GetReqPerspective()) {
            shader->SetMat4("VP", currentScene->sceneCam->GetVP(1));
        } else {
            shader->SetMat4("VP", currentScene->sceneCam->GetVP(0));
        }
    } else if(node->Type() == "TextNode"){
        shared_ptr<TextNode> textNode = static_pointer_cast<TextNode>(node);
        shared_ptr<Shader> shader = textNode->GetShader();
        if (textNode->TestIgnoreParent()) {
            shader->SetMat4("VP", currentScene->sceneCam->GetUI());
        }
        else {
            shader->SetMat4("VP", currentScene->sceneCam->GetVP(0));
        }
    } 
    else if(node->Type() == "Object3D") {
        shared_ptr<Object3D> obj3d = static_pointer_cast<Object3D>(node);
        shared_ptr<Shader> shader = obj3d->GetShader();
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

Renderer::Renderer(uint16_t windowW , uint16_t windowH) {
    this->windowW = windowW;
    this->windowH = windowH;
    Init();
}

Renderer::~Renderer() {
    glfwDestroyWindow(window);
}