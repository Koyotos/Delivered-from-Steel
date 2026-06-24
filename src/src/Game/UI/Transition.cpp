#include "include/Game/UI/Transition.hpp"

Transition::Transition(const std::unordered_map<std::string, std::any>& data) : UIElement(data)
{
	currentState = fromMap(bool, "beginState", data);
	changingState = false;
	if (!currentState) ChangeState(0.3f);

}

void Transition::ChangeState(float delay)
{
	if (changingState) return;
	changingState = true;
	ClearAllTweens();
	vec2 targetPos;
	if (currentState) targetPos = vec2(0.0f, 0.0f);
	else targetPos = vec2(0.0f, -1080.0f);
	this->MoveTo(targetPos, 0.8f, EaseType::InQuad, delay);
}

void Transition::Process()
{
	UIElement::Process();

	if (!changingState) return;
	if (GetActiveTweens().empty())
	{
		changingState = false;
		currentState = !currentState;
	}
}

bool Transition::GetCurrentState() const
{
	return currentState;
}

string Transition::Type()
{
	return "Transition";
}

void Transition::Draw(shared_ptr<Shader> sh)
{
	UIElement::Draw(sh);
}
