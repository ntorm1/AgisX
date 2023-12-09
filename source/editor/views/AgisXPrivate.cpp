#include "AgisXPrivate.h"
#include "AgisXPlot.h"

#include <imgui.h>
#include <implot.h>

import ExchangeModule;
import AssetModule;
import AgisTimeUtils;

static ImGuiTableFlags ASSET_TABLE_FLAGS =
ImGuiTableFlags_ScrollY |
ImGuiTableFlags_RowBg |
ImGuiTableFlags_BordersOuter |
ImGuiTableFlags_BordersV |
ImGuiTableFlags_Resizable |
ImGuiTableFlags_Hideable;

namespace AgisX
{


//============================================================================
AgisXExchangeViewPrivate::~AgisXExchangeViewPrivate()
{
}


//============================================================================
std::optional<AgisXAssetViewPrivate*>
AgisXExchangeViewPrivate::get_asset_view() const noexcept
{
    auto lock = std::shared_lock(_mutex);
	if (!asset_view) return std::nullopt;
	return (*asset_view).get();
}


//============================================================================
bool
AgisXExchangeViewPrivate::set_selected_asset(std::string const& asset_id)
{
	auto lock = std::unique_lock(_mutex);
	if(!selected_exchange) return false;
	auto asset = (*selected_exchange)->get_asset(asset_id);
	if (!asset) return false;
	asset_view = std::make_unique<AgisX::AgisXAssetViewPrivate>(
        app(),
        this, 
        *asset.value()
    );
	return true;
}


AgisXExchangeViewPrivate::AgisXExchangeViewPrivate(AgisX::AppState& app_state)
    : AppComponent(app_state, std::nullopt)
{
}


//============================================================================
void
AgisXExchangeViewPrivate::on_hydra_restore() noexcept
{
	auto lock = std::unique_lock(_mutex);
    asset_view = std::nullopt;
    selected_exchange = std::nullopt;
    asset_ids.clear();
}


//============================================================================
void
AgisXExchangeViewPrivate::set_selected_exchange(std::optional<Agis::Exchange const*> exchange)
{
	auto lock = std::unique_lock(_mutex);
	selected_exchange = exchange;
	asset_view = std::nullopt;
	this->asset_ids.clear();
	if (!selected_exchange) return;
	auto& assets = (*selected_exchange)->get_assets();
	for (auto& asset : assets)
	{
		this->asset_ids.push_back(asset->get_id());
	}
}


//============================================================================
std::optional<std::string>
AgisXExchangeViewPrivate::get_selected_asset_id()
{
	if(!asset_view) return std::nullopt;
	return (*asset_view)->_asset_id;
}


//============================================================================
AgisXAssetViewPrivate::AgisXAssetViewPrivate(
    AgisX::AppState& app_state,
    AgisXExchangeViewPrivate* parent,
    Agis::Asset const& asset)
    : AppComponent(app_state, parent), _asset(asset)
{
    _plot_view = new AgisXAssetPlot(app_state, this, asset);
	_asset_id = _asset.get_id();
	_data = &_asset.get_data();
	_columns = _asset.get_column_names();
	auto const& timestamps = _asset.get_dt_index();
	for (auto const& ts : timestamps)
	{
		_dt_index.push_back(Agis::epoch_to_str(ts, "%Y-%m-%d %H:%M:%S").value());
	}
}



//============================================================================
AgisXAssetViewPrivate::~AgisXAssetViewPrivate()
{
    delete _plot_view;
}


//============================================================================
void
AgisXAssetViewPrivate::asset_table_context_menu()
{
    int hovered_column = -1;
    for (int i = 1; i < _columns.size() + 1; i++)
    {
        ImGui::PushID(i);
        if (ImGui::TableGetColumnFlags(i) & ImGuiTableColumnFlags_IsHovered)
            hovered_column = i;
        if (hovered_column == i && !ImGui::IsAnyItemHovered() && ImGui::IsMouseReleased(1))
            ImGui::OpenPopup("MyPopup");
        if (ImGui::BeginPopup("MyPopup"))
        {
            if (ImGui::Button("Plot"))
            {
				auto& column_name = _columns[i-1];
				auto column_vector = _asset.get_column(column_name);
				_plot_view->add_data(column_name, *column_vector);
                ImGui::CloseCurrentPopup();
            }
            if (ImGui::Button("Close"))
            {
                _plot_view->remove_data(_columns[i-1]);
                ImGui::CloseCurrentPopup();
            }
            ImGui::EndPopup();
        }
        ImGui::PopID();
    }
}


//============================================================================
void AgisXAssetViewPrivate::draw()
{
    ImGui::Text("Asset: %s", _asset_id.c_str());
	draw_table();
	_plot_view->draw_plot();
}


//============================================================================
void AgisXAssetViewPrivate::draw_table()
{
    ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &ASSET_TABLE_FLAGS, ImGuiTableFlags_ScrollY);

    // Using those as a base value to create width/height that are factor of the size of our font
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    std::optional<size_t> _current_index = _asset.get_streaming_index();

    // When using ScrollX or ScrollY we need to specify a size for our table container!
    // Otherwise by default the table will fit all available space, like a BeginChild() call.
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
    auto& table_columns = _columns;
    if (ImGui::BeginTable("table_scrolly", static_cast<int>(table_columns.size()) + 1, ASSET_TABLE_FLAGS, outer_size))
    {
        ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
        ImGui::TableSetupColumn("DateTime", ImGuiTableColumnFlags_None);
        for (int column = 0; column < table_columns.size(); column++)
        {
            ImGui::TableSetupColumn(table_columns[column].c_str(), ImGuiTableColumnFlags_None);
        }
        ImGui::TableHeadersRow();

        ImGuiListClipper clipper;
        clipper.Begin(20);
        while (clipper.Step())
        {
            for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
            {
                // test if row is grater than number of rows in row major data array
                if (row >= _data->size() / table_columns.size()) break;

                ImGui::TableNextRow();
                // Highlight the current row
                if (_current_index && (*_current_index) == static_cast<size_t>(row))
                {
                    ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, IM_COL32(255, 0, 0, 255)); // Red background
                }

                ImGui::TableSetColumnIndex(0);
                ImGui::Text("%s", _dt_index[row].c_str());
                for (size_t column = 0; column < table_columns.size(); column++)
                {
                    // get index into row major data array and render
                    ImGui::TableSetColumnIndex(static_cast<int>(column) + 1);
                    auto index = row * table_columns.size() + column;
                    auto value = _data->at(index);
                    ImGui::Text("%.2f", value);
                }
            }
        }
        asset_table_context_menu();
        ImGui::EndTable();
    }
}

}