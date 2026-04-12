#ifndef FE_UI_ELEMENT
#define FE_UI_ELEMENT

#include "include/Core/Object2D.hpp"

enum class EaseType
{
	Linear,

	InSine,
	OutSine,
	InOutSine,

	InQuad,
	OutQuad,
	InOutQuad,

};

class UIElement : public Object2D {

	private:

	bool isVisible;
	float alpha;

	void UpdateTransform();

	public:

	UIElement();

	void SetVisible(bool value);
	bool GetVisible() const;
	void SetAlpha(float value);
	float GetAlpha() const;

	void FadeIn(float time, EaseType ease = EaseType::Linear);
	void FadeOut(float time, EaseType ease = EaseType::Linear);
	void MoveTo(glm::vec2 target, float time, EaseType ease = EaseType::Linear);


};

#endif