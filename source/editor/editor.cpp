#include "editor.h"
#include "nodes.h"

#include <chrono>
#include "../editor/ngdoc.h"

import AgisXNodeFactory;
import AgisXResponser;

using namespace AgisX;

namespace nged {


void AgisXEditor::init(AgisX::Application* app)
{
#ifdef _WIN32
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("", std::make_shared<spdlog::sinks::wincolor_stdout_sink_mt>()));
    spdlog::default_logger()->sinks().emplace_back(std::make_shared<spdlog::sinks::msvc_sink_mt>());
#else
    spdlog::set_default_logger(std::make_shared<spdlog::logger>("", std::make_shared<spdlog::sinks::ansicolor_stdout_sink_mt>()));
#endif
    spdlog::set_level(spdlog::level::trace);
    ImGui::GetIO().IniFilename = nullptr; // disable window size/pos/layout store

    ImGui::StyleColorsDark();
    ImVec4* colors = ImGui::GetStyle().Colors;
    colors[ImGuiCol_FrameBg] = ImVec4(0.28f, 0.28f, 0.28f, 0.54f);
    colors[ImGuiCol_FrameBgHovered] = ImVec4(0.68f, 0.67f, 0.64f, 0.40f);
    colors[ImGuiCol_FrameBgActive] = ImVec4(0.45f, 0.45f, 0.45f, 0.67f);
    colors[ImGuiCol_TitleBgActive] = ImVec4(0.19f, 0.19f, 0.19f, 1.00f);
    colors[ImGuiCol_CheckMark] = ImVec4(0.67f, 0.67f, 0.67f, 1.00f);
    colors[ImGuiCol_SliderGrab] = ImVec4(0.89f, 0.89f, 0.89f, 1.00f);
    colors[ImGuiCol_SliderGrabActive] = ImVec4(1.00f, 1.00f, 1.00f, 1.00f);
    colors[ImGuiCol_Button] = ImVec4(0.47f, 0.46f, 0.45f, 0.40f);
    colors[ImGuiCol_ButtonHovered] = ImVec4(0.50f, 0.50f, 0.50f, 1.00f);
    colors[ImGuiCol_ButtonActive] = ImVec4(0.69f, 0.69f, 0.69f, 1.00f);
    colors[ImGuiCol_Header] = ImVec4(0.33f, 0.31f, 0.28f, 0.31f);
    colors[ImGuiCol_HeaderHovered] = ImVec4(0.26f, 0.26f, 0.26f, 0.80f);
    colors[ImGuiCol_HeaderActive] = ImVec4(0.76f, 0.76f, 0.76f, 1.00f);
    colors[ImGuiCol_SeparatorHovered] = ImVec4(0.57f, 0.59f, 0.61f, 0.78f);
    colors[ImGuiCol_SeparatorActive] = ImVec4(0.58f, 0.58f, 0.58f, 1.00f);
    colors[ImGuiCol_ResizeGrip] = ImVec4(0.48f, 0.48f, 0.48f, 0.25f);
    colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.60f, 0.60f, 0.60f, 0.67f);
    colors[ImGuiCol_ResizeGripActive] = ImVec4(0.82f, 0.82f, 0.82f, 0.95f);
    colors[ImGuiCol_Tab] = ImVec4(0.23f, 0.23f, 0.23f, 0.86f);
    colors[ImGuiCol_TabHovered] = ImVec4(0.28f, 0.28f, 0.28f, 0.80f);
    colors[ImGuiCol_TabActive] = ImVec4(0.32f, 0.32f, 0.32f, 1.00f);
    colors[ImGuiCol_TabUnfocused] = ImVec4(0.05f, 0.05f, 0.05f, 0.97f);
    colors[ImGuiCol_TabUnfocusedActive] = ImVec4(0.22f, 0.22f, 0.22f, 1.00f);
    colors[ImGuiCol_DockingPreview] = ImVec4(0.61f, 0.61f, 0.61f, 0.70f);
    colors[ImGuiCol_TextSelectedBg] = ImVec4(1.00f, 1.00f, 1.00f, 0.35f);
    colors[ImGuiCol_NavHighlight] = ImVec4(0.78f, 0.78f, 0.78f, 1.00f);

    editor = nged::newImGuiNodeGraphEditor();
    editor->setResponser(std::make_shared<AgisX::AgisXResponser>());
    editor->setItemFactory(nged::addImGuiItems(nged::defaultGraphItemFactory()));
    editor->setViewFactory(nged::defaultViewFactory(*app));
    editor->setNodeFactory(std::make_shared<AgisX::AgisxNodeFactory>(*app));
    editor->initCommands();
    nged::addImGuiInteractions();

    nged::ImGuiResource::reloadFonts();
    auto doc = editor->createNewDocAndDefaultViews();
    auto root = doc->root();
}


//============================================================================
void AgisXEditor::set_dockspace_id(ImGuiID mainDockID_)
{
    auto p = static_cast<nged::ImGuiNodeGraphEditor*>(editor.get());
    p->setMainDockID(mainDockID_);
}


//============================================================================
AgisXEditor::AgisXEditor()
{
}


//============================================================================
char const* AgisXEditor::title()
{
	return "Node Graph Editor";
}


//============================================================================
wchar_t const* AgisXEditor::wtitle()
{
    return L"Demo"; 
}

//============================================================================
bool AgisXEditor::agreeToQuit()
{
    return editor->agreeToQuit();
}

//============================================================================
void AgisXEditor::render()
{
    static auto prev = std::chrono::system_clock::now();
    auto now = std::chrono::system_clock::now();
    auto dt = std::chrono::duration_cast<std::chrono::milliseconds>(now - prev);
    ImGui::PushFont(nged::ImGuiResource::instance().sansSerifFont);
    editor->update(dt.count() / 1000.f);
    editor->draw();
    ImGui::PopFont();
    prev = now;
}


//============================================================================
void AgisXEditor::quit()
{
}


//============================================================================
void AgisXEditor::info(std::string const& msg)
{
    MessageHub::info(msg);
}


//============================================================================
void AgisXEditor::error(std::string const& msg)
{
    MessageHub::error(msg);
}

}