#pragma once

#include "AgisDeclare.h"
#include "BaseComp.h"

using namespace Agis;

import AgisError;

import <shared_mutex>;
import <optional>;
import <vector>;

namespace AgisX
{

class Application;

class AssetComp : public BaseComp
{
public:
	AssetComp(Application& app);

	Asset const* get_asset() const{ return _asset.value_or(nullptr); }
	void set_asset(Asset const* asset);

	void render() override;
	void on_hydra_restore() override;

private:
	std::optional<Asset const*> _asset = std::nullopt;
	std::vector<std::string> _columns;
	std::vector<double> const* _data;
	std::string _asset_id;

	Application const& _app;
	mutable std::shared_mutex _mutex;
	std::optional<AgisException> _exception;

};

}