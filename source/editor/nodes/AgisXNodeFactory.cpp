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
    {"ExchangeViewNode", "Exchange View", 1},
    {"AssetReadNode", "Asset Read",0},
    {"AssetOpNode", "Asset Op",2},
    {"AllocationNode", "Allocation", 1}//,
    //{"StrategyNode", "StrategyNode", 0}
};


//==================================================================================================
AgisxNodeFactory::AgisxNodeFactory() : _instance(nged::appState())
{
    nged::Node::set_instance(&nged::appState());
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
    // verify strategy node. 
    if (!strategy_node_set)
    {
        nged::MessageHub::error("Strategy node is not set");
        return nullptr;
    }
    auto graph = dynamic_cast<AgisXGraph*>(parent);
    auto base_strategy_ptr = dynamic_cast<AgisXStrategyNode const*>(graph->strategy_node().get());
    auto& base_strategy = *const_cast<AgisXStrategyNode*>(base_strategy_ptr);

    if (type == "AssetReadNode")
        return std::make_shared<AgisXAssetReadNode>(parent, type, "Asset Read", base_strategy, 1);
	else if (type == "AssetOpNode")
        return std::make_shared<AgisXAssetOpNode>(parent, type, "Asset Op", base_strategy, 2);
    else if (type == "AllocationNode")
		return std::make_shared<AgisXAllocationNode>(parent, type, "Allocation", base_strategy, 1);
    else if (type == "ExchangeViewNode")
		return std::make_shared<AgisXExchangeViewNode>(parent, type, "Exchange View", base_strategy, 2);
    else {
        throw std::runtime_error("unknown node type");
    }
}


//==================================================================================================
nged::NodePtr
AgisxNodeFactory::createStrategyNode(nged::Graph* parent, Agis::ASTStrategy& strategy) const
{
    strategy_node_set = true;
    return std::make_shared<AgisXStrategyNode>(
        parent, "StrategyNode", "Strategy", strategy, 0
    );
}


//==================================================================================================
void
AgisxNodeFactory::listNodeTypes(
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