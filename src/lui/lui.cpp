#include <stdafx.hpp>
#include "lui.hpp"

namespace uie
{
	UIElementPool lui::element_pool_;

	ui_element* lui::create_element()
	{
		auto new_element = element_pool_.allocate();
		if (!new_element)
		{
			return nullptr;
		}

		memset(new_element, 0, sizeof(ui_element));

		new_element->id_ = element_pool_.size() - 1;

		// go through element pool and count the amount of elements with the same type and assign <type> + <type count> to name
		new_element->name_ = "element"s;
		new_element->name_.append(std::to_string(new_element->id_));

		new_element->type_ = ui_element_type::base;

		// move the below into a different function. new_element->setup_default_animation_state()
		new_element->states_.current_.leftAnchor = true;
		new_element->states_.current_.topAnchor = true;
		new_element->states_.current_.rightAnchor = true;
		new_element->states_.current_.bottomAnchor = true;

		new_element->states_.current_.red = 1.0f;
		new_element->states_.current_.green = 1.0f;
		new_element->states_.current_.blue = 1.0f;
		new_element->states_.current_.alpha = 1.0f;

		new_element->states_.current_.flags |= AS_BOTTOM_PX | AS_BOTTOM_PT | AS_RIGHT_PX | AS_RIGHT_PT | AS_TOP_PX | AS_TOP_PT | AS_ALPHA | AS_BLUE | AS_GREEN | AS_RED | AS_LEFT_PT | AS_LEFT_PX;

		new_element->visible_ = true;

		return new_element;
	}

	int lui::get_element_count()
	{
		return element_pool_.size();
	}

	void lui::run_frame()
	{
		auto root_element = element_pool_.front();

		// need to populate the time parameters
		ui_element::layout(root_element, root_element, 0);

		ui_element::render(root_element, root_element, 1.0f, 1.0f, 1.0f, 1.0f);
	}
}