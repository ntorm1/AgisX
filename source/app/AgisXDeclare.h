#pragma once

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
	class AgisXExchangeView;
	class AgisXPortfolioView;

	class AgisXAssetReadNode;
	class AgisXAssetOpNode;
	class AgisXStrategyNode;
}