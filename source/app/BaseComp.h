#pragma once

import <vector>;
import <memory>;

namespace AgisX
{

class BaseComp
{
	friend class Application;
protected:
	std::vector<std::unique_ptr<BaseComp>> _children;
private:
	virtual void on_step() {}
	virtual void render() = 0;
	virtual void on_hydra_restore() = 0;
};

}