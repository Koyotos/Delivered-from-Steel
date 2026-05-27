#ifndef FE_ICON
#define FE_ICON

#include "include/Game/UI/UIElement.hpp"
#include <string>

/*
* @brief This class represents an icon in the UI.
*/
class Icon : public UIElement {

	private:

	bool isAnimated;
	string name;


	public:

	Icon();
	Icon(const std::unordered_map<std::string, std::any>& data);

	void Draw(shared_ptr<Shader> sh = nullptr) override;

	string GetName() const;

};

#endif