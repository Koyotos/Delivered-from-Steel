#ifndef FE_ICON
#define FE_ICON

#include "include/Game/UI/UIElement.hpp"

class Icon : public UIElement {

	private:

	bool isAnimated;


	public:

	void Draw() override;

};

#endif