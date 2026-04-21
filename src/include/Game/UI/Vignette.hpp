#ifndef FE_VIGNETTE
#define FE_VIGNETTE

#include "include/Game/UI/UIElement.hpp"

class Vignette : public UIElement {

	private:

	int states;
	float currentValue;
	float minValue;
	float maxValue;

	void SetCurrentSprite();

	public:
	
	Vignette();
	Vignette(const std::unordered_map<std::string, std::any>& data);

	void SetCurrentValue(float value);
	float GetCurrentValue();

	void SetMinValue(float value);
	float GetMinValue();

	void SetMaxValue(float value);
	float GetMaxValue();

	void Draw() override;

};

#endif