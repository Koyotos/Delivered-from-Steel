#ifndef FE_TEXT_UI
#define FE_TEXT_UI
#include "include/Game/UI/UIElement.hpp"
#include "include/Renderer/TextNode.hpp"

/*
 * @brief UI wrapper for TextNode. Inherits UIElement for visibility,
 * alpha, tint and tweening support while delegating text rendering to
 * an owned TextNode instance.
 */
class TextUI : public UIElement {
private:
    TextNode textNode;

public:
    string Type() override;

    void SetContent(const string& content);
    void SetColor(const vec3& color);
    void SetTextPos(const ivec2& pos);
    void SetTextScale(const float& scale);
    void SetTextSize(const ivec2& size);

    void Draw(shared_ptr<Shader> sh = nullptr) override;

    TextUI();
    TextUI(const unordered_map<string, std::any>& data);
};
#endif