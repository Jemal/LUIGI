#include <stdafx.hpp>
#include "ui_element.hpp"

#include "editor/panels/canvas.hpp"

namespace uie
{
	void ui_element::set_parent(ui_element* parent)
	{
		this->parent_ = parent;

		this->invalidate_layout();

		this->inherit_parent_flags();
	}

	void ui_element::inherit_parent_flags()
	{
		auto child = this->first_child_;
		while (child)
		{
			child->inherit_parent_flags();
			child = child->next_sibling_;

			assert(child != this->first_child_);
		}
	}

	void ui_element::calculate_global_rectangle(animation_state* animation_state)
	{
		auto* parent = this->parent_;
		if (parent)
		{
			auto left_anchor = animation_state->position.x.anchors[0];
			auto right_anchor = animation_state->position.x.anchors[1];
			auto relative_left = (1.0f - left_anchor) * parent->states_.current_.position.x.global[0];
			auto relative_right = (1.0f - right_anchor) * parent->states_.current_.position.x.global[1];

			animation_state->position.x.global[0] = (relative_left + (left_anchor * parent->states_.current_.position.x.global[1])) + animation_state->position.x.offsets[0];
			animation_state->position.x.global[1] = (relative_right + (right_anchor * parent->states_.current_.position.x.global[0])) + animation_state->position.x.offsets[1];

			auto top_anchor = animation_state->position.y.anchors[0];
			auto bottom_anchor = animation_state->position.y.anchors[1];
			auto relative_top = parent->states_.current_.position.y.global[0] * (1.0f - top_anchor);
			auto relative_bottom = parent->states_.current_.position.y.global[1] * (1.0f - bottom_anchor);

			animation_state->position.y.global[0] = (relative_top + (top_anchor * parent->states_.current_.position.y.global[1])) + animation_state->position.y.offsets[0];
			animation_state->position.y.global[1] = (relative_bottom + (bottom_anchor * parent->states_.current_.position.y.global[0])) + animation_state->position.y.offsets[1];
		}
	}

	void ui_element::set_dimensions(ui_element* root)
	{
		if (this == root)
		{
			auto canvas_size = canvas::get_size();

			this->left_ = root->states_.current_.position.x.global[0] = 0.0f;
			this->top_ = root->states_.current_.position.y.global[0] = 0.0f;
			this->right_ = root->states_.current_.position.x.global[1] = canvas_size.x;
			this->bottom_ = root->states_.current_.position.y.global[1] = canvas_size.y;
		}
		else
		{
			this->left_ = root->states_.current_.position.x.offsets[0] + this->states_.current_.position.x.global[0];
			this->top_ = root->states_.current_.position.y.offsets[0] + this->states_.current_.position.y.global[0];
			this->right_ = root->states_.current_.position.x.offsets[0] + this->states_.current_.position.x.global[1];
			this->bottom_ = root->states_.current_.position.y.offsets[0] + this->states_.current_.position.y.global[1];
		}
	}

	bool ui_element::update_layout(ui_element* root, int delta_system_time)
	{
		if (this->parent_ && (this->parent_->states_.current_.flags_ & animation_state::layout_cached) == 0)
		{
			this->invalidate_layout();
		}

		if (this->states_.prev_ && this->states_.next_)
		{
			/*calculate_global_rectangle(element, element->prevAnimationState);
			calculate_global_rectangle(element, element->nextAnimationState);
			animation::calculate_current_animation_state(element, delta_game_time, delta_system_time);
			set_dimensions(element, root);*/

			return false;
		}
		else
		{
			if ((this->states_.current_.flags_ & layout_cached) == 0)
			{
				this->calculate_global_rectangle(&this->states_.current_);
				this->set_dimensions(root);
			}

			return true;
		}
	}

	void ui_element::default_layout(ui_element* root, int delta_time)
	{
		auto was_cached = (this->states_.current_.flags & layout_cached) != 0;
		auto cache_layout = this->update_layout(root, delta_time);

		if (this->states_.current_.alpha > 0.000099999997f)
		{
			layout(this->first_child_, root, delta_time);
		}

		if (!was_cached || (this->states_.current_.flags & layout_cached) != 0)
		{
			auto flags = 0;

			if (cache_layout)
			{
				flags = this->states_.current_.flags | layout_cached;
			}
			else
			{
				flags = this->states_.current_.flags & -3;
			}

			this->states_.current_.flags = flags;
		}
	}

	void ui_element::ui_image_render(ui_element* element, ui_element* root, float red, float green, float blue, float alpha)
	{
		auto left = element->left_;
		auto right = element->right_;
		auto top = element->top_;
		auto bottom = element->bottom_;

		auto width = right - left;
		auto height = bottom - top;

		canvas::draw_image((ID3D11ShaderResourceView*)element->states_.current_.image, left, top, width, height, 0.0f, ImVec4(red, green, blue, alpha));
	}

	void ui_element::invalidate_layout()
	{
		this->states_.current_.flags &= ~layout_cached;
	}

	void ui_element::setup()
	{
		this->render_callback_ = ui_image_render;
	}

	int ui_element::anchors_to_int()
	{
		auto value = 0;

		if (this->states_.current_.position.x.anchors[0])
		{
			value += anchor_type::left;
		}

		if (this->states_.current_.position.y.anchors[0])
		{
			value += anchor_type::top;
		}

		if (this->states_.current_.position.x.anchors[1])
		{
			value += anchor_type::right;
		}

		if (this->states_.current_.position.y.anchors[1])
		{
			value += anchor_type::bottom;
		}

		return value;
	}

	const char* ui_element::anchors_to_string(int anchor_value)
	{
		switch (anchor_value)
		{
		case animation_state::anchor_type::none:
			return "None";
		case animation_state::anchor_type::all:
			return "All";
		case animation_state::anchor_type::top:
			return "Top";
		case animation_state::anchor_type::top_left:
			return "Top-Left";
		case animation_state::anchor_type::top_right:
			return "Top-Right";
		case animation_state::anchor_type::top_left_right:
			return "Top-Left-Right";
		case animation_state::anchor_type::bottom:
			return "Bottom";
		case animation_state::anchor_type::bottom_left:
			return "Bottom-Left";
		case animation_state::anchor_type::bottom_right:
			return "Bottom-Right";
		case animation_state::anchor_type::bottom_left_right:
			return "Bottom-Left-Right";
		case animation_state::anchor_type::top_bottom:
			return "Top-Bottom";
		case animation_state::anchor_type::top_bottom_left:
			return "Top-Bottom-Left";
		case animation_state::anchor_type::top_bottom_right:
			return "Top-Bottom-Right";
		case animation_state::anchor_type::left:
			return "Left";
		case animation_state::anchor_type::right:
			return "Right";
		case animation_state::anchor_type::left_right:
			return "Left-Right";
		}

		return "Invalid Anchor";
	}

	void ui_element::add_element(ui_element* child)
	{
		child->set_parent(this);

		assert(child->prev_sibling_ == NULL);
		assert(child->next_sibling_ == NULL);

		if (this->first_child_)
		{
			auto sibling = this->first_child_;
			assert(sibling->prev_sibling_ == nullptr);

			while (1)
			{
				auto has_priority = 0;
				if (sibling)
				{
					has_priority = sibling->priority_ <= child->priority_;
				}

				if (!has_priority)
				{
					break;
				}

				sibling = sibling->next_sibling_;
			}

			if (sibling)
			{
				if (sibling->prev_sibling_)
				{
					child->prev_sibling_ = sibling->prev_sibling_;
					child->prev_sibling_->next_sibling_ = child;
				}
				else
				{
					this->first_child_ = child;
				}

				child->next_sibling_ = sibling;
				sibling->prev_sibling_ = child;
			}
			else
			{
				child->prev_sibling_ = this->last_child_;
				this->last_child_->next_sibling_ = child;
				this->last_child_ = child;
			}
		}
		else
		{
			assert(this->last_child_ == nullptr);

			this->first_child_= child;
			this->last_child_ = child;
		}
	}

	bool ui_element::is_descendent_of(ui_element* element)
	{
		auto parent = element;
		while (parent)
		{
			if (parent == this)
			{
				return true;
			}

			parent = parent->parent_;
		}

		return false;
	}

	void ui_element::layout(ui_element* element, ui_element* root, int delta_time)
	{
		if (element)
		{
			element->states_.current_.flags |= in_use;

			if (element->layout_callback_)
			{
				element->layout_callback_(root, delta_time);
			}
			else
			{
				element->default_layout(root, delta_time);
			}

			layout(element->next_sibling_, root, delta_time);
		}
	}

	void ui_element::render(ui_element* element, ui_element* root, float red, float green, float blue, float alpha)
	{
		while (1)
		{
			auto in_use = false;

			if (element)
			{
				in_use = (element->states_.current_.flags & in_use) != 0;
			}

			if (!in_use)
			{
				break;
			}

			if (!element->visible_)
			{
				element = element->next_sibling_;

				continue;
			}

			auto current_red = red * element->states_.current_.red;
			auto current_green = green * element->states_.current_.green;
			auto current_blue = blue * element->states_.current_.blue;
			auto current_alpha = alpha * element->states_.current_.alpha;

			if (current_alpha > 0.000099999997f)
			{
				if ((element->states_.current_.flags & stencil) != 0)
				{
					//canvas::push_stencil(element->left, element->top, element->right, element->bottom);
				}

				// cod also checks if the quad is cached here but i honestly think this is fine atm
				if (!element->render_callback_)
				{
					render(element->first_child_, root, current_red, current_green, current_blue, current_alpha);
				}
				else
				{
					element->render_callback_(element, root, current_red, current_green, current_blue, current_alpha);

					// fug it we ball (cod doesn't do this)
					render(element->first_child_, root, current_red, current_green, current_blue, current_alpha);
				}

				if ((element->states_.current_.flags & stencil) != 0)
				{
					//canvas::pop_stencil();
				}
			}

			element = element->next_sibling_;
		}
	}
}