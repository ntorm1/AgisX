#pragma once

#include "nged_declare.h"
#include <imgui_internal.h>

namespace nged {


// View {{{
class GraphView : public std::enable_shared_from_this<GraphView>
{
protected:
	NodeGraphDocPtr  doc_ = nullptr;
	WeakGraphPtr     graph_;
	String           kind_ = "unknown"; // can be "network", "inspector" ...
	String           title_ = "untitled";
	size_t           id_ = 0;
	bool             open_ = true;
	bool             isFocused_ = false;
	bool             isHovered_ = false;
	NodeGraphEditor* editor_ = nullptr;
	mutable std::shared_mutex _mutex;
	friend class ViewFactory;

public:
	GraphView(NodeGraphEditor* editor, NodeGraphDocPtr doc);
	virtual void postInit() {}

	NodeGraphDocPtr  doc() const { return doc_; }
	GraphPtr         graph() const { return graph_.lock(); }
	NodeGraphEditor* editor() const { return editor_; }
	StringView       kind() const { return kind_; }
	String const& title() const { return title_; }
	void             setTitle(String title) { title_ = std::move(title); }
	size_t           id() const { return id_; }
	bool             isOpen() const { return open_; }
	void             setOpen(bool open) { open_ = open; }
	void             setFocused(bool focus) { isFocused_ = focus; }
	bool             isFocused() const { return isFocused_; }
	void             setHovered(bool hovered) { isHovered_ = hovered; }
	bool             isHovered() const { return isHovered_; }
	bool             readonly() const;

	virtual ~GraphView() = default;
	virtual float dpiScale() const { return 1.0f; }
	virtual Vec2  defaultSize() const { return { 800, 600 }; }
	virtual void  reset(NodeGraphDocPtr doc);
	virtual void  reset(WeakGraphPtr graph);
	virtual void  update(float dt);
	virtual void  onDocModified() = 0;
	virtual void  onGraphModified() = 0;
	virtual void  draw() = 0;

	virtual void on_hydra_restore() { _mutex.unlock(); };
	virtual void on_step() {};
	virtual void on_reset() {};

	void write_lock()
	{
		_mutex.lock();
	}

	void write_unlock()
	{
		_mutex.unlock();
	}

	// events are customable, may be "focus", "select", "delete", ...
	virtual void onViewEvent(GraphView* view, StringView eventType) {}
	// try to execute certain command, but anything can happen in response,
	// so make sure the caller and callee are friends and they know each other well.
	virtual void please(StringView request) {}

	virtual bool hasMenu() const { return false; }
	virtual void updateMenu() {}
};

using GraphViewPtr = std::shared_ptr<GraphView>;


}