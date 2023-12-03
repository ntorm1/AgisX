module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXStrategyNodeMod;

import AgisXNode;

namespace AgisX
{

export class AgisXStrategyNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXStrategyNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	nged::sint numOutputs() const override { return 0; }

	void render_inspector() noexcept override {}

	bool deserialize(nged::Json const& json) override { return nged::Node::deserialize(json); }
	bool serialize(nged::Json& json) const override { return nged::Node::serialize(json); }
};


export class AgisXAllocationNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXAllocationNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	void render_inspector() noexcept override {}

	bool deserialize(nged::Json const& json) override { return nged::Node::deserialize(json); }
	bool serialize(nged::Json& json) const override { return nged::Node::serialize(json); }
};

}