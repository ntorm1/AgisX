#include <imgui.h>
#include <implot.h>

#include "AgisXPlot.h"
#include "AgisXPrivate.h"

import AgisXApp;
import OrderModule;
import AssetModule;


namespace AgisX
{

static double nanosecond_epoch_to_second_epoch(long long ns_epoch)
{
	return static_cast<double>(ns_epoch / 1000000000.0);
}

//============================================================================
AgisXPlotViewPrivate::AgisXPlotViewPrivate(
    AgisX::AppState& app_state,
    AgisX::AppComponent const* parent,
    std::vector<long long> const& ns_epoch_index
) : AppComponent(app_state, parent)
{
    _second_epoch_index.reserve(ns_epoch_index.size());
    for (size_t i = 0; i < ns_epoch_index.size(); i++)
    {
        long long ns_epoch = ns_epoch_index[i];
        double s_epoch = nanosecond_epoch_to_second_epoch(ns_epoch);
        _second_epoch_index.push_back(s_epoch);
    }
}


//============================================================================
AgisXAssetPlot::AgisXAssetPlot(
    AgisX::AppState& app_state,
    AgisXAssetViewPrivate* parent,
    Agis::Asset const& asset
) : AgisXPlotViewPrivate(app_state, this, asset.get_dt_index()),
_asset(asset),
_asset_view(*parent)
{
}


//============================================================================
void AgisXPortfolioPlot::draw_plot() noexcept
{
    if (ImPlot::BeginPlot("Plot"))
    {
        if (!second_epoch_index().size())
        {
            ImPlot::EndPlot();
            return;
        }
        plot_base_data(_tracers.get_current_index());
        if (ImPlot::BeginLegendPopup("Right Click Me")) {
            ImPlot::EndLegendPopup();
        }
        ImPlot::EndPlot();
    }
}


//============================================================================
void
AgisXAssetPlot::draw_plot() noexcept
{
    // add collapsing section for plot overlays
    if (ImGui::CollapsingHeader("Plot Overlays"))
    {
        if (ImGui::Checkbox("Orders", &_plot_orders))
        {
            _asset_view.toggle_order_buffer();
        }
    }

    if (ImPlot::BeginPlot(_asset.get_id().c_str()))
    {
        if (!second_epoch_index().size())
        {
            ImPlot::EndPlot();
            return;
        }
        plot_base_data();

        // plot the current close price as a point provided close col is plotted and asset is streaming
        if (
            data().count(_asset.get_close_column())
            &&
            _asset.get_streaming_index())
        {
            auto streaming_index = *_asset.get_streaming_index();
            double current_time = second_epoch_index()[streaming_index];
            ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, 6);
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerWeight, 3);
            ImPlot::PushStyleColor(ImPlotCol_MarkerOutline, ImVec4(1, 0, 0, 1));
            auto const& close_col = data().at(_asset.get_close_column());
            double current_close = close_col[streaming_index];
            ImPlot::PlotScatter("Current Close", &current_time, &current_close, 1);
            ImPlot::PopStyleVar(3);
            ImPlot::PopStyleColor(); 
        }

        // plot the order buff 
        if (_plot_orders)
        {
            ImPlot::PushStyleVar(ImPlotStyleVar_MarkerWeight, 3);
            auto& b = *(_asset_view.get_order_buffer().value());
            b.on_hydra_step(); // update buffer
            double max_units = b.get_max_units();
            for (auto const& order : b)
            {
                double units = order->get_units();
               
                // if units > 0 then it's a buy order color green else color red
                if (units > 0)
                {
                    ImPlot::PushStyleColor(ImPlotCol_MarkerOutline, ImVec4(0, 1, 0, 1));
                }
                else
                {
                    ImPlot::PushStyleColor(ImPlotCol_MarkerOutline, ImVec4(1, 0, 0, 1));
                }

                // if not filled, plot as cross else plot as circle
                if (order->get_order_state() != Agis::OrderState::FILLED)
                {
                    ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Cross);
				}
				else
				{
					ImPlot::PushStyleVar(ImPlotStyleVar_Marker, ImPlotMarker_Circle);
                }

                auto size_scale = static_cast<float>(6 * abs(units) / max_units);
				ImPlot::PushStyleVar(ImPlotStyleVar_MarkerSize, size_scale);
				double time = nanosecond_epoch_to_second_epoch(order->get_fill_time());
				double price = order->get_fill_price();
                ImPlot::PlotScatter("Orders", &time, &price, 1);
                ImPlot::PopStyleVar(2);
				ImPlot::PopStyleColor();
			}
            ImPlot::PopStyleVar(1); 
        }
        ImPlot::EndPlot();
    }
}


//============================================================================
void
AgisXAssetPlot::on_hydra_step() noexcept
{
}


//============================================================================
void
AgisXPlotViewPrivate::add_data(std::string const& column, std::vector<double> const& vec)
{
    if (vec.size() != _second_epoch_index.size())
    {
        app().errorf("AgisXPlotViewPrivate::add_data({}) - Data size mismatch", column);
        return;
    }
    _data.insert({ column, vec });
}



//============================================================================
void
AgisXAssetPlot::add_data(
    std::string const& column,
    std::vector<double> const& vec
)
{
    auto index_size = second_epoch_index().size();
    if (vec.size() != index_size)
    {
        return;
    }
    _asset_data.emplace(column, std::move(vec));
    return AgisXPlotViewPrivate::add_data(column, _asset_data.at(column));
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
void
AgisXPlotViewPrivate::plot_base_data(std::optional<size_t> index_clip) const noexcept
{
    ImPlot::SetupAxes(0, 0, ImPlotAxisFlags_AutoFit | ImPlotAxisFlags_RangeFit);
    ImPlot::SetupAxisScale(ImAxis_X1, ImPlotScale_Time);
    ImPlot::SetupAxisLimits(ImAxis_X1, second_epoch_index()[0], second_epoch_index().back());
    for (auto const& [column_name, column_vector] : data())
    {
        // if index clip is passed only plot up to that index. This handles the case 
        // when the underlying vector has been allocated and filled with 0 but we don't want to plot it
        int max_index = static_cast<int>(column_vector.size());
        if (index_clip)
		{
			max_index = static_cast<int>(index_clip.value());
		}
        ImPlot::PlotLine(
            column_name.c_str(),
            second_epoch_index().data(),
            column_vector.data(),
            static_cast<int>(max_index)
        );
    }
}


//============================================================================
AgisXPortfolioPlot::AgisXPortfolioPlot(
    AgisX::AppState& app_state,
    Agis::StrategyTracers const& tracers,
    AgisX::AppComponent const* portfolio_view
) : 
    AgisXPlotViewPrivate(app_state, portfolio_view, app_state.get_global_dt_index()),
    _tracers(tracers),
    _app_state(app_state)
{
    add_tracer("Nlv");
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
AgisXPortfolioPlot::add_tracer(std::string const& column)
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
    add_data(column, *(col_opt.value()));
}

}