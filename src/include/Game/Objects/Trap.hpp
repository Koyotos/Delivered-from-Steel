#ifndef FE_TRAP
#define FE_TRAP

#include "include/Core/Object3D.hpp"
#include "include/Game/Objects/Player.hpp"
class Trap :
    public Object3D
{
private:
    float damage = 1000.0f;

public:
    void OnCollisionStay(shared_ptr<Collider> other) override;

    Trap(const unordered_map<string, std::any>&);

	void ActivateEffect(shared_ptr<Player>);
};

#endif // FE_TRAP
