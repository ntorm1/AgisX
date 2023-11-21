module;
#include "AgisDeclare.h"
#include "../nged_imgui.h"

#define WRITE_LOCK auto lock = std::unique_lock(_mutex);
#define READ_LOCK auto lock = std::shared_lock(_mutex);
#include <shared_mutex>
#include <string>
#include <memory>

export module AgisXApp;

namespace AgisX
{

export class AppState
{
public:
	AppState();
	~AppState();


	std::optional<Agis::Exchange const*> get_exchange(std::string const& id) const noexcept;
	std::unordered_map<std::string, size_t> const* get_exchange_ids() const noexcept;
	
	void __save_state() const noexcept;
	void __load_state() noexcept;
	void __build() noexcept;
	void __step() noexcept;
	void __reset() noexcept;
	void add_view(std::string const& name, nged::GraphViewPtr view) { WRITE_LOCK _views[name] = view; }
	//void update_time(long long global_time, long long next_global_time);

	void emit_on_hydra_restore();

	void set_global_time(std::string const& time) { WRITE_LOCK global_time = time; }
	void set_next_global_time(std::string const& time) { WRITE_LOCK next_global_time = time; }
	std::string const& get_global_time() const { READ_LOCK return global_time; }
	std::string const& get_next_global_time() const { READ_LOCK return next_global_time; }
	void update_time(long long global_time, long long next_global_time);

private:
	mutable std::shared_mutex _mutex;
	std::string global_time = "";
	std::string next_global_time = "";

	std::string env_name = "default";
	std::string _env_dir = "";
	std::unique_ptr<Agis::Hydra> _hydra;
	std::unordered_map<std::string, nged::GraphViewPtr> _views;
};


}
