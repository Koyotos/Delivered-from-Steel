#include "include/Renderer/Renderer.hpp"
#include "include/Renderer/Shader.hpp"
#include "include/Renderer/Utils.hpp"

static auto normalizePlane = [](vec4& plane) {
        float length = glm::length(vec3(plane));
        plane /= length;
};

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
    postProcessingShader = rsm.LoadShader("postProcess");

    glGenFramebuffers(1, &depthFBO);
    GenShadowMaps();

    glGenFramebuffers(1,&mainFBO);
    glGenTextures(1, &mainColorBuffer);
    glBindTexture(GL_TEXTURE_2D, mainColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowW, windowH, 0, GL_RGBA, GL_FLOAT, NULL); 
    glBindTexture(GL_TEXTURE_2D, 0);

    tuple<GLuint, GLuint, GLuint> screenQuad = CreateQuad(windowW, windowH);
    screenQuadVAO = get<0>(screenQuad);
    screenQuadVBO = get<1>(screenQuad);
    screenQuadEBO = get<2>(screenQuad);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
}

void Renderer::GenShadowMaps() {
    // Cube array
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

   // Array 2D
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
}

void Renderer::ComputeFrustum() {
    frustumLeft = vec4(frameVP[0][3] + frameVP[0][0], frameVP[1][3] + frameVP[1][0], frameVP[2][3] + frameVP[2][0], frameVP[3][3] + frameVP[3][0]);
    frustumRight = vec4(frameVP[0][3] - frameVP[0][0], frameVP[1][3] - frameVP[1][0], frameVP[2][3] - frameVP[2][0], frameVP[3][3] - frameVP[3][0]);
    frustumBottom = vec4(frameVP[0][3] + frameVP[0][1], frameVP[1][3] + frameVP[1][1], frameVP[2][3] + frameVP[2][1], frameVP[3][3] + frameVP[3][1]);
    frustumTop = vec4(frameVP[0][3] - frameVP[0][1], frameVP[1][3] - frameVP[1][1], frameVP[2][3] - frameVP[2][1], frameVP[3][3] - frameVP[3][1]);

    normalizePlane(frustumLeft);
    normalizePlane(frustumRight);
    normalizePlane(frustumBottom);
    normalizePlane(frustumTop);
}  

bool Renderer::AffectsLight(const shared_ptr<VisualNode>& obj, const shared_ptr<Light>& light) {
    vec3 objPos = obj->GetTransform().GetGlobal()[3];
    float radius = obj->GetCullRadius();
    switch(light->type) {
        case LIGHT_POINT: {
            float maxDist = 20.0f;
            float dist2 = glm::length2(objPos - light->data1);
            float r = maxDist + radius + 2.0f;
            return dist2 < r * r;
        }
        case LIGHT_SPOT: {
            vec3 toObj = objPos - light->data1;
            float dist = glm::length(toObj);

            float maxDist = 20.0f;
            if(dist > maxDist + radius) return false;

            vec3 dir = normalize(light->data2);
            float angle = dot(normalize(toObj), dir);

            float cutoff = light->data4;
            return angle > cutoff - 0.1f;
        }
    }
    return true;
}

void Renderer::DepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_FRONT);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    for(uint8_t i = 0; i < lightsPos.size() && i < MAX_LIGHTS; i++) {
        shared_ptr<Light> light = lightsPos[i].first;
        mat4 projection, view;
        switch(light->type) {
            case LIGHT_DIRECTIONAL: {
                vec3 dir = normalize(light->data1);
                vec3 center = vec3(0.0f);
                vec3 pos = center - dir * 30.0f;
                projection = ortho(-30.f, 30.f, -30.f, 30.f, 0.1f, 100.f);
                view = lookAt(pos, center, vec3(0, 1, 0));
                lightSpaceMatrices[i] = projection * view;
                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);
                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps2DArray, 0, i);
                glClear(GL_DEPTH_BUFFER_BIT);
                for(auto& obj : potentialCasters) {
                    PROFILER_ADD_OBJECT();
                    obj->Draw(depthShaderNormal);
                }
                break;
            }
            case LIGHT_POINT: {
                vec3 pos = light->data1;
                projection = perspective(radians(90.f), 1.f, 0.1f, 20.f);
                depthShaderLayered->Use();
                for(int face = 0; face < 6; face++) {
                    view = lookAt(pos, pos + dirs[face], ups[face]);
                    mat4 shadowMat = projection * view;
                    depthShaderLayered->SetMat4("lightSpaceMatrix", shadowMat);
                    int layer = i * 6 + face;
                    glFramebufferTextureLayer( GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeArray, 0, layer);
                    glClear(GL_DEPTH_BUFFER_BIT);
                    for(auto& obj : potentialCasters){
                        if(!AffectsLight(obj, light)) {
                            continue;
                        }
                        PROFILER_ADD_OBJECT();
                        obj->Draw(depthShaderLayered);
                    }
                }
                farPlanes[i] = 20.0f;
                break;
            }
            case LIGHT_SPOT: {
                projection = perspective(light->data4, 1.f, 0.1f, 20.f);
                view = lookAt(light->data1, light->data1 + light->data2, vec3(0,1,0));
                lightSpaceMatrices[i] = projection * view;
                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);
                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps2DArray, 0, i);
                glClear(GL_DEPTH_BUFFER_BIT);
                for(auto& obj : potentialCasters){
                    if(!AffectsLight(obj, light)) {
                        continue;
                    }
                    PROFILER_ADD_OBJECT();
                    obj->Draw(depthShaderLayered);
                }
                break;
            }
        }
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,windowW, windowH);
    glCullFace(GL_BACK);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightsPos.clear();
    drawVector.clear();
    drawVectorUI.clear();
    potentialCasters.clear();
    lightsUpdatedList.clear();

    // Preallocate memory (optimization)
    lightsPos.reserve(MAX_LIGHTS);
    drawVector.reserve(OBJECTS_NUMBER_PREDICT);
    drawVectorUI.reserve(UI_NUMBER_PREDICT);
    lightsUpdatedList.reserve(LIGHTS_SHADERS_PREDICT);

    frameVP = scene->sceneCam->GetVP(1);
    frameVO = scene->sceneCam->GetVP(0);
    frameO = scene->sceneCam->GetUI();

    // Prepare culling
    currentScene = scene;
    ComputeFrustum();

    // Vectorize scene
    scene->UpdateTransforms(static_pointer_cast<PhysicsNode>(scene->root), Transform());
    PrepareDraw(scene->root, Transform());
    ResolveZ();

    // Setup shaders
    PrepareLights();
    DepthPass();
    BindShadowTextures();
    PrepareShaders();

    // Draw
    Draw();

    // Post processing
    PostProcessingPass();

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
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    for(auto& node : drawVector) {
        PROFILER_ADD_OBJECT();
        node->Draw();
    }
    glDisable(GL_CULL_FACE);
    for(auto& node : drawVectorUI) {
        PROFILER_ADD_OBJECT();
        node->Draw();
    }
    glEnable(GL_CULL_FACE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcessingPass() {
    glActiveTexture(GL_TEXTURE0+TEXTURES_SLOT_RENDERER_COLOR_BUFFER);
    glBindTexture(GL_TEXTURE_2D, mainColorBuffer);
    glDisable(GL_DEPTH_TEST);
    postProcessingShader->SetInt("hdrBuffer",TEXTURES_SLOT_RENDERER_COLOR_BUFFER);
    postProcessingShader->SetFloat("exposure", 0.1);
    glBindVertexArray(screenQuadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindVertexArray(0);
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
    if(node->RenderType() >= NRT_OBJECT2D) {
        shared_ptr<VisualNode> cast = static_pointer_cast<VisualNode>(node);
        PrepareDrawNode(cast, t);
        if(node->RenderType() == NRT_OBJECT3D) {
            potentialCasters.push_back(cast);
        }
        
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

void Renderer::PrepareDrawNode(shared_ptr<VisualNode> visualCast, Transform& t) {
    t = visualCast->GetTransform();
    if(Cull(visualCast)) {
        if(visualCast->RenderType() == NRT_OBJECT2D || visualCast->RenderType() == NRT_TEXTNODE) {
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
    for(auto& s : lightsUpdatedList) {
        if(s==shader.get()) {
            return;
        }
    }
    shader->SetInt("lights["+to_string(index)+"].type", light->type);
    shader->SetVec3("lights["+to_string(index)+"].data1", light->data1);
    shader->SetVec3("lights["+to_string(index)+"].data2", light->data2);
    shader->SetVec3("lights["+to_string(index)+"].data3", light->data3);
    shader->SetFloat("lights["+to_string(index)+"].data4", light->data4);
    shader->SetVec3("lights["+to_string(index)+"].colorAmbient", light->colorAmbient);
    shader->SetVec3("lights["+to_string(index)+"].colorDiffuse", light->colorDiffuse);
    shader->SetVec3("lights["+to_string(index)+"].colorSpecular", light->colorSpecular);
    lightsUpdatedList.push_back(shader.get());
}

void Renderer::ConfigureShader(shared_ptr<Node> node) {
    shared_ptr<Shader> shader;
    switch(node->RenderType()) {
        case NRT_OBJECT3D: {
            shared_ptr<Object3D> obj3D = static_pointer_cast<Object3D>(node);
            shader = obj3D->GetShader();
            shader->SetMat4("VP", frameVP);
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
            break;
        }
        case NRT_OBJECT2D: {
            shared_ptr<Object2D> obj2D = static_pointer_cast<Object2D>(node);
            shader = obj2D->GetShader();
            if(obj2D->GetReqPerspective()) {
                shader->SetMat4("VP", frameVP);
            } else {
                shader->SetMat4("VP", frameVO);
            }
            break;
        }
        case NRT_TEXTNODE: {
            shared_ptr<TextNode> textNode = static_pointer_cast<TextNode>(node);
            shader = textNode->GetShader();
            if(textNode->TestIgnoreParent()) {
                shader->SetMat4("VP", frameO);
            } else {
                shader->SetMat4("VP", frameVO);
            }
            break;
        }
        default : break;
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
    glDeleteBuffers(1,&screenQuadVBO);
    glDeleteVertexArrays(1,&screenQuadVAO);
    glDeleteBuffers(1,&screenQuadEBO);
}