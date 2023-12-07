#include "nodes.h"


//============================================================================
nged::GraphPtr
MyNodeFactory::createRootGraph(nged::NodeGraphDoc* root, std::optional<nged::NodePtr> output) const
{
    return std::make_shared<nged::Graph>(root, nullptr, "root");
}


//============================================================================
nged::NodePtr MyNodeFactory::createNode(nged::Graph* parent, std::string_view type) const
{
    std::string typestr(type);
    if (type == "subgraph")
        return std::make_shared<SubGraphNode>(parent);

    for (auto const& d : defs)
        if (d.type == type)
            return std::make_shared<DummyNode>(d.numinput, d.numoutput, parent, typestr, typestr);

    return std::make_shared<DummyNode>(4, 1, parent, typestr, typestr);
}


//============================================================================
void MyNodeFactory::listNodeTypes(
    nged::Graph* graph,
    void* context,
    void(*ret)(
        void* context,
        nged::StringView category,
        nged::StringView type,
        nged::StringView name)) const
{
    ret(context, "subgraph", "subgraph", "subgraph");
    for (auto const& d : defs)
        ret(context, "demo", d.type, d.type);
}


//============================================================================
DummyNode::DummyNode(int numInput, int numOutput, nged::Graph* parent, std::string const& type, std::string const& name)
    : nged::Node(parent, type, name)
    , numInput(numInput)
    , numOutput(numOutput)
{
}


//============================================================================
nged::sint DummyNode::numMaxInputs() const
{
    return numInput;
}


//============================================================================
nged::sint DummyNode::numOutputs() const
{
    return numOutput;
}


//============================================================================
bool DummyNode::acceptInput(nged::sint port, nged::Node const* srcNode, nged::sint srcPort) const
{
    // to test the sanity check
    if (srcNode->type() == "picky" && type() == "picky")
        return false;
    return true;
}


//============================================================================
SubGraphNode::SubGraphNode(nged::Graph* parent)
    : DummyNode(1, 1, parent, "subgraph", "subgraph")
{
    subgraph_ = std::make_shared<nged::Graph>(parent->docRoot(), parent, "subgraph");
}


//============================================================================
nged::Graph* SubGraphNode::asGraph()
{
    return subgraph_.get();
}


//============================================================================
nged::Graph const* SubGraphNode::asGraph() const
{
    return subgraph_.get();
}


//============================================================================
bool SubGraphNode::serialize(nged::Json& json) const
{
    return DummyNode::serialize(json) && subgraph_->serialize(json);
}


//============================================================================
bool SubGraphNode::deserialize(nged::Json const& json)
{
    return DummyNode::deserialize(json) && subgraph_->deserialize(json);
}
