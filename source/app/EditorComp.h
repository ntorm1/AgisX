#pragma once
#include <string>

#include "imgui.h"

namespace nged 
{
	class DemoApp;
	class MessageHub;
}

namespace AgisX
{
	class EditorComp
	{
	public:
		EditorComp();
		~EditorComp();
		void render();
		void init(ImGuiContext*);
		void set_dockspace_id(ImGuiID mainDockID_);

		void info(std::string const& msg);
		void error(std::string const& msg);

	private:
		nged::DemoApp* _DemoApp;

	};
}