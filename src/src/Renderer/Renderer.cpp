#include "include/Renderer/Renderer.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/Object3D.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Renderer/TextNode.hpp"

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

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightsPos.clear();
    currentScene = scene;
    PrepareDraw(scene->root, Transform());
    PrepareLights();
    Draw(scene->root);

    #if defined(DEBUG)
    glDisable(GL_DEPTH_TEST);
    DrawDebug(scene->root);
    glEnable(GL_DEPTH_TEST);
    #endif
}

void Renderer::Draw(shared_ptr<Node> node) {
    if (node->TestDraw()) {
        node->Draw();
    }
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
    bool childTransformState = false;
    if(node->TestDraw()) {
        shared_ptr<VisualNode> nodeCast = static_pointer_cast<VisualNode>(node);
        ConfigureShader(nodeCast);
        if(!nodeCast->TestIgnoreParent()) {
            if(nodeCast->TestTransformChanged()) {
                nodeCast->ApplyParentTransform(t);
                childTransformState = true;
            }
        } else {
            nodeCast->ResetGlobal();
        }
        t = nodeCast->GetTransform();
    } else if(node->Type() == "Light") {
        shared_ptr<Light> cast = static_pointer_cast<Light>(node);
        if(cast->type == LIGHT_DIRECTIONAL) {
            lightsPos.push_back({cast,0});
        } else{
            vec2 campos = currentScene->sceneCam->GetPos();
            float dist = (campos.x-cast->data1.x)*(campos.x-cast->data1.x) + (campos.y-cast->data1.y)*(campos.y-cast->data1.y);
            lightsPos.push_back({cast,dist});
        }
    }
    for(auto& k : node->GetChildren()) {
        k->SetTransformChanged(childTransformState);
        PrepareDraw(k, t);
    }
} 

void Renderer::DrawDebug(shared_ptr<Node> node) {
    if(node->Type() != "PhysicsNode") {
        return;
    }
	auto physicsNode = static_pointer_cast<PhysicsNode>(node);
    if (physicsNode) {
        physicsNode->drawDebug();
    }
    for (auto& k : node->GetChildren()) {
        DrawDebug(k);
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
        shader->SetMat4("VP",currentScene->sceneCam->GetVP(0) );
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