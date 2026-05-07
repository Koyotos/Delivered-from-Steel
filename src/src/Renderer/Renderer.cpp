#include "include/Renderer/Renderer.hpp"
#include "include/Core/VisualNode.hpp"
#include "include/Renderer/Light.hpp"
#include "include/Renderer/Shader.hpp"
#include "include/Renderer/Utils.hpp"

void Renderer::Init(ResourceManager& rsm) {
    if (!glfwInit())
        throw runtime_error("Can't initialize GLFW");

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GL_VERSION_MAJOR);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GL_VERSION_MINOR);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    window = glfwCreateWindow(DEF_WIN_W, DEF_WIN_H, "Delivered From Steel", nullptr, nullptr);
    if (!window)
        throw runtime_error("Can't create window");

    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
        throw runtime_error("GL loader error");

    lightSpaceMatrices.resize(MAX_LIGHTS_DIR_AND_SPOT);
    farPlanes.resize(MAX_LIGHTS_POINT);

    depthShaderLayered = rsm.LoadShader("layeredDepth");
    depthShaderNormal  = rsm.LoadShader("simpleDepth");
    postProcessingShader = rsm.LoadShader("postProcess");
    blurShader = rsm.LoadShader("blur");

    GenFramebuffers();

    tuple<GLuint, GLuint, GLuint> screenQuad = CreateQuad(windowW, windowH, true);
    screenQuadVAO = get<0>(screenQuad);
    screenQuadVBO = get<1>(screenQuad);
    screenQuadEBO = get<2>(screenQuad);

    // Preallocate memory (optimization)
    lightsPos.reserve(MAX_LIGHTS_DIR_AND_SPOT*2);
    lightsPosPoint.reserve(MAX_LIGHTS_POINT*2);
    drawVector.reserve(OBJECTS_NUMBER_PREDICT);
    drawVectorUI.reserve(UI_NUMBER_PREDICT);
    updatedShaders.reserve(SHADER_NUMBER_PREDICT);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glEnable(GL_CULL_FACE);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glDepthFunc(GL_LESS);
}

void Renderer::GenFramebuffers() {
    glGenFramebuffers(1, &depthFBO);
    GenShadowMaps();

    glGenFramebuffers(1,&mainFBO);
    glBindFramebuffer(GL_FRAMEBUFFER, mainFBO);
    glGenTextures(1, &mainColorBuffer);
    glBindTexture(GL_TEXTURE_2D, mainColorBuffer);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowW, windowH, 0, GL_RGBA, GL_FLOAT, NULL); 
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, mainColorBuffer, 0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glGenTextures(1, &depthColorBuffer);
    glBindTexture(GL_TEXTURE_2D, depthColorBuffer);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24,
                windowW, windowH, 0,
                GL_DEPTH_COMPONENT, GL_FLOAT, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

    glFramebufferTexture2D(GL_FRAMEBUFFER,
        GL_DEPTH_ATTACHMENT,
        GL_TEXTURE_2D,
        depthColorBuffer, 0);

    glGenTextures(1, &brightColorBuffer);
    glBindTexture(GL_TEXTURE_2D, brightColorBuffer);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowW, windowH, 0, GL_RGBA, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT1,
                        GL_TEXTURE_2D, brightColorBuffer, 0);

    GLuint attachments[2] = { GL_COLOR_ATTACHMENT0, GL_COLOR_ATTACHMENT1 };
    glDrawBuffers(2, attachments);

    glGenFramebuffers(2, blurFBOs);
    glGenTextures(2, blurColorBuffers);

    for (int i = 0; i < 2; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[i]);
        glBindTexture(GL_TEXTURE_2D, blurColorBuffers[i]);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, windowW, windowH, 0, GL_RGBA, GL_FLOAT, NULL);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0,
                            GL_TEXTURE_2D, blurColorBuffers[i], 0);
    }

    if(glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE) {
        throw std::runtime_error("Framebuffer not complete!");
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::GenShadowMaps() {
    // Cube array
    glGenTextures(1, &depthCubeArray);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, depthCubeArray);

    glTexStorage3D(GL_TEXTURE_CUBE_MAP_ARRAY, 1, GL_DEPTH_COMPONENT24,
                shadowW, shadowH, MAX_LIGHTS_POINT * 6);

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP_ARRAY, GL_TEXTURE_MAX_LEVEL, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP_ARRAY, 0);

   // Array 2D
    glGenTextures(1, &depthMaps2DArray);
    glBindTexture(GL_TEXTURE_2D_ARRAY, depthMaps2DArray);

    glTexStorage3D(
        GL_TEXTURE_2D_ARRAY,
        1,
        GL_DEPTH_COMPONENT24,
        shadowW,
        shadowH,
        MAX_LIGHTS_DIR_AND_SPOT
    );

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::Reconfigure(const RendererCommand& command, const int16_t& value) {
    switch(command) {
        case RCMD_RESIZE_W: { 
            windowW = value;
            break;
        }
        case RCMD_RESIZE_H: { 
            windowH = value;
            break;
        }
        case RCMD_REMAKE_WINDOW: {
            glfwDestroyWindow(window);
            window = glfwCreateWindow(windowW, windowH, "Delivered From Steel", nullptr, nullptr);
            break;
        }
        case RCMD_SHADOW_QUALITY: {
            switch (value) {
                case RCMDVAL_SHADOWS_LOW:
                    shadowW = 512;
                    shadowH = 512;
                    break;
                case RCMDVAL_SHADOWS_MEDIUM:
                    shadowW = 1024;
                    shadowH = 1024;
                    break;
                case RCMDVAL_SHADOWS_HIGH:
                    shadowW = 2048;
                    shadowH = 2048;
                    break;
            }
            glDeleteTextures(1,&depthMaps2DArray);
            glDeleteTextures(1,&depthCubeArray);
            GenShadowMaps();
            break;
        }
        case RCMD_BLOOM: {
            postProcessingShader->SetBool("bloom", value);
            break;
        }
        case RCMD_GOD_RAYS: {
            postProcessingShader->SetBool("godRays", value);
            break;
        }
    }
}

void Renderer::ComputeFrustum() {
    frustumPlanes[0] = frameVP[3] + frameVP[0];
    frustumPlanes[1] = frameVP[3] - frameVP[0];
    frustumPlanes[2] = frameVP[3] + frameVP[1];
    frustumPlanes[3] = frameVP[3] - frameVP[1];
    frustumPlanes[4] = frameVP[3] + frameVP[2];
    frustumPlanes[5] = frameVP[3] - frameVP[2];

    for(auto& p : frustumPlanes) {
        p*=1.0f/length(p);
    }
}

bool Renderer::AffectsLight(const shared_ptr<VisualNode>& obj, const shared_ptr<Light>& light) {
    vec3 objPos = obj->GetTransform().GetGlobal()[3];
    float radius = obj->GetCullRadius();
    switch(light->type) {
        case LIGHT_POINT: {
            float maxDist = 40.0f;
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
        default : break;
    }
    return true;
}

void Renderer::DepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glViewport(0, 0, SHADOW_WIDTH, SHADOW_HEIGHT);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);

    for(uint8_t i = 0; i < lightsPos.size() && i < MAX_LIGHTS_DIR_AND_SPOT; i++) {
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
                sunMatrix = lightSpaceMatrices[i];

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
            case LIGHT_SPOT: {
                projection = perspective(light->data4, 1.f, 0.1f, 10.f);
                view = lookAt(light->data1, light->data1 + light->data2, vec3(0,1,0));
                lightSpaceMatrices[i] = projection * view;

                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps2DArray, 0, i);
                glClear(GL_DEPTH_BUFFER_BIT);

                for(auto& obj : potentialCasters){
                    if(!AffectsLight(obj, light)) continue;

                    PROFILER_ADD_OBJECT();
                    obj->Draw(depthShaderNormal);
                }
                break;
            }
            default : break;
        }
    }
    for(uint8_t i = 0; i < lightsPosPoint.size() && i < MAX_LIGHTS_POINT; i++) {
        shared_ptr<Light> light = lightsPosPoint[i].first;
        mat4 projection, view;
        vec3 pos = light->data1;
        projection = perspective(radians(90.f), 1.f, 0.1f, 10.f);
        depthShaderLayered->Use();
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeArray, 0);  
        mat4 shadowMatrices[6];
        for(int face = 0; face < 6; face++) {
            view = lookAt(pos, pos + dirs[face], ups[face]);
            shadowMatrices[face] = projection * view;
        }
        for(int f = 0; f < 6; f++) {
            depthShaderLayered->SetMat4("shadowMatrices[" + std::to_string(f) + "]", shadowMatrices[f]);
        }
        depthShaderLayered->SetInt("lightIndex", i);
        glClear(GL_DEPTH_BUFFER_BIT);
        for(auto& obj : potentialCasters){
            if(!AffectsLight(obj, light)) continue;
            PROFILER_ADD_OBJECT();
            obj->Draw(depthShaderLayered);
        }
        farPlanes[i] = 10.0f;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,windowW, windowH);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightsPos.clear();
    lightsPosPoint.clear();
    drawVector.clear();
    drawVectorUI.clear();
    potentialCasters.clear();
    updatedShaders.clear();

    frameVP = scene->sceneCam->GetVP(1);
    frameVO = scene->sceneCam->GetVP(0);
    frameO = scene->sceneCam->GetO();
    frameV = scene->sceneCam->GetV();
    frameP = scene->sceneCam->GetP();

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
    BlurBloomPass();

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
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    glBindFramebuffer(GL_FRAMEBUFFER,0);
    glViewport(0, 0, windowW, windowH);
    glClear(GL_COLOR_BUFFER_BIT);
    glActiveTexture(GL_TEXTURE0+TEXTURES_SLOT_RENDERER_COLOR_BUFFER);
    glBindTexture(GL_TEXTURE_2D, mainColorBuffer);
    glDisable(GL_DEPTH_TEST);
    postProcessingShader->SetInt("hdrBuffer",TEXTURES_SLOT_RENDERER_COLOR_BUFFER);
    postProcessingShader->SetFloat("exposure", 0.6);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthColorBuffer);
    postProcessingShader->SetInt("depthBuffer", 1);
    postProcessingShader->SetMat4("invProjection", inverse(frameP));
    postProcessingShader->SetMat4("invView", inverse(frameV));
    postProcessingShader->SetVec3("lightDir", sunDir);
    postProcessingShader->SetMat4("sunMatrix", sunMatrix);
    postProcessingShader->SetVec3("lightColor", vec3(1.0));
    postProcessingShader->SetInt("shadowMaps2D", TEXTURES_SLOT_SHADOWMAPS);
    glBindVertexArray(screenQuadVAO);
    glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
    glBindTexture(GL_TEXTURE_2D,0);
    glBindVertexArray(0);
}

void Renderer::BlurBloomPass() {
    bool horizontal = true;
    bool first_iteration = true;
    int amount = 5;
    blurShader->SetInt("image", 0);
    glBindVertexArray(screenQuadVAO);
    for (int i = 0; i < amount; i++) {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[horizontal]);
        blurShader->SetBool("horizontal", horizontal);
        glBindTexture(GL_TEXTURE_2D,
            first_iteration ? brightColorBuffer : blurColorBuffers[!horizontal]);
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        horizontal = !horizontal;
        if (first_iteration)
            first_iteration = false;
    }
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PrepareLights() {
    sort(lightsPos.begin(), lightsPos.end(), [](const pair<shared_ptr<Light>,float>& a, 
        const pair<shared_ptr<Light>,float>& b) {
            return a.second < b.second;
    });
    sort(lightsPosPoint.begin(), lightsPosPoint.end(), [](const pair<shared_ptr<Light>,float>& a, 
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

bool Renderer::Cull(const std::shared_ptr<VisualNode>& node) {
    const float radius = node->GetCullRadius();
    if (radius < CULL_RADIUS_ALWAYS_TRUE)
        return true;
    const glm::vec3 pos = node->GetTransform().GetGlobal()[3];

    for (int i = 0; i < 6; i++) {
        const glm::vec4& p = frustumPlanes[i];
        if (glm::dot(glm::vec3(p), pos) + p.w > -radius)
            return false;
    }
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
    vec2 campos;
    float dist;
    switch(light->type) {
        case LIGHT_DIRECTIONAL: {
            sunDir = light->data1;
            lightsPos.push_back({light,0});
            break;
        }
        case LIGHT_POINT: {
            campos = currentScene->sceneCam->GetPos();
            dist = (campos.x-light->data1.x)*(campos.x-light->data1.x) + (campos.y-light->data1.y)*(campos.y-light->data1.y);
            lightsPosPoint.push_back({light,dist});
            break;
        }
        case LIGHT_SPOT: {
            campos = currentScene->sceneCam->GetPos();
            dist = (campos.x-light->data1.x)*(campos.x-light->data1.x) + (campos.y-light->data1.y)*(campos.y-light->data1.y);
            lightsPos.push_back({light,dist});
            break;
        }   
        default: break;
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

void Renderer::ConfigureShader(shared_ptr<VisualNode> node) {
    shared_ptr<Shader> shader = node->GetShader();
    for(auto& s : updatedShaders) {
        if(s == shader.get()) {
            return;
        }
    }
    switch(node->RenderType()) {
        case NRT_OBJECT3D: {
            shared_ptr<Object3D> obj3D = static_pointer_cast<Object3D>(node);
            shader->SetMat4("VP", frameVP);
            shader->SetVec3("viewPos", vec3(currentScene->sceneCam->GetPos(),0.0f));
            uint8_t count = std::min((int)lightsPos.size(),MAX_LIGHTS_DIR_AND_SPOT);
            uint8_t countPoint = std::min((int)lightsPosPoint.size(),MAX_LIGHTS_POINT);
            shader->SetInt("lightsNum", count+countPoint);
            for(uint8_t i = 0; i < count; i++) {
                shader->SetMat4("lightSpaceMatrices[" + to_string(i) + "]", lightSpaceMatrices[i]);
                SetLight(lightsPos[i].first, shader, i);
            }
            for(uint8_t i = 0; i < countPoint; i++) {
                shader->SetFloat("farPlanes[" + to_string(i) + "]", farPlanes[i]);
                SetLight(lightsPosPoint[i].first, shader, i+count);
            }
            shader->SetInt("shadowMaps2D", TEXTURES_SLOT_SHADOWMAPS);
            shader->SetInt("shadowCubemaps", TEXTURES_SLOT_SHADOWCUBEMAPS);
            break;
        }
        case NRT_OBJECT2D: {
            shared_ptr<Object2D> obj2D = static_pointer_cast<Object2D>(node);
            if(obj2D->GetReqPerspective()) {
                shader->SetMat4("VP", frameVP);
            } else {
                shader->SetMat4("VP", frameVO);
            }
            break;
        }
        case NRT_TEXTNODE: {
            shared_ptr<TextNode> textNode = static_pointer_cast<TextNode>(node);
            if(textNode->TestIgnoreParent()) {
                shader->SetMat4("VP", frameO);
            } else {
                shader->SetMat4("VP", frameVO);
            }
            break;
        }
        default : break;
    }
    updatedShaders.push_back(shader.get());
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
    glDeleteBuffers(1,&mainColorBuffer);
    glDeleteBuffers(1,&depthColorBuffer);
    glDeleteBuffers(1,&brightColorBuffer);
    glDeleteBuffers(2,blurColorBuffers);
    glDeleteFramebuffers(1,&mainFBO);
    glDeleteFramebuffers(1,&depthFBO);
    glDeleteFramebuffers(2,blurFBOs);
    glDeleteTextures(1,&depthMaps2DArray);
    glDeleteTextures(1,&depthCubeArray);
}