#pragma once
#include <string>

#include "imgui.h"


namespace nged 
{
	class AgisXEditor;
	class MessageHub;
}

namespace AgisX
{
class Application;

class EditorComp
{
public:
	EditorComp();
	~EditorComp();
	void render();
	void init(ImGuiContext*, AgisX::Application*);
	void set_dockspace_id(ImGuiID mainDockID_);
	bool agree_to_quit();

private:
	nged::AgisXEditor* _DemoApp;

};

}