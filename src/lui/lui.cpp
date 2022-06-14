#include <stdafx.hpp>
#include "lui.hpp"

namespace uie
{
	lui::element_pool lui::element_pool_;

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

		new_element->type_ = ui_element::type::base;

		// move the below into a different function. new_element->setup_default_animation_state()
		new_element->states_.current.position.x.anchors[0] = 1.0f;
		new_element->states_.current.position.y.anchors[0] = 1.0f;
		new_element->states_.current.position.x.anchors[1] = 1.0f;
		new_element->states_.current.position.y.anchors[1] = 1.0f;

		new_element->states_.current.red = 1.0f;
		new_element->states_.current.green = 1.0f;
		new_element->states_.current.blue = 1.0f;
		new_element->states_.current.alpha = 1.0f;

		// sets all flags
		//new_element->states_.current_.flags |= all;
		new_element->states_.current_.flags_ |= ui_element::all;

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