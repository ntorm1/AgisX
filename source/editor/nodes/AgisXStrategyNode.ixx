module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXStrategyNodeMod;

import AgisXNode;
import AgisXExchangeNodeMod;
import AllocationNode;
import ASTStrategyModule;

namespace AgisX
{

export class AgisXStrategyNode 
	: public AgisXNode<UniquePtr<Agis::AST::StrategyNode>>
{
	friend class AgisxNodeFactory;

private:
	SharedPtr<AgisXExchangeNode> _exchange_node = nullptr;
	Agis::ASTStrategy const& strategy() const noexcept { return _strategy; }
	Agis::ASTStrategy& _strategy;

public:
	using AgisType = UniquePtr<Agis::AST::StrategyNode>;

	AgisXStrategyNode(nged::Graph* parent,
		nged::StringView type,
		nged::StringView name,
		Agis::ASTStrategy& strategy,
		int num_inputs) : 
		AgisXNode(parent, type, name, *this, num_inputs), 
		_strategy(strategy)
	{
		_exchange_node = std::make_shared<AgisXExchangeNode>(
			parent, "ExchangeNode", "Exchange", *this, 0, strategy.get_exchange_id()
		);
	}

	nged::sint numOutputs() const override { return 0; }
	void render_inspector() noexcept override;
	AgisXExchangeNode const& get_exchange_node() const noexcept { return *_exchange_node.get(); }
	std::expected<UniquePtr<Agis::AST::StrategyNode>, Agis::AgisException> to_agis() const noexcept override;
	bool deserialize(nged::Json const& json) override { return nged::Node::deserialize(json); }
	bool serialize(nged::Json& json) const override { return nged::Node::serialize(json); }
	std::string const& graph_file_path() const noexcept { return strategy().graph_file_path(); }
	std::expected<bool, Agis::AgisException> on_strategy_changed() noexcept;
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