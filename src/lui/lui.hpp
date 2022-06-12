#pragma once

#include "lui_cod.hpp"
#include "ui_element.hpp"

namespace uie
{
	class lui
	{
	public:
		static UIElementPool element_pool_;

		static ui_element* create_element();

		static int get_element_count();

		static void run_frame();

	private:
	};
}