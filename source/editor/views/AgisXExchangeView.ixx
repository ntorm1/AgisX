module;
#pragma once
#include "../nged_imgui.h"

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
#include "AgisXPrivate.h"

export module AgisXExchangeViewModule;

import AgisXApp;

static int selected_asset_current_idx = 0;
static char asset_search_buffer[256] = "";


namespace AgisX {

//============================================================================
export class AgisXExchangeView 
    : public nged::ImGuiGraphView<AgisXExchangeView, nged::GraphView>
{
private:
    AgisX::AppState& _app_state;
    AgisX::AgisXExchangeViewPrivate _p;
    mutable std::shared_mutex _mutex;
    Agis::ExchangeMap const* _exchanges = nullptr;
    std::unordered_map<std::string, size_t> const* _exchange_ids = nullptr;


public:

    AgisXExchangeView(
        AgisX::AppState& app_state,
        nged::NodeGraphEditor* editor
    ) : ImGuiGraphView(editor, nullptr), _app_state(app_state)
    {
        setTitle("Exchanges");
    }
    void onDocModified() override {}
    void onGraphModified() override {}

    //============================================================================
    void on_hydra_restore() override
    {
        auto lock = std::unique_lock(_mutex);
        _exchange_ids = _app_state.get_exchange_ids();
        _p.set_selected_exchange(std::nullopt);
    }

    //============================================================================
    void drawAsset()
    {
        auto selected_asset_opt = _p.get_asset_view();
        if(!selected_asset_opt) return;
        (*selected_asset_opt)->draw();
    }

    //============================================================================
    void drawContent() 
    {
        auto lock = std::shared_lock(_mutex);
        static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
        if (ImGui::TreeNode("Exchanges") && _exchange_ids)
        {
            std::vector<bool> selected_items;
            int node_clicked = -1;
            std::string selected_exchange_id = "";

            // Initialize selected_items with the same number of items as exchange_id_map
            selected_items.resize(_exchange_ids->size(), false);

            int i = 0;
            for (auto& [exchange_id, exchange_index] : *_exchange_ids)
            {
                ImGuiTreeNodeFlags node_flags = base_flags;
                const bool is_selected = selected_items[i];

                if (is_selected)
                    node_flags |= ImGuiTreeNodeFlags_Selected;

                node_flags |= ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_NoTreePushOnOpen;
                ImGui::TreeNodeEx((void*)(intptr_t)i, node_flags, exchange_id.c_str());

                if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen())
                {
                    if (node_clicked != i && node_clicked != -1)
                    {
                        // Clear previous selection
                        selected_items[node_clicked] = false;
                    }
                    node_clicked = i;
                    selected_exchange_id = exchange_id;
                    selected_items[i] = true; // Select the clicked item
                }

                i++;
            }

            if (node_clicked != -1)
            {
                auto e = _app_state.get_exchange(selected_exchange_id);
                if (!e)
                {
                    errorf("failed to find exchange: {}", selected_exchange_id);
                }
                else {
                    _p.set_selected_exchange(e);
                    infof("selected exchange: {}", selected_exchange_id);
                }
            }

            // End the TreeNode loop
            ImGui::TreePop();
        }
        if (ImGui::BeginListBox("Assets"))
        {
            // Create an input field for searching with the label on the left
            ImGui::Text("Search: ");
            ImGui::SameLine();
            ImGui::InputText("##SearchField", asset_search_buffer, sizeof(asset_search_buffer));

            if (_p.has_exchange())
            {
                auto& asset_ids = _p.get_asset_ids();
                int n = 0;
                for (auto& asset_id : asset_ids)
                {
                    // Check if the item matches the search string
                    if (strlen(asset_search_buffer) == 0 || strstr(asset_id.c_str(), asset_search_buffer) != nullptr)
                    {
                        const bool is_selected = (selected_asset_current_idx == n);
                        if (ImGui::Selectable(asset_id.c_str(), is_selected))
                            selected_asset_current_idx = n;

                        if (is_selected)
                        {
                            ImGui::SetItemDefaultFocus();
                            if (_p.get_selected_asset_id() != asset_id)
                            {
                                auto res = _p.set_selected_asset(asset_id);
                                if (!res) errorf("failed to set selected asset: {}", asset_id);
                                else infof("selected asset: {}", asset_id);
                            }
                        }
                    }
                    n++;
                }
            }
            else
            {
                ImGui::Text("No exchange selected");
            }
            ImGui::EndListBox();
        }
        drawAsset();
    }
    
};

}