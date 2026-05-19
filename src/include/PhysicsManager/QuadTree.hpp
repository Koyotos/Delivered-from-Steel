#ifndef FE_QUADTREE
#define FE_QUADTREE

#include "include/PhysicsManager/AABB.hpp"
#include "include/PhysicsManager/PhysicsNode.hpp"

#include <memory>
#include <vector>
#include <array>

class QuadTree {
    private:
    static constexpr int MAX_OBJECTS = 8;
    static constexpr int MAX_LEVELS = 5;

    int level;

    AABB bounds;

    vector<shared_ptr<PhysicsNode>> objects;
    array<unique_ptr<QuadTree>, 4> children;

    public:
    QuadTree(int level, const AABB& bounds);
    void Clear();
    void Insert(shared_ptr<PhysicsNode> node);
    void Query(const AABB& area, vector<shared_ptr<PhysicsNode>>& result);
    QuadTree() = default;

    private:
    void Split();
    int GetIndex(const AABB& rect);
};

#endif
