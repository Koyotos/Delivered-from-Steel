#include "include/Game/UI/Card.hpp"

void Card::Draw() {
	UIElement::UpdateTransform();
	Object2D::Draw();
}