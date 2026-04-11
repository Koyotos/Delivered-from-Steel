#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"
#include "include/Profiler/ProfilerNode.hpp"

void Game::Init() {
    engine.Init();
    gameScene = engine.LoadScene("res/scenes/base.json");
    gameScene->GetRoot()->AddChild(engine.LoadScene("res/scenes/test3d.json")->GetRoot());
    gameScene->GetRoot()->AddChild(engine.LoadScene("res/scenes/test2d.json")->GetRoot());

	shared_ptr<ProfilerNode> profiler = make_shared<ProfilerNode>();

	const string vertCode2d = "#version 460 \n layout (location = 0) in vec4 vertex; \n uniform mat4 M; \n uniform mat4 VP; \n out vec2 TexCoords; \n void main() { \n TexCoords = vertex.zw; \n gl_Position = VP * M * vec4(vertex.xy, 0.0, 1.0);}";
	const string fragmentShaderText = "#version 460 \n in vec2 TexCoords; \n uniform sampler2D spriteTexture; \n uniform vec3 color; \n out vec4 FragColor; \n void main(){ \n vec4 sampled = vec4(1.0, 1.0, 1.0, texture(spriteTexture, TexCoords).r); \n FragColor = vec4(color, 1.0) * sampled;}";

	shared_ptr<Shader> textShader = make_shared<Shader>(vertCode2d, "", "", fragmentShaderText, "TextShader");

	profiler->SetShader(textShader);
	profiler->SetColor(vec3(1.0f, 0.55f, 0.0f));
	profiler->SetPos(ivec2(20, 900));
	profiler->SetSize(ivec2(0, 20));
	profiler->SetScale(1.0f);

	gameScene->GetRoot()->AddChild(profiler);

    engine.SetActiveScene(gameScene);
}

void Game::Start() {
    engine.Run();
}