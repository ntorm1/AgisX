module;

#pragma once
#include "../nged_imgui.h"

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"

export module AgisXAppViewModule;

import AgisXApp;

namespace AgisX {

//============================================================================
export class AgisXAppView
    : public nged::ImGuiGraphView<AgisXAppView, nged::GraphView>
{
public:
    AgisXAppView(
        AgisX::AppState& app_state,
        nged::NodeGraphEditor* editor
    ) : ImGuiGraphView(editor, nullptr), _app_state(app_state)
    {
        setTitle("App State");
    }
    void onDocModified() override {}
    void onGraphModified() override {}
    void on_hydra_restore() override 
    {
        write_unlock();
    }

    void drawContent() 
    {
        if (ImGui::CollapsingHeader("State IO"))
        {
            if (ImGui::Button("Save"))
            {
                _app_state.__save_state();
            }
            ImGui::SameLine();
            if (ImGui::Button("Load"))
            {
                _app_state.__load_state();
            }
            // add separator
            ImGui::Separator();
            if (ImGui::Button("Build"))
            {
				_app_state.__build();
            }
            ImGui::SameLine();
            if (ImGui::Button("Step"))
            {
                _app_state.__step();
            }
            ImGui::SameLine();
            if (ImGui::Button("Reset"))
            {
                _app_state.__reset();
            }
            ImGui::Text("Global Time: %s", _app_state.get_global_time().c_str());
            ImGui::Text("Next Global Time: %s", _app_state.get_next_global_time().c_str());
            ImGui::Separator();
        }
    }

private:
    AgisX::AppState& _app_state;
};


}