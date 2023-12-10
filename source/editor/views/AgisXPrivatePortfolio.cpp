#include "AgisXPrivatePortfolio.h"
#include "AgisXPlot.h"
#include <imgui.h>
#include "../nged_imgui.h"
#include "../res/fa_icondef.h"



import <string>;
import <optional>;

import PortfolioModule;
import StrategyModule;
import PositionModule;
import ExchangeMapModule;
import TradeModule;
import AgisXApp;

namespace AgisX
{


//============================================================================
AgisXPortfolioViewPrivate::AgisXPortfolioViewPrivate(AgisX::AppState& app_state)
    : AppComponent(app_state,std::nullopt)
{
}


//============================================================================
AgisXPortfolioViewPrivate::~AgisXPortfolioViewPrivate()
{
    if (_plot)
    {
        delete _plot.value();
    }
}


//============================================================================
void
AgisXPortfolioViewPrivate::on_portfolio_click(const Agis::Portfolio& portfolio)
{
    if (_selected_portfolio && *_selected_portfolio == &portfolio) {
        app().infof("deselected portfolio: {}", portfolio.get_portfolio_id());
        _selected_portfolio = std::nullopt;
        _plot = std::nullopt;
    }
    else {
        app().infof("selected portfolio: {}", portfolio.get_portfolio_id());
        _selected_strategy = std::nullopt;
        _selected_portfolio = const_cast<Agis::Portfolio*>(&portfolio);
    }
}


//============================================================================
void
AgisXPortfolioViewPrivate::on_hydra_build() noexcept
{
    // if the plot was init before hydra was build, it does not have the correct 
    // datetime index so force a rebuild of it on build
    if (_plot)
    {
        auto& tracers = (*_plot)->get_tracers();
        delete _plot.value();
        _plot = new AgisX::AgisXPortfolioPlot(app(), tracers, this);
    }
}

//============================================================================
void
AgisXPortfolioViewPrivate::on_strategy_click(const Agis::Strategy& strategy)
{
    if (_selected_strategy && *_selected_strategy == &strategy) {
        app().infof("deselected strategy: {}", strategy.get_strategy_id());
        _selected_strategy = std::nullopt;
        _plot = std::nullopt;
    }
    else {
        app().infof("selected strategy: {}", strategy.get_strategy_id());
        _selected_portfolio = std::nullopt;
        _selected_strategy = const_cast<Agis::Strategy*>(&strategy);
        _plot = new AgisX::AgisXPortfolioPlot(
            app(),
            strategy.get_tracers(),
            this
        );

    }
    app().emit_on_strategy_select(_selected_strategy);
}

//============================================================================
void
AgisXPortfolioViewPrivate::draw_portfolio_node(Agis::Portfolio const& portfolio)
{
    auto id = portfolio.get_portfolio_id();
    ImGui::PushID(id.c_str());

    // draw the tree element corresponding to this portfolio
    static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;
   
    ImGuiTreeNodeFlags node_flags = base_flags;
    if (
        _selected_portfolio &&
        *_selected_portfolio == &portfolio)
    {
        node_flags |= ImGuiTreeNodeFlags_Selected;
    }
    bool is_node_open = ImGui::TreeNodeEx(id.c_str(), node_flags, id.c_str());
    node_flags = base_flags;
    if (ImGui::IsItemClicked()) {
        on_portfolio_click(portfolio);
    }
    // Check for right-click within the TreeNode
    if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
        ImGui::OpenPopupOnItemClick("PortfolioContextMenu", ImGuiPopupFlags_MouseButtonRight);
    }
    if (is_node_open) {
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
             ImGui::OpenPopupOnItemClick("PortfolioContextMenu", ImGuiPopupFlags_MouseButtonRight);
        }
        // draw the child portfolios
        for (auto& [index, child_portfolio] : portfolio.child_portfolios())
        {
            draw_portfolio_node(*child_portfolio);
        }

        // draw the child strategies
        for (auto& [index, child_Strategy] : portfolio.child_strategies())
        {
            auto const& strategy_id = child_Strategy->get_strategy_id();
            ImU32 circleColor = child_Strategy->is_disabled() ? IM_COL32(255, 0, 0, 255) : IM_COL32(0, 255, 0, 255);
            if (
                _selected_strategy &&
                *_selected_strategy == child_Strategy.get())
            {
                node_flags |= ImGuiTreeNodeFlags_Selected;
            }
            ImGui::PushID(strategy_id.c_str());

            // Add a circle with a color based on the boolean value in the strategy
            ImVec2 circlePos = ImGui::GetCursorScreenPos();
            ImVec2 circleCenter = ImVec2(circlePos.x + 22, circlePos.y + 10); // Adjust the position as needed
            float circleRadius = 5.0f;
            ImGui::GetWindowDrawList()->AddCircleFilled(circleCenter, circleRadius, circleColor);
            is_node_open = ImGui::TreeNodeEx(strategy_id.c_str(), node_flags, strategy_id.c_str());
            if (is_node_open)
            {
                if (ImGui::IsItemClicked())
                {
                    on_strategy_click(*child_Strategy);
                }
                ImGui::TreePop();
            }
            ImGui::PopID();
        }
        ImGui::TreePop();
    }
    ImGui::PopID();

    // Context Menu
    static bool open_new_portfolio_popup = false;
    static bool open_new_strategy_popup = false;
    if (ImGui::BeginPopupContextItem("PortfolioContextMenu"))
    {

        if (ImGui::MenuItem("New Portfolio"))
        {
            open_new_portfolio_popup = true;
        }
        if (ImGui::MenuItem("New Strategy"))
        {
            open_new_strategy_popup = true;
        }
        ImGui::EndPopup();
    }
    if (open_new_portfolio_popup)
    {
        if (!_selected_portfolio)
        {
            app().errorf("No portfolio selected");
        }
        else
        {
            ImGui::OpenPopup("NewPortfolioPopup");
            open_new_portfolio_popup = false;
        }
    }
    else if (open_new_strategy_popup)
    {
        if (!_selected_portfolio)
        {
            app().errorf("No portfolio selected");
        }
        else
        {
            ImGui::OpenPopup("NewStrategyPopup");
            open_new_strategy_popup = false;
        }
	}
}


//============================================================================
void AgisXPortfolioViewPrivate::draw_strategy()
{
    if (!_selected_strategy)
    {
		return;
	}
    auto& strategy = *(_selected_strategy.value());
    ImGui::Text("Strategy ID: %s", strategy.get_strategy_id().c_str());
    ImGui::Text("Exchange ID: %s", strategy.get_exchange_id().c_str());
    ImGui::Text("Portfolio ID: %s", strategy.get_portfolio_id().c_str());
    ImGui::Separator();

    ImGui::Text("Cash: %f", strategy.get_cash());
    ImGui::Text("Net Liquidation Value: %f", strategy.get_nlv());
    ImGui::Separator();

    if (ImGui::Button("Disable"))
    {
		strategy.set_is_disabled(true);
	}
    ImGui::SameLine();
    if (ImGui::Button("Enable"))
    {
        strategy.set_is_disabled(false);
    }
    (*_plot)->draw_plot();
}


//============================================================================
void
AgisXPortfolioViewPrivate::draw_new_portfolio()
{
    if (ImGui::BeginPopup("NewPortfolioPopup"))
    {
        static std::string portfolio_id;
        static std::string exchange_id;

        ImGui::Text("Portfolio ID: ");
        ImGui::SameLine();
        ImGui::InputText("##PortfolioId", &portfolio_id);

        ImGui::Text("Exchange ID: ");
        ImGui::SameLine();
        auto exchange_ids = app().get_exchange_ids();
        if (ImGui::BeginCombo("##ExchangeID", exchange_id.c_str()))
        {
            for (auto& [id, index] : *exchange_ids)
            {
                bool is_selected = (exchange_id == id);
                if (ImGui::Selectable(id.c_str(), is_selected))
                {
                    exchange_id = id;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (ImGui::Button("Create"))
        {
            std::optional<Agis::Portfolio*> parent = std::nullopt;
            auto parent_portfolio_id = (*_selected_portfolio)->get_portfolio_id();
            parent = app().get_portfolio_mut(parent_portfolio_id);
            if (!parent)
            {
                app().errorf("Parent portfolio not found: %s", parent_portfolio_id.c_str());
            }
            else
            {
                app().__create_portfolio(portfolio_id, exchange_id, parent);
                portfolio_id = "";
                exchange_id = "";
            }
            // Close the popup after creating the portfolio
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


//============================================================================
void
AgisXPortfolioViewPrivate::draw_new_strategy()
{
    if (ImGui::BeginPopup("NewStrategyPopup"))
    {
        static std::string strategy_id;
        static std::string parent_exchange_id;
        static double starting_capital = 0.0;

        ImGui::Text("Strategy ID: ");
        ImGui::SameLine();
        ImGui::InputText("##StrategyId", &strategy_id);

        ImGui::Text("Starting Cash: ");
        ImGui::SameLine();
		ImGui::InputDouble("##StartingCash", &starting_capital);

        ImGui::Text("Parent Exchange ID: ");
        ImGui::SameLine();
        auto exchange_ids = app().get_exchange_ids();
        if (ImGui::BeginCombo("##ExchangeID", parent_exchange_id.c_str()))
        {
            for (auto& [id, index] : *exchange_ids)
            {
                bool is_selected = (parent_exchange_id == id);
                if (ImGui::Selectable(id.c_str(), is_selected))
                {
                    parent_exchange_id = id;
                }
                if (is_selected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        if (ImGui::Button("Cancel"))
        {
            ImGui::CloseCurrentPopup();
        }
        ImGui::SameLine();
        if (!strategy_id.empty() &&
            !parent_exchange_id.empty() &&
            ImGui::Button("Create"))
        {
            auto parent_portfolio_id = (*_selected_portfolio)->get_portfolio_id();
            app().__create_strategy(strategy_id, parent_portfolio_id, parent_exchange_id, starting_capital);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


//============================================================================
void
AgisXPortfolioViewPrivate::draw_book(Agis::Portfolio const& portfolio)
{
    auto read_lock = portfolio.__aquire_read_lock();
    auto const& positions = portfolio.positions();
    auto const& exchange_map = app().get_exchanges();

    const float TEXT_BASE_WIDTH = ImGui::CalcTextSize("A").x;
    static ImGuiTableFlags flags = ImGuiTableFlags_SizingFixedFit | ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody;
    static int decimalPlaces = 2;
    //static ImGuiTreeNodeFlags tree_node_flags = flags;

        // Add a counter to toggle the number of decimal places
    if (ImGui::CollapsingHeader("Portfolio Book Layout"))
    {
        ImGui::Text("Decimal Places: %d", decimalPlaces);
        if (ImGui::Button("Decrease") && decimalPlaces > 0)
            decimalPlaces--;
        ImGui::SameLine();
        if (ImGui::Button("Increase") && decimalPlaces < 12)
            decimalPlaces++;
    }

    if (ImGui::BeginTable("PortfolioBook", 6, flags))
    {
        // The first column will use the default _WidthStretch when ScrollX is Off and _WidthFixed when ScrollX is On
        ImGui::TableSetupColumn("Asset ID", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Strategy ID", ImGuiTableColumnFlags_NoHide);
        ImGui::TableSetupColumn("Average Cost");
        ImGui::TableSetupColumn("NLV");
        ImGui::TableSetupColumn("Unrealized PnL");
        ImGui::TableSetupColumn("Realized PnL");
        ImGui::TableHeadersRow();

        for (auto const& [asset_index, position] : positions)
        {
            ImGui::TableNextRow();
            ImGui::TableNextColumn();
            auto asset_id = exchange_map.get_asset_id(asset_index).value();
            bool open = ImGui::TreeNodeEx(asset_id.c_str(), flags);
            ImGui::TableNextColumn();
            ImGui::TextDisabled("--");
            ImGui::TableNextColumn();
            ImGui::Text("%.*f", decimalPlaces, position->get_avg_price());
            ImGui::TableNextColumn();
            ImGui::Text("%.*f", decimalPlaces, position->get_nlv());
            ImGui::TableNextColumn();
            ImGui::Text("%.*f", decimalPlaces, position->get_unrealized_pnl());
            ImGui::TableNextColumn();
            ImGui::Text("%.*f", decimalPlaces, position->get_realized_pnl());

            if (open)
            {
                auto const& trades = position->trades();
                for (auto const& [trade_id, trade] : trades)
                {
					ImGui::TableNextRow();
					ImGui::TableNextColumn();
                    ImGui::TableNextColumn();
                    ImGui::TextUnformatted(trade->get_strategy()->get_strategy_id().c_str());
                    ImGui::TableNextColumn();
					ImGui::Text("%.*f", decimalPlaces, trade->get_avg_price());
					ImGui::TableNextColumn();
					ImGui::Text("%.*f", decimalPlaces, trade->get_nlv());
					ImGui::TableNextColumn();
					ImGui::Text("%.*f", decimalPlaces, trade->get_unrealized_pnl());
					ImGui::TableNextColumn();
					ImGui::Text("%.*f", decimalPlaces, trade->get_realized_pnl());
				}   
            }
            ImGui::TreePop();
        }
    }
    ImGui::EndTable();
}


//============================================================================
void
AgisXPortfolioViewPrivate::draw_portfolio_tree(Agis::Portfolio const& portfolio)
{
	// draw popups if needed
    draw_new_portfolio();
    draw_new_strategy();

    if (ImGui::CollapsingHeader("Portfolio Tree"))
    {
        draw_portfolio_node(portfolio);
    }
    if (ImGui::CollapsingHeader("Strategy"))
    {
        draw_strategy();
    }
    if (ImGui::CollapsingHeader("Portfolio Book"))
    {
        draw_book(portfolio);
    }
}


//============================================================================
void
AgisXPortfolioViewPrivate::on_hydra_restore() noexcept
{
    _selected_strategy = std::nullopt;
    _selected_portfolio = std::nullopt;
    if (_plot)
    {
        delete _plot.value();
    }
    _plot = std::nullopt;
}


} // namespace AgisX