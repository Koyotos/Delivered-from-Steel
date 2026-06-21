#include "include/Game/UI/Tooltip.hpp"

#include "include/AudioManager/AudioManager.hpp"
#include "include/Globals/Globals.hpp"

Tooltip::Tooltip(const std::unordered_map<std::string, std::any>& data) : UIElement(data)
{
 	activated = false;
	deactivated = fromMap(bool, "deactivated", data);
	activationTime = fromMap(float, "activationTime", data);
	deactivationTime = fromMap(float, "deactivationTime", data);
	name = fromMap(string, "name", data);
}

void Tooltip::Activate()
{
	activated = true;
}

void Tooltip::Deactivate()
{
	deactivated = true;
}

void Tooltip::Process()
{
	UIElement::Process();
	if (!activated) return;
	counter += Globals::GetGlobals().GetDeltaTime();
	if (counter >= activationTime)
	{
		if (!isShown) ShowTooltip();
	} 
	if (counter >= (deactivationTime + activationTime) && deactivated)
	{
		HideTooltip();
	}
}

void Tooltip::OnCollisionEnter(Collider* other)
{
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (!owner) return;
	if (owner->GetObjectType() == ObjectType::Player)
	{
		Activate();
	}
}

void Tooltip::OnCollisionExit(Collider* other)
{
	shared_ptr<PhysicsNode> owner = other->GetOwner();
	if (!owner) return;
	if (owner->GetObjectType() == ObjectType::Player)
	{
		Deactivate();
		this->SetPhysics(false);
	}
}


void Tooltip::ShowTooltip()
{
	this->SetVisible(true);
	this->FadeIn(0.3f, EaseType::OutSine);
	Globals::GetGlobals().audioManager->PlaySound2D("tooltip");
	isShown = true;
}

void Tooltip::HideTooltip()
{
	this->FadeOut(0.1f, EaseType::InSine);
}

string Tooltip::Type()
{
	return "Tooltip";
}

void Tooltip::Draw(shared_ptr<Shader> sh)
{
	UIElement::Draw(sh);
}

string Tooltip::GetName()
{
	return name;
}
