#include "include/SceneManager/SceneManager.hpp"
#include <fstream>

shared_ptr<Scene> SceneManager::GetActive() {
    return scenes[activeIndex];
}

void SceneManager::TestScene() {
    shared_ptr<Scene> scene = make_shared<Scene>();
    shared_ptr<Model> mdl = make_shared<Model>("res/house/house.obj");
    Transform testT;
    testT.SetTranslation(vec3(0,-1,-5));
    testT.SetRotation(vec3(0,0.1,0));
    testT.SetScale(vec3(1,1,1));
    shared_ptr<VisualNode> test = make_shared<VisualNode>();
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
    scene->SetRoot(test);
    scenes.push_back(scene);
    activeIndex = 0;
}