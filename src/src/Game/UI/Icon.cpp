#include "include/Game/UI/Icon.hpp"

void Icon::Draw(shared_ptr<Shader> sh) {
	UIElement::Draw();
}


Icon::Icon(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
	isAnimated = fromMap(bool, "animated", data);
}