#include "include/Game/UI/Tooltip.hpp"

#include "include/AudioManager/AudioManager.hpp"
#include "include/Globals/Globals.hpp"

Tooltip::Tooltip(const std::unordered_map<std::string, std::any>& data) : UIElement(data)
{

}

void Tooltip::Activate()
{
	activated = true;
}

void Tooltip::Process()
{
	if (!activated) return;
	counter += Globals::GetGlobals().GetDeltaTime();
	if (counter >= activationTime)
	{
		ShowTooltip();
	}
	if (deactivationTime != 0 && counter >= deactivationTime)
	{
		HideTooltip();
	}
}


void Tooltip::ShowTooltip()
{
	this->SetVisible(true);
	this->FadeIn(0.3f, EaseType::OutSine);
	Globals::GetGlobals().audioManager->PlaySound2D("tooltip");
}

void Tooltip::HideTooltip()
{
	this->FadeOut(0.3f, EaseType::InSine);
}