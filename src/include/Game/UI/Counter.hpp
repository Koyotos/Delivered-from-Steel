#ifndef FE_COUNTER
#define FE_COUNTER

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/TextUI.hpp"
#include "include/Game/UI/Icon.hpp"

class Counter : public UIElement {

	private:
	std::shared_ptr<TextUI> text;
	std::shared_ptr<Icon> icon;
	std::shared_ptr<Icon> manaIcon;
	int maxVal;
	int currentVal;

	public:
	Counter(shared_ptr<TextUI> counter, shared_ptr<Icon> icon, int maxVal);
	~Counter();

	void SetText(shared_ptr<TextUI> newText);
	void SetIcon(shared_ptr<Icon> newIcon);
	void SetManaIcon(shared_ptr<Icon> newIcon);
	void UpdateValue(int newVal);

	void FinishAllTweens();
	void FadeOut(float time, EaseType ease = EaseType::Linear, float delay = 0.0f);
	void FadeIn(float time, EaseType ease = EaseType::Linear, float delay = 0.0f);
};

#endif
