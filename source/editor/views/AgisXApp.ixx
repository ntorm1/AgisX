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

	std::optional<Agis::Portfolio*> get_portfolio_mut(std::string const& id) const noexcept;
	std::optional<Agis::Portfolio const*> get_portfolio(std::string const& id) const noexcept;
	std::optional<Agis::Exchange const*> get_exchange(std::string const& id) const noexcept;
	std::unordered_map<std::string, size_t> const* get_exchange_ids() const noexcept;
	
	void __save_state() const noexcept;
	void __load_state() noexcept;
	void __build() noexcept;
	void __step() noexcept;
	void __reset() noexcept;

	void __create_strategy(
		std::string const& strategy_id,
		std::string const& portfolio_id,
		std::string const& exchange_id,
		double cash
	);

	void __create_exchange(
		std::string const& id,
		std::string const& dt_format,
		std::string const& source,
		std::string const& symbols
	) noexcept;
	void __create_portfolio(
		std::string const& id,
		std::string const& exchange_id,
		std::optional<Agis::Portfolio*> parent = std::nullopt
	) noexcept;

	void add_view(std::string const& name, nged::GraphViewPtr view) { WRITE_LOCK _views[name] = view; }
	//void update_time(long long global_time, long long next_global_time);

	void emit_on_hydra_restore();
	void emit_lock(bool lock = true);

	void set_global_time(std::string const& time) { WRITE_LOCK global_time = time; }
	void set_next_global_time(std::string const& time) { WRITE_LOCK next_global_time = time; }
	std::string const& get_global_time() const { READ_LOCK return global_time; }
	std::string const& get_next_global_time() const { READ_LOCK return next_global_time; }
	void update_time(long long global_time, long long next_global_time);

	// variadic template info func
	template<class... T>
	void infof(T... args)
	{
		nged::MessageHub::infof(std::forward<T>(args)...);
	}

	template<class... T>
	void errorf(T... args)
	{
		nged::MessageHub::errorf(std::forward<T>(args)...);
	}

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
