module;
#include "AgisMacros.h"
#include "../nged_imgui.h"
#include "../ngdoc.h"

#include "AgisXSerialize.h"
#include "AgisAST.h"

import AssetNode;

module AgisXAssetNodeMod;
import AgisXApp;
import AgisXExchangeNodeMod;
import AgisXStrategyNodeMod;


namespace AgisX
{
//==================================================================================================
static bool is_asset_node(std::string const& nonde_type) {
	return nonde_type == "AssetReadNode" || nonde_type == "AssetOpNode";
}


//==================================================================================================
bool
AgisXAssetOpNode::acceptInput(nged::sint port, nged::Node const* sourceNode, nged::sint sourcePort) const
{
	// input node can be read or operation
	if (!is_asset_node(sourceNode->type())) return false;
	if (port == 0) {
		_left = sourceNode;
	}
	else if (port == 1) 
	{
		_right = sourceNode;
	}
	return true;
}




//==================================================================================================
std::expected<UniquePtr<Agis::AST::AssetLambdaNode>, Agis::AgisException>
AgisXAssetReadNode::to_agis() const noexcept
{
	return std::make_unique<Agis::AST::AssetLambdaReadNode>(static_cast<size_t>(_column), _index);
}


//==================================================================================================
void AgisXAssetReadNode::render_inspector() noexcept
{
	static bool first_pass = true;
	auto& exchange_node = parent_strategy_node().get_exchange_node();

	ImGui::Text("exchange: %s", exchange_node.exchangeName().c_str());
	auto& _columns = exchange_node.get_columns();
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
	if (first_pass || _column != current_item || _index != current_index) {
		setDirty(true);
		auto n = name();
		rename(_columns[_column] +" " + std::to_string(_index), n);
	}

	if (first_pass) {
		first_pass = false;
	}
}


//==================================================================================================
std::expected<UniquePtr<Agis::AST::AssetLambdaNode>, Agis::AgisException>
asset_node_to_agis(nged::Node const* node)
{
	if ((node)->type() == "AssetReadNode")
	{
		auto res = static_cast<AgisXAssetReadNode const*>(node)->to_agis();
		if (!res)
		{
			return std::unexpected(
				Agis::AgisException(res.error().what())
			);
		}
		return std::move(*res);
	}
	else if ((node)->type() == "AssetOpNode")
	{
		auto res = static_cast<AgisXAssetOpNode const*>(node)->to_agis();
		if (!res)
		{
			return std::unexpected(
				Agis::AgisException(res.error().what())
			);
		}
		return std::move(*res);
	}
	else
	{
		return std::unexpected(
			Agis::AgisException("invalid left node type")
		);
	}
}



//==================================================================================================
std::expected<UniquePtr<Agis::AST::AssetLambdaNode>, Agis::AgisException>
AgisXAssetOpNode::to_agis() const noexcept
{
	UniquePtr<AssetLambdaNode> right_node = nullptr;
	std::optional<UniquePtr<AssetLambdaNode>> left_node = std::nullopt;
	if (_left)
	{
		AGIS_ASSIGN_OR_RETURN(node, asset_node_to_agis(*_left));
		left_node = std::move(node);
	}
	if (!_right)
	{
		return std::unexpected(
			Agis::AgisException("right node is null")
		);
	}
	AGIS_ASSIGN_OR_RETURN(node, asset_node_to_agis(*_right));
	right_node = std::move(node);
	return std::make_unique<Agis::AST::AssetOpperationNode>(
		std::move(left_node),
		std::move(right_node),
		_opp
	);
}


//==================================================================================================
void AgisXAssetOpNode::render_inspector() noexcept
{
	ImGui::Text("Opp Type: ");
	ImGui::SetItemDefaultFocus();
	for(auto& [opp_str, opp] : Agis::AST::AssetLambdaNode::AgisOperatorMap()) {
		if (ImGui::RadioButton(opp_str.c_str(), _opp == opp)) {
			_opp = opp;
			setDirty(true);
		}
	}
}


//==================================================================================================
bool
AgisXAssetReadNode::serialize(Json& json) const
{
	AgisX::serialize_pair(json, "column",std::to_string(_column));
	AgisX::serialize_pair(json, "index", std::to_string(_index));
	return nged::Node::serialize(json);
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
	return nged::Node::serialize(json);
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
