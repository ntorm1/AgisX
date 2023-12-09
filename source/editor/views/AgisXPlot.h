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
class AgisXPlotViewPrivate
{
public:
	AgisXPlotViewPrivate(std::vector<long long> const& ns_epoch_index);
	void add_data(
		std::string const& column,
		std::vector<double>& data
	);
	void remove_data(std::string const& column);
	void reset();

	auto const& data() const noexcept{ return _data; }
	auto const& second_epoch_index() const noexcept{ return _second_epoch_index; }

private:
	std::unordered_map<std::string, std::vector<double>> _data;
	std::vector<double> _second_epoch_index;
};



//============================================================================
class AgisXAssetPlot : public AppComponent
{
public:
	AgisXAssetPlot(
		AppComponent const* parent,
		Agis::Asset const& asset
	);
	Agis::Asset const& asset() const noexcept{ return _asset; }
	void draw_plot() noexcept;

	template<typename... Args>
	void add_data(Args&&... args)
	{
		_plot_view.add_data(std::forward<Args>(args)...);
	}

	template<typename... Args>
	void remove_data(Args&&... args)
	{
		_plot_view.remove_data(std::forward<Args>(args)...);
	}

	AppComponentType type() const noexcept override { return AppComponentType::ASSET_PLOT; }
	void on_hydra_restore() noexcept {}

private:
	AgisXPlotViewPrivate _plot_view;
	Agis::Asset const& _asset;
};



class AgisXPortfolioPlot : public AppComponent
{
public:
	AgisXPortfolioPlot(
		Agis::StrategyTracers const&,
		AgisX::AppComponent const*,
		AgisX::AppState const& _app_state
	);

	void on_step() noexcept;

	AppComponentType type() const noexcept override{ return AppComponentType::PORTFOLIO_PLOT; }
	void on_hydra_restore() noexcept;

private:
	void add_data(std::string const& column);
	Agis::StrategyTracers const& _tracers;
	AgisX::AppState const& _app_state;
	std::unordered_map<std::string, std::vector<double> const&> _data;
	std::vector<double> _dt_index;

};

}