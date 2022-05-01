#pragma once

namespace uieditor
{
	class tree
	{
	public:
		static void draw();

		static void select_element(UIElement* element);
	
	private:
		static void display_element(UIElement* element);
		static void display_element_tree(UIElement* element);
	};
}