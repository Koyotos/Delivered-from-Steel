#ifndef FE_TOOLTIP
#define FE_TOOLTIP

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/TextUI.hpp"
#include "include/Game/UI/Icon.hpp"

class Tooltip : public UIElement
{
	private:

	float activationTime;
	float deactivationTime;
	float counter = 0.0f;
	string name;
	bool activated;

	void ShowTooltip();

	public:

	Tooltip(const std::unordered_map<std::string, std::any>& data);
	void Activate();
	void HideTooltip();
	void Process() override;
};

#endif
