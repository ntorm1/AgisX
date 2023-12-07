module;
#include "../nged_imgui.h"
#include "AgisXSerialize.h"
#include "AgisMacros.h"

module AgisXStrategyNodeMod;

import <filesystem>;

import AgisXApp;
import StrategyNode;
import AllocationNode;

namespace AgisX
{

//==================================================================================================
void AgisXAllocationNode::render_inspector() noexcept
{
	ImGui::Text("Allocation Type: ");
	ImGui::SetItemDefaultFocus();
	for (auto& [opp_str, opp] : Agis::AST::AllocationNode::AllocTypeMap()) {
		if (ImGui::RadioButton(opp_str.c_str(), _alloc_type == opp)) {
			_alloc_type = opp;
			setDirty(true);
		}
	}
}


//==================================================================================================
bool AgisXAllocationNode::acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const
{
	if (sourceNode->type() != "ExchangeViewNode")
	{
		return false;
	}
	_ev_input = static_cast<AgisXExchangeViewNode const*>(sourceNode);
	return true;
}


//==================================================================================================
std::expected<UniquePtr<Agis::AST::AllocationNode>, Agis::AgisException>
AgisXAllocationNode::to_agis() const noexcept
{
	return std::unexpected(Agis::AgisException("not implemented"));
}


//==================================================================================================
bool AgisXAllocationNode::deserialize(nged::Json const& json)
{
	if (nged::Node::deserialize(json)) {
		auto alloc_opt = deserialize_string(json, "alloc_type");
		if (!alloc_opt) {
			return false;
		}
		if (!Agis::AST::AllocationNode::AllocTypeMap().contains(*alloc_opt)) {
			return false;
		}
		_alloc_type = Agis::AST::AllocationNode::AllocTypeMap().at(*alloc_opt);
		return true;
	}
	return false;
}


//==================================================================================================
bool AgisXAllocationNode::serialize(nged::Json& json) const
{
	AgisX::serialize_pair(
		json,
		"alloc_type",
		Agis::AST::AllocationNode::AllocTypeToString(_alloc_type).value()
	);
	return nged::Node::serialize(json);
}


//==================================================================================================
bool AgisXStrategyNode::onSave() noexcept
{
	auto res = to_agis();
	if (!res)
	{
		app().errorf("failed to rebuild AST strategy: {}", res.error().what());
	}
	return true;
}

//==================================================================================================
bool
AgisXStrategyNode::acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const
{
	if (sourceNode->type() != "AllocationNode")
	{
		return false;
	}
	_alloc_input = static_cast<AgisXAllocationNode const*>(sourceNode);
	return true;
}

//==================================================================================================
void
AgisXStrategyNode::render_inspector() noexcept
{
	ImGui::Text("Strategy ID: %s", strategy().get_strategy_id().c_str());
}


//==================================================================================================
std::expected<bool, Agis::AgisException>
AgisXStrategyNode::to_agis() const noexcept
{
	app().infof("rebuilding AST strategy: {}", strategy().get_strategy_id());
	if (!_alloc_input)
	{
		return std::unexpected(Agis::AgisException("no allocation input"));
	}
	AGIS_ASSIGN_OR_RETURN(alloc_node, (*_alloc_input)->to_agis());
	_strategy.set_alloc_node(std::move(alloc_node));
	_strategy.set_epsilon(_alloc_epsilon);
	app().infof("AST strategy: {} built", strategy().get_strategy_id());
	return true;
}


//==================================================================================================
std::expected<bool, Agis::AgisException>
AgisXStrategyNode::on_strategy_changed() noexcept
{
	parent()->docRoot()->open(strategy().graph_file_path());
	return true;
}

}