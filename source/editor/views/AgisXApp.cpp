module;
#include <Windows.h>
#include <fstream>
#include <rapidjson/allocators.h>
#include <rapidjson/document.h>
#include <string>
#include "../ngdoc.h"
#include "AgisMacros.h"
module AgisXApp;

import <filesystem>;

import SerializeModule;
import HydraModule;
import ExchangeMapModule;
import ASTStrategyModule;
import AgisTimeUtils;

import AgisXNodeFactory;
import AgisXGraph;

using namespace Agis;

namespace AgisX
{

std::string formatDuration(const std::chrono::high_resolution_clock::time_point& start,
    const std::chrono::high_resolution_clock::time_point& stop,
    int precision = 2) {
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    if (elapsed.count() < 1000) {
        return std::to_string(elapsed.count()) + "us";
    }
    else {
        auto elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);

        if (elapsed.count() < 1000000) {
            // Display with microsecond precision
            return std::to_string(elapsed.count()) + "us";
        }
        else if (elapsedMillis.count() < 60000) {
            // Display with millisecond precision
            return std::to_string(elapsedMillis.count()) + "ms";
        }
        else {
            // Display with second precision
            std::ostringstream ss;
            ss << std::fixed << std::setprecision(precision)
                << static_cast<double>(elapsedSeconds.count()) + elapsedMillis.count() / 1000.0 << "s";
            return ss.str();
        }
    }
}


//============================================================================
AppState::AppState()
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
        std::filesystem::create_directories(env_path / "strategies");
    }
    _env_dir = env_path.string();
}


//============================================================================
AppState::~AppState()
{
}


//============================================================================
std::optional<Agis::Portfolio*>
AppState::get_portfolio_mut(std::string const& id) const noexcept
{
    return _hydra->get_portfolio_mut(id);
}

//============================================================================
std::optional<Agis::Portfolio const*>
AppState::get_portfolio(std::string const& id) const noexcept
{
   return _hydra->get_portfolio(id);
}

//============================================================================
std::optional<Agis::Exchange const*>
AppState::get_exchange(std::string const& id) const noexcept
{
    auto res = _hydra->get_exchanges().get_exchange(id);
    if (res)
    {
        return res.value();
    }
    return std::nullopt;
}


//============================================================================
std::unordered_map<std::string, size_t> const*
AppState::get_exchange_ids() const noexcept
{
    return &_hydra->get_exchanges().get_exchange_indecies();
}


//============================================================================
std::unordered_map<std::string, size_t> const*
AppState::get_portfolio_ids() const noexcept
{
    return nullptr;
}


//============================================================================
std::unordered_map<std::string, Agis::Strategy*> const&
AppState::get_strategies() const noexcept
{
    return _hydra->get_strategies();
}


//============================================================================
void
AppState::__save_state() const noexcept
{
    auto document = rapidjson::Document();
    auto& allocator = document.GetAllocator();
    auto output_path = _env_dir + std::string("/hydra.json");
    auto res = Agis::serialize_hydra(allocator, *_hydra, output_path);
    if (!res) {
        nged::MessageHub::errorf("failed to save state: {}",res.error().what());
    }
    else {
		nged::MessageHub::infof("saved state to: {}", output_path);
    }
}


//============================================================================
void
AppState::__load_state() noexcept
{
    std::thread load_thread([this] {
        nged::MessageHub::infof("loading state from: {}", _env_dir);
        auto now = std::chrono::system_clock::now();
        auto res = deserialize_hydra(_env_dir + "/hydra.json");
        if (res)
        {
            emit_lock(true);
            _hydra = std::move(res.value());
            auto end = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - now);
            emit_on_hydra_restore();
            nged::MessageHub::infof("Hydra loaded successfully in {} ms", std::to_string(elapsed.count()));
        }
        else
        {
            nged::MessageHub::errorf("failed to save state: {}", res.error().what());
        }
    });
    load_thread.detach();
}


//============================================================================
void
AppState::__build() noexcept
{
    std::thread build_thread([this] {
        nged::MessageHub::info("building hydra");
        if (_hydra->get_state() != HydraState::INIT)
        {
            nged::MessageHub::error("Hydra has already been built");
            return;
        }
        auto start = std::chrono::high_resolution_clock::now();
        auto res = _hydra->build();
        auto stop = std::chrono::high_resolution_clock::now();
        if (!res)
        {
            nged::MessageHub::errorf("failed to build hydra: {}", res.error().what());
        }
        update_time(0, _hydra->get_next_global_time());
        nged::MessageHub::infof("Hydra built successfully in {}", formatDuration(start,stop));
    });
    build_thread.detach();
}


//============================================================================
void
AppState::__step() noexcept
{
    auto lock = _hydra->__aquire_write_lock();
    std::thread step_thread([this] {
        nged::MessageHub::debugf("Starting step, current time: {}", get_global_time());
        if (_hydra->get_state() != HydraState::BUILT && _hydra->get_state() != HydraState::RUN)
        {
            nged::MessageHub::error("Hydra is not built or running");
            return;
        }
        auto start = std::chrono::high_resolution_clock::now();
        auto res = _hydra->step();
        auto stop = std::chrono::high_resolution_clock::now();
        long long global_epoch = _hydra->get_global_time();
        long long next_epoch = _hydra->get_next_global_time();
        update_time(global_epoch, next_epoch);
        nged::MessageHub::debugf("Step Complete, current time: {}", get_global_time());
        nged::MessageHub::infof("Hydra step successfully in {}", formatDuration(start, stop));
    });
    step_thread.detach();
}


//============================================================================
void
AppState::__reset() noexcept
{
    auto res = _hydra->reset();
    if (!res)
    {
        nged::MessageHub::errorf("failed to reset hydra: {}", res.error().what());
        return;
    }
	update_time(0, _hydra->get_next_global_time());
}


//============================================================================
void
AppState::__create_strategy(
    std::string const& strategy_id,
    std::string const& portfolio_id,
    std::string const& exchange_id,
    double cash)
{
    auto exchange_opt  = _hydra->get_exchange_mut(exchange_id);
    auto portfolio_opt = _hydra->get_portfolio_mut(portfolio_id);
    if (!exchange_opt)
	{
		nged::MessageHub::errorf("failed to create strategy: exchange {} does not exist", exchange_id);
		return;
	}
    if (!portfolio_opt)
	{
        nged::MessageHub::errorf("failed to create strategy: portfolio {} does not exist", portfolio_id);
		return;
    }
    // create the .ng graph file if needed 
    auto env_dir_fs = std::filesystem::path(env_dir());
    auto strategy_file = env_dir_fs / "strategies" / (strategy_id + ".ng");
    if (!std::filesystem::exists(strategy_file))
    {
        std::ofstream f(strategy_file);
        if (!f.is_open())
        {
            nged::MessageHub::errorf("could not create strategy file");
        }
        f.close();
    }
    auto strategy = std::make_unique<ASTStrategy>(
        strategy_id,
        cash,
        *exchange_opt.value(),
        *portfolio_opt.value(),
        strategy_file.string()
    );
    auto res = _hydra->register_strategy(std::move(strategy));
    if (!res) 
    {
        nged::MessageHub::errorf("failed to create strategy: {}", res.error().what());
    }
    else
    {
        nged::MessageHub::infof("created strategy: {}", strategy_id);
    }
}


//============================================================================
void AppState::__create_exchange(
    std::string const& id,
    std::string const& dt_format,
    std::string const& source,
    std::string const& symbols
) noexcept
{
    nged::MessageHub::infof("creating exchange: {}", id);

    std::optional<std::vector<std::string>> symbol_list;
    if (!symbols.empty())
	{
        symbol_list = std::vector<std::string>();
		// split symbols by comma and store in symbol_list
        std::istringstream ss(symbols);
        std::string token;
        while (std::getline(ss, token, ',')) {
            (*symbol_list).push_back(token);
        }
	}

    auto res = _hydra->create_exchange(id, dt_format, source, symbol_list);
    if (!res) {
        nged::MessageHub::errorf("failed to create exchange: {}", res.error().what());
        return;
    }
    nged::MessageHub::infof("created exchange: {}", id);
}


//============================================================================
void
AppState::__create_portfolio(
    std::string const& id,
    std::string const& exchange_id,
    std::optional<Agis::Portfolio*> parent
) noexcept
{
    if (_hydra->get_state() != HydraState::BUILT)
    {
        nged::MessageHub::error("Hydra is not built");
		return;
    }
    nged::MessageHub::infof("creating portfolio: {}", id);
    auto res = _hydra->create_portfolio(id, exchange_id, parent);
    if (!res) {
        nged::MessageHub::errorf("failed to create portfolio: {}", res.error().what());
        return;
    }
	nged::MessageHub::infof("created portfolio: {}", id);
}



//============================================================================
void
AppState::emit_on_strategy_select(std::optional<Agis::Strategy*> strategy)
{
    if (!strategy) return;
    nged::MessageHub::infof("selecting strategy: {}", (*strategy)->get_strategy_id());
    for (auto& [type, view] : _views)
    {
        if (type != "network") continue;
        auto agisx_graph = dynamic_cast<AgisXGraph*>(view->graph().get());
        auto agisx_node_factory = dynamic_cast<AgisxNodeFactory const*>(agisx_graph->docRoot()->nodeFactory());
        auto ast_strategy = dynamic_cast<Agis::ASTStrategy*>(*strategy);
        assert(strategy);
        auto strategy_node = agisx_node_factory->createStrategyNode(
            agisx_graph,
            *ast_strategy
        );
        agisx_graph->set_strategy_node(strategy_node);
        agisx_graph->add(strategy_node);
        //agisx_graph->docRoot()->open(ast_strategy->graph_file_path());
    }
    nged::MessageHub::infof("strategy: {} selected", (*strategy)->get_strategy_id());
}

//============================================================================
void
AppState::emit_on_hydra_restore()
{
    for (auto& [type, view] : _views)
    {
        nged::MessageHub::debugf("emitting on_hydra_restore for {}", type);
        view->on_hydra_restore();
        nged::MessageHub::debugf("on_hydra_restore complete for {}", type);
    }
}


//============================================================================
void
AppState::emit_lock(bool lock)
{
    for (auto& [type, view] : _views)
    {
        if(lock) view->write_lock();
		else view->write_unlock();
    }
}

void AppState::update_time(long long global_time, long long next_global_time)
{
    auto t = Agis::epoch_to_str(global_time, "%Y-%m-%d %H:%M:%S");
    auto t_next = Agis::epoch_to_str(next_global_time, "%Y-%m-%d %H:%M:%S");
    set_global_time(t.value());
    set_next_global_time(t_next.value());
}

}