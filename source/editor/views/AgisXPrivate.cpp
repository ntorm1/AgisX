#include "AgisXPrivate.h"

#include <imgui.h>

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
	if (!asset_view) return std::nullopt;
	return (*asset_view).get();
}


//============================================================================
bool
AgisXExchangeViewPrivate::set_selected_asset(std::string const& asset_id)
{
	auto lock = std::shared_lock(_mutex);
	if(!selected_exchange) return false;
	auto asset = (*selected_exchange)->get_asset(asset_id);
	if (!asset) return false;
	asset_view = std::make_unique<AgisX::AgisXAssetViewPrivate>(*asset.value());
	return true;
}

//============================================================================
void
AgisXExchangeViewPrivate::set_selected_exchange(std::optional<Agis::Exchange const*> exchange)
{
	auto lock = std::shared_lock(_mutex);
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
AgisXAssetViewPrivate::AgisXAssetViewPrivate(Agis::Asset const& asset) : _asset(asset)
{
	_asset_id = _asset.get_id();
	_data = &_asset.get_data();
	_columns = _asset.get_column_names();
	auto timestamps = _asset.get_dt_index();
	for (auto const& ts : timestamps)
	{
		_dt_index.push_back(Agis::epoch_to_str(ts, "%Y-%m-%d %H:%M:%S").value());
	}
}


//============================================================================
AgisXAssetViewPrivate::~AgisXAssetViewPrivate()
{
}


//============================================================================
void AgisXAssetViewPrivate::draw()
{
    ImGui::Text("Asset: %s", _asset_id.c_str());
    ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &ASSET_TABLE_FLAGS, ImGuiTableFlags_ScrollY);

    // Using those as a base value to create width/height that are factor of the size of our font
    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();
    std::optional<size_t> _current_index = _asset.get_streaming_index();

    // When using ScrollX or ScrollY we need to specify a size for our table container!
    // Otherwise by default the table will fit all available space, like a BeginChild() call.
    ImVec2 outer_size = ImVec2(0.0f, TEXT_BASE_HEIGHT * 8);
    auto& table_columns = _columns;
    if (ImGui::BeginTable("table_scrolly", table_columns.size() + 1, ASSET_TABLE_FLAGS, outer_size))
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
                for (int column = 0; column < table_columns.size(); column++)
                {
                    // get index into row major data array and render
                    ImGui::TableSetColumnIndex(column + 1);
                    int index = row * table_columns.size() + column;
                    auto value = _data->at(index);
                    ImGui::Text("%.2f", value);
                }
            }
        }
        ImGui::EndTable();
    }
}
}