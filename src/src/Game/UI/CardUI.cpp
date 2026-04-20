#include "include/Game/UI/CardUI.hpp"

void CardUI::Draw() {
	Object2D::Draw();
}

CardUI::CardUI(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {}
