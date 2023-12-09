#pragma once
#include "../../app/AgisXDeclare.h"

#include <vector>
#include <optional>

namespace AgisX
{
enum AppComponentType
{
	APP_STATE,
	PORTFOLIO_PLOT,
	PORTFOLIO_VIEW,
	ASSET_PLOT,
	ASSET_VIEW,
	EXCHANGE_VIEW
};

class AppComponent
{
private:
	std::optional<AppComponent const*> _parent;
	std::vector<AppComponent*> _children;

public:
	AppComponent() = delete;
	AppComponent(std::optional<AppComponent const*> parent)
		: _parent(parent)
	{}

	virtual ~AppComponent() = default;

	virtual AppComponentType type() const noexcept = 0;
	virtual void on_hydra_restore() noexcept = 0;
};

}