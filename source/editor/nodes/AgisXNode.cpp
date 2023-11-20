module;
#include "../nged_imgui.h"
#include "../ngdoc.h"
#include "../res/fa_icondef.h"
#include "../../app/App.h"

#include "AgisXSerialize.h"
#include "AgisAST.h"

module AgisXNode;

import AgisXGraph;

using namespace nged;


namespace AgisX
{

Application* AgisXNode::_instance = nullptr;


//==================================================================================================
AgisXNode::AgisXNode(
	nged::Graph* parent,
	nged::StringView type,
	nged::StringView name,
	int num_inputs)
	: nged::Node(parent, String(type), String(name))
{
	_intputs = num_inputs;
	_dirty = true;
}


//==================================================================================================
void
AgisXNode::set_instance(Application* instance)
{
	_instance = instance; 
}

//==================================================================================================
bool
AgisXNode::serialize(nged::Json& json) const
{
	return nged::Node::serialize(json);
}


//==================================================================================================
bool
AgisXNode::getIcon(nged::IconType& iconType, nged::StringView& iconData) const
{
	iconType = IconType::Text;
	if (type() == "ExchangeNode") {
		iconType = IconType::IconFont;
		iconData = ICON_FA_DATABASE;
	}
	else if (type() == "AssetReadNode") {
		iconType = IconType::IconFont;
		iconData = ICON_FA_BOOK_OPEN;
	}
	else if (type() == "AssetOpNode") {
		iconType = IconType::IconFont;
		iconData = ICON_FA_CALCULATOR;
	}
	else if (type() == "AllocationNode") {
		iconType = IconType::IconFont;
		iconData = ICON_FA_FLAG_CHECKERED;
	}
	else if (type() == "ExchangeViewNode") {
		iconType = IconType::IconFont;
		iconData = ICON_FA_TABLE;
	}
	else {
		iconData = type();
	}
	return true;
}


//==================================================================================================
bool is_asset_node(std::string const& nonde_type) {
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
AgisXExchangeViewNode::acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const
{
	// left port must be exchange node 
	if (port == 0) {
		if (sourceNode->type() != "ExchangeNode") {
			MessageHub::errorf("expected ExchangeNode, found {}", sourceNode->type());
			return false;
		}
		auto node = static_cast<AgisXExchangeNode const*>(sourceNode);
		if (!app().exchange_exists(node->exchangeName())) {
			MessageHub::errorf("exchange {} not found", node->exchangeName());
			return false;
		}
		return true;
	}
	// right port must be asset node
	else if (port == 1) {
		return is_asset_node(sourceNode->type());
	}
	
	return false;
}


//==================================================================================================
void
AgisXExchangeNode::render_inspector() noexcept
{
	bool input_changed = false;
	// create text upload for exchange name
	ImGui::Text("Exchange Name");
	input_changed |= ImGui::InputText("##exchange_name", &_exchange_name);
	if (input_changed) {
		auto parent = static_cast<AgisXGraph*>(this->parent());
		parent->markNodeAndDownstreamDirty(id());
	}

}


//==================================================================================================
void AgisXAssetReadNode::render_inspector() noexcept
{
}


//==================================================================================================
void AgisXAssetOpNode::render_inspector() noexcept
{
}


//==================================================================================================
bool
AgisXAssetReadNode::serialize(Json& json) const
{
	AgisX::serialize_pair(json, "column", _column);
	AgisX::serialize_pair(json, "index", std::to_string(_index));
	return AgisXNode::serialize(json);
}


//==================================================================================================
bool
AgisXAssetReadNode::deserialize(Json const& json)
{
	if (nged::Node::deserialize(json)) {
		auto opt_str = deserialize_string(json, "column");
		if (!opt_str) {
			return false;
		}
		_column = *opt_str;
		opt_str = deserialize_string(json, "index");
		if (!opt_str) {
			return false;
		}
		try {
			_index = std::stoi(*opt_str);
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
bool AgisXExchangeNode::serialize(Json& json) const
{
	AgisX::serialize_pair(json, "exchange_name", _exchange_name);
	return AgisXNode::serialize(json);
}


//==================================================================================================
bool AgisXExchangeNode::deserialize(Json const& json)
{
	if (nged::Node::deserialize(json)) {
		auto opt_str = deserialize_string(json, "exchange_name");
		if (!opt_str) {
			return false;
		}
		_exchange_name = *opt_str;
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