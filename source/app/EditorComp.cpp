#include <nlohmann/json.hpp>

#include "../editor/editor.h"
#include "EditorComp.h"

using namespace nged;


namespace AgisX
{

	//============================================================================
	EditorComp::EditorComp()
	{
		_DemoApp = new nged::AgisXEditor();
	}


	//============================================================================
	EditorComp::~EditorComp()
	{
		delete _DemoApp;
	}


	//============================================================================
	void EditorComp::render()
	{
		_DemoApp->render();
	}


	//============================================================================
	void EditorComp::init(ImGuiContext* ctx, Application* app)
	{
		_DemoApp->init(app);
	}


	//============================================================================
	void EditorComp::set_dockspace_id(ImGuiID mainDockID_)
	{
		_DemoApp->set_dockspace_id(mainDockID_);
	}

	bool EditorComp::agree_to_quit()
	{
		return _DemoApp->agreeToQuit();
	}

}