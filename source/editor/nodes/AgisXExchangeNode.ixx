module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXExchangeNodeMod;

import AgisXNode;
import ExchangeNode;

namespace AgisX
{

export class AgisXExchangeNode 
	: public AgisXNode<SharedPtr<Agis::AST::ExchangeNode const>>
{
public:
	using AgisType = SharedPtr<Agis::AST::ExchangeNode const>;
	// template variadic constructor forward to base class
	AgisXExchangeNode(nged::Graph* parent,
		nged::StringView type,
		nged::StringView name,
		AgisX::AgisXStrategyNode& strategy_node,
		int num_inputs,
		std::string exchange_id)
		: AgisXNode(parent, type, name, strategy_node, num_inputs)
	{
		_exchange_name = exchange_id;
		_columns = copy_columns();
	}

	nged::sint numOutputs() const override { return 1; }
	void render_inspector() noexcept override;
	bool serialize(nged::Json& json) const override;
	bool deserialize(nged::Json const& json) override;
	std::expected<SharedPtr<Agis::AST::ExchangeNode const>, Agis::AgisException> to_agis() const noexcept override;


	std::string const& exchangeName() const { return _exchange_name; }
	std::vector<std::string> const& get_columns() const noexcept { return _columns; }
	std::vector<std::string> copy_columns() const noexcept;

private:
	std::vector<std::string> _columns;
	std::string _exchange_name = "";
};


export class AgisXExchangeViewNode : 
	public AgisXNode<UniquePtr<Agis::AST::ExchangeViewSortNode>>
{
public:
	template<typename... Args>
	AgisXExchangeViewNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	std::expected<UniquePtr<Agis::AST::ExchangeViewSortNode>, Agis::AgisException> to_agis() const noexcept override;
	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	virtual void render_inspector() noexcept override;
	bool deserialize(nged::Json const& json) override;
	bool serialize(nged::Json& json) const override;

private:
	Agis::AST::ExchangeQueryType _query_type = Agis::AST::ExchangeQueryType::Default;
	int _n = -1;
	mutable std::optional<AgisX::AgisXAssetOpNode const*> _asset_input = std::nullopt;
};


}