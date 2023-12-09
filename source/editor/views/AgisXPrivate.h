#pragma once

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
#include "AgisXAppComp.h"

#include <optional>
#include <vector>
#include <string>
#include <shared_mutex>
#include <unordered_map>

namespace AgisX
{


//============================================================================
class AgisXAssetViewPrivate : public AppComponent
{
public:
	AgisXAssetViewPrivate() = delete;
	AgisXAssetViewPrivate(AgisXExchangeViewPrivate* parent, Agis::Asset const&);
	~AgisXAssetViewPrivate();

	void asset_table_context_menu();
	void draw();
	void draw_table();

	AppComponentType type() const noexcept override{ return AppComponentType::ASSET_VIEW; }
	void on_hydra_restore() noexcept override {}

	Agis::Asset const& _asset;
	AgisX::AgisXAssetPlot* _plot_view;
	std::vector<std::string> _columns;
	std::vector<double> const* _data;
	std::vector<std::string> _dt_index;
	std::string _asset_id = "";
};




//============================================================================
class AgisXExchangeViewPrivate : public AppComponent
{
private:
	mutable std::shared_mutex _mutex;
	std::optional<std::unique_ptr<AgisXAssetViewPrivate>> asset_view = std::nullopt;
	std::optional<Agis::Exchange const*> selected_exchange = std::nullopt;
	std::vector<std::string> asset_ids;

public:
	AgisXExchangeViewPrivate();
	~AgisXExchangeViewPrivate();

	AppComponentType type() const noexcept override { return AppComponentType::EXCHANGE_VIEW; }
	void on_hydra_restore() noexcept override;

	std::optional<AgisXAssetViewPrivate*> get_asset_view() const noexcept;
	bool set_selected_asset(std::string const& asset_id);
	void set_selected_exchange(std::optional<Agis::Exchange const*> exchange);

	auto has_exchange() const -> bool{ return selected_exchange.has_value(); }
	auto const& get_asset_ids(){ return asset_ids; }
	std::optional<std::string> get_selected_asset_id();
};

}