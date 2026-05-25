#include "include/PhysicsManager/QuadTree.hpp"

QuadTree::QuadTree(int level, const AABB& bounds) : level(level), bounds(bounds) {
}

void QuadTree::Clear() {
    objects.clear();
    for (auto& child : children) {
        if (child) {
            child->Clear();
            child.reset();
        }
    }
}

void QuadTree::Split() {
    float halfWidth = (bounds.max.x - bounds.min.x) * 0.5f;
    float halfHeight = (bounds.max.y - bounds.min.y) * 0.5f;

    float x = bounds.min.x;
    float y = bounds.min.y;

    children[0] = std::make_unique<QuadTree>(
        level + 1,
        AABB{
            {x, y},
            {x + halfWidth, y + halfHeight}
        });

    children[1] = std::make_unique<QuadTree>(
        level + 1,
        AABB{
            {x + halfWidth, y},
            {x + halfWidth * 2, y + halfHeight}
        });

    children[2] = std::make_unique<QuadTree>(
        level + 1,
        AABB{
            {x, y + halfHeight},
            {x + halfWidth, y + halfHeight * 2}
        });

    children[3] = std::make_unique<QuadTree>(
        level + 1,
        AABB{
            {x + halfWidth, y + halfHeight},
            {x + halfWidth * 2, y + halfHeight * 2}
        });
}

int QuadTree::GetIndex(const AABB& rect) {
    float verticalMid = bounds.min.x + (bounds.max.x - bounds.min.x) * 0.5f;
    float horizontalMid = bounds.min.y + (bounds.max.y - bounds.min.y) * 0.5f;

    bool left = rect.max.x < verticalMid;
    bool right = rect.min.x > verticalMid;
    bool bottom = rect.max.y < horizontalMid;
    bool top = rect.min.y > horizontalMid;

    if (left) {
        if (bottom) return 0;
        if (top) return 2;
    }
    else if (right) {
        if (bottom) return 1;
        if (top) return 3;
    }

    return -1;
}

void QuadTree::Insert(PhysicsNode* node) {
    auto collider = node->GetCollider();

    if (!collider)
        return;

    AABB rect = collider->GetBounds();

    if (children[0]) {
        int index = GetIndex(rect);

        if (index != -1) {
            children[index]->Insert(node);
            return;
        }
    }

    objects.push_back(node);

    if(objects.size() > MAX_OBJECTS && level < MAX_LEVELS) {
        if (!children[0])
            Split();

        size_t i = 0;

        while (i < objects.size()) {
            auto objCollider =
                objects[i]->GetCollider();

            if(!objCollider) {
                i++;
                continue;
            }

            int index = GetIndex(objCollider->GetBounds());

            if(index != -1) {
                children[index]->Insert(objects[i]);

                objects.erase(objects.begin() + i);
            }
            else {
                i++;
            }
        }
    }
}

void QuadTree::Query(const AABB& area, std::vector<PhysicsNode*>& result) {
    if(!Intersects(bounds, area))
        return;

    for(PhysicsNode* object : objects) {
        auto collider = object->GetCollider();

        if(!collider)
            continue;

        if(Intersects(collider->GetBounds(), area)) {
            result.push_back(object);
        }
    }

    for(auto& child : children) {
        if(child) {
            child->Query(area, result);
        }
    }
}
