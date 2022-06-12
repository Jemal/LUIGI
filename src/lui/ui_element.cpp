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

	void ui_element::calculate_global_rectangle(UIAnimationState* animation_state)
	{
		if (this->parent_)
		{
			if (animation_state->leftAnchor)
			{
				animation_state->leftPct = 0;

				if (animation_state->rightAnchor)
				{
					animation_state->rightPct = 1.0f;
				}
				else
				{
					animation_state->rightPct = 0.0f;
				}
			}
			else if (animation_state->rightAnchor)
			{
				animation_state->leftPct = 1.0f;
				animation_state->rightPct = 1.0f;
			}
			else
			{
				animation_state->leftPct = 0.5f;
				animation_state->rightPct = 0.5f;
			}

			if (animation_state->topAnchor)
			{
				animation_state->topPct = 0;

				if (animation_state->bottomAnchor)
				{
					animation_state->bottomPct = 1.0f;
				}
				else
				{
					animation_state->bottomPct = 0;
				}
			}
			else if (animation_state->bottomAnchor)
			{
				animation_state->topPct = 1.0f;
				animation_state->bottomPct = 1.0f;
			}
			else
			{
				animation_state->topPct = 0.5f;
				animation_state->bottomPct = 0.5f;
			}

			auto parent_width = this->parent_->states_.current_.globalRight - this->parent_->states_.current_.globalLeft;
			animation_state->globalLeft = (this->parent_->states_.current_.globalLeft + (parent_width * animation_state->leftPct)) + animation_state->leftPx;
			animation_state->globalRight = (this->parent_->states_.current_.globalLeft + (parent_width * animation_state->rightPct)) + animation_state->rightPx;

			auto parent_height = this->parent_->states_.current_.globalBottom - this->parent_->states_.current_.globalTop;
			animation_state->globalTop = (this->parent_->states_.current_.globalTop + (parent_height * animation_state->topPct)) + animation_state->topPx;
			animation_state->globalBottom = (this->parent_->states_.current_.globalTop + (parent_height * animation_state->bottomPct)) + animation_state->bottomPx;
		}
	}

	void ui_element::set_dimensions(ui_element* root)
	{
		if (this == root)
		{
			auto canvas_size = canvas::get_size();

			this->left_ = root->states_.current_.globalLeft = 0.0f;
			this->top_ = root->states_.current_.globalTop = 0.0f;
			this->right_ = root->states_.current_.globalRight = canvas_size.x;
			this->bottom_ = root->states_.current_.globalBottom = canvas_size.y;
		}
		else
		{
			this->left_ = root->states_.current_.leftPx + this->states_.current_.globalLeft;
			this->top_ = root->states_.current_.topPx + this->states_.current_.globalTop;
			this->right_ = root->states_.current_.leftPx + this->states_.current_.globalRight;
			this->bottom_ = root->states_.current_.topPx + this->states_.current_.globalBottom;
		}
	}

	bool ui_element::update_layout(ui_element* root, int delta_system_time)
	{
		if (this->parent_ && (this->parent_->states_.current_.flags & AS_LAYOUT_CACHED) == 0)
		{
			this->states_.current_.flags &= -3;
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
			if ((this->states_.current_.flags & AS_LAYOUT_CACHED) == 0)
			{
				this->calculate_global_rectangle(&this->states_.current_);
				this->set_dimensions(root);
			}

			return true;
		}
	}

	void ui_element::default_layout(ui_element* root, int delta_time)
	{
		auto was_cached = (this->states_.current_.flags & AS_LAYOUT_CACHED) != 0;
		auto cache_layout = this->update_layout(root, delta_time);

		if (this->states_.current_.alpha > 0.000099999997f)
		{
			layout(this->first_child_, root, delta_time);
		}

		if (!was_cached || (this->states_.current_.flags & AS_LAYOUT_CACHED) != 0)
		{
			auto flags = 0;

			if (cache_layout)
			{
				flags = this->states_.current_.flags | AS_LAYOUT_CACHED;
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
		this->states_.current_.flags &= ~AS_LAYOUT_CACHED;
	}

	void ui_element::setup()
	{
		this->render_callback_ = ui_image_render;
	}

	int ui_element::anchors_to_int()
	{
		auto value = 0;

		if (this->states_.current_.leftAnchor)
		{
			value += 1;
		}

		if (this->states_.current_.topAnchor)
		{
			value += 2;
		}

		if (this->states_.current_.rightAnchor)
		{
			value += 4;
		}

		if (this->states_.current_.bottomAnchor)
		{
			value += 8;
		}

		return value;
	}

	const char* ui_element::anchors_to_string(int anchor_value)
	{
		switch (anchor_value)
		{
		case UIAnchorType::ANCHOR_NONE:
			return "None";
		case UIAnchorType::ANCHOR_ALL:
			return "All";
		case UIAnchorType::ANCHOR_TOP:
			return "Top";
		case UIAnchorType::ANCHOR_TOP_LEFT:
			return "Top-Left";
		case UIAnchorType::ANCHOR_TOP_RIGHT:
			return "Top-Right";
		case UIAnchorType::ANCHOR_TOP_LEFT_RIGHT:
			return "Top-Left-Right";
		case UIAnchorType::ANCHOR_BOTTOM:
			return "Bottom";
		case UIAnchorType::ANCHOR_BOTTOM_LEFT:
			return "Bottom-Left";
		case UIAnchorType::ANCHOR_BOTTOM_RIGHT:
			return "Bottom-Right";
		case UIAnchorType::ANCHOR_BOTTOM_LEFT_RIGHT:
			return "Bottom-Left-Right";
		case UIAnchorType::ANCHOR_TOP_BOTTOM:
			return "Top-Bottom";
		case UIAnchorType::ANCHOR_TOP_BOTTOM_LEFT:
			return "Top-Bottom-Left";
		case UIAnchorType::ANCHOR_TOP_BOTTOM_RIGHT:
			return "Top-Bottom-Right";
		case UIAnchorType::ANCHOR_LEFT:
			return "Left";
		case UIAnchorType::ANCHOR_RIGHT:
			return "Right";
		case UIAnchorType::ANCHOR_LEFT_RIGHT:
			return "Left-Right";
		}

		return "Unknown Anchor";
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
			element->states_.current_.flags |= AS_IN_USE;

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
				in_use = (element->states_.current_.flags & AS_IN_USE) != 0;
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
				if ((element->states_.current_.flags & AS_STENCIL) != 0)
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

				if ((element->states_.current_.flags & AS_STENCIL) != 0)
				{
					//canvas::pop_stencil();
				}
			}

			element = element->next_sibling_;
		}
	}
}