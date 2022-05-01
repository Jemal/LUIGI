#pragma once

#define LUI_MAX_ELEMENTS 4500

#include "element.hpp"
#include "pool.hpp"

namespace lui
{
	typedef UIPool<UIElement, 0, LUI_MAX_ELEMENTS> UIElementPool;

	class core
	{
	public:
		static void init();

		static void frame();

		static void create_root();

		static void clear_element_pool();

		static UIElement* get_root_element();

		static UIElementPool element_pool_;
		static int allocated_elements_;

	private:
		static void layout();
		static void render();

	};
}