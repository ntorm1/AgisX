module;
#include <Eigen/Dense>
#include "AgisMacros.h"
#include "../nged_imgui.h"
#include "../ngdoc.h"

#include "AgisXSerialize.h"

#include "AgisAST.h"

module AgisXExchangeNodeMod;
import AgisXAssetNodeMod;
import AgisXStrategyNodeMod;
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
std::vector<std::string> AgisXExchangeNode::copy_columns() const noexcept
{
	auto exchange_opt = app().get_exchange(_exchange_name);
	if(!exchange_opt) {
		return {};
	}
	return (*exchange_opt)->get_columns();
}


//==================================================================================================
void
AgisXExchangeNode::render_inspector() noexcept
{
	// create text upload for exchange name
	ImGui::Text("Exchange Name: %s", _exchange_name.c_str());
}


//==================================================================================================
std::expected<UniquePtr<Agis::AST::ExchangeViewSortNode>, Agis::AgisException>
AgisXExchangeViewNode::to_agis() const noexcept
{
	if (!_asset_input) {
		return std::unexpected(Agis::AgisException("no asset input"));
	}
	AGIS_ASSIGN_OR_RETURN(asset_node, (*_asset_input)->to_agis());
	auto& agisx_exchange_node = parent_strategy_node().get_exchange_node();
	AGIS_ASSIGN_OR_RETURN(exchange_node, agisx_exchange_node.to_agis());
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
	if (sourceNode->type() != "AssetOpNode")
	{
		nged::MessageHub::errorf("expected AssetOpNode, found {}", sourceNode->type());
		return false;
	}
	_asset_input = static_cast<AgisXAssetOpNode const*>(sourceNode);
	return true;
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
		return true;
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