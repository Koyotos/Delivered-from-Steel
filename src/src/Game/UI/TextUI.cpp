#include "include/Game/UI/TextUI.hpp"

string TextUI::Type() {
    return "TextUI";
}

void TextUI::SetContent(const string& content) {
    textNode.SetContent(content);
}

void TextUI::SetColor(const vec3& color) {
    textNode.SetColor(color);
}

// Named SetTextPos to avoid ambiguity with UIElement/Object2D position
void TextUI::SetTextPos(const ivec2& pos) {
    textNode.SetPos(pos);
}

void TextUI::SetTextScale(const float& scale) {
    textNode.SetScale(scale);
}

void TextUI::SetTextSize(const ivec2& size) {
    textNode.SetSize(size);
}

void TextUI::Draw(shared_ptr<Shader> sh) {
    if (!GetVisible()) return;
    // UIElement::Draw(sh); // sets VP, alpha, tint uniforms via shader
    textNode.Draw(this->shader);
}

TextUI::TextUI() : UIElement() {}

TextUI::TextUI(const unordered_map<string, std::any>& data) : UIElement(data) {
    vector<std::any> values = fromMap(vector<std::any>, "color", data);
    vec3 color;
    color.x = any_cast<float>(values[0]);
    color.y = any_cast<float>(values[1]);
    color.z = any_cast<float>(values[2]);
    textNode.SetColor(color);

    values = fromMap(vector<std::any>, "textSize", data);
    ivec2 size;
    size.x = any_cast<int64_t>(values[0]);
    size.y = any_cast<int64_t>(values[1]);
    textNode.SetSize(size);

    values = fromMap(vector<std::any>, "textPos", data);
    ivec2 pos;
    pos.x = any_cast<int64_t>(values[0]);
    pos.y = any_cast<int64_t>(values[1]);
    textNode.SetPos(pos);

    textNode.SetScale(fromMap(float, "textScale", data));
    textNode.SetContent(fromMap(string, "content", data));
}