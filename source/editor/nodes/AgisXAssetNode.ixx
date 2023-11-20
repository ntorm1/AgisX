module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXAssetNodeMod;

import AgisXNode;

namespace AgisX
{

export class AgisXAssetReadNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXAssetReadNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	nged::sint numOutputs() const override { return 1; }

	auto const& column() const { return _column; }
	int getIndex() const { return _index; }

	void render_inspector() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool deserialize(nged::Json const& json) override;

private:
	int _column = 0;
	int _index = 0;
};


export class AgisXAssetOpNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXAssetOpNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	nged::sint numOutputs() const override { return 1; }

	Agis::AST::AgisOperator getOperator() const { return _opp; }
	auto get_opp() const { return _opp; }

	void render_inspector() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool deserialize(nged::Json const& json) override;


private:
	Agis::AST::AgisOperator _opp = Agis::AST::AgisOperator::INIT;

};


}