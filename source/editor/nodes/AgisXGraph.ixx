module;

#include "../ngdoc.h"

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
	std::shared_ptr<AgisXNode> outputNode() const;

	virtual void clear() override;
	virtual void remove(nged::HashSet<nged::ItemID> const& items) override;
	virtual nged::ItemID add(nged::GraphItemPtr item) override;
	virtual void removeLink(nged::ItemID destItem, nged::sint destPort) override;
	virtual nged::LinkPtr setLink(nged::ItemID sourceItem, nged::sint sourcePort, nged::ItemID destItem, nged::sint destPort)	override;
	
	void markNodeAndDownstreamDirty(nged::ItemID id);
	
};




}
