#pragma once
#include "../nged_imgui.h"
#include "ImGuiFileDialog.h"

#include "AgisDeclare.h"
#include "../../app/AgisXDeclare.h"

import AgisXApp;

namespace AgisX {

//============================================================================
class AgisXPortfolioView
    : public nged::ImGuiGraphView<AgisXPortfolioView, nged::GraphView>
{
private:
    AgisX::AppState& _app_state;
    AgisX::AgisXPortfolioViewPrivate* _p = nullptr;
    Agis::Portfolio const* _master_portfolio;

public:
    ~AgisXPortfolioView();
    AgisXPortfolioView(
        AgisX::AppState& app_state,
        nged::NodeGraphEditor* editor
    );
    void onDocModified() override {}
    void onGraphModified() override {}

    void on_hydra_restore() override;
    void on_hydra_build() noexcept override;
    void drawContent();
};

}
