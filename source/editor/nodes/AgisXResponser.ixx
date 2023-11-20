module;

#include "../nged_imgui.h"

export module AgisXResponser;

namespace AgisX
{

	export class AgisXResponser : public nged::DefaultImGuiResponser
	{
	public:
		void onInspect(nged::InspectorView* view, nged::GraphItem** items, size_t numItems) override;
		void onItemHovered(nged::NetworkView* view, nged::GraphItem* item) override;
	};



}