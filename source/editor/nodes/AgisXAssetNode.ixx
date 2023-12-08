module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXAssetNodeMod;

import AgisXNode;

namespace AgisX
{

export class AgisXAssetReadNode 
	: public AgisXNode<UniquePtr<Agis::AST::AssetLambdaNode>>
{
public:
	using AgisType = UniquePtr<Agis::AST::AssetLambdaNode>;

	template<typename... Args>
	AgisXAssetReadNode(Args&&... args) : AgisXNode<AgisType>(std::forward<Args>(args)...) 
	{
		setColor(gmath::hexCodeToSRGB(Color::YELLOW_HEXCODE));
	}
	nged::sint numOutputs() const override { return 1; }
	nged::sint numMaxInputs() const override { return 0; }

	auto const& column() const { return _column; }
	int getIndex() const { return _index; }

	std::expected<UniquePtr<Agis::AST::AssetLambdaNode>, Agis::AgisException> to_agis() const noexcept;
	void render_inspector() noexcept override;
	bool serialize(nged::Json& json) const override;
	bool deserialize(nged::Json const& json) override;

private:
	int _column = 0;
	int _index = 0;
};


export class AgisXAssetOpNode 
	: public AgisXNode<UniquePtr<Agis::AST::AssetLambdaNode>>
{
public:
	using AgisType = UniquePtr<Agis::AST::AssetLambdaNode>;


	template<typename... Args>
	AgisXAssetOpNode(Args&&... args) : AgisXNode<AgisType>(std::forward<Args>(args)...) {}
	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	nged::sint numOutputs() const override { return 1; }

	Agis::AST::AgisOperator getOperator() const { return _opp; }
	auto get_opp() const { return _opp; }

	std::expected<UniquePtr<Agis::AST::AssetLambdaNode>, Agis::AgisException> to_agis() const noexcept;
	void render_inspector() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	bool deserialize(nged::Json const& json) override;


private:
	Agis::AST::AgisOperator _opp = Agis::AST::AgisOperator::INIT;
	mutable std::optional<nged::Node const*> _left = std::nullopt;
	mutable std::optional<nged::Node const*> _right = std::nullopt;
};


}