#ifndef FE_ICON
#define FE_ICON

#include "include/Game/UI/UIElement.hpp"

/*
* @brief This class represents an icon in the UI.
*/
class Icon : public UIElement {

	private:

	bool isAnimated;


	public:

	Icon();
	Icon(const std::unordered_map<std::string, std::any>& data);

	void Draw() override;

};

#endif