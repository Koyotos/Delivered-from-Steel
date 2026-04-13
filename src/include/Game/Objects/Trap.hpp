#ifndef FE_TRAP
#define FE_TRAP

#include "include/Core/Object3D.hpp"
class Trap :
    public Object3D
{
private:
    float damage = 100.0f;

public:
    void OnCollisionEnter(shared_ptr<Collider> other) override;

    Trap(const unordered_map<string, std::any>&);
};

#endif // FE_TRAP
