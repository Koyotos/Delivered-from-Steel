#ifndef FE_PROFILER_NODE
#define FE_PROFILER_NODE

#include "include/Renderer/TextNode.hpp"
#include "include/Profiler/Profiler.hpp"

class ProfilerNode : public TextNode {
public:
	void Process() override {
		SetContent(Profiler::Get().GetStatsString());
	}

	ProfilerNode() : TextNode() {
		SetProcess(true);
		SetDraw(true);
		SetIgnoreParent(true);
	}

	ProfilerNode(const unordered_map<string, std::any>& data) : TextNode(data) {
		SetProcess(true);
		SetDraw(true);
		SetIgnoreParent(true);
	}
};

#endif
