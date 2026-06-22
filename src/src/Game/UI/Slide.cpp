#include "include/Game/UI/Slide.hpp"

Slide::Slide(const std::unordered_map<std::string, std::any>& data) : UIElement(data)
{
	float duration = fromMap(float, "duration", data);
	float delay = fromMap(float, "delay", data);
	
	this->FadeIn(0.25f, EaseType::OutSine, delay);
	this->FadeIn(0.25f, EaseType::InSine, (duration + delay));


}
