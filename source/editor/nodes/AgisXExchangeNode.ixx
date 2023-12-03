module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXExchangeNodeMod;

import AgisXNode;

namespace AgisX
{

export class AgisXExchangeNode : public AgisXNode
{
public:
	// template variadic constructor forward to base class
	template<typename... Args>
	AgisXExchangeNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	nged::sint numOutputs() const override { return 1; }
	void render_inspector() noexcept override;
	void on_render_deactivate() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool deserialize(nged::Json const& json) override;

	std::string const& exchangeName() const { return _exchange_name; }
	bool exchange_exists() const noexcept;
	std::vector<std::string> get_columns() const noexcept;

private:
	std::string _exchange_name = "";

};


export class AgisXExchangeViewNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXExchangeViewNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	virtual void render_inspector() noexcept override {}
	bool deserialize(nged::Json const& json) override { return nged::Node::deserialize(json); }
	bool serialize(nged::Json& json) const override { return nged::Node::serialize(json); }
};

}