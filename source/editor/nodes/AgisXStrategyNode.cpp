module;
#include "../nged_imgui.h"

#include "AgisXSerialize.h"

module AgisXStrategyNodeMod;

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
std::expected<UniquePtr<Agis::AST::AllocationNode>, Agis::AgisException> AgisXAllocationNode::to_agis() const noexcept
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
std::expected<UniquePtr<Agis::AST::StrategyNode>, Agis::AgisException> AgisXStrategyNode::to_agis() const noexcept
{
	return std::unexpected(Agis::AgisException("not implemented"));
}

}