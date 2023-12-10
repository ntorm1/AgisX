#pragma once
#include <memory>

namespace nged
{
	class GraphView;
	using GraphViewPtr = std::shared_ptr<GraphView>;
}

namespace AgisX
{
	template <typename T>
	using UniquePtr = std::unique_ptr<T>;

	template <typename T>
	using SharedPtr = std::shared_ptr<T>;

	class Application;
	class AppState;
	class BaseComp;

	class AgisXExchangeViewPrivate;
	class AgisXPortfolioViewPrivate;
	class AgisXAssetViewPrivate;
	class AgisXPlotViewPrivate;
	class AgisXExchangeView;
	class AgisXPortfolioView;
	class AgisXPlotViewPrivate;
	class AgisXAssetReadNode;
	class AgisXAssetOpNode;
	class AgisXStrategyNode;
	class AgisXAssetPlot;
	class AgisXPortfolioPlot;
}