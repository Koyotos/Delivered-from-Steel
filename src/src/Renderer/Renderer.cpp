#include "include/Renderer/Renderer.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/Object3D.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Renderer/Shader.hpp"
#include "include/Renderer/TextNode.hpp"
#include "include/Profiler/Profiler.hpp"
#include "include/Renderer/Utils.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include <iostream>
#include <memory>

void Renderer::Init(ResourceManager& rsm)
{
    if (!glfwInit())
        throw runtime_error("Can't initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(DEF_WIN_W, DEF_WIN_H, "Game", nullptr, nullptr);
    if (!window)
        throw runtime_error("Can't create window");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw runtime_error("GL loader error");

    lightSpaceMatrices.resize(MAX_LIGHTS);
    farPlanes.resize(MAX_LIGHTS);

    depthShaderLayered = rsm.LoadShader("layeredDepth");
    depthShaderNormal  = rsm.LoadShader("simpleDepth");

    glGenFramebuffers(1, &FBO);

    // -----------------------------
    // CUBE ARRAY (FIXED)
    // -----------------------------
    glGenTextures(1, &depthCubeArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubeArray);

    glTexStorage3D(
        GL_TEXTURE_CUBE_MAP_ARRAY,
        1,
        GL_DEPTH_COMPONENT24,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        MAX_LIGHTS * 6
    );

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    // -----------------------------
    // 2D ARRAY (FIXED)
    // -----------------------------
    glGenTextures(1, &depthMaps2DArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps2DArray);

    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY,
        1,
        GL_DEPTH_COMPONENT24,
        SHADOW_WIDTH,
        SHADOW_HEIGHT,
        MAX_LIGHTS
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
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

void Renderer::DepthPass()
{
    glBindFramebuffer(GL_FRAMEBUFFER, FBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);

    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    glClear(GL_DEPTH_BUFFER_BIT);

    for (uint8_t i = 0; i < lightsPos.size() && i < MAX_LIGHTS; i++)
    {
        auto light = lightsPos[i].first;

        mat4 projection, view;

        switch (light->type)
        {
            case LIGHT_DIRECTIONAL:
            {
                vec3 dir = normalize(light->data1);
                vec3 center = vec3(0.0f);
                vec3 pos = center - dir * 30.0f;

                projection = ortho(-30.f, 30.f, -30.f, 30.f, 0.1f, 100.f);
                view = lookAt(pos, center, vec3(0, 1, 0));

                lightSpaceMatrices[i] = projection * view;

                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

                glFramebufferTextureLayer(
                    GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    depthMaps2DArray,
                    0,
                    i
                );

                glClear(GL_DEPTH_BUFFER_BIT);

                for (auto& obj : drawVector)
                    obj->Draw(depthShaderNormal);

                break;
            }

            case LIGHT_POINT:
            {
                vec3 pos = light->data1;
                projection = perspective(radians(90.f), 1.f, 0.1f, 20.f);

                static const vec3 dirs[6] = {
                    {1,0,0},{-1,0,0},
                    {0,1,0},{0,-1,0},
                    {0,0,1},{0,0,-1}
                };

                static const vec3 ups[6] = {
                    {0,-1,0},{0,-1,0},
                    {0,0,1},{0,0,-1},
                    {0,-1,0},{0,-1,0}
                };

                depthShaderLayered->Use();

                for (int face = 0; face < 6; face++)
                {
                    view = lookAt(pos, pos + dirs[face], ups[face]);

                    mat4 shadowMat = projection * view;
                    depthShaderLayered->SetMat4("lightSpaceMatrix", shadowMat);

                    int layer = i * 6 + face;

                    glFramebufferTextureLayer(
                        GL_FRAMEBUFFER,
                        GL_DEPTH_ATTACHMENT,
                        depthCubeArray,
                        0,
                        layer
                    );

                    glClear(GL_DEPTH_BUFFER_BIT);

                    for (auto& obj : drawVector)
                        obj->Draw(depthShaderLayered);
                }

                farPlanes[i] = 20.0f;
                break;
            }

            case LIGHT_SPOT:
            {
                projection = perspective(light->data4, 1.f, 0.1f, 20.f);
                view = lookAt(light->data1, light->data1 + light->data2, vec3(0,1,0));

                lightSpaceMatrices[i] = projection * view;

                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

                glFramebufferTextureLayer(
                    GL_FRAMEBUFFER,
                    GL_DEPTH_ATTACHMENT,
                    depthMaps2DArray,
                    0,
                    i
                );

                glClear(GL_DEPTH_BUFFER_BIT);

                for (auto& obj : drawVector)
                    obj->Draw(depthShaderNormal);

                break;
            }
        }
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,windowW, windowH);
    glCullFace(GL_BACK);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    if(scene->scenePlayer && scene->sceneCam) {
        vec3 ppos = scene->scenePlayer->GetTransform().GetTranslation();
        scene->sceneCam->SetPos(ppos);
    }
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightsPos.clear();
    drawVector.clear();
    drawVectorUI.clear();

    // Prepare culling
    currentScene = scene;
    ComputeFrustum();

    // Vectorize sceneCull
    PrepareDraw(scene->root, Transform());
    ResolveZ();

    // Setup shaders
    PrepareLights();
    DepthPass();
    BindShadowTextures();
    PrepareShaders();

    // Draw
    Draw();

    #if defined(DEBUG)
    glDisable(GL_DEPTH_TEST);
    DrawDebug();
    glEnable(GL_DEPTH_TEST);
    #endif
}

void Renderer::BindShadowTextures() {
    glActiveTexture(GL_TEXTURE0 + TEXTURES_SLOT_SHADOWMAPS);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps2DArray);
    glActiveTexture(GL_TEXTURE0 + TEXTURES_SLOT_SHADOWCUBEMAPS);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubeArray);
    glActiveTexture(GL_TEXTURE0);
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
    if(node->RenderType() == "Object2D") {
        shared_ptr<Object2D> obj2D = static_pointer_cast<Object2D>(node);
        shared_ptr<Shader> shader = obj2D->GetShader();
        if(obj2D->GetReqPerspective()) {
            shader->SetMat4("VP", currentScene->sceneCam->GetVP(1));
        } else {
            shader->SetMat4("VP", currentScene->sceneCam->GetVP(0));
        }
    } else if(node->Type() == "Object3D") {
        shared_ptr<Object3D> obj3D = static_pointer_cast<Object3D>(node);
        shared_ptr<Shader> shader = obj3D->GetShader();
        auto vp = currentScene->sceneCam->GetVP(1);
        shader->SetMat4("VP", currentScene->sceneCam->GetVP(1));
        shader->SetVec3("viewPos", vec3(currentScene->sceneCam->GetPos(),0.0f));
        uint8_t count = std::min((int)lightsPos.size(),20);
        shader->SetInt("lightsNum", count);
        for(uint8_t i = 0; i < count; i++) {
            SetLight(lightsPos[i].first, shader, i);
        }
        shader->SetInt("shadowMaps2D", TEXTURES_SLOT_SHADOWMAPS);
        shader->SetInt("shadowCubemaps", TEXTURES_SLOT_SHADOWCUBEMAPS);
        for (uint8_t i = 0; i < count; i++) {
            shader->SetMat4("lightSpaceMatrices[" + to_string(i) + "]", lightSpaceMatrices[i]);
            shader->SetFloat("farPlanes[" + to_string(i) + "]", farPlanes[i]);
        }
    } else if(node->RenderType() == "TextNode") {
        shared_ptr<TextNode> textNode = static_pointer_cast<TextNode>(node);
        shared_ptr<Shader> shader = textNode->GetShader();
        if(textNode->TestIgnoreParent()) {
            shader->SetMat4("VP", currentScene->sceneCam->GetUI());
        } else {
            shader->SetMat4("VP", currentScene->sceneCam->GetVP(0));
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
}

Renderer::~Renderer() {
    glfwDestroyWindow(window);
}