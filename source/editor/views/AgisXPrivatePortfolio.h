#pragma once


#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"

#include <optional>

namespace AgisX
{

//============================================================================
class AgisXPortfolioViewPrivate
{
public:
	AgisXPortfolioViewPrivate(AgisX::AppState& _app_stat) : _app_state(_app_stat) {}
	~AgisXPortfolioViewPrivate();

	void draw_portfolio_tree(Agis::Portfolio const& portfolio);
	void draw_portfolio_node(Agis::Portfolio const& portfolio);
	void draw_new_portfolio();
	void draw_new_strategy();
	void on_hydra_restore() noexcept
	{
		_selected_strategy = std::nullopt;
		_selected_portfolio = std::nullopt; 
	}
private:
	AgisX::AppState& _app_state;

	std::optional<Agis::Strategy*> _selected_strategy = std::nullopt;
	std::optional<Agis::Portfolio*> _selected_portfolio = std::nullopt;
};

} // namespace AgisX