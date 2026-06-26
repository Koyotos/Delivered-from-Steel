#include "include/Renderer/Renderer.hpp"

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

    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
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

    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D_ARRAY, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Renderer::Reconfigure(const RendererCommand& command, const int16_t& iv, const float& fv) {
    switch(command) {
        case RCMD_RESIZE_W: { 
            windowW = iv;
            break;
        }
        case RCMD_RESIZE_H: { 
            windowH = iv;
            break;
        }
        case RCMD_REMAKE_WINDOW: {
            glfwSetWindowSize(window, windowW, windowH);
            glViewport(0, 0, windowW, windowH);
            DestroyBuffers();
            GenFramebuffers();
            GenShadowMaps();
            break;
        }
        case RCMD_SHADOW_QUALITY: {
            switch (iv) {
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
            postProcessingShader->SetBool("bloom", iv);
            break;
        }
        case RCMD_GOD_RAYS: {
            postProcessingShader->SetBool("godRays", iv);
            break;
        }
        case RCMD_POINT_CULL_DIST: {
            pointCull = fv;
            break;
        }
        case RCMD_SPOT_CULL_DIST: {
            spotCull = fv;
            break;
        }
        case RCMD_DIR_DISTANCE: {
            dirDistance = fv;
            break;
        }
        case RCMD_LIGHT_CULL_RADIUS: {
            lightCullRadius = fv;
            break;
        }
        case RCMD_SATURATION_CONTROL: {
            postProcessingShader->SetBool("saturationControl", iv);
            break;
        }
        case RCMD_SATURATION_VALUE: {
            postProcessingShader->SetFloat("saturationValue", fv);
            break;
        }
        case RCMD_FULLSCREEN: {
            if(iv) {
                const GLFWvidmode* mode = glfwGetVideoMode(monitor);
                glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, mode->refreshRate);
            } else {
                glfwSetWindowMonitor(window,nullptr,0,0,windowW, windowH, GLFW_DONT_CARE);
            }
        }
    }
}

void Renderer::ComputeFrustum() {
    const mat4& m = frameVP;

    vec4 row0(m[0][0], m[1][0], m[2][0], m[3][0]);
    vec4 row1(m[0][1], m[1][1], m[2][1], m[3][1]);
    vec4 row2(m[0][2], m[1][2], m[2][2], m[3][2]);
    vec4 row3(m[0][3], m[1][3], m[2][3], m[3][3]);

    frustumPlanes[0] = row3 + row0; // left
    frustumPlanes[1] = row3 - row0; // right
    frustumPlanes[2] = row3 + row1; // bottom
    frustumPlanes[3] = row3 - row1; // top
    frustumPlanes[4] = row3 + row2; // near
    frustumPlanes[5] = row3 - row2; // far

    for (auto& p : frustumPlanes) {
        p /= length(vec3(p));
    }
}

bool Renderer::Cull(const shared_ptr<VisualNode>& node) {
    float radius = node->GetCullRadius();
    if (radius < CULL_RADIUS_ALWAYS_TRUE)
        return true;
    vec3 pos = vec3(node->GetTransform().GetGlobal()[3]);
    for(int i = 0; i < 6; ++i) {
        const vec4& p = frustumPlanes[i];
        if(dot(vec3(p), pos) + p.w < -radius)
            return false;
    }

    return true;
}

void Renderer::DepthPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, depthFBO);
    glViewport(0, 0, shadowW, shadowH);
    glEnable(GL_DEPTH_TEST);
    glCullFace(GL_BACK);

    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glColorMask(GL_FALSE, GL_FALSE, GL_FALSE, GL_FALSE);

    for(uint8_t i = 0; i < lightsPos.size() && i < MAX_LIGHTS_DIR_AND_SPOT; i++) {
        shared_ptr<Light> light = lightsPos[i].first;
        mat4 projection, view;

        switch(light->type) {
            case LIGHT_DIRECTIONAL: {
                vec3 dir = normalize(light->data1);
                vec3 center = vec3(currentScene->sceneCam->GetPos(), 0);
                vec3 pos = center - dir * dirDistance;

                projection = ortho(-30.f, 30.f, -30.f, 30.f, 0.1f, 100.f);
                view = lookAt(pos, center, vec3(0, 1, 0));
                lightSpaceMatrices[i] = projection * view;
                sunMatrix = lightSpaceMatrices[i];

                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps2DArray, 0, i);
                glClear(GL_DEPTH_BUFFER_BIT);

                for(auto& data : potentialCasters) {
                    PROFILER_ADD_OBJECT();
                    data.model->DrawInstanced(*depthShaderNormal, data.matrices);
                }
                break;
            }
            case LIGHT_SPOT: {
                vec3 dir = normalize(light->data2);
                vec3 up = abs(dot(dir, vec3(0,1,0))) > 0.999f ? vec3(0,0,1) : vec3(0,1,0);
                float outerAngle = light->data4 * 0.5f + glm::radians(5.0f);
                projection = glm::perspective(outerAngle * 2.0f, 1.0f, 0.1f, 10.0f);
                view = lookAt(light->data1,light->data1 + dir,up);
                lightSpaceMatrices[i] = projection * view;

                depthShaderNormal->Use();
                depthShaderNormal->SetMat4("lightSpaceMatrix", lightSpaceMatrices[i]);

                glFramebufferTextureLayer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthMaps2DArray, 0, i);
                glClear(GL_DEPTH_BUFFER_BIT);

                for(auto& data : potentialCasters) {
                    PROFILER_ADD_OBJECT();
                    data.model->DrawInstanced(*depthShaderNormal, data.matrices);
                }
                break;
            }
            default : break;
        }
    }
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeArray, 0);  
    glClear(GL_DEPTH_BUFFER_BIT);
    for(uint8_t i = 0; i < lightsPosPoint.size() && i < MAX_LIGHTS_POINT; i++) {
        
        shared_ptr<Light> light = lightsPosPoint[i].first;
        mat4 projection, view;
        vec3 pos = light->data1;
        projection = perspective(radians(90.f), 1.f, 0.01f, 10.f);
        mat4 shadowMatrices[6];
        for(int f = 0; f < 6; f++) {
            view = lookAt(pos, pos + dirs[f], ups[f]);
            shadowMatrices[f] = projection * view;
            depthShaderLayered->SetMat4("shadowMatrices[" + std::to_string(f) + "]", shadowMatrices[f]);
        }
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depthCubeArray, 0);  
        depthShaderLayered->SetInt("lightIndex", i);
        depthShaderLayered->SetVec3("lightPos", pos);
        depthShaderLayered->SetFloat("farPlane", 10.0f);
        farPlanes[i] = 10.0f;
        for(auto& data : potentialCasters){
            PROFILER_ADD_OBJECT();
            data.model->DrawInstanced(*depthShaderLayered, data.matrices);
        }
        farPlanes[i] = 10.0f;
    }
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0,0,windowW, windowH);
}

void Renderer::DrawScene(shared_ptr<Scene> scene) {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    lightsPos.clear();
    lightsPosPoint.clear();
    drawVector.clear();
    drawLookup.clear();
    casterLookup.clear();
    drawVectorUI.clear();
    drawVector2D.clear();
    potentialCasters.clear();
    updatedShaders.clear();
    sunExists = false;

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
    for(auto& data : drawVector) {
        PROFILER_ADD_OBJECT();
        data.model->DrawInstanced(*data.shader, data.matrices);
    }
    glDisable(GL_CULL_FACE);
    for(auto& node : drawVector2D) {
        PROFILER_ADD_OBJECT();
        node->Draw();
    }
    glDisable(GL_DEPTH_TEST);
    for(auto& node : drawVectorUI) {
        PROFILER_ADD_OBJECT();
        node->Draw();
    }
    glEnable(GL_CULL_FACE);
    glEnable(GL_DEPTH_TEST);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Renderer::PostProcessingPass() {
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, windowW, windowH);
    glClear(GL_COLOR_BUFFER_BIT);
    postProcessingShader->Use();
    glDisable(GL_DEPTH_TEST);

    // HDR scene
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, mainColorBuffer);
    postProcessingShader->SetInt("hdrBuffer", 0);

    // Depth
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, depthColorBuffer);
    postProcessingShader->SetInt("depthBuffer", 1);

    // Bloom
    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D, bloomTexture);
    postProcessingShader->SetInt("bloomBlur", 2);
    postProcessingShader->SetFloat("exposure", 0.6f);
    postProcessingShader->SetBool("bloom", true);
    postProcessingShader->SetBool("godRays", true);
    postProcessingShader->SetBool("sunExists", sunExists);
    postProcessingShader->SetMat4("invProjection",inverse(frameP));
    postProcessingShader->SetMat4("invView",inverse(frameV));
    postProcessingShader->SetMat4("projection", currentScene->sceneCam->GetP());
    postProcessingShader->SetMat4("view", currentScene->sceneCam->GetV());
    postProcessingShader->SetVec3("lightDir", sunDir);
    postProcessingShader->SetMat4("sunMatrix", sunMatrix);
    postProcessingShader->SetVec3("lightColor", vec3(1.0f));
    postProcessingShader->SetInt("shadowMaps2D",TEXTURES_SLOT_SHADOWMAPS);
    postProcessingShader->SetVec3("camPos",vec3(currentScene->sceneCam->GetPos(),0));
    glBindVertexArray(screenQuadVAO);
    glDrawElements(
        GL_TRIANGLES,
        6,
        GL_UNSIGNED_INT,
        nullptr
    );
    glBindVertexArray(0);
}

void Renderer::BlurBloomPass() {
    blurShader->Use();
    bool horizontal = true;
    bool firstIteration = true;
    const int amount = 10;
    glViewport(0, 0, windowW, windowH);
    glBindVertexArray(screenQuadVAO);
    blurShader->SetInt("image", 0);
    for (int i = 0; i < amount; ++i) {
        glBindFramebuffer(GL_FRAMEBUFFER, blurFBOs[horizontal]);
        blurShader->SetBool("horizontal", horizontal);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(
            GL_TEXTURE_2D,
            firstIteration
                ? brightColorBuffer
                : blurColorBuffers[!horizontal]
        );
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        horizontal = !horizontal;
        if (firstIteration)
            firstIteration = false;
    }
    glBindVertexArray(0);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    bloomTexture = blurColorBuffers[!horizontal];
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
    for(auto& data : drawVector) {
        ConfigureShader(data.shader, NRT_OBJECT3D, false);
    }
    for(auto& node : drawVector2D) {
        ConfigureShader2D(node);
    }
    for(auto& node : drawVectorUI) {
        ConfigureShader2D(node);
    }
}

void Renderer::PrepareDraw(shared_ptr<Node> node, Transform t) {
    bool childTransformState = false;
    if(node->RenderType() >= NRT_OBJECT2D) {
        shared_ptr<VisualNode> cast = static_pointer_cast<VisualNode>(node);
        PrepareDrawNode(cast);
        if(node->RenderType() == NRT_OBJECT3D) {
            float dist = distance(vec2(currentScene->sceneCam->GetPos()),vec2(cast->GetTransform().GetGlobal()[3]));
            if(dist < lightCullRadius && node->TestDraw()) {
                CreateRenderData(static_pointer_cast<Object3D>(node), potentialCasters, casterLookup);
            }
        }
        
    } else if(node->Type() == "Light" && node->TestDraw()) {
        PrepareDrawLight(static_pointer_cast<Light>(node));
    }
    for(auto& k : node->GetChildren()) {
        k->SetTransformChanged(childTransformState);
        PrepareDraw(k, t);
    }
} 

void Renderer::ResolveZ() {
    sort(drawVectorUI.begin(), drawVectorUI.end(),[](const shared_ptr<VisualNode>& a, 
        const shared_ptr<VisualNode>& b) {
        return a->GetZIndex() < b->GetZIndex();
    });
}

void Renderer::PrepareDrawNode(shared_ptr<VisualNode> visualCast) {
    if(visualCast->TestDraw() && Cull(visualCast)) {
        if(visualCast->RenderType() == NRT_OBJECT2D) {
            if(static_pointer_cast<Object2D>(visualCast)->GetReqPerspective()) {
                drawVector2D.push_back(visualCast);
            } else {
                drawVectorUI.push_back(visualCast);
            }
        } else if(visualCast->RenderType() == NRT_TEXTNODE) {
            drawVectorUI.push_back(visualCast);
        } else {
            CreateRenderData(static_pointer_cast<Object3D>(visualCast),drawVector, drawLookup);
        }
    }
}

void Renderer::CreateRenderData(shared_ptr<Object3D> node, vector<RenderData>& dataset, unordered_map<BatchKey, size_t, BatchKeyHash>& batchLookup) {
    BatchKey key{node->GetModel().get(), node->GetShader().get()};
    auto it = batchLookup.find(key);
    if(it != batchLookup.end()) {
        dataset[it->second].matrices.push_back(node->GetTransform().GetGlobal());
        return;
    }
    size_t index = dataset.size();
    dataset.emplace_back(node->GetModel(), node->GetShader(),vector<mat4> {node->GetTransform().GetGlobal()});
    batchLookup[key] = index;
}

void Renderer::PrepareDrawLight(shared_ptr<Light> light) {
    vec2 campos;
    float dist;
    switch(light->type) {
        case LIGHT_DIRECTIONAL: {
            sunDir = light->data1;
            lightsPos.push_back({light,0});
            sunExists = true;
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

void Renderer::ConfigureShader(shared_ptr<Shader> shader, const NodeRenderType& type, const bool info2D) {
    if(updatedShaders.contains(shader.get())) {
        return;
    }
    shader->SetMat4("VP", frameVP);
    shader->SetVec3("viewPos", vec3(currentScene->sceneCam->GetPos(),0.0f));
    uint8_t count = std::min((int)lightsPos.size(),MAX_LIGHTS_DIR_AND_SPOT);
    uint8_t countPoint = std::min((int)lightsPosPoint.size(),MAX_LIGHTS_POINT);
    shader->SetInt("lightsNum", count+countPoint);
    shader->SetInt("dirSpotCount", count);
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
    updatedShaders.insert(shader.get());
}

void Renderer::ConfigureShader2D(shared_ptr<VisualNode> node) {
    shared_ptr<Shader> shader = node->GetShader();
    if(updatedShaders.contains(shader.get())) {
        return;
    }
    switch(node->RenderType()) {
        case NRT_OBJECT2D: {
            shared_ptr<Object2D> obj2d = static_pointer_cast<Object2D>(node);
            shader->SetMat4("M", obj2d->GetTransform().GetGlobal());
            if(obj2d->GetReqPerspective()) {
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
    shader->SetMat4("sunMatrix", sunMatrix);
    shader->SetInt("shadowMaps2D", TEXTURES_SLOT_SHADOWMAPS);
    updatedShaders.insert(shader.get());
}

void Renderer::EndFrame() {
    glfwMakeContextCurrent(window);
    glfwSwapBuffers(window);
}

void Renderer::DestroyBuffers() {
    glDeleteTextures(1,&mainColorBuffer);
    glDeleteTextures(1,&depthColorBuffer);
    glDeleteTextures(1,&brightColorBuffer);
    glDeleteTextures(2,blurColorBuffers);
    glDeleteFramebuffers(1,&mainFBO);
    glDeleteFramebuffers(1,&depthFBO);
    glDeleteFramebuffers(2,blurFBOs);
    glDeleteTextures(1,&depthMaps2DArray);
    glDeleteTextures(1,&depthCubeArray);
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
    DestroyBuffers();
}