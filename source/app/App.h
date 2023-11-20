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

#include "imgui.h"


#define WRITE_LOCK auto lock = std::unique_lock(_mutex);
#define READ_LOCK auto lock = std::shared_lock(_mutex);


namespace AgisX 
{

class ExchangeMapComp;
class AssetComp;
class EditorComp;


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
	Agis::Hydra* get_hydra() { return _hydra.get(); }
	Agis::ExchangeMap const& get_exchanges();
	bool& get_show_demo_window() { return show_demo_window; }
	void set_dockspace_id(ImGuiID mainDockID_);
	
	bool agree_to_quit();
	void render_app_state();
	void render();
	void init();
	void reset();
	std::expected<bool, Agis::AgisException> step();
	std::expected<bool, Agis::AgisException> build();

	bool exchange_exists(std::string const& name) const noexcept;

	float TEXT_BASE_HEIGHT = 0.0f;
	float TEXT_BASE_WIDTH = 0.0f;

private:
	// === Signals === //
	void emit_new_hydra_ptr();
	void emit_step();
	void emit_reset();

	std::vector<BaseComp*> _comps;
	std::optional<Agis::AgisException> _exception;
	bool show_demo_window = true;
	
	ExchangeMapComp* exchange_comp = nullptr;
	EditorComp* editor_comp = nullptr;
	ApplicationState _app_state;

	std::string env_name = "default";
	std::string _env_dir = "";
	std::unique_ptr<Agis::Hydra> _hydra;
};

static Application instance;

}