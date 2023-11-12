#pragma once

#include "AgisDeclare.h"
#include "BaseComp.h"

import HydraModule;
import AgisError;

import <optional>;
import <string>;
import <memory>;
import <vector>;
import <expected>;
import <shared_mutex>;

#define WRITE_LOCK auto lock = std::unique_lock(_mutex);
#define READ_LOCK auto lock = std::shared_lock(_mutex);

using namespace Agis;

namespace AgisX 
{

class ExchangeMapComp;
class AssetComp;


class ApplicationState
{
public:
	void set_global_time(std::string const& time) { WRITE_LOCK global_time = time; }
	void set_next_global_time(std::string const& time) { WRITE_LOCK next_global_time = time; }

	std::string const& get_global_time() const { READ_LOCK return global_time; }
	std::string const& get_next_global_time() const { READ_LOCK return next_global_time; }

	void update_time(long long global_time, long long next_global_time);
private:
	mutable std::shared_mutex _mutex;
	std::string global_time = "";
	std::string next_global_time = "";
};



class Application {
public:
	Application();
	~Application();

	std::string const & get_env_name() const { return env_name; }
	Hydra* get_hydra() { return _hydra.get(); }
	ExchangeMap const& get_exchanges();
	bool& get_show_demo_window() { return show_demo_window; }
	
	void render_app_state();
	void render();

	std::expected<bool, AgisException> step();
	std::expected<bool, AgisException> build();

	float TEXT_BASE_HEIGHT = 0.0f;
	float TEXT_BASE_WIDTH = 0.0f;

private:
	// === Signals === //
	void emit_new_hydra_ptr();

	std::vector<BaseComp*> _comps;
	std::optional<AgisException> _exception;
	bool show_demo_window = true;
	
	ExchangeMapComp* exchange_comp = nullptr;

	ApplicationState _app_state;

	std::string env_name = "default";
	std::string _env_dir = "";
	std::unique_ptr<Hydra> _hydra;
};

static Application instance;

}