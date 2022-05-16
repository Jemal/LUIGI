#include <stdafx.hpp>
#include "element.hpp"
#include "uieditor/app.hpp"
#include "uieditor/canvas.hpp"
#include "uieditor/project.hpp"
#include "uieditor/properties.hpp"
#include "uieditor/settings.hpp"
#include "uieditor/tree.hpp"
#include "uieditor/widgets.hpp"

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
		if (element)
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
			
			auto mouse_pos = uieditor::canvas::mouse_pos_ / uieditor::canvas::zoom_pct_;

			auto left = mouse_pos.x - element->left;
			auto top = mouse_pos.y - element->top;

			child->currentAnimationState.leftPx = left;
			child->currentAnimationState.topPx = top;
			child->currentAnimationState.rightPx = left + 75.0f;
			child->currentAnimationState.bottomPx = top + 75.0f;

			lui::element::invalidate_layout(child);
		}

		uieditor::tree::select_element(child);

		return child;
	}

	int element::string_to_type(std::string type)
	{
		if (type == "UIImage"s)
		{
			return UI_IMAGE;
		}
		else if (type == "UIText"s)
		{
			return UI_TEXT;
		}

		return UI_ELEMENT;
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
		remove_children(element);

		if (element->parent)
		{
			remove_from_parent(element);
		}

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

		uieditor::project::set_project_modified();
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

			if (element->type == UIElementType::UI_TEXT && element->currentAnimationState.font != NULL)
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
		if (element->currentAnimationState.font == NULL || element->currentAnimationState.font->handle == NULL)
		{
			return;
		}

		auto wrap_width = -1.0f;
		if (element->currentAnimationState.alignment)
		{
			wrap_width = element->right - element->left;
		}

		auto font_height = element->bottom - element->top;

		uieditor::canvas::draw_text(element, element->textLeft, element->textTop, red, green, blue, alpha, element->text.data(), element->currentAnimationState.font, font_height, wrap_width, element->currentAnimationState.alignment);
	}

	void element::context_menu(UIElement* element, bool from_canvas)
	{
		if (ImGui::BeginPopupContextItem(element->id.data()))
		{
			if (uieditor::properties::element_ != element)
			{
				uieditor::tree::select_element(element);
			}

			auto is_root_element = element == lui::core::get_root_element();

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
					new_element->text = "New"s;
				}

				if (!uieditor::widgets::widgets_.empty())
				{
					ImGui::Separator();

					if (ImGui::BeginMenu("Widgets"))
					{
						for (auto& widget : uieditor::widgets::widgets_)
						{
							if (ImGui::MenuItem(widget.data()))
							{
								uieditor::widgets::load_widget(widget.data());
							}
						}

						ImGui::EndMenu();
					}
				}

				ImGui::EndMenu();
			}

			if (!is_root_element)
			{
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

				ImGui::Separator();

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
			}

			if (ImGui::BeginMenu("Edit Name"))
			{
				ImGui::InputText("##id", &element->name);

				ImGui::EndMenu();
			}

			if (element->firstChild)
			{
				ImGui::Separator();

				if (ImGui::MenuItem("Save as Widget"))
				{
					uieditor::app::file_dialog_mode_ = uieditor::FILE_DIALOG_SAVE_AS_WIDGET;
				}
			}

			ImGui::EndPopup();
		}
	}

	void element::serialize(UIElement* element, nlohmann::ordered_json* json, bool is_widget)
	{
		// ignore elements that were saved already when we're saving as a project
		if (!is_widget)
		{
			if (std::find(uieditor::project::saved_elements_.begin(), uieditor::project::saved_elements_.end(), element->id) != uieditor::project::saved_elements_.end())
			{
				return;
			}
		
			uieditor::project::saved_elements_.push_back(element->id);
		}

		(*json)["Type"] = lui::element::type_to_string(element->type);
		(*json)["Priority"] = element->priority;

		if (is_widget)
		{
			if (element != uieditor::widgets::widget_element_)
			{
				(*json)["Anchors"][0] = element->currentAnimationState.leftAnchor;
				(*json)["Anchors"][1] = element->currentAnimationState.topAnchor;
				(*json)["Anchors"][2] = element->currentAnimationState.rightAnchor;
				(*json)["Anchors"][3] = element->currentAnimationState.bottomAnchor;

				(*json)["Position"][0] = element->currentAnimationState.leftPx;
				(*json)["Position"][1] = element->currentAnimationState.topPx;
				(*json)["Position"][2] = element->currentAnimationState.rightPx;
				(*json)["Position"][3] = element->currentAnimationState.bottomPx;
			}
		}
		else
		{
			(*json)["Anchors"][0] = element->currentAnimationState.leftAnchor;
			(*json)["Anchors"][1] = element->currentAnimationState.topAnchor;
			(*json)["Anchors"][2] = element->currentAnimationState.rightAnchor;
			(*json)["Anchors"][3] = element->currentAnimationState.bottomAnchor;

			(*json)["Position"][0] = element->currentAnimationState.leftPx;
			(*json)["Position"][1] = element->currentAnimationState.topPx;
			(*json)["Position"][2] = element->currentAnimationState.rightPx;
			(*json)["Position"][3] = element->currentAnimationState.bottomPx;
		}

		(*json)["Color"]["r"] = element->currentAnimationState.red;
		(*json)["Color"]["g"] = element->currentAnimationState.green;
		(*json)["Color"]["b"] = element->currentAnimationState.blue;

		(*json)["Alpha"] = element->currentAnimationState.alpha;

		(*json)["Rotation"] = element->currentAnimationState.rotation;
		(*json)["Stencil"] = (element->currentAnimationState.flags & AS_STENCIL) != 0;

		if (element->type == UI_IMAGE)
		{
			if (element->currentAnimationState.image != NULL)
			{
				(*json)["Image"]["Name"] = element->currentAnimationState.image->name;
				(*json)["Image"]["Path"] = element->currentAnimationState.image->path;
			}
			else
			{
				(*json)["Image"] = nullptr;
				(*json)["Path"] = nullptr;
			}
		}
		else if (element->type == UI_TEXT && element->currentAnimationState.font != NULL)
		{
			(*json)["Alignment"] = element->currentAnimationState.alignment;
			(*json)["Font"] = element->currentAnimationState.font->name;
			(*json)["Text"] = element->text.data();
		}

		// children
		for (auto i = 0; i < element->child_count; i++)
		{
			auto child = element->firstChild;
			while (child)
			{
				auto children_data = &(*json)["Children"][child->name];

				element::serialize(child, children_data, is_widget);

				child = child->nextSibling;
			}
		}
	}

	void element::deserialize(UIElement* element, std::string name, nlohmann::ordered_json* json, bool is_widget)
	{
		lui::element::create_element();

		auto new_element = lui::core::element_pool_.back();

		if (element == nullptr)
		{
			element = new_element;
		}
		else
		{
			lui::element::add_element(element, new_element);
		}

		new_element->name = name;

		new_element->type = lui::element::string_to_type((*json)["Type"]);
		new_element->priority = (*json)["Priority"];

		new_element->currentAnimationState.leftAnchor = (*json)["Anchors"][0];
		new_element->currentAnimationState.topAnchor = (*json)["Anchors"][1];
		new_element->currentAnimationState.rightAnchor = (*json)["Anchors"][2];
		new_element->currentAnimationState.bottomAnchor = (*json)["Anchors"][3];

		new_element->currentAnimationState.leftPx = (*json)["Position"][0];
		new_element->currentAnimationState.topPx = (*json)["Position"][1];
		new_element->currentAnimationState.rightPx = (*json)["Position"][2];
		new_element->currentAnimationState.bottomPx = (*json)["Position"][3];

		new_element->currentAnimationState.red = (*json)["Color"]["r"];
		new_element->currentAnimationState.green = (*json)["Color"]["g"];
		new_element->currentAnimationState.blue = (*json)["Color"]["b"];

		new_element->currentAnimationState.alpha = (*json)["Alpha"];
		new_element->currentAnimationState.rotation = (*json)["Rotation"];

		auto uses_stencil = (*json)["Stencil"];
		if (uses_stencil)
		{
			new_element->currentAnimationState.flags |= AS_STENCIL;
		}

		if (new_element->type == UI_IMAGE)
		{
			auto image_name = (*json)["Image"]["Name"];
			auto image_path = (*json)["Image"]["Path"];

			if (!image_name.is_null())
			{
				auto image = renderer::image::register_handle(image_name, image_path);
				if (image)
				{
					new_element->currentAnimationState.image = image;
				}
			}

			new_element->renderFunction = lui::element::ui_image_render;
		}
		else if (new_element->type == UI_TEXT)
		{
			new_element->currentAnimationState.alignment = (*json)["Alignment"];

			auto font_name = (*json)["Font"];
			new_element->currentAnimationState.font = renderer::font::get_font_handle(font_name);

			new_element->text = std::string((*json)["Text"]);

			new_element->renderFunction = lui::element::ui_text_render;
		}

		lui::element::invalidate_layout(element);
		//lui::element::invalidate_layout(new_element);

		auto children_data = &(*json)["Children"];
		if (!children_data->is_null())
		{
			for (auto& item : children_data->items())
			{
				auto element_name = item.key();
				auto val = item.value();

				deserialize(new_element, element_name, &val, is_widget);
			}
		}
	}
}