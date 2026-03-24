#include "include/SceneManager/SceneManager.hpp"
#include "include/Core/Object2D.hpp"
#include "include/Core/Object3D.hpp"
#include "include/Core/Transform.hpp"
#include "include/Renderer/Sprite.hpp"
#include <fstream>

void SceneManager::AddScene(shared_ptr<Scene> scn) {
    scenes.push_back(scn);
} 

shared_ptr<Scene> SceneManager::GetActive() {
    return scenes[activeIndex];
}

void SceneManager::SetActive(shared_ptr<Scene> scn) {
    for(uint16_t i = 0; i < scenes.size(); i++) {
        if(scn == scenes[i]) {
            activeIndex = i;
            return;
        }
    }
}

void SceneManager::SetActive(const uint16_t& idx) {
    if(idx < scenes.size() && idx>=0) {
        activeIndex = idx;
    }
}

void SceneManager::TestScene() {
    shared_ptr<Scene> scene = make_shared<Scene>();
    shared_ptr<Model> mdl = make_shared<Model>("res/models/house/house.obj");
    Transform testT;
    testT.SetTranslation(vec3(0,-1,-5));
    testT.SetRotation(vec3(0,0.1,0));
    testT.SetScale(vec3(1,1,1));
    shared_ptr<Object3D> test = make_shared<Object3D>();
    test->SetTransform(testT);

    const string vertCode = "#version 420 \n layout(location = 0) in vec3 vertexPosition; \n layout(location = 1) in vec3 normals; \n \
    layout(location = 2) in vec2 texCoords; \n out vec2 TexCoord; \n uniform mat4 M; \n uniform mat4 VP; \n \
    void main() { \n gl_Position =  VP * M * vec4(vertexPosition,1); \n  TexCoord = texCoords;}";

    const string fragmentShader = "#version 420 \n in vec2 TexCoord; \n out vec4 FragColor; \n uniform sampler2D txt; \n \
    void main(){ \n FragColor = texture(txt, TexCoord);}";

    shared_ptr<Shader> sh = make_shared<Shader>(vertCode,"","",fragmentShader,"test");

    test->SetModel(mdl);
    test->SetShader(sh);
    test->SetDraw(true);

    shared_ptr<Sprite> testSpr = make_shared<Sprite>("res/sprites/test");
    shared_ptr<Object2D> test2d = make_shared<Object2D>();
    test2d->SetSprite(testSpr);

    const string vertCode2d = "#version 420 \n layout (location = 0) in vec2 aPos; \n layout (location = 1) in vec2 aUV; \n \
    uniform mat4 M; \n uniform mat4 VP; \n out vec2 TexCoords; \n void main() { \n \
    TexCoords = aUV; \n gl_Position = VP * M * vec4(aPos, 0.0, 1.0);}";

    const string fragmentShader2d = "#version 420 \n in vec2 TexCoords; \n uniform sampler2D spriteTexture; \n out vec4 FragColor; \n \
    void main(){ \n FragColor = texture(spriteTexture, TexCoords);}";

    shared_ptr<Shader> sh2d = make_shared<Shader>(vertCode2d,"","",fragmentShader2d,"test2");

    testT = Transform();
    testT.SetTranslation(vec3(500,500,0));
    testT.SetRotation(vec3(0,0,0));
    testT.SetScale(vec3(1,1,1));

    test2d->SetShader(sh2d);
    test2d->SetTransform(testT);
    test->AddChild(test2d);
    test2d->SetDraw(true);

    scene->SetRoot(test);
    scenes.push_back(scene);
    activeIndex = 0;
}