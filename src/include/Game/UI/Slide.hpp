#ifndef FE_SLIDE
#define FE_SLIDE

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/TextUI.hpp"

class Slide : public UIElement
{
	private:


	shared_ptr<TextUI> text;


	public:

	Slide();
	Slide(const std::unordered_map<std::string, std::any>& data);

};

#endif
