#include <imgui.h>
#include <implot.h>
#include "AgisXPlot.h"


import AgisXApp;
import AssetModule;


namespace AgisX
{

//============================================================================
AgisXPlotViewPrivate::AgisXPlotViewPrivate(std::vector<long long> const& ns_epoch_index)
{
    _second_epoch_index.reserve(ns_epoch_index.size());
    for (size_t i = 0; i < ns_epoch_index.size(); i++)
    {
        long long ns_epoch = ns_epoch_index[i];
        double s_epoch = static_cast<double>(ns_epoch / 1000000000.0);
        _second_epoch_index.push_back(s_epoch);
    }
}


//============================================================================
AgisXAssetPlot::AgisXAssetPlot(
    AppComponent const* parent,
    Agis::Asset const& asset
) : AppComponent(parent), _asset(asset), _plot_view(asset.get_dt_index())
{
}


//============================================================================
void
AgisXAssetPlot::draw_plot() noexcept
{
    if (ImPlot::BeginPlot(_asset.get_id().c_str()))
    {
        if (!_plot_view.data().size())
        {
            ImPlot::EndPlot();
            return;
        }
        ImPlot::SetupAxes(0, 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
        ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
        ImPlot::SetupAxisLimits(ImAxis_X1, _plot_view.second_epoch_index()[0], _plot_view.second_epoch_index().back());
        for (auto const& [column_name, column_vector] : _plot_view.data())
        {
            ImPlot::PlotLine(
                column_name.c_str(),
                _plot_view.second_epoch_index().data(),
                column_vector.data(),
                static_cast<int>(column_vector.size())
            );
        }
        // plot the current close price as a point provided close col is plotted and asset is streaming
        if (
            _plot_view.data().count(_asset.get_close_column())
            &&
            _asset.get_streaming_index())
        {
            auto streaming_index = *_asset.get_streaming_index();
            double current_time = _plot_view.second_epoch_index()[streaming_index];
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, 6);
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerWeight, 3);
            ImPlot::PushStyleColor(ImPlotCol_MarkerOutline, ImVec4(1, 0, 0, 1));
            auto const& close_col = _plot_view.data().at(_asset.get_close_column());
            double current_close = close_col[streaming_index];
            ImPlot::PlotScatter("Close", &current_time, &current_close, 1);
            ImPlot::PopStyleVar(3);
            ImPlot::PopStyleColor();
        }
        ImPlot::EndPlot();
    }
}

//============================================================================
void
    AgisXPlotViewPrivate::add_data(std::string const& column, std::vector<double>& data)
{
    if (!data.size()) return;
    _data.insert({ column, data });
}


//============================================================================
void
    AgisXPlotViewPrivate::remove_data(std::string const& column)
{
    if (!_data.count(column)) return;
    _data.erase(column);
}


//============================================================================
void
AgisXPlotViewPrivate::reset()
{
    _data.clear();
}


//============================================================================
AgisXPortfolioPlot::AgisXPortfolioPlot(
    Agis::StrategyTracers const& tracers,
    AgisX::AppComponent const* portfolio_view,
    AgisX::AppState const& app_state
) : AppComponent(portfolio_view), _tracers(tracers), _app_state(app_state)
{

}


//============================================================================
void
AgisXPortfolioPlot::on_step() noexcept
{
    auto global_time_epoch = _app_state.get_global_time_epoch();
    double s_epoch = static_cast<double>(global_time_epoch / 1000000000.0);
    _dt_index.push_back(s_epoch);
}


//============================================================================
void
AgisXPortfolioPlot::on_hydra_restore() noexcept
{
    _dt_index.clear();
	_data.clear();
}


//============================================================================
void
AgisXPortfolioPlot::add_data(std::string const& column)
{
    auto tracer_type_opt = _tracers.get_type(column);
    if (!tracer_type_opt)
	{
		_app_state.errorf("AgisXPortfolioPlot::add_data() - Unknown column: {}", column.c_str());
	}
    auto col_opt = _tracers.get_column(*tracer_type_opt);
    if(!col_opt)
    {
        _app_state.errorf("AgisXPortfolioPlot::add_data() - Tracer not tracking: {}", column.c_str());
    }
    _data.insert({ column, *(col_opt.value()) });
}

}