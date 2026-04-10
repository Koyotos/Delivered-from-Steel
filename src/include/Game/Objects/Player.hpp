#ifndef FE_PLAYER
#define FE_PLAYER

#include "include/Core/Object2D.hpp"
#include "include/IOManager/InputEvent.hpp"
#include "include/Renderer/Camera.hpp"

class Player : public Object2D {
	private:
	shared_ptr<Camera> camera;
	
	public:

	void SetCamera(shared_ptr<Camera>);

	Player();
	Player(const unordered_map<string, std::any>&);
	void Process() override;
	bool Input(InputEvent& event) override;
};

#endif