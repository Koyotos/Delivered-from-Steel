#pragma once

#include <memory>
#include <vector>
#include <array>

#include "include/PhysicsManager/AABB.hpp"

class PhysicsNode;

class QuadTree
{
private:

    static constexpr int MAX_OBJECTS = 4;
    static constexpr int MAX_LEVELS = 5;

    int level;

    AABB bounds;

    std::vector<std::shared_ptr<PhysicsNode>> objects;

    std::array<std::unique_ptr<QuadTree>, 4> children;

public:

    QuadTree(int level, const AABB& bounds);

    void Clear();

    void Insert(std::shared_ptr<PhysicsNode> node);

    void Query(const AABB& area, std::vector<std::shared_ptr<PhysicsNode>>& result);

    QuadTree() = default;

private:

    void Split();

    int GetIndex(const AABB& rect);
};
