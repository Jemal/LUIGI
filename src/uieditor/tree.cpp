#include <stdafx.hpp>
#include "tree.hpp"
#include "properties.hpp"

namespace uieditor
{
	void tree::select_element(UIElement* element)
	{
		properties::element_ = element;
	}

	void tree::display_element_tree(UIElement* element)
	{
		lui::element::context_menu(element, false);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && ImGui::IsItemHovered())
		{
			select_element(element);
		}

		ImGui::TableNextColumn();

		ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), lui::element::type_to_string(element->type).data());
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