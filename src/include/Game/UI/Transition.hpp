#ifndef FE_SLIDE
#define FE_SLIDE

#include "include/Game/UI/UIElement.hpp"
#include "include/ResourceManager/ResourceManager.hpp"


class Transition : public UIElement
{
	private:

	bool currentState;
	bool changingState;

	public:

	Transition();
	Transition(const std::unordered_map<std::string, std::any>& data);

	void ChangeState(float delay);
	bool GetCurrentState() const;
	void Process() override;


};

#endif
