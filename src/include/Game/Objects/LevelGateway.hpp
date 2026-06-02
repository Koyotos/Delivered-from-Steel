#ifndef FE_LEVEL_GATEWAY
#define FE_LEVEL_GATEWAY

#include "include/PhysicsManager/PhysicsNode.hpp"
#include <string>

class LevelGateway final : public PhysicsNode {
private:
	bool hasTriggered = false;

public:
	std::string targetLevel = "";

	LevelGateway();
	LevelGateway(const unordered_map<string, std::any>& data);

	string Type() override;

	void OnCollisionEnter(shared_ptr<Collider> other) override;
	void OnCollisionExit(shared_ptr<Collider> other) override;
};

#endif