module;
#include <Eigen/Dense>
#include "AgisMacros.h"
#include "../nged_imgui.h"
#include "../ngdoc.h"

#include "AgisXSerialize.h"

#include "AgisAST.h"

module AgisXExchangeNodeMod;
import AgisXAssetNodeMod;
import AgisXGraph;
import AgisXApp;
import AssetNode;
import ExchangeNode;
import ExchangeModule;


namespace AgisX
{

//==================================================================================================
bool AgisXExchangeNode::serialize(nged::Json& json) const
{
	AgisX::serialize_pair(json, "exchange_name", _exchange_name);
	return nged::Node::serialize(json);
}


//==================================================================================================
bool AgisXExchangeNode::deserialize(nged::Json const& json)
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
std::expected<SharedPtr<Agis::AST::ExchangeNode const>, Agis::AgisException>
AgisXExchangeNode::to_agis() const noexcept
{
	auto exchange_opt = app().get_exchange(_exchange_name);
	if (!exchange_opt) {
		return std::unexpected(Agis::AgisException("exchange not found"));
	}
	return std::make_shared<Agis::AST::ExchangeNode>(*exchange_opt);
}


//==================================================================================================
bool
AgisXExchangeNode::exchange_exists() const noexcept
{
	return app().get_exchange(_exchange_name).has_value();
}


//==================================================================================================
std::vector<std::string> AgisXExchangeNode::get_columns() const noexcept
{
	auto exchange_opt = app().get_exchange(_exchange_name);
	if(!exchange_opt) {
		return {};
	}
	return (*exchange_opt)->get_columns();
}


//==================================================================================================
void
AgisXExchangeNode::on_render_deactivate() noexcept
{
	auto parent = static_cast<AgisXGraph*>(this->parent());
	parent->markNodeAndDownstreamDirty(id());
	if (!this->exchange_exists()) {
		nged::MessageHub::errorf("exchange {} not found", _exchange_name);
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


//==================================================================================================
std::expected<UniquePtr<Agis::AST::ExchangeViewSortNode>, Agis::AgisException>
AgisXExchangeViewNode::to_agis() const noexcept
{
	if (!_asset_input) {
		return std::unexpected(Agis::AgisException("no asset input"));
	}
	if (!_exchange) {
		return std::unexpected(Agis::AgisException("no exchange input"));
	}
	AGIS_ASSIGN_OR_RETURN(asset_node, (*_asset_input)->to_agis());
	AGIS_ASSIGN_OR_RETURN(exchange_node, (*_exchange)->to_agis());
	auto ev_node = std::make_unique<Agis::AST::ExchangeViewNode>(
		exchange_node,
		std::move(asset_node)
	);
	return std::make_unique<Agis::AST::ExchangeViewSortNode>(
		std::move(ev_node),
		_query_type,
		_n
	);
}


//==================================================================================================
bool
AgisXExchangeViewNode::acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const
{
	// left port must be exchange node 
	if (port == 0) {
		if (sourceNode->type() != "ExchangeNode") {
			nged::MessageHub::errorf("expected ExchangeNode, found {}", sourceNode->type());
			return false;
		}
		auto node = static_cast<AgisXExchangeNode const*>(sourceNode);
		if (!node->exchange_exists()) {
			nged::MessageHub::errorf("exchange {} not found", node->exchangeName());
			return false;
		}
		_exchange = node;
		auto non_const_this = const_cast<AgisXExchangeViewNode*>(this);
		node->add_dest(non_const_this, 0);
		return true;
	}
	// right port must be asset node
	else if (port == 1) {
		if (sourceNode->type() != "AssetOpNode")
		{
			return false;
		}
		_asset_input = static_cast<AgisXAssetOpNode const*>(sourceNode);
		return true;
	}
	return false;
}


//==================================================================================================
void
AgisXExchangeViewNode::render_inspector() noexcept
{
	ImGui::Text("Exchange View Query Type: ");
	ImGui::SetItemDefaultFocus();
	for (auto& [opp_str, opp] : Agis::AST::ExchangeViewSortNode::ExchangeQueryTypeMap()) {
		if (ImGui::RadioButton(opp_str.c_str(), _query_type == opp)) {
			_query_type = opp;
			setDirty(true);
		}
	}
	ImGui::Text("N: ");
	if (ImGui::InputInt("##N", &_n)) {
		setDirty(true);
	}
}


//==================================================================================================
bool AgisXExchangeViewNode::deserialize(nged::Json const& json)
{
	if (nged::Node::deserialize(json)) {
		auto query_opt = deserialize_string(json, "query_type");
		if (!query_opt) {
			return false;
		}
		auto& query_string = *query_opt;
		auto it = Agis::AST::ExchangeViewSortNode::ExchangeQueryTypeMap().find(query_string);
		if (it == Agis::AST::ExchangeViewSortNode::ExchangeQueryTypeMap().end()) {
			return false;
		}
		_query_type = it->second;
		auto n_opt = deserialize_string(json, "n");
		if (!n_opt) {
			return false;
		}
		try {
			_n = std::stoi(*n_opt);
		}
		catch (...) {
			return false;
		}
	}
	return false;
}


//==================================================================================================
bool AgisXExchangeViewNode::serialize(nged::Json& json) const
{
	// find query type string in map
	auto it = std::find_if(
		Agis::AST::ExchangeViewSortNode::ExchangeQueryTypeMap().begin(),
		Agis::AST::ExchangeViewSortNode::ExchangeQueryTypeMap().end(),
		[this](auto const& pair) {
			return pair.second == _query_type;
		}
	);
	if (it == Agis::AST::ExchangeViewSortNode::ExchangeQueryTypeMap().end()) {
		return false;
	}
	AgisX::serialize_pair(json, "query_type", it->first);
	AgisX::serialize_pair(json, "n", std::to_string(_n));
	return nged::Node::serialize(json);
}

}