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
import PortfolioModule;
import ExchangeMapModule;
import ASTStrategyModule;
import AgisTimeUtils;

import AgisXNodeFactory;
import AgisXGraph;


namespace AgisX
{


//============================================================================
std::string
formatDuration(const std::chrono::high_resolution_clock::time_point& start,
    const std::chrono::high_resolution_clock::time_point& stop,
    int precision = 2) {
    auto elapsed = std::chrono::duration_cast<std::chrono::microseconds>(stop - start);

    if (elapsed.count() < 1000) {
        return std::to_string(elapsed.count()) + "us";
    }
    else if (elapsed.count() < 1000000) {
        // Display with millisecond precision
        auto elapsedMillis = std::chrono::duration_cast<std::chrono::milliseconds>(elapsed);
        return std::to_string(elapsedMillis.count()) + "ms";
    }
    else {
        // Display with second precision
        auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(elapsed);
        std::ostringstream ss;
        ss << std::fixed << std::setprecision(precision)
            << static_cast<double>(elapsedSeconds.count()) << "s";
        return ss.str();
    }
}

//============================================================================
AppState::AppState()
{
    _hydra = std::make_unique<Agis::Hydra>();

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
Agis::ExchangeMap const&
AppState::get_exchanges() const noexcept
{
    return _hydra->get_exchanges();
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
std::optional<nged::GraphViewPtr>
AppState::get_network_view()
{
    for (auto& [type, view] : _views)
    {
		if (type == "network") return view;
	}
    return std::nullopt;
}


//============================================================================
void
AppState::__load_strategies_from_disk() noexcept
{
    // find all files in the strategies directory
    auto strategy_dir = _env_dir + "/strategies";
    std::vector<std::string> ng_files;
    for (const auto& entry : std::filesystem::directory_iterator(strategy_dir))
    {
        if (entry.is_regular_file() && entry.path().extension() == ".ng")
        {
            ng_files.push_back(entry.path().string());
        }
    }
    auto view = get_network_view().value();
    auto agisx_graph = dynamic_cast<AgisXGraph*>(view->graph().get());
    auto agisx_node_factory = dynamic_cast<AgisxNodeFactory const*>(agisx_graph->docRoot()->nodeFactory());
    bool loaded_successfully = true;
    for (const auto& file : ng_files)
    {
        // get the file name 
        auto file_path = std::filesystem::path(file);
        auto file_name = file_path.stem().string();
        nged::MessageHub::infof("Loading strategy: {}", file_name);
        auto strategy_opt = _hydra->get_strategy_mut(file_name);
        if (!strategy_opt)
        {
			nged::MessageHub::errorf("failed to get strategy: {}", file_name);
            loaded_successfully = false;
			continue;
		}
        auto ast_strategy = dynamic_cast<Agis::ASTStrategy*>(*strategy_opt);
        if (!ast_strategy)
        {
			nged::MessageHub::errorf("failed to cast strategy to AST type: {}", file_name);
			loaded_successfully = false;
            continue;
		}
        // load in .ng graph file and create the AST Strategy graph
        view->editor()->set_strategy(ast_strategy);
        agisx_node_factory->reset_strategy_node_count();
        view->editor()->loadDocInto(ast_strategy->graph_file_path(), view->doc());
        agisx_graph = dynamic_cast<AgisXGraph*>(view->graph().get());
        agisx_graph->strategy_node()->onSave();
    }
    if (loaded_successfully)
    {
        nged::MessageHub::infof("loaded {} strategies", ng_files.size());
    }
    else
    {
        nged::MessageHub::errorf("failed to load strategies");
    }
}

//============================================================================
void
AppState::__load_state() noexcept
{
    std::thread load_thread([this] {
        nged::MessageHub::infof("loading state from: {}", _env_dir);
        auto now = std::chrono::system_clock::now();
        auto res = Agis::deserialize_hydra(_env_dir + "/hydra.json");
        if (res)
        {
            emit_lock(true);
            _hydra = std::move(res.value());
            __load_strategies_from_disk();
            auto end = std::chrono::system_clock::now();
            auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(end - now);
            emit_hydra_event(&nged::GraphView::on_hydra_restore, "hydra_restore");
            nged::MessageHub::infof("Hydra loaded in {} ms", std::to_string(elapsed.count()));
        }
        else
        {
            nged::MessageHub::errorf("failed to load state: {}", res.error().what());
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
        if (_hydra->get_state() != Agis::HydraState::INIT)
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
        for (auto& [type, view] : _views)
        {
            view->on_hydra_build();
        }
        });
    build_thread.detach();
}


//============================================================================
void
AppState::__step() noexcept
{
    std::thread step_thread([this] {
        auto portfolio_lock = _hydra->get_portfolio_mut("master").value()->__aquire_write_lock();
        auto lock = _hydra->__aquire_write_lock();
        nged::MessageHub::debugf("Starting step, current time: {}", get_global_time());
        if (_hydra->get_state() != Agis::HydraState::BUILT && _hydra->get_state() != Agis::HydraState::RUN)
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
        
        portfolio_lock.unlock();
        emit_hydra_event(&nged::GraphView::on_hydra_step, "hydra_step");
        nged::MessageHub::infof("Hydra step successfully in {}", formatDuration(start, stop));
        nged::MessageHub::debugf("Step Complete, current time: {}", get_global_time());
    });
    step_thread.detach();
}


//============================================================================
void
AppState::__run() noexcept
{
    std::thread run_thread([this] {
        nged::MessageHub::debugf("Starting Run, current time: {}", get_global_time());
        if (_hydra->get_state() != Agis::HydraState::BUILT && _hydra->get_state() != Agis::HydraState::RUN)
        {
            nged::MessageHub::error("Hydra is not built or running");
            return;
        }
        auto start = std::chrono::high_resolution_clock::now();
        auto res = _hydra->run();
        auto stop = std::chrono::high_resolution_clock::now();
        long long global_epoch = _hydra->get_global_time();
        long long next_epoch = 0;
        update_time(global_epoch, next_epoch);
        nged::MessageHub::debugf("Run Complete, current time: {}", get_global_time());
        nged::MessageHub::infof("Hydra Run successfully in {}", formatDuration(start, stop));
    
        // get number of seconds elapsed
        auto duration = std::chrono::duration_cast<std::chrono::duration<double>>(stop - start);
        double seconds = duration.count();
        double candles = _hydra->get_exchanges().get_candle_count();
        nged::MessageHub::infof("Candles {}", candles);
        nged::MessageHub::infof("Candles Per Second {}", candles / seconds);
    });
    run_thread.detach();
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
    emit_hydra_event(&nged::GraphView::on_hydra_reset, "hydra_step");
}


//============================================================================
void AppState::__interupt() noexcept
{
    _hydra->interupt();
}


//============================================================================
void
AppState::__create_strategy(
    std::string const& strategy_id,
    std::string const& portfolio_id,
    std::string const& exchange_id,
    double cash)
{
    if (_hydra->get_state() != Agis::HydraState::BUILT)
    {
        nged::MessageHub::errorf("Hydra must be in build state for new strategy");
        return;
    }
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
    auto strategy = std::make_unique<Agis::ASTStrategy>(
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
    if (_hydra->get_state() != Agis::HydraState::BUILT)
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
    
    auto view =  get_network_view().value();
    auto agisx_graph = dynamic_cast<AgisXGraph*>(view->graph().get());
    auto agisx_node_factory = dynamic_cast<AgisxNodeFactory const*>(agisx_graph->docRoot()->nodeFactory());
    auto ast_strategy = dynamic_cast<Agis::ASTStrategy*>(*strategy);
    if(!ast_strategy)
	{
		nged::MessageHub::errorf("failed to cast strategy to AST type: {}", (*strategy)->get_strategy_id());
		return;
	}
    auto strategy_node = agisx_node_factory->createStrategyNode(
        agisx_graph,
        *ast_strategy
    );
    //agisx_graph->docRoot()->open(ast_strategy->graph_file_path(), strategy_node);
    view->editor()->set_strategy(ast_strategy);
    agisx_node_factory->reset_strategy_node_count();
    view->editor()->loadDocInto(ast_strategy->graph_file_path(), view->doc());
    nged::MessageHub::infof("strategy: {} selected", (*strategy)->get_strategy_id());
}


//============================================================================
template <typename MemberFunction> void
AppState::emit_hydra_event(MemberFunction member_func, const char* msg) noexcept
{
    for (auto& [type, view] : _views)
    {
        nged::MessageHub::debugf("emitting {} for {}", type, msg);
        (view.get()->*member_func)();
        nged::MessageHub::debugf("{} complete for {}", type, msg);
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


//============================================================================
std::vector<long long> const&
AppState::get_global_dt_index() const noexcept
{
    return _hydra->get_dt_index();
}


//============================================================================
size_t
AppState::get_current_index() const noexcept
{
    return _hydra->get_current_index();
}


void
AppState::update_time(long long global_time, long long next_global_time)
{
    global_time_epoch = global_time;
    next_global_time_epoch = next_global_time;
    auto t = Agis::epoch_to_str(global_time, "%Y-%m-%d %H:%M:%S");
    auto t_next = Agis::epoch_to_str(next_global_time, "%Y-%m-%d %H:%M:%S");
    set_global_time(t.value());
    set_next_global_time(t_next.value());
}

}