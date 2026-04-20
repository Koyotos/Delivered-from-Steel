#include "include/Game/UI/Card.hpp"

void Card::Draw() {
	Object2D::Draw();
}

Card::Card(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {}
