#ifndef FE_SPIKE_PLATFORM
#define FE_SPIKE_PLATFORM

#include "include/Game/Objects/Platform.hpp"

enum class SpikeState {
    Retracted,
    Extending,
    Extended,
    Retracting
};

class SpikePlatform :
    public Platform
{
    private:
        SpikeState state;

        bool upDown;

        float extendTime;
        float extendedTime;
        float retractTime;
        float retractedTime;

        float retractedPos;
        float extendedPos;

		float timer;

        bool childrenDirty = true;
        vector<shared_ptr<Object3D>> children;

    public:
		SpikePlatform(const unordered_map<string, std::any>&);

        void Physics(const float& deltaTime) override;
};

#endif // FE_SPIKE_PLATFORM
