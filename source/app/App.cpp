#include <Windows.h>


#include "ImGuiFileDialog.h"

#include "App.h"
#include "EditorComp.h"

#include <rapidjson/allocators.h>
#include <rapidjson/document.h>

import SerializeModule;
import AgisTimeUtils;

using namespace Agis;

namespace AgisX
{

//============================================================================
Application::Application()
{   
    editor_comp = new EditorComp();
}


//============================================================================
Application::~Application()
{
	delete editor_comp;
}



//============================================================================
void Application::set_dockspace_id(unsigned int mainDockID_)
{

    editor_comp->set_dockspace_id(mainDockID_);
}


//============================================================================
bool
Application::agree_to_quit()
{
    return editor_comp->agree_to_quit();
}

//============================================================================
void
Application::render()
{
    editor_comp->render();
}


//============================================================================
void Application::init()
{
    auto context = ImGui::GetCurrentContext();
    editor_comp->init(context, this);
}

}