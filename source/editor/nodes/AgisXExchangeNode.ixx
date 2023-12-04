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
	template<typename... Args>
	AgisXExchangeNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	nged::sint numOutputs() const override { return 1; }
	void render_inspector() noexcept override;
	void on_render_deactivate() noexcept override;
	bool serialize(nged::Json& json) const override;
	bool deserialize(nged::Json const& json) override;
	std::expected<SharedPtr<Agis::AST::ExchangeNode const>, Agis::AgisException> to_agis() const noexcept override;


	std::string const& exchangeName() const { return _exchange_name; }
	bool exchange_exists() const noexcept;
	std::vector<std::string> get_columns() const noexcept;

private:
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
	mutable std::optional<AgisX::AgisXExchangeNode const*> _exchange = std::nullopt;
};


}