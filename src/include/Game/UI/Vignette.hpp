#ifndef FE_VIGNETTE
#define FE_VIGNETTE

#include "include/Game/UI/UIElement.hpp"

/*
* @brief This class represents a vignette effect in the UI.
* It changes its sprite based on a current value that can be set from code.
*/
class Vignette : public UIElement {

	private:

	int states;
	float currentValue;
	float minValue;
	float maxValue;

	/*
	* @brief Helper function to set the active texture of the sprite based on currentValue. 
	* It should be called only inside Draw() function.
	*/
	void SetCurrentSprite();

	public:
	
	Vignette();
	Vignette(const std::unordered_map<std::string, std::any>& data);

	/*
	* @brief Sets the current value of the vignette. 
	* This value will determine which sprite is active based on the number of states and the min/max values.
	*/
	void SetCurrentValue(float value);

	/*
	* @brief Gets the current value of the vignette.
	* @return float - Current value.
	*/
	float GetCurrentValue();

	/*
	* @brief Sets the minimum value of the vignette. 
	*/
	void SetMinValue(float value);

	/*
	* @brief Gets the minimum value of the vignette.
	* @return float - Minimum value.
	*/
	float GetMinValue();

	/*
	* @brief Sets the maximum value of the vignette.
	*/
	void SetMaxValue(float value);

	/*
	* @brief Gets the maximum value of the vignette.
	* @return float - Maximum value.
	*/
	float GetMaxValue();

	void Draw(shared_ptr<Shader> sh = nullptr) override;

};

#endif