#include <stdafx.hpp>
#include "element.hpp"
#include "uieditor/canvas.hpp"
#include "uieditor/log.hpp"
#include "uieditor/properties.hpp"
#include "uieditor/tree.hpp"

namespace lui
{
	void element::add_element(UIElement* element, UIElement* child)
	{
		element::set_parent(child, element);

		assert(child->nextSibling == nullptr);
		assert(child->prevSibling == nullptr);

		if (element->firstChild)
		{
			auto sibling = element->firstChild;
			assert(sibling->prevSibling == nullptr);

			while (1)
			{
				auto has_priority = 0;
				if (sibling)
				{
					has_priority = sibling->priority <= child->priority;
				}

				if (!has_priority)
				{
					break;
				}

				sibling = sibling->nextSibling;
			}

			if (sibling)
			{
				if (sibling->prevSibling)
				{
					child->prevSibling = sibling->prevSibling;
					child->prevSibling->nextSibling = child;
				}
				else
				{
					element->firstChild = child;
				}

				child->nextSibling = sibling;
				sibling->prevSibling = child;
			}
			else
			{
				child->prevSibling = element->lastChild;
				element->lastChild->nextSibling = child;
				element->lastChild = child;
			}
		}
		else
		{
			assert(element->lastChild == nullptr);

			element->firstChild = child;
			element->lastChild = child;
		}
	}

	void element::create_element()
	{
		auto element = core::element_pool_.allocate();
		if (!element)
		{
			uieditor::log::print(uieditor::log_error, "Failed to allocate element");
		}
		else
		{
			memset(element, 0, sizeof(UIElement));

			element->id = "UIElement"s;
			element->id.append(std::to_string(core::allocated_elements_));

			element->name = element->id;

			element->type = UIElementType::UI_ELEMENT;

			element->currentAnimationState.leftAnchor = true;
			element->currentAnimationState.topAnchor = true;
			element->currentAnimationState.rightAnchor = true;
			element->currentAnimationState.bottomAnchor = true;

			element->currentAnimationState.red = 1.0f;
			element->currentAnimationState.green = 1.0f;
			element->currentAnimationState.blue = 1.0f;
			element->currentAnimationState.alpha = 1.0f;

			element->currentAnimationState.flags |= AS_BOTTOM_PX | AS_BOTTOM_PT | AS_RIGHT_PX | AS_RIGHT_PT | AS_TOP_PX | AS_TOP_PT | AS_ALPHA | AS_BLUE | AS_GREEN | AS_RED | AS_LEFT_PT | AS_LEFT_PX;

			core::allocated_elements_++;
		}
	}

	UIElement* element::add_and_select_element(bool from_canvas)
	{
		lui::element::create_element();

		auto element = uieditor::properties::element_;

		auto child = lui::core::element_pool_.at(lui::core::element_pool_.size());

		lui::element::add_element(element, child);

		// place the element based on where we clicked in the canvas
		if (from_canvas)
		{
			child->currentAnimationState.topAnchor = true;
			child->currentAnimationState.leftAnchor = true;
			child->currentAnimationState.rightAnchor = false;
			child->currentAnimationState.bottomAnchor = false;

			auto left = uieditor::canvas::mouse_pos_.x - (element->left * uieditor::canvas::zoom_pct_);
			auto top = uieditor::canvas::mouse_pos_.y - (element->top * uieditor::canvas::zoom_pct_);

			//::log::print(0, "%g %g [%g, %g]", left, top, uieditor::canvas::mouse_pos.x, uieditor::canvas::mouse_pos.y);

			child->currentAnimationState.leftPx = left;
			child->currentAnimationState.topPx = top;
			child->currentAnimationState.rightPx = left + 75.0f;
			child->currentAnimationState.bottomPx = top + 75.0f;
		}

		uieditor::tree::select_element(child);

		return child;
	}

	std::string element::type_to_string(int type)
	{
		if (type == UIElementType::UI_IMAGE)
		{
			return "UIImage";
		}
		else if (type == UIElementType::UI_TEXT)
		{
			return "UIText";
		}

		return "UIElement";
	}

	void element::remove_element(UIElement* element)
	{
		uieditor::log::print(uieditor::log_normal, "Removed element '%s'\n", element->name.data());

		remove_children(element);

		remove_from_parent(element);

		core::element_pool_.free(element);

		core::allocated_elements_--;
	}

	void element::remove_children(UIElement* element)
	{
		auto child = element->firstChild;
		while (child)
		{
			auto next = child->nextSibling;

			remove_element(child);

			child = next;
		}
	}

	bool element::is_descendent_of(UIElement* element, UIElement* child)
	{
		auto parent = element;
		while (parent)
		{
			if (parent == child)
			{
				return true;
			}

			parent = parent->parent;
		}

		return false;
	}

	void element::invalidate_layout(UIElement* element)
	{
		element->currentAnimationState.flags &= ~AS_LAYOUT_CACHED;
	}

	void element::inherit_parent_flags(UIElement* element, UIElement* parent)
	{
		auto child = element->firstChild;
		while (child)
		{
			element::inherit_parent_flags(child, parent);
			child = child->nextSibling;

			assert(child != element->firstChild);
		}
	}

	void element::remove_from_parent(UIElement* element)
	{
		auto* parent = element->parent;
		auto* child = element;

		assert(child->parent == parent);

		if (child->prevSibling)
		{
			child->prevSibling->nextSibling = child->nextSibling;
		}
		else
		{
			assert(parent->firstChild == child);
			parent->firstChild = child->nextSibling;
		}

		if (child->nextSibling)
		{
			child->nextSibling->prevSibling = child->prevSibling;
		}
		else
		{
			assert(parent->lastChild == child);
			parent->lastChild = child->prevSibling;
		}

		parent->child_count--;

		child->parent = nullptr;
		child->prevSibling = nullptr;
		child->nextSibling = nullptr;

		invalidate_layout(parent);
	}

	void element::set_parent(UIElement* element, UIElement* parent)
	{
		element->parent = parent;

		element->currentAnimationState.flags &= -3;
		element::inherit_parent_flags(element, parent);

		element->name = parent->name + "Child"s.append(std::to_string(parent->child_count));
		
		parent->child_count++;
	}

	int element::anchors_to_int(UIElement* element)
	{
		auto value = 0;

		if (element->currentAnimationState.leftAnchor)
		{
			value += 1;
		}

		if (element->currentAnimationState.topAnchor)
		{
			value += 2;
		}

		if (element->currentAnimationState.rightAnchor)
		{
			value += 4;
		}

		if (element->currentAnimationState.bottomAnchor)
		{
			value += 8;
		}

		return value;
	}

	const char* element::anchors_to_string(int anchor_value)
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
			return "TopLeft";
		case UIAnchorType::ANCHOR_TOP_RIGHT:
			return "TopRight";
		case UIAnchorType::ANCHOR_TOP_LEFT_RIGHT:
			return "TopLeftRight";
		case UIAnchorType::ANCHOR_BOTTOM:
			return "Bottom";
		case UIAnchorType::ANCHOR_BOTTOM_LEFT:
			return "BottomLeft";
		case UIAnchorType::ANCHOR_BOTTOM_RIGHT:
			return "BottomRight";
		case UIAnchorType::ANCHOR_BOTTOM_LEFT_RIGHT:
			return "BottomLeftRight";
		case UIAnchorType::ANCHOR_TOP_BOTTOM:
			return "TopBottom";
		case UIAnchorType::ANCHOR_TOP_BOTTOM_LEFT:
			return "TopBottomLeft";
		case UIAnchorType::ANCHOR_TOP_BOTTOM_RIGHT:
			return "TopBottomRight";
		case UIAnchorType::ANCHOR_LEFT:
			return "Left";
		case UIAnchorType::ANCHOR_RIGHT:
			return "Right";
		case UIAnchorType::ANCHOR_LEFT_RIGHT:
			return "LeftRight";
		}

		return "Unknown Anchor";
	}

	void element::calculate_global_rectangle(UIElement* element, UIAnimationState* animation_state)
	{
		if (element->parent)
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

			auto parent_width = element->parent->currentAnimationState.globalRight - element->parent->currentAnimationState.globalLeft;
			animation_state->globalLeft = (element->parent->currentAnimationState.globalLeft + (parent_width * animation_state->leftPct)) + animation_state->leftPx;
			animation_state->globalRight = (element->parent->currentAnimationState.globalLeft + (parent_width * animation_state->rightPct)) + animation_state->rightPx;

			auto parent_height = element->parent->currentAnimationState.globalBottom - element->parent->currentAnimationState.globalTop;
			animation_state->globalTop = (element->parent->currentAnimationState.globalTop + (parent_height * animation_state->topPct)) + animation_state->topPx;
			animation_state->globalBottom = (element->parent->currentAnimationState.globalTop + (parent_height * animation_state->bottomPct)) + animation_state->bottomPx;
		}
	}

	void element::get_text_dimensions(UIElement* root, const char* text, renderer::font_t* font, float font_height, float* left, float* top, float* right, float* bottom, float wrap_width)
	{
		auto scale = 1.0f;

		auto text_width = renderer::font::get_text_size(text, font, font_height, wrap_width).x;

		*left = 0.0f;
		*top = font_height;

		if (wrap_width >= 0.0f)
		{
			*right = wrap_width;
		}
		else
		{
			*right = text_width;
		}

		*bottom = 0.0f;
	}

	void element::set_dimensions(UIElement* element, UIElement* root)
	{
		if (element == root)
		{
			element->left = root->currentAnimationState.globalLeft = 0.0f;
			element->top = root->currentAnimationState.globalTop = 0.0f;
			element->right = root->currentAnimationState.globalRight = uieditor::canvas::size_.x;
			element->bottom = root->currentAnimationState.globalBottom = uieditor::canvas::size_.y;
		}
		else
		{
			element->left = root->currentAnimationState.leftPx + element->currentAnimationState.globalLeft;
			element->top = root->currentAnimationState.topPx + element->currentAnimationState.globalTop;
			element->right = root->currentAnimationState.leftPx + element->currentAnimationState.globalRight;
			element->bottom = root->currentAnimationState.topPx + element->currentAnimationState.globalBottom;

			if (element->type == UIElementType::UI_TEXT)
			{
				auto text = element->text;

				auto wrap_width = -1.0f;
				if (element->currentAnimationState.alignment)
				{
					wrap_width = element->currentAnimationState.globalRight - element->currentAnimationState.globalLeft;
				}

				auto unitsToPixels = 1.0f;

				get_text_dimensions(root,
					text.data(), 
					element->currentAnimationState.font,
					element->currentAnimationState.globalBottom - element->currentAnimationState.globalTop, 
					&element->textDimLeft,
					&element->textDimTop,
					&element->textDimRight,
					&element->textDimBottom,
					wrap_width);

				if ((element->currentAnimationState.flags & AS_LEFT_PT) != 0)
				{
					if (element->currentAnimationState.leftPct != element->currentAnimationState.rightPct || element->currentAnimationState.leftPct == 0.5f)
					{
						element->textLeft = ((element->left + element->right) / 2.0f) - (((element->textDimRight - element->textDimLeft) * unitsToPixels) / 2.0f);
					}
					else if (element->currentAnimationState.leftPct == 0.0f)
					{
						element->textLeft = element->left - (element->textDimLeft * unitsToPixels);
					}
					else
					{
						element->textLeft = element->right - (element->textDimRight * unitsToPixels);
					}
				}

				if ((element->currentAnimationState.flags & AS_TOP_PT) != 0)
				{
					if (element->currentAnimationState.topPct == element->currentAnimationState.bottomPct)
					{
						element->textTop = ((element->top + element->bottom) / 2.0f) - (((element->textDimBottom - element->textDimTop) * unitsToPixels) / 2.0f);
					}
					else if (element->currentAnimationState.topPct == 0.0f)
					{
						element->textTop = element->top + (element->textDimTop * unitsToPixels);
					}
					else
					{
						element->textTop = element->bottom;
					}
				}
			}
		}
	}

	void element::default_layout(UIElement* element, UIElement* root, int delta_game_time, int delta_system_time)
	{
		auto was_cached = (element->currentAnimationState.flags & AS_LAYOUT_CACHED) != 0;
		auto cache_layout = element::update_layout(element, root, delta_game_time, delta_system_time);

		if (element->currentAnimationState.alpha > 0.000099999997f)
		{
			element::layout(element->firstChild, root, delta_game_time, delta_system_time);
		}

		if (!was_cached || (element->currentAnimationState.flags & AS_LAYOUT_CACHED) != 0)
		{
			auto flags = 0;

			if (cache_layout)
			{
				flags = element->currentAnimationState.flags | AS_LAYOUT_CACHED;
			}
			else
			{
				flags = element->currentAnimationState.flags & -3;
			}

			element->currentAnimationState.flags = flags;
		}
	}

	void element::layout(UIElement* element, UIElement* root, int delta_game_time, int delta_system_time)
	{
		if (element)
		{
			element->currentAnimationState.flags |= AS_IN_USE;

			if (element->layoutFunction)
			{
				element->layoutFunction(element, root, delta_game_time, delta_system_time);
			}
			else
			{
				element::default_layout(element, root, delta_game_time, delta_system_time);
			}

			element::layout(element->nextSibling, root, delta_game_time, delta_system_time);
		}
	}

	bool element::update_layout(UIElement* element, UIElement* root, int delta_game_time, int delta_system_time)
	{
		if (element->parent && (element->parent->currentAnimationState.flags & AS_LAYOUT_CACHED) == 0)
		{
			element->currentAnimationState.flags &= -3;
		}

		if (element->prevAnimationState && element->nextAnimationState)
		{
			/*calculate_global_rectangle(element, element->prevAnimationState);
			calculate_global_rectangle(element, element->nextAnimationState);
			animation::calculate_current_animation_state(element, delta_game_time, delta_system_time);
			set_dimensions(element, root);*/

			return false;
		}
		else
		{
			if ((element->currentAnimationState.flags & AS_LAYOUT_CACHED) == 0)
			{
				calculate_global_rectangle(element, &element->currentAnimationState);
				set_dimensions(element, root);
			}

			return true;
		}
	}

	void element::ui_image_render(UIElement* element, UIElement* root, float red, float green, float blue, float alpha)
	{
		if (element->currentAnimationState.image != nullptr)
		{
			auto left = element->left;
			auto right = element->right;
			auto top = element->top;
			auto bottom = element->bottom;

			auto width_ = right - left;
			auto height_ = bottom - top;

			float color[4] = { red, green, blue, alpha };

			uieditor::canvas::draw_image(element->currentAnimationState.image->texture, left, top, width_, height_, element->currentAnimationState.rotation, ImVec4(red, green, blue, alpha));
		}
	}

	void element::render(UIElement* element, UIElement* root, float red, float green, float blue, float alpha)
	{
		while (1)
		{
			auto should_render = false;

			if (element)
			{
				should_render = (element->currentAnimationState.flags & AS_IN_USE) != 0;
			}

			if (!should_render)
			{
				break;
			}

			auto current_red = red * element->currentAnimationState.red;
			auto current_green = green * element->currentAnimationState.green;
			auto current_blue = blue * element->currentAnimationState.blue;
			auto current_alpha = alpha * element->currentAnimationState.alpha;

			if (current_alpha > 0.000099999997f)
			{
				if ((element->currentAnimationState.flags & AS_STENCIL) != 0)
				{
					uieditor::canvas::push_stencil(element->left, element->top, element->right, element->bottom);
				}

				// cod also checks if the quad is cached here but i honestly think this is fine atm
				if (!element->renderFunction)
				{
					element::render(element->firstChild, root, current_red, current_green, current_blue, current_alpha);
				}
				else
				{
					element->renderFunction(element, root, current_red, current_green, current_blue, current_alpha);

					// fug it we ball (cod doesn't do this)
					element::render(element->firstChild, root, current_red, current_green, current_blue, current_alpha);
				}

				if ((element->currentAnimationState.flags & AS_STENCIL) != 0)
				{
					uieditor::canvas::pop_stencil();
				}
			}

			element = element->nextSibling;
		}
	}

	void element::ui_text_render(UIElement* element, UIElement* root, float red, float green, float blue, float alpha)
	{
		auto wrap_width = -1.0f;
		if (element->currentAnimationState.alignment)
		{
			wrap_width = element->right - element->left;
		}

		auto font_height = element->bottom - element->top;

		uieditor::canvas::draw_text(element->textLeft, element->textTop, red, green, blue, alpha, element->text.data(), element->currentAnimationState.font, font_height, wrap_width, element->currentAnimationState.alignment);
	}

	void element::context_menu(UIElement* element, bool from_canvas)
	{
		if (ImGui::BeginPopupContextItem(element->id.data()))
		{
			if (uieditor::properties::element_ != element)
			{
				uieditor::tree::select_element(element);
			}

			if (ImGui::BeginMenu("Add"))
			{
				if (ImGui::MenuItem("Element"))
				{
					auto new_element = add_and_select_element(from_canvas);

					new_element->type = UI_ELEMENT;
					new_element->layoutFunction = NULL;
					new_element->renderFunction = NULL;
				}

				if (ImGui::MenuItem("Image"))
				{
					auto new_element = add_and_select_element(from_canvas);

					new_element->type = UI_IMAGE;
					new_element->renderFunction = lui::element::ui_image_render;
				}

				if (ImGui::MenuItem("Text"))
				{
					auto new_element = add_and_select_element(from_canvas);

					new_element->type = UI_TEXT;
					new_element->renderFunction = lui::element::ui_text_render;
				}

				ImGui::EndMenu();
			}

			if (ImGui::Selectable("Remove"))
			{
				auto parent = element->parent;

				lui::element::remove_element(element);

				if (parent)
				{
					uieditor::properties::element_ = parent;
				}
				else
				{
					uieditor::properties::element_ = lui::core::element_pool_.at(0);
				}
			}

			if (ImGui::Selectable("Fit to parent"))
			{
				auto* parent = element->parent;
				if (parent)
				{
					element->currentAnimationState.leftAnchor = true;
					element->currentAnimationState.topAnchor = true;
					element->currentAnimationState.rightAnchor = true;
					element->currentAnimationState.bottomAnchor = true;

					element->currentAnimationState.leftPx = 0.0f;
					element->currentAnimationState.topPx = 0.0f;
					element->currentAnimationState.rightPx = 0.0f;
					element->currentAnimationState.bottomPx = 0.0f;

					lui::element::invalidate_layout(element);
				}
			}

			if (ImGui::BeginMenu("Edit Name"))
			{
				ImGui::InputText("##id", uieditor::properties::element_name_, IM_ARRAYSIZE(uieditor::properties::element_name_));

				if (element->name.data() != uieditor::properties::element_name_)
				{
					element->name = uieditor::properties::element_name_;
				}

				ImGui::EndMenu();
			}

			ImGui::EndPopup();
		}
	}
}