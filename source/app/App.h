#pragma once

#include "AgisDeclare.h"
#include "BaseComp.h"

import HydraModule;
import AgisError;

import <optional>;
import <string>;
import <memory>;
import <vector>;

using namespace Agis;

namespace AgisX 
{

class ExchangeMapComp;

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

private:
	// === Signals === //
	void emit_new_hydra_ptr();

	std::vector<BaseComp*> _comps;
	std::optional<AgisException> _exception;
	bool show_demo_window = true;
	
	ExchangeMapComp* exchange_comp = nullptr;

	std::string env_name = "default";
	std::string _env_dir = "";
	std::unique_ptr<Hydra> _hydra;
};

static Application instance;

}