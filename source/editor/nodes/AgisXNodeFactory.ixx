module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"
#include "AgisDeclare.h"

export module AgisXNodeFactory;

import <memory>;
import <string_view>;


namespace AgisX
{

export struct AgisXNodeDef
{
	char const* type;
	char const* name;
	int			numInputs;
};

export class AgisxNodeFactory: public nged::NodeFactory
{
private:
	mutable bool strategy_node_set = false;

public:
	AgisxNodeFactory();
	AgisX::AppState& _instance;

	nged::GraphPtr createRootGraph(nged::NodeGraphDoc* doc) const override;
	nged::NodePtr createNode(nged::Graph* parent, std::string_view type) const override;
	nged::NodePtr createStrategyNode(
		nged::Graph* parent,
		Agis::ASTStrategy& strategy
	) const;
	void listNodeTypes(
		nged::Graph* graph,
		void* context,
		void (*ret)(void* context,
			nged::StringView category,
			nged::StringView type,
			nged::StringView name)
	) const override;

};

}