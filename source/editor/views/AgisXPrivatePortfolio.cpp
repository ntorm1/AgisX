#include "AgisXPrivatePortfolio.h"
#include <imgui.h>
#include "../nged_imgui.h"

import <string>;
import <optional>;

import PortfolioModule;
import AgisXApp;

namespace AgisX
{



//============================================================================
AgisXPortfolioViewPrivate::~AgisXPortfolioViewPrivate()
{
}


//============================================================================
void
AgisXPortfolioViewPrivate::draw_portfolio_node(Agis::Portfolio const& portfolio)
{
    auto id = portfolio.get_portfolio_id();
    ImGui::PushID(id.c_str());

    // draw the tree element corresponding to this portfolio
    if (ImGui::TreeNode(id.c_str())) {
        auto& child_portfolios = portfolio.child_portfolios();

        // Check for right-click within the TreeNode
        if (ImGui::IsItemClicked(ImGuiMouseButton_Right)) {
            ImGui::OpenPopupOnItemClick("PortfolioContextMenu", ImGuiPopupFlags_MouseButtonRight);
        }

        // draw the child portfolios
        for (auto& [id, child_portfolio] : child_portfolios)
        {
            draw_portfolio_node(*child_portfolio);
        }

        // Close the current tree node
        ImGui::TreePop();
    }


    // Context Menu
    static bool open_new_portfolio_popup = false;
    static bool open_new_strategy_popup = false;
    if (ImGui::BeginPopupContextItem("PortfolioContextMenu"))
    {
        // set the selected portfolio if not set 
        if (!_selected_portfolio)
		{
			// get pointer to the portfolio, remove const qualifier
            _selected_portfolio = const_cast<Agis::Portfolio*>(&portfolio);
		}

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
    ImGui::PopID();
    if (open_new_portfolio_popup)
    {
        ImGui::OpenPopup("NewPortfolioPopup");
        open_new_portfolio_popup = false;
    }
    else if (open_new_strategy_popup)
	{
		ImGui::OpenPopup("NewStrategyPopup");
		open_new_strategy_popup = false;
	}
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
        auto exchange_ids = _app_state.get_exchange_ids();
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
            parent = _app_state.get_portfolio_mut(parent_portfolio_id);
            if (!parent)
            {
                _app_state.errorf("Parent portfolio not found: %s", parent_portfolio_id.c_str());
            }
            else
            {
                _app_state.__create_portfolio(portfolio_id, exchange_id, parent);
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
        auto exchange_ids = _app_state.get_exchange_ids();
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
            _app_state.__create_strategy(strategy_id, parent_portfolio_id, parent_exchange_id, starting_capital);
            ImGui::CloseCurrentPopup();
        }
        ImGui::EndPopup();
    }
}


//============================================================================
void
AgisXPortfolioViewPrivate::draw_portfolio_tree(Agis::Portfolio const& portfolio)
{
	// draw popups if needed
    draw_new_portfolio();
    draw_new_strategy();

    if (ImGui::CollapsingHeader("Portfolios"))
    {
        draw_portfolio_node(portfolio);
    }
}

} // namespace AgisX