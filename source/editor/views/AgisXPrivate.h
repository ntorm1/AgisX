#pragma once

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
#include "AgisXAppComp.h"
#include <tbb/concurrent_vector.h>

#include <optional>
#include <vector>
#include <string>
#include <shared_mutex>
#include <unordered_map>

namespace AgisX
{


struct OrderBuffer : public AppComponent
{
public:
	OrderBuffer(
		AgisX::AppState& app_state,
		AgisX::AppComponent* parent,
		Agis::Portfolio const& portfolio,
		size_t asset_index
	);

	AppComponentType type() const noexcept override { return AppComponentType::ORDER_BUFFER; }
	void on_hydra_restore() noexcept override {}
	void on_hydra_step() noexcept override;
	void on_hydra_reset() noexcept override;
	
	auto get_max_units() const noexcept{ return max_units; }

	auto begin() const noexcept {
		return order_buffer.cbegin();
	}
	auto end() const noexcept{
		return order_buffer.cend();
	}

private:
	size_t historical_index = 0;
	double max_units = 0;
	std::vector<Agis::Order*> order_buffer;
	Agis::Portfolio const& portfolio;
	tbb::concurrent_vector<Agis::Order*> const& order_history;
	size_t asset_index;

};


//============================================================================
class AgisXAssetViewPrivate : public AppComponent
{
public:
	AgisXAssetViewPrivate() = delete;
	AgisXAssetViewPrivate(
		AgisX::AppState& app_state,
		AgisXExchangeViewPrivate* parent,
		Agis::Asset const&
	);
	~AgisXAssetViewPrivate();

	void asset_table_context_menu();
	void draw();
	void draw_table();

	void toggle_order_buffer() noexcept;
	auto const& get_order_buffer() const noexcept{ return _order_buffer; }
	const auto& get_asset_id() const noexcept{ return _asset_id; }

	AppComponentType type() const noexcept override{ return AppComponentType::ASSET_VIEW; }
	void on_hydra_restore() noexcept override {}
	void on_hydra_step() noexcept override;
	void on_hydra_reset() noexcept override;


private:
	std::optional<UniquePtr<OrderBuffer>> _order_buffer = std::nullopt;
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
	AgisXExchangeViewPrivate(AgisX::AppState& app_state);
	~AgisXExchangeViewPrivate();

	AppComponentType type() const noexcept override { return AppComponentType::EXCHANGE_VIEW; }
	void on_hydra_restore() noexcept override;
	void on_hydra_step() noexcept override;
	void on_hydra_reset() noexcept override;

	std::optional<AgisXAssetViewPrivate*> get_asset_view() const noexcept;
	bool set_selected_asset(std::string const& asset_id);
	void set_selected_exchange(std::optional<Agis::Exchange const*> exchange);

	auto has_exchange() const -> bool{ return selected_exchange.has_value(); }
	auto const& get_asset_ids(){ return asset_ids; }
	std::optional<std::string> get_selected_asset_id();
};

}