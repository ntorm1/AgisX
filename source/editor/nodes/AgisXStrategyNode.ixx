module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXStrategyNodeMod;

import AgisXNode;
import AgisXExchangeNodeMod;
import AllocationNode;

namespace AgisX
{

export class AgisXStrategyNode 
	: public AgisXNode<UniquePtr<Agis::AST::StrategyNode>>
{
public:
	using AgisType = UniquePtr<Agis::AST::StrategyNode>;
	template<typename... Args>
	AgisXStrategyNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	nged::sint numOutputs() const override { return 0; }

	void render_inspector() noexcept override {}

	std::expected<UniquePtr<Agis::AST::StrategyNode>, Agis::AgisException> to_agis() const noexcept override;
	bool deserialize(nged::Json const& json) override { return nged::Node::deserialize(json); }
	bool serialize(nged::Json& json) const override { return nged::Node::serialize(json); }
};


export class AgisXAllocationNode 
	: public AgisXNode<UniquePtr<Agis::AST::AllocationNode>>
{
public:
	template<typename... Args>
	AgisXAllocationNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	void render_inspector() noexcept override;

	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	std::expected<UniquePtr<Agis::AST::AllocationNode>, Agis::AgisException> to_agis() const noexcept override;
	bool deserialize(nged::Json const& json) override;
	bool serialize(nged::Json& json) const override;

private:
	Agis::AST::AllocType _alloc_type = Agis::AST::AllocType::UNIFORM;
	mutable std::optional<AgisX::AgisXExchangeViewNode const*> _ev_input = std::nullopt;


};





}