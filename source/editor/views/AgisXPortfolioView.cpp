#pragma once

#include "AgisXPortfolioView.h"
#include "AgisXPrivatePortfolio.h"

namespace AgisX
{

AgisXPortfolioView::~AgisXPortfolioView()
{
	delete _p;
}
    
AgisXPortfolioView::AgisXPortfolioView(
    AgisX::AppState& app_state,
    nged::NodeGraphEditor* editor
) : ImGuiGraphView(editor, nullptr),
    _app_state(app_state),
    _master_portfolio(app_state.get_portfolio("master").value())
{
    _p = new AgisX::AgisXPortfolioViewPrivate(app_state);
    setTitle("Portfolio");
}

//============================================================================
void
AgisXPortfolioView::on_hydra_restore()
{
    // unique lock on mutex already aquired by AppState
    _master_portfolio = _app_state.get_portfolio("master").value();
    _p->on_hydra_restore();
    write_unlock();
}


//============================================================================
void
AgisXPortfolioView::on_hydra_build() noexcept
{
    _p->on_hydra_build();
}

//============================================================================
void
AgisXPortfolioView::drawContent()
{
    auto lock = std::shared_lock(_mutex);
    _p->draw_portfolio_tree(*_master_portfolio);
}

}