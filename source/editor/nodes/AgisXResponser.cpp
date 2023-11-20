module;

#include "../nged_imgui.h"
#include "../nged.h"

module AgisXResponser;

import AgisXNode;

using namespace nged;

namespace AgisX
{

void AgisXResponser::onInspect(InspectorView* view, GraphItem** items, size_t numItems)
{
    bool  handled = false;
    Node* solyNode = nullptr;
    for (size_t i = 0; i < numItems; ++i) {
        if (auto* node = items[i]->asNode())
            if (solyNode) {
                solyNode = nullptr;
                break;
            }
            else {
                solyNode = node;
            }
    }
    if (auto* node = solyNode) {
        auto* agisx_node = static_cast<AgisXNode*>(node);
        ImGui::PushFont(ImGuiResource::instance().monoFont);
        agisx_node->render_inspector();
        ImGui::PopFont();
        ImGui::Separator();
        handled = true;
    }
    if (!handled) {
        DefaultImGuiResponser::onInspect(view, items, numItems);
    }
    if (ImGui::IsWindowFocused() && ImGui::IsKeyPressed(ImGuiKey_Escape)) {
        NetworkView* netviewToFocus = nullptr;
        if (auto lv = view->linkedView()) {
            if (auto* netview = dynamic_cast<NetworkView*>(lv.get()))
                netviewToFocus = netview;
        }
        if (!netviewToFocus && numItems > 0) {
            for (auto v : view->editor()->views()) {
                if (auto* netview = dynamic_cast<NetworkView*>(v.get())) {
                    if (!netviewToFocus)
                        netviewToFocus = netview;
                    else if (netview->graph().get() == items[0]->parent())
                        netviewToFocus = netview;
                }
            }
        }
        if (netviewToFocus) {
            if (auto* imguiWindow = dynamic_cast<ImGuiNamedWindow*>(netviewToFocus)) {
                msghub::infof("focusing window {}", imguiWindow->titleWithId());
                ImGui::SetWindowFocus(imguiWindow->titleWithId().c_str());
            }
        }
    }
}

void AgisXResponser::onItemHovered(NetworkView* view, GraphItem* item)
{
}

}