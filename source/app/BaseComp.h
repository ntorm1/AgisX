#pragma once


namespace AgisX
{

class BaseComp
{
	friend class Application;
private:
	virtual void render() = 0;
	virtual void on_hydra_restore() = 0;
};

}