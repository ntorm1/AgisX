#pragma once
#include "../../app/AgisXDeclare.h"

#include <vector>
#include <optional>

namespace AgisX
{
enum AppComponentType
{
	APP_STATE,
	PLOT_BASE,
	PORTFOLIO_PLOT,
	PORTFOLIO_VIEW,
	ASSET_PLOT,
	ASSET_VIEW,
	EXCHANGE_VIEW
};

class AppComponent
{
private:
	AgisX::AppState& _app_state;
	std::optional<AppComponent const*> _parent;
	std::vector<AppComponent*> _children;

protected:
	auto& app() const noexcept { return _app_state; }

public:
	AppComponent() = delete;
	AppComponent(
		AgisX::AppState& app_state,
		std::optional<AppComponent const*> parent
	)
		: _parent(parent), _app_state(app_state)
	{}

	virtual ~AppComponent() = default;

	virtual AppComponentType type() const noexcept = 0;
	virtual void on_hydra_restore() noexcept = 0;
};

}