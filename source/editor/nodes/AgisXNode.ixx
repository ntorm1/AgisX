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

	export namespace Color
	{
		constexpr const char* RED_HEXCODE = "#FF0000";
		constexpr const char* GREEN_HEXCODE = "#00FF00";
		constexpr const char* BLUE_HEXCODE = "#0000FF";
		constexpr const char* YELLOW_HEXCODE = "#FFFF00";
		constexpr const char* PURPLE_HEXCODE = "#800080";
		constexpr const char* ORANGE_HEXCODE = "#FFA500";
		constexpr const char* CYAN_HEXCODE = "#00FFFF";
		constexpr const char* MAGENTA_HEXCODE = "#FF00FF";
		constexpr const char* PINK_HEXCODE = "#FFC0CB";
		constexpr const char* LIME_HEXCODE = "#00FF00";
		constexpr const char* TEAL_HEXCODE = "#008080";
		constexpr const char* BROWN_HEXCODE = "#A52A2A";
		constexpr const char* NAVY_HEXCODE = "#000080";
		constexpr const char* GOLD_HEXCODE = "#FFD700";
		constexpr const char* SILVER_HEXCODE = "#C0C0C0";
		constexpr const char* GRAY_HEXCODE = "#808080";
		constexpr const char* BLACK_HEXCODE = "#000000";
		constexpr const char* WHITE_HEXCODE = "#FFFFFF";
	}

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