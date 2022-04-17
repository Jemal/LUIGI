#include <stdafx.hpp>
#include "tree.hpp"
#include "properties.hpp"

namespace uieditor
{
	UIElement* tree::payload_element_ = nullptr;

	void tree::select_element(UIElement* element)
	{
		properties::element_ = element;
		strcpy(properties::element_name_, element->name.data());
		strcpy(properties::element_text_, element->text.data());
	}

	void tree::handle_drop_element_to_parent(UIElement* element)
	{
		auto* new_parent = element;

		if (lui::element::is_descendent_of(new_parent, payload_element_))
		{
			return;
		}

		if (payload_element_->parent == new_parent)
		{
			return;
		}

		if (payload_element_->parent)
		{
			lui::element::remove_from_parent(payload_element_);
		}

		lui::element::add_element(new_parent, payload_element_);
	}

	void tree::display_element_tree(UIElement* element)
	{
		lui::element::context_menu(element, false);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && ImGui::IsItemHovered())
		{
			select_element(element);
		}

		if (ImGui::BeginDragDropSource())
		{
			payload_element_ = element;

			ImGui::SetDragDropPayload("ELEMENT", NULL, 0);
			ImGui::Text(payload_element_->name.data());
			ImGui::EndDragDropSource();
		}

		if (ImGui::BeginDragDropTarget())
		{
			if (const ImGuiPayload* payload = ImGui::AcceptDragDropPayload("ELEMENT"))
			{
				handle_drop_element_to_parent(element);
			}
			ImGui::EndDragDropTarget();
		}

		ImGui::TableNextColumn();

		char buf[64];
		sprintf(buf, "##%sType", element->name.data());

		ImGui::PushItemWidth(125);

		if (ImGui::BeginCombo(buf, lui::element::type_to_string(element->type).data()))
		{
			for (auto i = 0; i < UIElementType::UI_TYPE_COUNT; i++)
			{
				if (ImGui::Selectable(lui::element::type_to_string(i).data()))
				{
					element->type = i;

					if (i == UIElementType::UI_IMAGE)
					{
						element->currentAnimationState.image = &renderer::image::images_.at(0);

						element->renderFunction = lui::element::ui_image_render;
					}
					else if (i == UIElementType::UI_TEXT)
					{
						element->currentAnimationState.font = &renderer::font::fonts_.at(0);
						element->currentAnimationState.textScale = 1.0f;

						element->renderFunction = lui::element::ui_text_render;
					}
					else
					{
						element->renderFunction = nullptr;
					}
				}
			}

			ImGui::EndCombo();
		}

		ImGui::PopItemWidth();
	}

	void tree::display_element(UIElement* element)
	{
		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		ImGui::Spacing();

		if (element->firstChild != nullptr)
		{
			auto node_flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_SpanFullWidth;
			if (element == properties::element_)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
			}

			if (element == properties::element_->parent)
			{
				ImGui::SetNextItemOpen(true);
			}

			char buf[64];
			sprintf(buf, "%s###Element", element->name.data()); // ### operator override ID ignoring the preceding label

			auto open = ImGui::TreeNodeEx(buf, node_flags);

			display_element_tree(element);

			if (open)
			{
				auto child = element->firstChild;
				while (child)
				{
					if (!element)
					{
						break;
					}

					display_element(child);

					child = child->nextSibling;
				}

				ImGui::TreePop();
			}

		}
		else
		{
			auto node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
			if (element == properties::element_)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				ImGui::SetNextItemOpen(true);
			}

			char buf[64];
			sprintf(buf, "%s###Element", element->name.data()); // ### operator override ID ignoring the preceding label

			ImGui::TreeNodeEx(buf, node_flags);

			display_element_tree(element);
		}
	}

	void tree::draw()
	{
		if (ImGui::Begin("Tree"))
		{
			if (ImGui::BeginTable("3ways", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings))
			{
				char buf[64];
				sprintf(buf, "Elements (%i/%i)", lui::core::allocated_elements_, LUI_MAX_ELEMENTS);

				ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_NoHide);
				ImGui::TableSetupColumn("Type", ImGuiTableColumnFlags_WidthFixed, 125);
				ImGui::TableHeadersRow();

				display_element(lui::core::get_root_element());

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}
}