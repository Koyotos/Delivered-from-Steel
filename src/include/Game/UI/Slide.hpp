#ifndef FE_SLIDE
#define FE_SLIDE

#include "include/Game/UI/UIElement.hpp"
#include "include/Game/UI/TextUI.hpp"
#include "include/Game/UI/Icon.hpp"
#include "include/ResourceManager/ResourceManager.hpp"
#include "Include/Core/Scene.hpp"


class Slide : public UIElement
{
	private:

	float loadTime;

	function<void()> onEndScene;

	
	vector<shared_ptr<Icon>> slides;
	vector<shared_ptr<TextUI>> texts;
	shared_ptr<Scene> cutscene;


	public:

	Slide();
	void Init(shared_ptr<ResourceManager> rsm, string path);
	void FindNodes(shared_ptr<Node> node);
	void SetLoadTime(float value);
	void SetOnEndScene(function<void()> cb);
	void Process() override;


	shared_ptr<Scene> GetScene();


};

#endif
