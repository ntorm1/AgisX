#pragma once


#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
#include "AgisXAppComp.h"


namespace AgisX
{

//============================================================================
class AgisXPortfolioViewPrivate : public AppComponent
{

private:
	std::optional<AgisX::AgisXPortfolioPlot*>	_plot = std::nullopt;
	std::optional<Agis::Strategy*>				_selected_strategy = std::nullopt;
	std::optional<Agis::Portfolio*>				_selected_portfolio = std::nullopt;

public:
	AgisXPortfolioViewPrivate(AgisX::AppState& _app_state);
	~AgisXPortfolioViewPrivate();
	AppComponentType type() const noexcept { return AppComponentType::PORTFOLIO_VIEW; }
	void draw_portfolio_tree(Agis::Portfolio const& portfolio);
	void draw_portfolio_node(Agis::Portfolio const& portfolio);
	void draw_strategy();
	void draw_new_portfolio();
	void draw_new_strategy();
	void draw_book(Agis::Portfolio const& portfolio);

	void on_portfolio_click(Agis::Portfolio const& portfolio);
	void on_strategy_click(Agis::Strategy const& strategy);
	void on_hydra_restore() noexcept override;


};

} // namespace AgisX