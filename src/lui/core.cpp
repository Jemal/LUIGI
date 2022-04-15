#include <stdafx.hpp>
#include "core.hpp"
#include "uieditor/tree.hpp"

namespace lui
{
	UIElementPool core::element_pool_;
	int core::allocated_elements_ = 0;

	void core::init()
	{
		allocated_elements_ = 0;
		element_pool_.init();

		element::create_element();

		auto* root = element_pool_.at(0);
		if (root)
		{
			root->id = root->name = "UIRoot";

			root->currentAnimationState.leftAnchor = true;
			root->currentAnimationState.topAnchor = true;
			root->currentAnimationState.rightAnchor = true;
			root->currentAnimationState.bottomAnchor = true;

			root->currentAnimationState.red = 1.0f;
			root->currentAnimationState.green = 1.0f;
			root->currentAnimationState.blue = 1.0f;
			root->currentAnimationState.alpha = 1.0f;

			uieditor::tree::select_element(root);
		}
	}

	void core::frame()
	{
		layout();
		render();
	}

	UIElement* core::get_root_element()
	{
		auto* root = element_pool_.at(0);

		return root;
	}

	void core::layout()
	{
		auto* root = get_root_element();

		element::layout(root, root, 0, 0);
	}

	void core::render()
	{
		auto* root = get_root_element();

		element::render(root, root, 1.0f, 1.0f, 1.0f, 1.0f);
	}
}