#include "include/Game/Game.hpp"
#include "include/Game/Objects/Player.hpp"

void Game::Init() {
    engine.Init();
    gameScene = engine.LoadScene("res/scenes/base.json");
    gameScene->GetRoot()->AddChild(engine.LoadScene("res/scenes/test3d.json")->GetRoot());

	shared_ptr<Player> player = make_shared<Player>();
	shared_ptr<Sprite> playerSprite = make_shared<Sprite>("res/sprites/test");
	player->SetSprite(playerSprite);

	const string vertCode2d = "#version 420 \n layout (location = 0) in vec2 aPos; \n layout (location = 1) in vec2 aUV; \n uniform mat4 M; \n uniform mat4 VP; \n out vec2 TexCoords; \n void main() { \n TexCoords = aUV; \n gl_Position = VP * M * vec4(aPos, 0.0, 1.0);}";
	const string fragmentShader2d = "#version 420 \n in vec2 TexCoords; \n uniform sampler2D spriteTexture; \n out vec4 FragColor; \n void main(){ \n FragColor = texture(spriteTexture, TexCoords);}";
	shared_ptr<Shader> playerShader = make_shared<Shader>(vertCode2d, "", "", fragmentShader2d, "playerShader");
	player->SetShader(playerShader);

	Transform startTransform;
	startTransform.SetTranslation(vec3(960.0f, 540.0f, 0.0f));
	startTransform.SetScale(vec3(1.0f, 1.0f, 1.0f));
	player->SetTransform(startTransform);

	gameScene->GetRoot()->AddChild(player);

    engine.SetActiveScene(gameScene);
}

void Game::Start() {
    engine.Run();
}