#ifndef FE_UI_ELEMENT
#define FE_UI_ELEMENT

#include "include/Core/Object2D.hpp"
#include "include/Game/UI/Tween.hpp"

/*
@brief Base class for every UI element.
It also allows for basic tweening of position, alpha and tint.
*/
class UIElement : public Object2D {

	private:

	bool isVisible;
	float alpha;
	glm::vec3 tint;

	/* vector of all currently active tweens. */
	std::vector<Tween> tweens; 

	/*
	* @brief Helper function to calculate eased time based on easing type.
	* @return float - eased time value between 0 and 1.
	*/
	float Ease(EaseType ease, float t);

	/*
	* @brief Helper function for updating all active tweens. It should be called only inside Process().
	* @param dt - Delta time.
	*/
	void UpdateTweens(float dt);

	public:

	/*
	* @brief Override of Process() function. It updates all active tweens.
	*/
	void Process() override;

	UIElement();
	UIElement(const std::unordered_map<std::string, std::any>& data);

	/*
	* @brief Sets visibility of the element. If false, it won't be drawn and won't process tweens.
	*/
	void SetVisible(bool value);

	/*
	* @brief Returns visibility of the element.
	* @return bool - Visibility state.
	*/
	bool GetVisible() const;

	/*
	* @brief Sets alpha transparency of the element.
	*/
	void SetAlpha(float value);

	/*
	* @brief Returns alpha transparency of the element.
	* @return float - Alpha transparency value between 0 and 1.
	*/
	float GetAlpha() const;

	/*
	* @brief Sets tint color of the element. This color will be multiplied with the sprite's color in shader.
	*/
	void SetTint(glm::vec3 color);

	/*
	* @brief Returns tint color of the element.
	* @return glm::vec3 - Tint color.
	*/
	glm::vec3 GetTint() const;

	/* Tweening functions */

	/*
	* @brief Fades in the element by tweening alpha from current value to 1 over specified time.
	* @param time - Duration of the fade in effect.
	* @param ease - Easing function to use for the tween (Linear by default).
	*/
	void FadeIn(float time, EaseType ease = EaseType::Linear, float delay = 0.0f);

	/*
	* @brief Fades out the element by tweening alpha from current value to 0 over specified time.
	* @param time - Duration of the fade out effect.
	* @param ease - Easing function to use for the tween (Linear by default).
	*/
	void FadeOut(float time, EaseType ease = EaseType::Linear, float delay = 0.0f);

	/*
	* @brief Moves the element to a target position by tweening its transform's position over specified time.
	* @param target - Target position to move to.
	* @param time - Duration of the move effect.
	* @param ease - Easing function to use for the tween (Linear by default).
	*/
	void MoveTo(glm::vec2 target, float time, EaseType ease = EaseType::Linear, float delay = 0.0f);

	/*
	* @brief Tweens the element's tint color to a target color over specified time.
	* @param color - Target tint color.
	* @param time - Duration of the tint effect.
	* @param ease - Easing function to use for the tween (Linear by default).
	*/
	void Tint(glm::vec3 color, float time, EaseType ease = EaseType::Linear, float delay = 0.0f);

	/*
	 * @brief Instantly finishes all active tweens, setting the element's properties to their target values.
	 */
	void FinishAllTweens();

	void Draw(shared_ptr<Shader> sh = nullptr) override;

};

#endif