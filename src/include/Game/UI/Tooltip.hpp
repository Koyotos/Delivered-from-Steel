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
	bool deactivated;
	bool isShown = false;

	float proximityRadius;
	vec2 pos;

	void ShowTooltip();
	void HideTooltip();


	public:

	Tooltip();
	Tooltip(const std::unordered_map<std::string, std::any>& data);
	void Draw(shared_ptr<Shader> sh = nullptr) override;
	void Activate();
	void Deactivate();
	void Process() override;
	string Type() override;

	string GetName();

	void OnCollisionEnter(Collider* other) override;
	void OnCollisionExit(Collider* other) override;
};

#endif
