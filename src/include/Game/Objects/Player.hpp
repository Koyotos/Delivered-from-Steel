#ifndef FE_PLAYER
#define FE_PLAYER

#include "include/Core/Object2D.hpp"
#include "include/IOManager/InputEvent.hpp"

class Player : public Object2D {
public:
	Player();
	Player(unordered_map<string, std::any>);
	void Process() override;
	bool Input(InputEvent& event) override;
};

#endif