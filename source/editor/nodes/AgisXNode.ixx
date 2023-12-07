module;

#include "../ngdoc.h"
#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXNode;

import <expected>;
import AgisError;

namespace AgisX
{
	class AgisXGraph;
	class AgisXResponser;


export template<typename AgisType>
class AgisXNode : public nged::Node
{
	friend class AgisxNodeFactory;
	friend class AgisXGraph;
	friend class AgisXResponser;

private:
	int _intputs = -1;
	bool _editable = true;
	AgisX::AgisXStrategyNode& _strategy_node;
protected:
	virtual std::expected<AgisType, Agis::AgisException> to_agis() const noexcept = 0;
	AgisX::AppState const& app() const { return *_instance; }
	AgisX::AgisXStrategyNode const& parent_strategy_node() const noexcept { return _strategy_node; }

public:
	AgisXNode(
		nged::Graph* parent,
		nged::StringView type,
		nged::StringView name,
		AgisX::AgisXStrategyNode& strategy_node,
		int num_inputs)
		: nged::Node(parent, nged::String(type), nged::String(name)),
		_strategy_node(strategy_node)
	{
		_intputs = num_inputs;
	}
	virtual ~AgisXNode() = default;
	virtual void render_inspector() noexcept = 0;
	virtual void on_render_deactivate() noexcept {};

	bool editable() const { return _editable; }
	virtual nged::sint numMaxInputs() const override { return _intputs; }
};



}