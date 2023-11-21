#pragma once
#include "nged.h"
#include "nged_imgui.h"
#include "style.h"

#include <spdlog/spdlog.h>
#ifdef _WIN32
#include <spdlog/sinks/wincolor_sink.h>
#include <spdlog/sinks/msvc_sink.h>
#else
#include <spdlog/sinks/ansicolor_sink.h>
#endif

#include <imgui.h>

namespace AgisX {
    class Application;
}


namespace nged {

    class  AgisXEditor 
    {
    public:
        AgisXEditor();

        nged::EditorPtr editor = nullptr;

        void init(AgisX::Application* app);
        void set_dockspace_id(ImGuiID mainDockID_);
        char    const* title();
        wchar_t const* wtitle();
        bool agreeToQuit();
        void render();
        void quit();
        void info(std::string const& msg);
        void error(std::string const& msg);
    }; 
}