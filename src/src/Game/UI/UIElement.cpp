#include "include/Game/UI/UIElement.hpp"
#include "include/Globals/Globals.hpp"


void UIElement::SetVisible(bool value) {
	isVisible = value;
}

bool UIElement::GetVisible() const {
	return isVisible;
}

void UIElement::SetAlpha(float value) {
	alpha = value;
}

float UIElement::GetAlpha() const {
	return alpha;
}

void UIElement::SetTint(glm::vec3 color) {
	tint = color;
}

glm::vec3 UIElement::GetTint() const {
	return tint;
}

void UIElement::UpdateTweens(float dt) {
	for(auto &tween : tweens) {
		if(tween.finished) continue;
		tween.elapsed += dt;
		float t = tween.elapsed / tween.duration;
		if(t >= 1.0f) {
			t = 1.0f;
			tween.finished = true;
		}
		float easedT = Ease(tween.ease, t);
		switch (tween.type) {
			case Tween::Type::Move:
				SetTransform(Transform(glm::vec3(glm::mix(tween.startPos, tween.targetPos, easedT), 0.0f), GetTransform().GetRotation(), GetTransform().GetScale()));
				break;
			case Tween::Type::Fade:
				SetAlpha(glm::mix(tween.startAlpha, tween.targetAlpha, easedT));
				break;
			case Tween::Type::Tint:
				SetTint(glm::mix(tween.startTint, tween.targetTint, easedT));
				break;
		}
	}

	tweens.erase(
		std::remove_if(tweens.begin(), tweens.end(),
			[](const Tween& t) { return t.finished; }),
		tweens.end()
	);
}

void UIElement::MoveTo(glm::vec2 target, float time, EaseType ease)
{
	Tween t;
	t.type = Tween::Type::Move;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;

	t.startPos = glm::vec2(GetTransform().GetGlobal()[3].x, GetTransform().GetGlobal()[3].y); // get global position
	t.targetPos = target;

	tweens.push_back(t);
}

void UIElement::FadeIn(float time, EaseType ease)
{
	Tween t;
	t.type = Tween::Type::Fade;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.startAlpha = alpha;
	t.targetAlpha = 1.0f;
	tweens.push_back(t);
}

void UIElement::FadeOut(float time, EaseType ease)
{
	Tween t;
	t.type = Tween::Type::Fade;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.startAlpha = alpha;
	t.targetAlpha = 0.0f;
	tweens.push_back(t);
}

void UIElement::Tint(glm::vec3 color, float time, EaseType ease)
{
	Tween t;
	t.type = Tween::Type::Tint;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.startTint = tint;
	t.targetTint = color;
	tweens.push_back(t);
}

float UIElement::Ease(EaseType ease, float t)
{
	switch (ease)
	{
	case EaseType::Linear:
		return t;
	case EaseType::InSine:
		return 1.0f - cosf(t * (3.14f / 2.0f)); // zamienic pozniej jak sobie przypomne jak na windowsie robi sie PI
	case EaseType::OutSine:
		return sinf(t * (3.14f / 2.0f));
	case EaseType::InOutSine:
		return -0.5f * (cosf(3.14f * t) - 1.0f);
	case EaseType::InQuad:
		return t * t;
	case EaseType::OutQuad:
		return t * (2.0f - t);
	case EaseType::InOutQuad:
		return t < 0.5f ? 2.0f * t * t : -1.0f + (4.0f - 2.0f * t) * t;
	default:
		return t;
	}
}

UIElement::UIElement(const std::unordered_map<std::string, std::any>& data) : Object2D(data) {
	isVisible = fromMap(bool, "visible", data);
	alpha = fromMap(float, "alpha", data);

	tint = glm::vec3(1.0f, 1.0f, 1.0f);

	SetDraw(true);

}

UIElement::UIElement() : Object2D() {
	tint = glm::vec3(1.0f, 1.0f, 1.0f);
	SetDraw(true);
}

void UIElement::Process() {
	UpdateTweens(Globals::GetGlobals().GetDeltaTime());
}

void UIElement::Draw(shared_ptr<Shader> sh) {
	if (!isVisible) return;
	shader->SetMat4("VP", glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f));
	shader->SetFloat("alpha", alpha);
	shader->SetVec3("tint", tint);

	Object2D::Draw();
}