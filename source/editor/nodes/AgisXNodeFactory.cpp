module;

#include "../ngdoc.h"
#include "../nged_imgui.h"
#include "../../app/AgisXDeclare.h"

module AgisXNodeFactory;

import AgisXGraph;

import AgisXNode;
import AgisXExchangeNodeMod;
import AgisXAssetNodeMod;
import AgisXStrategyNodeMod;

using namespace nged;



namespace AgisX
{

static const AgisXNodeDef builtinNodeDefs_[] = {
	{"ExchangeNode", "Exchange",0},
    {"ExchangeViewNode", "Exchange View", 2},
    {"AssetReadNode", "Asset Read",1},
    {"AssetOpNode", "Asset Op",2},
    {"AllocationNode", "Allocation", 1}//,
    //{"StrategyNode", "StrategyNode", 0}
};


//==================================================================================================
AgisxNodeFactory::AgisxNodeFactory() : _instance(nged::appState())
{
    AgisXNode::set_instance(&nged::appState());
}


//==================================================================================================
nged::GraphPtr AgisxNodeFactory::createRootGraph(nged::NodeGraphDoc* doc) const
{
    return std::make_shared<AgisXGraph>(doc, nullptr, "root");
}


//==================================================================================================
nged::NodePtr
AgisxNodeFactory::createNode(nged::Graph* parent, std::string_view type) const
{
    if (type == "ExchangeNode")
		return std::make_shared<AgisXExchangeNode>(parent, type, "Exchange", 0);
    else if (type == "AssetReadNode")
        return std::make_shared<AgisXAssetReadNode>(parent, type, "Asset Read", 1);
	else if (type == "AssetOpNode")
        return std::make_shared<AgisXAssetOpNode>(parent, type, "Asset Op", 2);
    else if (type == "AllocationNode")
		return std::make_shared<AgisXAllocationNode>(parent, type, "Allocation", 1);
    else if (type == "StrategyNode")
        return std::make_shared<AgisXStrategyNode>(parent, type, "Strategy", 0);
    else if (type == "ExchangeViewNode")
		return std::make_shared<AgisXExchangeViewNode>(parent, type, "Exchange View", 2);
    else {
        throw std::runtime_error("unknown node type");
    }
}

void AgisxNodeFactory::listNodeTypes(
    Graph* graph,
    void* context,
    void(*ret)(void* context, nged::StringView category, nged::StringView type, nged::StringView name)) const
{
    for (auto* def = builtinNodeDefs_;
        def < builtinNodeDefs_ + sizeof(builtinNodeDefs_) / sizeof(*builtinNodeDefs_);
        ++def) {
        ret(context, "node", def->type, def->name);
    }
}



}