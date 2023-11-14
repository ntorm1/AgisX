#pragma once
#include "nged.h"


class DummyNode : public nged::Node
{
    int numInput = 1;
    int numOutput = 1;

public:
    DummyNode(int numInput, int numOutput, nged::Graph* parent, std::string const& type, std::string const& name);

    nged::sint numMaxInputs() const override;
    nged::sint numOutputs() const override;
    bool acceptInput(nged::sint port, nged::Node const* srcNode, nged::sint srcPort) const override;
};


class SubGraphNode : public DummyNode
{
    nged::GraphPtr subgraph_;

public:
    SubGraphNode(nged::Graph* parent);

    virtual nged::Graph* asGraph() override;
    virtual nged::Graph const* asGraph() const override;
    virtual bool serialize(nged::Json& json) const override;
    virtual bool deserialize(nged::Json const& json) override;
};

struct DummyNodeDef
{
    std::string type;
    int numinput, numoutput;
};

static DummyNodeDef defs[] = {
  { "exec", 4, 1 },
  { "null", 1, 1 },
  { "merge", -1, 1 },
  { "split", 1, 2 },
  { "picky", 3, 2 },
  { "out", 1, 0 },
  { "in", 0, 1 }
};

class MyNodeFactory : public nged::NodeFactory
{
public:
    nged::GraphPtr createRootGraph(nged::NodeGraphDoc* root) const override;
    nged::NodePtr createNode(nged::Graph* parent, std::string_view type) const override;
    void listNodeTypes(nged::Graph* graph, void* context,
        void(*ret)(void* context, nged::StringView category, nged::StringView type, nged::StringView name)) const override;
};
