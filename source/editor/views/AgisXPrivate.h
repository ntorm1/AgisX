#pragma once

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"

#include <optional>
#include <vector>
#include <string>
#include <shared_mutex>

namespace AgisX
{

struct AgisXAssetViewPrivate
{
	AgisXAssetViewPrivate() = delete;
	AgisXAssetViewPrivate(Agis::Asset const&);
	~AgisXAssetViewPrivate();
	void draw();
	Agis::Asset const& _asset;
	std::vector<std::string> _columns;
	std::vector<double> const* _data;
	std::vector<std::string> _dt_index;
	std::string _asset_id = "";
};


class AgisXExchangeViewPrivate
{
private:
	mutable std::shared_mutex _mutex;
	std::optional<std::unique_ptr<AgisXAssetViewPrivate>> asset_view = std::nullopt;
	std::optional<Agis::Exchange const*> selected_exchange = std::nullopt;
	std::vector<std::string> asset_ids;

public:
	AgisXExchangeViewPrivate() = default;
	~AgisXExchangeViewPrivate();

	std::optional<AgisXAssetViewPrivate*> get_asset_view() const noexcept;
	bool set_selected_asset(std::string const& asset_id);
	void set_selected_exchange(std::optional<Agis::Exchange const*> exchange);

	auto has_exchange() const -> bool{ return selected_exchange.has_value(); }
	auto const& get_asset_ids(){ return asset_ids; }
	std::optional<std::string> get_selected_asset_id();
};

}