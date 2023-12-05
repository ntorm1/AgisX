module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

export module AgisXGraph;


namespace AgisX
{

class AgisXNode;

export class AgisXGraph : public nged::Graph
{
private:
	nged::ItemID outputNodeID_ = nged::ID_None;
	bool   deserializing_ = false;
	
public:
	AgisXGraph(nged::NodeGraphDoc* root, Graph* parent, nged::String name);
	//std::shared_ptr<AgisXStrategyNode> outputNode() const;

	virtual void clear() override;
	virtual void remove(nged::HashSet<nged::ItemID> const& items) override;
	virtual nged::ItemID add(nged::GraphItemPtr item) override;
	virtual void removeLink(nged::ItemID destItem, nged::sint destPort) override;
	virtual nged::LinkPtr setLink(nged::ItemID sourceItem, nged::sint sourcePort, nged::ItemID destItem, nged::sint destPort)	override;
	
	void markNodeAndDownstreamDirty(nged::ItemID id);

	void set_strategy_node(nged::Node* node) noexcept { _strategy_node = node; }
	nged::Node const* strategy_node() const noexcept { return _strategy_node; }

private:
	nged::Node* _strategy_node = nullptr;
	
};




}
