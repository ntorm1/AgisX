module;
#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"
#include "AgisDeClare.h"

export module AgisXGraph;


namespace AgisX
{

class AgisXNode;

export class AgisXGraph : public nged::Graph
{
private:
	nged::Node* _strategy_node = nullptr;
	nged::ItemID outputNodeID_ = nged::ID_None;
	bool   deserializing_ = false;
	
protected:

public:
	AgisXGraph(
		nged::NodeGraphDoc* root,
		Graph* parent,
		nged::String name,
		std::optional<nged::NodePtr> outputNode = std::nullopt
	);
	//std::shared_ptr<AgisXStrategyNode> outputNode() const;

	void clear() override;
	void remove(nged::HashSet<nged::ItemID> const& items) override;
	nged::ItemID add(nged::GraphItemPtr item) override;
	void removeLink(nged::ItemID destItem, nged::sint destPort) override;
	nged::LinkPtr setLink(nged::ItemID sourceItem, nged::sint sourcePort, nged::ItemID destItem, nged::sint destPort)	override;
	
	void markNodeAndDownstreamDirty(nged::ItemID id);

	void set_strategy_node(nged::Node* node) noexcept { _strategy_node = node; }
	nged::Node* const strategy_node() const noexcept { return _strategy_node; }
};




}
