#ifndef FE_TRANSITION
#define FE_TRANSITION

#include "include/Game/UI/UIElement.hpp"


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
	string Type() override;
	void Draw(shared_ptr<Shader> sh = nullptr) override;



};

#endif
