#include "include/Game/UI/Slide.hpp"
#include "include/Globals/Globals.hpp"

Slide::Slide()
{
	SetProcess(true);
	SetDraw(false);
}

void Slide::Init(shared_ptr<ResourceManager> rsm, string path)
{
	// zmienic pozniej ten resize
	slides.resize(5);
	texts.resize(5);

	cutscene = rsm->LoadScene(path);
	FindNodes(cutscene->GetRoot());

	float duration = 5.25f;
	for (int i = 0; i < slides.size(); i++)
	{
		slides[i]->FadeIn(0.25f, EaseType::OutSine, duration * i);
		slides[i]->FadeOut(0.25f, EaseType::InSine, duration*i + duration);
	}

	SetLoadTime(duration * slides.size());

}

void Slide::FindNodes(shared_ptr<Node> node)
{
	if (!node) return;

	if (node->Type() == "Icon") {
		shared_ptr<Icon> cast = static_pointer_cast<Icon>(node);
		if (cast->GetName() == "slide0") {
			slides[0] = cast;
		}
		else if (cast->GetName() == "slide1") {
			slides[1] = cast;
		}
		else if (cast->GetName() == "slide2") {
			slides[2] = cast;
		}
		else if (cast->GetName() == "slide3") {
			slides[3] = cast;
		}
		else if (cast->GetName() == "slide4") {
			slides[4] = cast;
		}

	}
	if (node->Type() == "TextUI")
	{
		// logika dla koncowej cutscenki
	}

	for (auto& k : node->GetChildren()) {
		FindNodes(k);
	}
}

void Slide::SetLoadTime(float value)
{
	loadTime = value;
}

void Slide::SetOnEndScene(function<void()> cb)
{
	onEndScene = cb;
}

shared_ptr<Scene> Slide::GetScene()
{
	return cutscene;
}

void Slide::Process()
{
	float dt = Globals::GetGlobals().GetDeltaTime();
	loadTime -= dt;
	if (loadTime < 0)
	{
		onEndScene();
	}
}

