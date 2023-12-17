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

    void draw_progress_bar()
    {
        // Progress value (between 0.0 and 1.0)
        auto progress = static_cast<float>(_app_state.get_current_index()) / _app_state.get_global_dt_index().size();

        // Calculate the color based on the progress percentage
        ImVec4 color;
        if (progress < 0.5f)
        {
            // Yellow to orange transition
            color = ImVec4(1.0f, 1.0f - 2 * progress, 0.0f, 1.0f);
        }
        else
        {
            // Orange to green transition
            color = ImVec4(2 * (1.0f - progress), 2 * progress - 1.0f, 0.0f, 1.0f);
        }

        // Push the modified color for the progress bar
        ImGui::PushStyleColor(ImGuiCol_PlotHistogram, color);

        // Draw the progress bar
        ImGui::ProgressBar(progress);

        // Pop the color changes
        ImGui::PopStyleColor();
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
            if (ImGui::Button("Run"))
            {
                _app_state.__run();
            }
            ImGui::SameLine();
            if (ImGui::Button("Pause"))
            {
                _app_state.__interupt();
            }
            if (ImGui::Button("Reset"))
            {
                _app_state.__reset();
            }
            draw_progress_bar();
            ImGui::Text("Global Time: %s", _app_state.get_global_time().c_str());
            ImGui::Text("Next Global Time: %s", _app_state.get_next_global_time().c_str());
            ImGui::Separator();
        }
    }

private:
    AgisX::AppState& _app_state;
};


}