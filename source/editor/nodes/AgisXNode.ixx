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
	static Application* _instance;
	static void set_instance(Application* instance);
	int _intputs = -1;
	bool _dirty = false;
	bool _editable = true;

protected:
	Application const& app() const { return *_instance; }

public:
	AgisXNode(nged::Graph* parent, nged::StringView type, nged::StringView name, int num_inputs);
	virtual ~AgisXNode() = default;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool getIcon(nged::IconType& iconType, nged::StringView& iconData) const override;
	virtual void render_inspector() noexcept = 0;

	bool        dirty() const { return _dirty; }
	void setDirty(bool dirty) { _dirty = dirty; }
	bool editable() const { return _editable; }
	virtual nged::sint numMaxInputs() const override { return _intputs; }
};

export class AgisXExchangeNode : public AgisXNode
{
public:
	// template variadic constructor forward to base class
	template<typename... Args>
	AgisXExchangeNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	nged::sint numOutputs() const override { return 1; }
	void render_inspector() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool deserialize(nged::Json const& json) override;
	std::string const& exchangeName() const { return _exchange_name; }

private:
	std::string _exchange_name = "";

};


export class AgisXAssetReadNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXAssetReadNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	nged::sint numOutputs() const override { return 1; }

	auto const& column() const { return _column; }
	int getIndex() const { return _index; }

	void render_inspector() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool deserialize(nged::Json const& json) override;

private:
	std::string _column = "";
	int _index = 0;
};


export class AgisXAssetOpNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXAssetOpNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	nged::sint numOutputs() const override{ return 1; }

	Agis::AST::AgisOperator getOperator() const { return _opp; }
	auto get_opp() const { return _opp; }

	void render_inspector() noexcept override;
	virtual bool serialize(nged::Json& json) const override;
	virtual bool deserialize(nged::Json const& json) override;


private:
	Agis::AST::AgisOperator _opp = Agis::AST::AgisOperator::INIT;

};

export class AgisXExchangeViewNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXExchangeViewNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}

	virtual bool acceptInput(nged::sint port, Node const* sourceNode, nged::sint sourcePort) const override;
	virtual void render_inspector() noexcept override {}
};


export class AgisXAllocationNode : public AgisXNode
{
public:
	template<typename... Args>
	AgisXAllocationNode(Args&&... args) : AgisXNode(std::forward<Args>(args)...) {}
	virtual void render_inspector() noexcept override {}
};


}