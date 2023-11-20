module;
#include "../nged_imgui.h"
#include "../ngdoc.h"
#include "../../app/App.h"

#include "AgisXSerialize.h"

#include "AgisAST.h"

module AgisXExchangeNodeMod;

import AgisXGraph;

using namespace nged;


namespace AgisX
{

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
bool
	AgisXExchangeNode::exchange_exists() const noexcept
{
	return app().exchange_exists(_exchange_name);
}


//==================================================================================================
void
AgisXExchangeNode::on_render_deactivate() noexcept
{
	auto parent = static_cast<AgisXGraph*>(this->parent());
	parent->markNodeAndDownstreamDirty(id());
	if (!this->exchange_exists()) {
		MessageHub::errorf("exchange {} not found", _exchange_name);
		remove_downstream_links();
	}
}


//==================================================================================================
void
	AgisXExchangeNode::render_inspector() noexcept
{
	static bool isTextActive = false;  // Track whether the text input is active

	bool input_changed = false;
	// create text upload for exchange name
	ImGui::Text("Exchange Name");
	if (ImGui::InputText("##exchange_name", &_exchange_name)) {
		if (name() != _exchange_name) {
			auto n = name();
			rename(_exchange_name,n);
			input_changed = true;
		}
		isTextActive = true;  // Text input is active
	}
	// Check if the input text field is deactivated and was previously active
	if (!ImGui::IsItemActive() && isTextActive) {
		on_render_deactivate();
		isTextActive = false;
	}
}

static bool is_asset_node(std::string const& nonde_type) {
	return nonde_type == "AssetReadNode" || nonde_type == "AssetOpNode";
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
		if (!node->exchange_exists()) {
			MessageHub::errorf("exchange {} not found", node->exchangeName());
			return false;
		}
		// remove const from this 
		auto non_const_this = const_cast<AgisXExchangeViewNode*>(this);
		node->add_dest(non_const_this, 0);
		return true;
	}
	// right port must be asset node
	else if (port == 1) {
		return is_asset_node(sourceNode->type());
	}

	return false;
}

}