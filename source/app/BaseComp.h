#pragma once

import <vector>;
import <memory>;

namespace AgisX
{

class Application;
class ExchangeComp;

class BaseComp
{
	friend class Application;
	friend class ExchangeMapComp;

protected:
	std::vector<std::unique_ptr<BaseComp>> _children;
	
	virtual void on_step()
	{
		for (auto& child : _children)
			child->on_step();
	
	}
	virtual void on_reset()
	{
		for (auto& child : _children)
			child->on_reset();
	}

private:
	virtual void render() = 0;
	virtual void on_hydra_restore() = 0;
};

}