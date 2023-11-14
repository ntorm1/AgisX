#pragma once

#include "AgisDeclare.h"
#include "BaseComp.h"

using namespace Agis;

import AgisError;

import <shared_mutex>;
import <optional>;
import <unordered_map>;

using namespace Agis;

namespace AgisX
{

class Application;
class AssetComp;

class ExchangeMapComp : public BaseComp
{
public:
	ExchangeMapComp(Application& app);

	void render_new_exchange_comp();
	void render() override;

	void on_reset() override;
	void on_step() override;
	void on_hydra_restore() override;

private:
	mutable std::shared_mutex _mutex;
	std::optional<AgisException> _exception;
	std::optional<Exchange const*> _selected_exchange;
	
	ExchangeMap const* _exchanges;
	std::unordered_map<std::string, size_t> const* _exchange_ids = nullptr;

	Application& _app;
	AssetComp* _asset_comp = nullptr;
};


}