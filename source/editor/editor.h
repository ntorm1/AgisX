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




namespace nged {

    class NodeEditor {
    public:
         virtual ~NodeEditor();
        virtual char    const* title() { return "Demo App"; }
        virtual wchar_t const* wtitle();
        virtual bool           agreeToQuit() { return true; }
        virtual void           init(ImGuiContext*);
        virtual void           update(ImGuiContext*) = 0;
        virtual void           quit() {};
    };

    //void startApp(NodeEditor* app);


    class  DemoApp : public nged::NodeEditor
    {
    public:
        nged::EditorPtr editor = nullptr;
         DemoApp();
         void init(ImGuiContext*);
         void set_dockspace_id(ImGuiID mainDockID_);
         char    const* title();
         wchar_t const* wtitle();
         bool agreeToQuit();
         void update(ImGuiContext*);
         void quit();

         nged::MessageHub* get_message_hub();

    }; // Demo App
}