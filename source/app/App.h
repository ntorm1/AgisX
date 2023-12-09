#pragma once
#include "AgisDeclare.h"
#include "AgisXDeclare.h"
#include "BaseComp.h"

#include "imgui.h"

namespace AgisX 
{

class ExchangeMapComp;
class AssetComp;
class EditorComp;


class Application {
public:
	Application();
	~Application();
	bool& get_show_demo_window() { return show_demo_window; }
	void set_dockspace_id(ImGuiID mainDockID_);
	void init();
	bool agree_to_quit();
	void render();

private:
	bool show_demo_window = true;
	EditorComp* editor_comp = nullptr;
};

static Application instance;
}