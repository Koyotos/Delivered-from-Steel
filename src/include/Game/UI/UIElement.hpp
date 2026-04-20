#ifndef FE_UI_ELEMENT
#define FE_UI_ELEMENT

#include "include/Core/Object2D.hpp"
#include "include/Game/UI/Tween.hpp"

class UIElement : public Object2D {

	private:

	bool isVisible;
	float alpha;
	glm::vec3 tint;
	std::vector<Tween> tweens; // actvie tweens

	float Ease(EaseType ease, float t);
	void UpdateTweens(float dt);

	public:

	UIElement();
	UIElement(const std::unordered_map<std::string, std::any>& data);

	void SetVisible(bool value);
	bool GetVisible() const;
	void SetAlpha(float value);
	float GetAlpha() const;
	void SetTint(glm::vec3 color);
	glm::vec3 GetTint() const;


	void FadeIn(float time, EaseType ease = EaseType::Linear);
	void FadeOut(float time, EaseType ease = EaseType::Linear);
	void MoveTo(glm::vec2 target, float time, EaseType ease = EaseType::Linear);
	void Tint(glm::vec3 color, float time, EaseType ease = EaseType::Linear);


};

#endif