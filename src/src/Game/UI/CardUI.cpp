#include "include/Game/UI/CardUI.hpp"

void CardUI::Draw(shared_ptr<Shader> sh) {
	UIElement::Draw();
}

CardUI::CardUI(const std::unordered_map<std::string, std::any>& data) : UIElement(data) {
	string typeStr = fromMap(string, "cardType", data);
	if (typeStr == "WallJump") {
		type = CardType::WallJump;
	} else if (typeStr == "DoubleJump") {
		type = CardType::DoubleJump;
	} else if (typeStr == "Dash") {
		type = CardType::Dash;
	}
	else if (typeStr == "Bounce") {
		type = CardType::Bounce;
	}
	else if (typeStr == "WallSnap") {
		type = CardType::WallSnap;
	} else if (typeStr == "FeatherFalling") {
		type = CardType::FeatherFalling;
	} else {
		throw runtime_error("Invalid card type: " + typeStr);
	}
}

CardType CardUI::GetCardType() {
	return type;
}

string CardUI::Type() {
	return "CardUI";
}
