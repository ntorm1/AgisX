#include <Windows.h>

#include "imgui.h"
#include "ImGuiFileDialog.h"

#include "App.h"
#include "ExchangeComp.h"

#include <rapidjson/allocators.h>
#include <rapidjson/document.h>

import SerializeModule;
import AgisTimeUtils;

import <filesystem>;

namespace AgisX
{

//============================================================================
Application::Application()
{   
	_hydra = std::make_unique<Hydra>();

    // set the exe path
    wchar_t buffer[MAX_PATH]; // Use wchar_t instead of char
    GetModuleFileNameW(NULL, buffer, MAX_PATH);
    // convert to string
    std::wstring ws(buffer);
    std::string exe_path(ws.begin(), ws.end());

    // get the parent directory
    auto pos = exe_path.find_last_of("\\/");
    auto exe_dir = exe_path.substr(0, pos);

    // create filesystem path 
    auto path = std::filesystem::path(exe_dir);
    auto env_path = path / "envs" / env_name;
    if (!std::filesystem::exists(env_path))
    {
		std::filesystem::create_directories(env_path);
	}
    _env_dir = env_path.string();

    // init exchange map comp
	exchange_comp = new ExchangeMapComp(*this);
    _comps.push_back(exchange_comp);
}


//============================================================================
Application::~Application()
{
	delete exchange_comp;
}


//============================================================================
ExchangeMap const&
Application::get_exchanges()
{
	return _hydra->get_exchanges();
}


//============================================================================
void Application::render_app_state()
{
    if (ImGui::CollapsingHeader("State IO"))
    {
        if (ImGui::Button("Save"))
        {
            auto document = rapidjson::Document();
            auto& allocator = document.GetAllocator();
            auto output_path = _env_dir + "/hydra.json";
            auto res = serialize_hydra(allocator, *_hydra, output_path);
            if (!res)
            {
                _exception = res.error();
                ImGui::OpenPopup("AppError");
            }
        }
        if (ImGui::Button("Load"))
        {
            // Create a new thread to perform deserialization
            std::thread deserializationThread([this] {
                auto res = deserialize_hydra(_env_dir + "/hydra.json");
                if (res)
                {
                    _hydra = std::move(res.value());

                    // Emit a signal or perform any necessary UI updates in the main thread
                    // after deserialization is complete
                    emit_new_hydra_ptr();
                }
                else
                {
                    _exception = res.error();
                }
                });

            // Detach the thread so it can run independently
            deserializationThread.detach();
        }
    }

    if (ImGui::CollapsingHeader("Manage State"))
    {
        if (ImGui::Button("Build"))
        {
            std::thread build_thread([this] {
                auto res = build();
                if (!res)
                {
                    _exception = res.error();
                }
                });
            build_thread.detach();
        }

        // same line 
        ImGui::SameLine();
        if (ImGui::Button("Step"))
        {
            std::thread step_thread([this] {
                auto res = step();
                if (!res)
                {
                    _exception = res.error();
                }
                });
            step_thread.detach();
        }

        ImGui::Text("Global Time: %s", _app_state.get_global_time().c_str());
        ImGui::Text("Next Global Time: %s", _app_state.get_next_global_time().c_str());
    }
}


//============================================================================
std::expected<bool, AgisException>
Application::step()
{
    if(_hydra->get_state() != HydraState::BUILT && _hydra->get_state() != HydraState::RUN)
	{
        return std::unexpected<AgisException>("Hydra is not built or running");
	}

    auto lock = _hydra->__aquire_write_lock();
    auto res = _hydra->step();

    if (!res) return res;
    
    long long global_epoch = _hydra->get_global_time();
    long long next_epoch = _hydra->get_next_global_time();
    _app_state.update_time(global_epoch, next_epoch);
    return true;
}


//============================================================================
std::expected<bool, AgisException>
Application::build()
{
    if (_hydra->get_state() != HydraState::INIT)
    {
        return std::unexpected<AgisException>("Hydra has already been built");
    }
    auto res = _hydra->build();
    if (!res) return res;
    _app_state.update_time(0, _hydra->get_next_global_time());
}



//============================================================================
void
Application::render()
{
    if (_exception.has_value()) {
        ImGui::OpenPopup("AppError");
    }
    if (ImGui::BeginPopupModal("AppError")) {
        ImGui::Text(_exception->what());

        if (ImGui::Button("Dismiss", ImVec2(120, 0)))
        {
            ImGui::CloseCurrentPopup();
            _exception = std::nullopt;
        }
        ImGui::EndPopup();
        return;
    }
    ImGui::Begin("Application");
	render_app_state();
    exchange_comp->render();
    ImGui::End();
}


//============================================================================
void
Application::emit_new_hydra_ptr()
{
    for (auto comp : _comps)
    {
		comp->on_hydra_restore();
	}
}


//============================================================================
void
ApplicationState::update_time(long long global_time, long long next_global_time)
{
    auto t = epoch_to_str(global_time, "%Y-%m-%d %H:%M:%S");
    auto t_next = epoch_to_str(next_global_time, "%Y-%m-%d %H:%M:%S");
    set_global_time(t.value());
    set_next_global_time(t_next.value());
}

}