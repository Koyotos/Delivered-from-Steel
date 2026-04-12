#include "include/Profiler/Profiler.hpp"
#include <iomanip>
#include <sstream>

void Profiler::Init() {
#ifdef FE_ENABLE_PROFILER
	glGenQueries(1, &timeQuery);
#endif
}

void Profiler::BeginFrame(float deltaTime) {
#ifdef FE_ENABLE_PROFILER
	fpsCounter++;
	fpsTimer += deltaTime;
	if (fpsTimer >= 1.0f) {
		currentFPS = fpsCounter;
		fpsCounter = 0;
		fpsTimer -= 1.0f;
	}

	if (queryStarted) {
		GLuint available = 0;
		glGetQueryObjectuiv(timeQuery, GL_QUERY_RESULT_AVAILABLE, &available);
		if (available) {
			GLuint64 elapsed;
			glGetQueryObjectui64v(timeQuery, GL_QUERY_RESULT, &elapsed);
			gpuTime = elapsed / 1000000.0f;
		}
	}

	drawCalls = 0;
	renderedObjects = 0;
	triangles = 0;
	stateChanges = 0;

	frameStart = high_resolution_clock::now();
#endif
}

void Profiler::EndLogic() {
#ifdef FE_ENABLE_PROFILER
	logicEnd = high_resolution_clock::now();
	cpuLogicTime = duration<float, std::milli>(logicEnd - frameStart).count();

	glBeginQuery(GL_TIME_ELAPSED, timeQuery);
	queryStarted = true;
#endif
}

void Profiler::EndRender() {
#ifdef FE_ENABLE_PROFILER
	glEndQuery(GL_TIME_ELAPSED);

	auto renderEnd = high_resolution_clock::now();
	cpuRenderTime = duration<float, std::milli>(renderEnd - logicEnd).count();

	lastDrawCalls = drawCalls;
	lastRenderedObjects = renderedObjects;
	lastTriangles = triangles;
	lastStateChanges = stateChanges;
#endif
}

string Profiler::GetStatsString() {
#ifdef FE_ENABLE_PROFILER
	stringstream ss;
	ss << fixed << setprecision(2);
	ss << "FPs: " << currentFPS << "\n";
	if (currentFPS > 0) {
		ss << "Frame time: " << 1000.0f / currentFPS << " ms\n";
	}
	else {
		ss << "Frame time: N/A\n";
	}
	ss << "CPU Logic: " << cpuLogicTime << " ms\n";
	ss << "CPU Render: " << cpuRenderTime << " ms\n";
	ss << "GPU Time: " << gpuTime << " ms\n";
	ss << "Draw Calls: " << lastDrawCalls << "\n";
	ss << "Triangles: " << lastTriangles << "\n";
	ss << "Objects: " << lastRenderedObjects << "\n";
	ss << "state Changes: " << lastStateChanges;
	return ss.str();
#else
	return "";
#endif
}
