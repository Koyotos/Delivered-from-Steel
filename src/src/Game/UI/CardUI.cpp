#include "include/Game/UI/CardUI.hpp"

void CardUI::Draw(shared_ptr<Shader> sh) {
	UIElement::Draw();
}

CardUI::CardUI(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {}
