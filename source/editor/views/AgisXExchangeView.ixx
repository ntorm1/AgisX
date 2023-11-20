module;
#pragma once
#include "../nged_imgui.h"
#include "../nged_graph_view.h"

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
export module AgisXExchangeViewModule;


namespace AgisX {

//============================================================================
export class AgisXExchangeView 
    : public nged::ImGuiGraphView<AgisXExchangeView, nged::GraphView>
{
public:
    AgisXExchangeView(
        AgisX::Application& app,
        nged::NodeGraphEditor* editor
    ) : ImGuiGraphView(editor, nullptr), _app(app)
    {
        setTitle("Exchanges");
    }
    void drawContent(){}
    void onDocModified() override {}
    void onGraphModified() override {}

private:
    AgisX::Application& _app;

    mutable std::shared_mutex _mutex;
    std::optional<Agis::Exchange const*> _selected_exchange;

    Agis::ExchangeMap const* _exchanges;
    std::unordered_map<std::string, size_t> const* _exchange_ids = nullptr;

};

}