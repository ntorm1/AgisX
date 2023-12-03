module;

#pragma once
#include "../nged_imgui.h"
#include "ImGuiFileDialog.h"

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
#include "AgisXPrivatePortfolio.h"

export module AgisXPortfolioViewModule;

import AgisXApp;

namespace AgisX {

//============================================================================
export class AgisXPortfolioView
    : public nged::ImGuiGraphView<AgisXPortfolioView, nged::GraphView>
{
private:
    AgisX::AppState& _app_state;
    AgisX::AgisXPortfolioViewPrivate _p;
    Agis::Portfolio const* _master_portfolio;

public:
    AgisXPortfolioView(
        AgisX::AppState& app_state,
        nged::NodeGraphEditor* editor
    ) : ImGuiGraphView(editor, nullptr),
        _app_state(app_state),
        _master_portfolio(app_state.get_portfolio("master").value()),
        _p(app_state)
    {
        setTitle("Portfolio");
    }
    void onDocModified() override {}
    void onGraphModified() override {}

    //============================================================================
    void on_hydra_restore() override
    {    
        // unique lock on mutex already aquired by AppState
        _master_portfolio = _app_state.get_portfolio("master").value();
        write_unlock();
    }

    //============================================================================
    void drawContent()
    {
        auto lock = std::shared_lock(_mutex);
        _p.draw_portfolio_tree(*_master_portfolio);
    }
};

}
