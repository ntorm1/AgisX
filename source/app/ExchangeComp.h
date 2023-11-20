#pragma once

#include "AgisDeclare.h"
#include "BaseComp.h"

using namespace Agis;

import AgisError;

import <shared_mutex>;
import <optional>;
import <unordered_map>;


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
	std::vector<std::string> const& get_exchange_columns(std::string const& exchange_id) const noexcept;

private:
	mutable std::shared_mutex _mutex;
	std::optional<Agis::AgisException> _exception;
	std::optional<Agis::Exchange const*> _selected_exchange;
	
	Agis::ExchangeMap const* _exchanges;
	std::unordered_map<std::string, size_t> const* _exchange_ids = nullptr;

	Application& _app;
	AssetComp* _asset_comp = nullptr;
};


}