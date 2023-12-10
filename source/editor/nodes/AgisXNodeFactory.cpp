module;

#include "../ngdoc.h"
#include "../nged_imgui.h"
#include "../../app/AgisXDeclare.h"

module AgisXNodeFactory;

import AgisXGraph;

import AgisXNode;
import AgisXExchangeNodeMod;
import AgisXAssetNodeMod;

using namespace nged;



namespace AgisX
{


static const AgisXNodeDef builtinNodeDefs_[] = {
    {"ExchangeViewNode", "Exchange View", 1},
    {"AssetReadNode", "Asset Read",0},
    {"AssetOpNode", "Asset Op",2},
    {"AllocationNode", "Allocation", 1},
    {"StrategyNode", "StrategyNode", 1}
};


//==================================================================================================
AgisxNodeFactory::AgisxNodeFactory() : _instance(nged::appState())
{
    nged::Node::set_instance(&nged::appState());
}


//==================================================================================================
nged::GraphPtr
AgisxNodeFactory::createRootGraph(
    nged::NodeGraphDoc* doc,
    std::optional<Agis::ASTStrategy*> strategy) const
{
    return std::make_shared<AgisXGraph>(doc, nullptr, "root", strategy);
}


//==================================================================================================
nged::NodePtr
AgisxNodeFactory::createNode(nged::Graph* parent, std::string_view type) const
{
    // prevent strategy node creation
    if (!strategy_node) 
    {
        nged::MessageHub::error("Strategy not set");
        return nullptr;
    }
    if (type == "StrategyNode")
    {
        if (_strategy_node_count > 0)
        {
			nged::MessageHub::error("Strategy node already created");
			return nullptr;
		}
        _strategy_node_count++;
        return strategy_node;
    }

    // verify strategy node. 
    auto graph = dynamic_cast<AgisXGraph*>(parent);
    if (!graph->strategy_node())
    {
        nged::MessageHub::error("Strategy node is not set");
        return nullptr;
    }
    auto base_strategy_ptr = dynamic_cast<AgisXStrategyNode const*>(graph->strategy_node());
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
        nged::MessageHub::errorf("Unknown/Unexpected node type: {}", type);
        return nullptr;
    }
}


//==================================================================================================
nged::NodePtr
AgisxNodeFactory::createStrategyNode(nged::Graph* parent, Agis::ASTStrategy& strategy) const
{
    strategy_node = std::make_shared<AgisXStrategyNode>(
        parent, "StrategyNode", "Strategy", strategy, 1
    );
    return strategy_node;
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