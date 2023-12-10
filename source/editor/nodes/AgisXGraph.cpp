module;

#include "../ngdoc.h"
#include "../nged.h"

module AgisXGraph;

import AgisXNode;
import AgisXNodeFactory;
import AgisXStrategyNodeMod;

using namespace nged;

namespace AgisX
{


AgisXGraph::AgisXGraph(
    NodeGraphDoc* root,
    Graph* parent,
    String name,
    std::optional<Agis::ASTStrategy*> strategy)
    : Graph(root, parent, name)
{
    if (!strategy)
    {
        nged::msghub::error("strategy is null");
        return;
    }
    nged::msghub::info("building new AgisXGraph");
    root->setDeserializeInplace(false);
    auto node_factory = static_cast<AgisX::AgisxNodeFactory const*>(this->docRoot()->nodeFactory());
    auto output_node = node_factory->createStrategyNode(
        this, *(strategy.value())
    );
    set_strategy_node(output_node.get());
    output_node->setParent(this);
    outputNodeID_ = docRoot()->addItem(output_node);
    output_node->resetID(outputNodeID_);
    items_.insert(outputNodeID_);
    nged::msghub::info("AgisXGraph build complete");
}


//==================================================================================================
//std::shared_ptr<AgisXStrategyNode> AgisXGraph::outputNode() const
//{
//	return std::static_pointer_cast<AgisXStrategyNode>(get(outputNodeID_));
//}


//==================================================================================================
void AgisXGraph::clear()
{
    for (auto id : items_)
        //if (id != outputNodeID_)
            docRoot()->removeItem(id);
    items_.clear();
    //items_.insert(outputNodeID_);
    links_.clear();
    linkIDs_.clear();
}


//==================================================================================================
void
AgisXGraph::remove(HashSet<ItemID> const& items)
{
    HashSet<ItemID> itemsToRemove = items;
    itemsToRemove.erase(outputNodeID_); // always keep output node
    Vector<ItemID> dirtySources;
    for (auto id : itemsToRemove)
        if (auto item = get(id)) {
            if (auto* link = item->asLink())
                dirtySources.push_back(link->output().destItem);
            else if (item->asNode())
                dirtySources.push_back(id);
        }
    GraphTraverseResult affected;
    if (travelTopDown(affected, dirtySources)) {
        for (auto affectedItem : affected) {
            static_cast<nged::Node*>(affectedItem.node())->setDirty(true);
        }
    }
    Graph::remove(itemsToRemove);
}


//==================================================================================================
ItemID
AgisXGraph::add(GraphItemPtr item)
{
    if (item->id() != ID_None) // if already added, e.g., output node can be added while it already exists (at creation)
        return item->id();
    return Graph::add(item);
}



//==================================================================================================
void AgisXGraph::removeLink(ItemID destItem, sint destPort)
{
    //if (auto* node = get(destItem)->asNode())
        //static_cast<AgisXNode*>(node)->settle();
    markNodeAndDownstreamDirty(destItem);
    Graph::removeLink(destItem, destPort);
}

//==================================================================================================
LinkPtr AgisXGraph::setLink(ItemID sourceItem, sint sourcePort, ItemID destItem, sint destPort)
{
    auto result = Graph::setLink(sourceItem, sourcePort, destItem, destPort);
    if (!deserializing_) { // don't check loop during deserializing
        if (Vector<ItemID> loop; checkLoopBottomUp(destItem, loop)) {
            nged::msghub::error("loop detected, please don't do this");
            removeLink(result->output().destItem, result->output().destPort);
            return nullptr;
        }
        if (auto* node = get(destItem)->asNode())
            //static_cast<S7Node*>(node)->settle();
        markNodeAndDownstreamDirty(destItem);
    }
    return result;
}


//==================================================================================================
void AgisXGraph::markNodeAndDownstreamDirty(ItemID id)
{
    GraphTraverseResult affected;
    if (travelTopDown(affected, id)) {
        for (auto affectedItem : affected) {
            static_cast<nged::Node*>(affectedItem.node())->setDirty(true);
        }
    }
    else {
        nged::msghub::errorf("cannot travel from {} to bottom", id.value());
    }
    if (parent_) {
        for (auto id : parent_->items()) {
            if (auto* node = parent_->get(id)->asNode()) {
                if (node->asGraph() == static_cast<Graph*>(this)) {
                    static_cast<AgisXGraph*>(parent_)->markNodeAndDownstreamDirty(id);
                    break;
                }
            }
        }
    }
}


}