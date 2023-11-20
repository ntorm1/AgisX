module;
#include "../nged_imgui.h"
#include "../ngdoc.h"
#include "../res/fa_icondef.h"

#include "AgisXSerialize.h"
#include "AgisAST.h"

module AgisXNode;

import AgisXGraph;

using namespace nged;


namespace AgisX
{

Application* AgisXNode::_instance = nullptr;


//==================================================================================================
void
AgisXNode::add_dest(AgisXNode* p, nged::sint port) const
{
	_dests.push_back({ p, port });
}


//==================================================================================================
void
AgisXNode::remove_downstream_links() const
{
	auto parent = static_cast<AgisXGraph*>(this->parent());
	for (auto& dest : _dests) {
		auto dest_id = dest.first->id();
		parent->removeLink(dest_id, dest.second);
	}
	_dests.clear();
}


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


}