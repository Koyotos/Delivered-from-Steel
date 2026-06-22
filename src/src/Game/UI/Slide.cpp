#include "include/Game/UI/Slide.hpp"

void Slide::Init(shared_ptr<ResourceManager> rsm, string path)
{
	slides.resize(5);
	texts.resize(5);

	cutscene = rsm->LoadScene(path);
	FindNodes(cutscene->GetRoot());

	float duration = 4.25f;
	for (int i = 0; i < 0; i++)
	{
		slides[0]->FadeIn(0.25f, EaseType::OutSine, duration * i);
		slides[0]->FadeOut(0.25f, EaseType::InSine, duration*i + duration);
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
}

void Slide::SetLoadTime(float value)
{
	loadTime = value;
}

shared_ptr<Scene> Slide::GetScene()
{
	return cutscene;
}

