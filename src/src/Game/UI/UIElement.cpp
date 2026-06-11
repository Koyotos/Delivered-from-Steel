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

		if (tween.elapsed < tween.delay) continue;

		float localElapsed = tween.elapsed - tween.delay;
		float t = localElapsed / tween.duration;
		if(t >= 1.0f) {
			t = 1.0f;
			tween.finished = true;

			if (tween.type == Tween::Type::Fade && tween.targetAlpha == 0.0f) {
				// SetVisible(false);
			}
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
			case Tween::Type::Rotate:
				SetTransform(Transform(
					GetTransform().GetTranslation(),
					glm::vec3(0.0f, 0.0f, glm::mix(tween.startAngle, tween.targetAngle, easedT)),
					GetTransform().GetScale()
				));
				break;

			case Tween::Type::Scale:
				SetTransform(Transform(
					GetTransform().GetTranslation(),
					GetTransform().GetRotation(),
					glm::vec3(glm::mix(tween.startScale, tween.targetScale, easedT), 1.0f)
				));
				break;

			case Tween::Type::Shake: {
				float remaining = 1.0f - (tween.elapsed / tween.duration);
				float offsetX = ((float)(rand() % 200 - 100) / 100.0f) * tween.intensity * remaining;
				float offsetY = ((float)(rand() % 200 - 100) / 100.0f) * tween.intensity * remaining;
				SetTransform(Transform(
					glm::vec3(tween.shakeOrigin.x + offsetX, tween.shakeOrigin.y + offsetY, 0.0f),
					GetTransform().GetRotation(),
					GetTransform().GetScale()
				));
				break;
			}
		}
	}

	tweens.erase(
		std::remove_if(tweens.begin(), tweens.end(),
			[](const Tween& t) { return t.finished; }),
		tweens.end()
	);
}

void UIElement::MoveTo(glm::vec2 target, float time, EaseType ease, float delay, vec2 overrideStartPos)
{
	Tween t;
	t.type = Tween::Type::Move;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.delay = delay;

	if (overrideStartPos != vec2(0.0f, 0.0f)) t.startPos = overrideStartPos;
	else t.startPos = glm::vec2(GetTransform().GetGlobal()[3].x, GetTransform().GetGlobal()[3].y); // get global position
	t.targetPos = target;

	tweens.push_back(t);
}

void UIElement::FadeIn(float time, EaseType ease, float delay)
{
	Tween t;
	t.type = Tween::Type::Fade;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.startAlpha = 0.0f;
	t.targetAlpha = 1.0f;
	t.delay = delay;
	tweens.push_back(t);
}

void UIElement::FadeOut(float time, EaseType ease, float delay)
{
	Tween t;
	t.type = Tween::Type::Fade;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.startAlpha = alpha;
	t.targetAlpha = 0.0f;
	t.delay = delay;
	tweens.push_back(t);
}

void UIElement::Tint(glm::vec3 color, float time, EaseType ease, float delay)
{
	Tween t;
	t.type = Tween::Type::Tint;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.startTint = tint;
	t.targetTint = color;
	t.delay = delay;
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
	case EaseType::OutElastic: {
		float c4 = (2.0f * 3.14159f) / 3.0f;
		if (t == 0.f) return 0.f;
		if (t == 1.f) return 1.f;
		return powf(2.f, -10.f * t) * sinf((t * 10.f - 0.75f) * c4) + 1.f;
	}
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
	Object2D::Process();
	UpdateTweens(Globals::GetGlobals().GetDeltaTime());
}

void UIElement::Draw(shared_ptr<Shader> sh) {
	if (!isVisible) return;
	shader->SetMat4("VP", glm::ortho(0.0f, 1920.0f, 1080.0f, 0.0f, -1.0f, 1.0f));
	shader->SetFloat("alpha", alpha);
	shader->SetVec3("tint", tint);

	Object2D::Draw();
}

void UIElement::FinishAllTweens() {
	for (auto& tween : tweens) {
		switch (tween.type) {
		case Tween::Type::Move:
			SetTransform(Transform(glm::vec3(tween.targetPos, 0.0f), GetTransform().GetRotation(), GetTransform().GetScale()));
			break;
		case Tween::Type::Fade:
			SetAlpha(tween.targetAlpha);
			break;
		case Tween::Type::Tint:
			SetTint(tween.targetTint);
			break;
		case Tween::Type::Rotate:
			SetTransform(Transform(GetTransform().GetTranslation(), glm::vec3(0.0f, 0.0f, tween.targetAngle), GetTransform().GetScale()));
			break;
		case Tween::Type::Scale:
			SetTransform(Transform(GetTransform().GetTranslation(), GetTransform().GetRotation(), glm::vec3(tween.targetScale, 1.0f)));
			break;
		case Tween::Type::Shake:
			SetTransform(Transform(glm::vec3(tween.shakeOrigin, 0.0f), GetTransform().GetRotation(), GetTransform().GetScale()));
			break;
		}
	}
	tweens.clear();
}

void UIElement::ClearAllTweens()
{
	tweens.clear();
}

vector<shared_ptr<Tween>> UIElement::GetActiveTweens() const {
	vector<shared_ptr<Tween>> activeTweens;
	for (auto& tween : tweens) {
		if (!tween.finished) {
			activeTweens.push_back(make_shared<Tween>(tween));
		}
	}
	return activeTweens;
}

void UIElement::RotateTo(float targetAngle, float time, EaseType ease, float delay)
{
	Tween t;
	t.type = Tween::Type::Rotate;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.delay = delay;
	t.startAngle = GetTransform().GetRotation().z;
	t.targetAngle = targetAngle;
	tweens.push_back(t);
}

void UIElement::ScaleTo(glm::vec2 targetScale, float time, EaseType ease, float delay, glm::vec2 overrideStart)
{
	Tween t;
	t.type = Tween::Type::Scale;
	t.duration = time;
	t.elapsed = 0.0f;
	t.ease = ease;
	t.delay = delay;
	t.startScale = (overrideStart.x < 0.f)
		? glm::vec2(GetTransform().GetScale())
		: overrideStart;
	t.targetScale = targetScale;
	tweens.push_back(t);
}

void UIElement::Shake(float intensity, float duration, float delay)
{
	Tween t;
	t.type = Tween::Type::Shake;
	t.duration = duration;
	t.elapsed = 0.0f;
	t.ease = EaseType::Linear;
	t.delay = delay;
	t.intensity = intensity;
	t.shakeOrigin = glm::vec2(GetTransform().GetGlobal()[3].x, GetTransform().GetGlobal()[3].y);
	tweens.push_back(t);
}