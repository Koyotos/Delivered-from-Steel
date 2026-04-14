#include "include/Game/UI/Vignette.hpp"


Vignette::Vignette(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
}

void Vignette::Draw() {
	UIElement::UpdateTransform();
	Object2D::Draw();
}
