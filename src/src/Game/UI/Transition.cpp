#include "include/Game/UI/Transition.hpp"

Transition::Transition(const std::unordered_map<std::string, std::any>& data)
{
	bool currentState = fromMap(bool, "beginState", data);

	if (!currentState) ChangeState(0.5f);
}

void Transition::ChangeState(float delay)
{
	changingState = true;
	ClearAllTweens();
	vec2 targetPos;
	if (currentState) targetPos = vec2(0.0f, 0.0f);
	else targetPos = vec2(0.0f, -1920.0f);
	MoveTo(targetPos, 0.6f, EaseType::InQuad, 0.1f);
}

void Transition::Process()
{
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
