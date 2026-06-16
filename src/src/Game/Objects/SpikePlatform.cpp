#include "include/Game/Objects/SpikePlatform.hpp"

SpikePlatform::SpikePlatform(const unordered_map<string, std::any>& data)
    : Platform(data)
{
	state = SpikeState::Retracted;
    timer = fromMap(float, "startTime", data);

    extendTime = fromMap(float,"extendTime", data);
    retractTime = fromMap(float,"retractTime", data);
    extendedTime = fromMap(float,"extendedTime", data);
    retractedTime = fromMap(float,"retractedTime", data);

    upDown = fromMap(bool,"upDown", data);
    retractedPos = fromMap(float, "retractedPos", data);
    extendedPos = fromMap(float, "extendedPos", data);

}

float Lerp(float a, float b, float t)
{
    t = std::clamp(t, 0.0f, 1.0f);
    return a + (b - a) * t;

}

void SpikePlatform::Physics(const float& dt)
{
    if (childrenDirty)
    {
        children.clear();

        for (auto& child : GetChildren())
        {
            if (child->Type() == "Object3D")
            {
                children.push_back(
                    std::static_pointer_cast<Object3D>(child)
                );
            }
        }

        childrenDirty = false;
    }

    timer += dt;

    float from = (state == SpikeState::Extending) ? retractedPos : extendedPos;
    float to = (state == SpikeState::Extending) ? extendedPos : retractedPos;
    float moveDuration = (state == SpikeState::Extending) ? extendTime : retractTime;
    float pauseDuration = (state == SpikeState::Extended) ? extendedTime : retractedTime;

    switch (state)
    {
        case SpikeState::Retracted:
        case SpikeState::Extended:
            if (timer >= pauseDuration) {
                state = (state == SpikeState::Extended) ? SpikeState::Retracting : SpikeState::Extending;
                timer -= pauseDuration;
            }
            break;

        case SpikeState::Extending:
        case SpikeState::Retracting:
        {
            for (shared_ptr<Object3D> child : children) {
                
                float t = timer / moveDuration;

                Transform trans = child->GetTransform();

                vec3 pos = trans.GetTranslation();

                if (upDown)
                    pos.y = Lerp(from, to, t);
                else
                    pos.x = Lerp(from, to, t);

                trans.SetTranslation(pos);

                child->SetTransform(trans);
                child->ResetGlobal();
                
                
            }

            if (timer >= moveDuration) {
                state = (state == SpikeState::Retracting) ? SpikeState::Retracted : SpikeState::Extended;
                timer -= moveDuration;
            }
            break;
        }
    }
}
