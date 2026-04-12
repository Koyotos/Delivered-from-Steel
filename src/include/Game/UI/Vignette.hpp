#ifndef FE_VIGNETTE
#define FE_VIGNETTE

#include "include/Game/UI/UIElement.hpp"

class Vignette : public UIElement {

	private:

	int states;
	float currentValue;

	public:

	void Draw() override;

};

#endif