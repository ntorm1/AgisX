#include "imgui.h"
#include "Helpers.h"

#include "AssetComp.h"
#include "App.h"

import AssetModule;
import AgisTimeUtils;


namespace AgisX
{

AssetComp::AssetComp(Application& app) : _app(app)
{
}


//============================================================================
void AssetComp::set_asset(Asset const* asset)
{
	auto lock = std::unique_lock(_mutex);
	_asset = asset;
	_data = &asset->get_data();
	_columns = asset->get_column_names();
	_asset_id = asset->get_id();
	_current_index = _asset.value()->get_streaming_index();
	_dt_index.clear();
	auto timestamps = asset->get_dt_index();
	for (auto const& ts : timestamps)
	{
		_dt_index.push_back(epoch_to_str(ts, "%Y-%m-%d %H:%M:%S").value());
	}
}


//============================================================================
void AssetComp::render()
{
	auto lock = std::shared_lock(_mutex);
	if (!_asset) return;
	
	ImGui::Begin("Asset");

	ImGui::Text("Asset: %s", _asset_id.c_str());

	// Using those as a base value to create width/height that are factor of the size of our font
	const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
	const float TEXT_BASE_HEIGHT = ImGui::GetTextLineHeightWithSpacing();

	// render table
	static ImGuiTableFlags flags = ImGuiTableFlags_ScrollY | ImGuiTableFlags_RowBg | ImGuiTableFlags_BordersOuter | ImGuiTableFlags_BordersV | ImGuiTableFlags_Resizable | ImGuiTableFlags_Reorderable | ImGuiTableFlags_Hideable;

	PushStyleCompact();
	ImGui::CheckboxFlags("ImGuiTableFlags_ScrollY", &flags, ImGuiTableFlags_ScrollY);
	PopStyleCompact();

	// When using ScrollX or ScrollY we need to specify a size for our table container!
	// Otherwise by default the table will fit all available space, like a BeginChild() call.
	ImVec2 outer_size = ImVec2(0.0f, _app.TEXT_BASE_HEIGHT * 8);
	if (ImGui::BeginTable("table_scrolly", _columns.size()+1, flags, outer_size))
	{
		ImGui::TableSetupScrollFreeze(0, 1); // Make top row always visible
		ImGui::TableSetupColumn("DateTime", ImGuiTableColumnFlags_None);
		for (int column = 0; column < _columns.size(); column++)
		{
			ImGui::TableSetupColumn(_columns[column].c_str(), ImGuiTableColumnFlags_None);
		}
		ImGui::TableHeadersRow();

		// Demonstrate using clipper for large vertical lists
		ImGuiListClipper clipper;
		clipper.Begin(200);
		while (clipper.Step())
		{
			for (int row = clipper.DisplayStart; row < clipper.DisplayEnd; row++)
			{
				// test if row is grater than number of rows in row major data array
				if (row >= _data->size() / _columns.size()) break;

				ImGui::TableNextRow();
				// Highlight the current row
				if (_current_index && (*_current_index) == static_cast<size_t>(row))
				{
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg1, IM_COL32(255, 0, 0, 255)); // Red background
				}

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("%s", _dt_index[row].c_str());
				for (int column = 0; column < _columns.size(); column++)
				{
					// get index into row major data array and render
					ImGui::TableSetColumnIndex(column + 1);
					int index = row * _columns.size() + column;
					auto value = _data->at(index);
					ImGui::Text("%f", value);
				}
			}
		}
		
		ImGui::EndTable();
	}
	ImGui::End();
	
}


//============================================================================
void
AssetComp::on_step()
{
	if(!_asset) return;
	auto lock = std::unique_lock(_mutex);
	_current_index = _asset.value()->get_streaming_index();
	BaseComp::on_step();
}


//============================================================================
void
AssetComp::on_reset()
{
	auto lock = std::unique_lock(_mutex);
	_current_index = _asset.value()->get_streaming_index();
	BaseComp::on_reset();
}


//============================================================================
void
AssetComp::on_hydra_restore()
{
	auto lock = std::unique_lock(_mutex);
	_asset = std::nullopt;	
	_data = nullptr;
	std::vector<std::string> columns;           
	_columns = columns;
}


}