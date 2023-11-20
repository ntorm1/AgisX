#include "imgui.h"
#include "imgui_stdlib.h"
#include "ImGuiFileDialog.h"
#include "App.h"

#include "ExchangeComp.h"
#include "AssetComp.h"

import ExchangeMapModule;
import ExchangeModule;
import AssetModule;

using namespace Agis;

namespace AgisX
{


//============================================================================
ExchangeMapComp::ExchangeMapComp(Application& app)
	: _app(app), _exchanges(&app.get_exchanges()), _exchange_ids(&_exchanges->get_exchange_indecies())
{
    _children.push_back(std::make_unique<AssetComp>(app));
    _asset_comp = dynamic_cast<AssetComp*>(_children.back().get());
}


void ExchangeMapComp::on_reset()
{
    BaseComp::on_reset();
}


//============================================================================
void ExchangeMapComp::on_step()
{
    BaseComp::on_step();
}


//============================================================================
void ExchangeMapComp::on_hydra_restore()
{
    auto lock = std::unique_lock(_mutex);
    _exchanges = &_app.get_exchanges();
    _exchange_ids = &_exchanges->get_exchange_indecies();
    _selected_exchange = std::nullopt;
}


//============================================================================
void
ExchangeMapComp::render_new_exchange_comp()
{
    static std::string exchange_id = "";
    static std::string dt_format = "";
    static std::string source = "";
    static const char* commonDateFormats[] = {
        "%Y-%m-%d",
        "%m/%d/%Y",
        "%Y-%m-%d %H:%M:%S"
    };
    static int selectedDateFormatIndex = 0;

    if (ImGui::CollapsingHeader("New Exchange"))
    {
        ImGui::Text("Exchange ID:");
        ImGui::InputText("##ExchangeIDInput", &exchange_id);

        ImGui::Text("Datetime Format:");
        if (ImGui::Combo("##DateFormatCombo", &selectedDateFormatIndex, commonDateFormats, IM_ARRAYSIZE(commonDateFormats))) {
            // User selected a new date format from the dropdown
            dt_format = commonDateFormats[selectedDateFormatIndex];
        }

        // open Dialog Simple
        if (ImGui::Button("Select Source"))
            ImGuiFileDialog::Instance()->OpenDialog("ChooseDirDlgKey", "Choose a Directory", nullptr, ".");

        // display
        if (ImGuiFileDialog::Instance()->Display("ChooseDirDlgKey"))
        {
            // action if OK
            if (ImGuiFileDialog::Instance()->IsOk())
            {
                source = ImGuiFileDialog::Instance()->GetCurrentPath();
            }

            // close
            ImGuiFileDialog::Instance()->Close();
        }
        ImGui::SameLine();
        ImGui::Text(("Source: " + source).c_str());

        // submit button
        if (ImGui::Button("Submit"))
        {
            if (exchange_id == "")
            {
                _exception = AgisException("Exchange ID cannot be empty");
                ImGui::OpenPopup("ExchangeMapCompError");
            }
            else if (dt_format == "")
            {
                _exception = AgisException("Datetime Format cannot be empty");
                ImGui::OpenPopup("ExchangeMapCompError");
			}
            else if (source == "")
            {
                _exception = AgisException("Source cannot be empty");
                ImGui::OpenPopup("ExchangeMapCompError");
			}
			else
            {
				auto res = _app.get_hydra()->create_exchange(exchange_id, dt_format, source);
                if (res)
                {
					exchange_id = "";
					dt_format = "";
					source = "";
				}
                else
                {
                    _exception = res.error();
                    ImGui::OpenPopup("ExchangeMapCompError");
				}
			}
		}
    }
}


//============================================================================
void
ExchangeMapComp::render()
{
    auto lock = std::shared_lock(_mutex);
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
    ImGui::Begin("Exchanges");                          // Create a window called "Hello, world!" and append into it.

    if (ImGui::BeginPopupModal("ExchangeMapCompError")) {
        ImGui::Text(_exception->what());
        
        if (ImGui::Button("Dismiss", ImVec2(120, 0))) 
        { 
            ImGui::CloseCurrentPopup(); 
            _exception = std::nullopt;
        }
        ImGui::EndPopup();
        ImGui::End();
        return;
    }

    render_new_exchange_comp();

    // render the available exchanges
    if (ImGui::TreeNode("Exchanges"))
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
            // update the selected exchange
            auto exchange_opt = _exchanges->get_exchange(selected_exchange_id);
            if (!exchange_opt)
            {
                _exception = exchange_opt.error();
                ImGui::OpenPopup("ExchangeMapCompError");
            }
            _selected_exchange = exchange_opt.value();
        }

        // End the TreeNode loop
        ImGui::TreePop();
    }
    
    static int item_current_idx = 0;
    static char search_buffer[256] = "";  // A buffer to store the search string
    if (ImGui::BeginListBox("Assets"))
    {
        // Create an input field for searching with the label on the left
        ImGui::Text("Search: ");
        ImGui::SameLine();
        ImGui::InputText("##SearchField", search_buffer, sizeof(search_buffer));

        if (_selected_exchange)
        {
            auto& assets = _selected_exchange.value()->get_assets();
            int n = 0;
            for (auto& asset : assets)
            {
                // Check if the item matches the search string
                if (strlen(search_buffer) == 0 || strstr(asset->get_id().c_str(), search_buffer) != nullptr)
                {
                    const bool is_selected = (item_current_idx == n);
                    if (ImGui::Selectable(asset->get_id().c_str(), is_selected))
                        item_current_idx = n;

                    if (is_selected) 
                    {
                        ImGui::SetItemDefaultFocus();
                        if (_asset_comp->get_asset() != asset.get())
                            _asset_comp->set_asset(asset.get());
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

    _asset_comp->render();

    ImGui::Text("This is some useful text.");               // Display some text (you can use a format strings too)
    ImGui::Checkbox("Demo Window", &instance.get_show_demo_window());      // Edit bools storing our window open/close state
    ImGui::SameLine();
    ImGui::End();
}


}