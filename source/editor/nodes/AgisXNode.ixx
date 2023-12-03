module;

#include "../ngdoc.h"
#include "../../app/AgisXDeclare.h"

#include "AgisAST.h"

export module AgisXNode;


namespace AgisX
{
	class AgisXGraph;
	class AgisXResponser;

export class AgisXNode : public nged::Node
{
	friend class AgisxNodeFactory;
	friend class AgisXGraph;
	friend class AgisXResponser;

private:
	mutable std::vector<std::pair<AgisXNode*, nged::sint>> _dests;
	static AgisX::AppState* _instance;
	static void set_instance(AgisX::AppState* instance);
	int _intputs = -1;
	bool _dirty = false;
	bool _editable = true;

protected:
	AgisX::AppState const& app() const { return *_instance; }
	void remove_downstream_links() const;
public:
	AgisXNode(nged::Graph* parent, nged::StringView type, nged::StringView name, int num_inputs);
	virtual ~AgisXNode() = default;
	virtual bool serialize(nged::Json& json) const override = 0;
	virtual bool deserialize(nged::Json const& json) override = 0;
	virtual bool getIcon(nged::IconType& iconType, nged::StringView& iconData) const override;
	virtual void render_inspector() noexcept = 0;
	virtual void on_render_deactivate() noexcept {};

	void add_dest(AgisXNode* dest, nged::sint port) const;
	auto const& dests() const { return _dests; }
	bool        dirty() const { return _dirty; }
	void setDirty(bool dirty) { _dirty = dirty; }
	bool editable() const { return _editable; }
	virtual nged::sint numMaxInputs() const override { return _intputs; }
};


}