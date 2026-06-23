#ifndef FE_COUNTER
#define FE_COUNTER

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/TextUI.hpp"
#include "include/Game/UI/Icon.hpp"

class Counter : public UIElement {

	private:
	std::shared_ptr<TextUI> text;
	std::shared_ptr<Icon> icon;
	std::shared_ptr<Icon> infinityIcon;
	std::vector<std::shared_ptr<Icon>> manaIcons;
	float origTextPosX;
	int maxVal;
	int currentVal;
	bool valueChanged = false;
	bool learning = false;

	void UpdateManaIcons();
	void UpdateManaIconsValue();

	public:
	Counter(shared_ptr<TextUI> counter, shared_ptr<Icon> icon, int maxVal);
	~Counter();

	void SetText(shared_ptr<TextUI> newText);
	void SetIcon(shared_ptr<Icon> newIcon);
	void SetInfinityIcon(shared_ptr<Icon> newIcon);
	void AddManaIcon(shared_ptr<Icon> newIcon);
	void UpdateValue(int newVal);
	void UpdateMaxVal(int newVal);
	void IsLearning(bool value);


	void FinishAllTweens();
	void FadeOut(float time, EaseType ease = EaseType::Linear, float delay = 0.0f);
	void FadeIn(float time, EaseType ease = EaseType::Linear, float delay = 0.0f);
};

#endif
