#ifndef FE_PROFILER
#define FE_PROFILER

#include <string>
#include <chrono>
#include <glad/glad.h>

#define FE_ENABLE_PROFILER 

using namespace std;
using namespace std::chrono;

class Profiler {
private:
	int drawCalls = 0;
	int renderedObjects = 0;
	int triangles = 0;
	int stateChanges = 0;

	int lastDrawCalls = 0;
	int lastRenderedObjects = 0;
	int lastTriangles = 0;
	int lastStateChanges = 0;

	int fpsCounter = 0;
	int currentFPS = 0;
	float fpsTimer = 0.0f;

	time_point<high_resolution_clock> frameStart;
	time_point<high_resolution_clock> logicEnd;

	float cpuLogicTime = 0.0f;
	float cpuRenderTime = 0.0f;
	float gpuTime = 0.0f;

	GLuint timeQuery = 0;
	bool queryStarted = false;

	Profiler() {}

public:
	static Profiler& Get() {
		static Profiler instance;
		return instance;
	}

	void Init();
	void BeginFrame(float deltaTime);
	void EndLogic();
	void EndRender();

	void AddDrawCall(int triCount) { drawCalls++; triangles += triCount; }
	void AddStateChange() { stateChanges++; }
	void AddRenderedObject() { renderedObjects++; }

	string GetStatsString();
};

#ifdef FE_ENABLE_PROFILER
#define PROFILER_INIT() Profiler::Get().Init()
#define PROFILER_BEGIN_FRAME(dt) Profiler::Get().BeginFrame(dt)
#define PROFILER_END_LOGIC() Profiler::Get().EndLogic()
#define PROFILER_END_RENDER() Profiler::Get().EndRender()
#define PROFILER_ADD_DRAW_CALL(tris) Profiler::Get().AddDrawCall(tris)
#define PROFILER_ADD_STATE_CHANGE() Profiler::Get().AddStateChange()
#define PROFILER_ADD_OBJECT() Profiler::Get().AddRenderedObject()
#else
#define PROFILER_INIT()
#define PROFILER_BEGIN_FRAME(dt)
#define PROFILER_END_LOGIC()
#define PROFILER_END_RENDER()
#define PROFILER_ADD_DRAW_CALL(tris)
#define PROFILER_ADD_STATE_CHANGE()
#define PROFILER_ADD_OBJECT()
#endif

#endif
