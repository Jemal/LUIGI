#pragma once

namespace uieditor
{
	class widgets
	{
	public:
		static UIElement* widget_element_;
		static std::string new_widget_name_;
		static std::vector<std::string> widgets_;

		static void populate_widgets_list();

		static void save_widget(UIElement* element, std::string filepath);

		static void load_widget(std::string name);

	private:

	};
}