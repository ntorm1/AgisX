module;
#include "../nged_imgui.h"
#include "../ngdoc.h"

#include "AgisXSerialize.h"

#include "AgisAST.h"

module AgisXAssetNodeMod;
import AgisXApp;
import AgisXExchangeNodeMod;

using namespace nged;

namespace AgisX
{
//==================================================================================================
static bool is_asset_node(std::string const& nonde_type) {
	return nonde_type == "AssetReadNode" || nonde_type == "AssetOpNode";
}


//==================================================================================================
bool
AgisXAssetOpNode::acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const
{
	// left port can be read or operation
	if (port == 0) {
		return is_asset_node(sourceNode->type());
	}
	// right port must be read opp
	else if (port == 1) {
		return sourceNode->type() == "AssetReadNode";
	}
	return false;
}


//==================================================================================================
bool
AgisXAssetReadNode::acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const
{
	if (sourceNode->type() != "ExchangeNode") {
		nged::MessageHub::errorf("expected ExchangeNode, found {}", sourceNode->type());
		return false;
	}
	auto node = static_cast<AgisXExchangeNode const*>(sourceNode);
	if (!node->exchange_exists()) {
		MessageHub::errorf("exchange {} not found", node->exchangeName());
		return false;
	}
	_columns = node->get_columns();
	return true;
}


//==================================================================================================
void AgisXAssetReadNode::render_inspector() noexcept
{
	NodePtr exchange_parent = nullptr;
	sint out_port = 0;
	if (!getInput(0, exchange_parent, out_port)) {
		ImGui::Text("no exchange");
	}
	else {
		auto exchange_node = static_cast<AgisXExchangeNode*>(exchange_parent.get());
		ImGui::Text("exchange: %s", exchange_node->exchangeName().c_str());
		if (!_columns.size()){
			ImGui::Text("no columns");
		}
		else {
			ImGui::Text("column: ");
			ImGui::SameLine();
			std::vector<const char*> columnItems;
			columnItems.reserve(_columns.size());
			for (const auto& column : _columns) {
				columnItems.push_back(column.c_str());
			}
			int current_item = _column;
			int current_index = _index;
			ImGui::Combo(
				"##column",
				&_column,
				columnItems.data(),
				static_cast<int>(columnItems.size())
			);
		
			// make input for index integer, on change check if valid int
			ImGui::Text("index: ");
			ImGui::SameLine();
			ImGui::InputInt("##index", &_index);
			if (_column != current_item || _index != current_index) {
				setDirty(true);
				auto n = name();
				rename(_columns[_column] +" " + std::to_string(_index), n);
			}
		}
	}
}


//==================================================================================================
void AgisXAssetOpNode::render_inspector() noexcept
{
}


//==================================================================================================
bool
AgisXAssetReadNode::serialize(Json& json) const
{
	AgisX::serialize_pair(json, "column",std::to_string(_column));
	AgisX::serialize_pair(json, "index", std::to_string(_index));
	return AgisXNode::serialize(json);
}


//==================================================================================================
bool
AgisXAssetReadNode::deserialize(Json const& json)
{
	if (nged::Node::deserialize(json)) {
		auto col_str = deserialize_string(json, "column");
		if (!col_str) {
			return false;
		}
		auto index_str = deserialize_string(json, "index");
		if (!index_str) {
			return false;
		}
		try {
			_column = std::stoi(*col_str);
			_index = std::stoi(*index_str);
		}
		catch (...) {
			return false;
		}
		setDirty(true);
		return true;
	}
	return false;
}

//==================================================================================================
bool AgisXAssetOpNode::serialize(Json& json) const
{
	AgisX::serialize_pair(json, "op_type", AgisX::agis_operator_to_string(_opp));
	return AgisXNode::serialize(json);
}


//==================================================================================================
bool AgisXAssetOpNode::deserialize(Json const& json)
{
	if (nged::Node::deserialize(json)) {
		auto opt_str = deserialize_string(json, "op_type");
		if (!opt_str) {
			return false;
		}
		_opp = string_to_agis_operator(*opt_str);
		setDirty(true);
		return true;
	}
	return false;
}

}
