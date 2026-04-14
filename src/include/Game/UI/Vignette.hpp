#ifndef FE_VIGNETTE
#define FE_VIGNETTE

#include "include/Game/UI/UIElement.hpp"

class Vignette : public UIElement {

	private:

	int states;
	float currentValue;

	public:

	Vignette();
	Vignette(const std::unordered_map<std::string, std::any>& data);

	void Draw() override;

};

#endif