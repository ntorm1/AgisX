#pragma once


#include <optional>
#include <vector>
#include <string>
#include <shared_mutex>
#include <unordered_map>


#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"
#include "AgisXAppComp.h"

import StrategyTracerModule;


namespace AgisX
{


//============================================================================
class AgisXPlotViewPrivate : public AppComponent
{
public:
	AgisXPlotViewPrivate(
		AgisX::AppState& app_state,
		AgisX::AppComponent const* parent,
		std::vector<long long> const& ns_epoch_index
	);
	virtual ~AgisXPlotViewPrivate() = default;
	virtual void add_data(
		std::string const& column,
		std::vector<double> const& vec
	);
	void remove_data(std::string const& column);
	void reset();
	void plot_base_data(std::optional<size_t> index_clip = std::nullopt) const noexcept;
	virtual void draw_plot() noexcept = 0;
	auto const& data() const noexcept{ return _data; }
	auto const& second_epoch_index() const noexcept{ return _second_epoch_index; }


	AppComponentType type() const noexcept override { return AppComponentType::PLOT_BASE; }
	void on_hydra_restore() noexcept override{}

private:

	std::unordered_map<std::string, std::vector<double> const&> _data;
	std::vector<double> _second_epoch_index;
};



//============================================================================
class AgisXAssetPlot : public AgisXPlotViewPrivate
{
public:
	AgisXAssetPlot(
		AgisX::AppState& app_state,
		AppComponent const* parent,
		Agis::Asset const& asset
	);

	// asset data is stored in strided matrix, so to store column the plot view 
	// keeps a copy of the column data and pass the reference to the plot
	void add_data(
		std::string const& column,
		std::vector<double> const& vec
	) override;

	Agis::Asset const& asset() const noexcept{ return _asset; }
	void draw_plot() noexcept override;

	AppComponentType type() const noexcept override { return AppComponentType::ASSET_PLOT; }
	void on_hydra_restore() noexcept {}

private:
	Agis::Asset const& _asset;
	std::unordered_map<std::string, std::vector<double> const> _asset_data;

};


//============================================================================
class AgisXPortfolioPlot : public AgisXPlotViewPrivate
{
public:
	AgisXPortfolioPlot(
		AgisX::AppState& _app_state,
		Agis::StrategyTracers const&,
		AgisX::AppComponent const*
	);

	void draw_plot() noexcept override;

	AppComponentType type() const noexcept override{ return AppComponentType::PORTFOLIO_PLOT; }
	void on_hydra_restore() noexcept;

private:
	void add_tracer(std::string const& column);
	Agis::StrategyTracers const& _tracers;
	AgisX::AppState const& _app_state;
	std::unordered_map<std::string, std::vector<double> const&> _data;
	std::vector<double> _dt_index;

};

}