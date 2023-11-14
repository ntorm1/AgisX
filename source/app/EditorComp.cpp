#include <nlohmann/json.hpp>

#include "../editor/editor.h"
#include "EditorComp.h"

using namespace nged;


namespace AgisX
{

	//============================================================================
	EditorComp::EditorComp()
	{
		_DemoApp = new nged::DemoApp();
	}


	//============================================================================
	EditorComp::~EditorComp()
	{
		delete _DemoApp;
	}


	//============================================================================
	void EditorComp::render()
	{
		_DemoApp->update(nullptr);
	}


	//============================================================================
	void EditorComp::init(ImGuiContext* ctx)
	{
		_DemoApp->init(ctx);
	}


	//============================================================================
	void EditorComp::set_dockspace_id(ImGuiID mainDockID_)
	{
		_DemoApp->set_dockspace_id(mainDockID_);
	}


	//============================================================================
	void EditorComp::info(std::string const& msg)
	{
		MessageHub::info(msg);
	}


//============================================================================
	void EditorComp::error(std::string const& msg)
	{
		MessageHub::error(msg);
	}


}