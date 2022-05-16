#include <stdafx.hpp>
#include "tree.hpp"
#include "properties.hpp"

static void PushStyleCompact()
{
	ImGuiStyle& style = ImGui::GetStyle();
	ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(style.FramePadding.x, (float)(int)(style.FramePadding.y * 0.60f)));
	ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(style.ItemSpacing.x, (float)(int)(style.ItemSpacing.y * 0.60f)));
}

static void PopStyleCompact()
{
	ImGui::PopStyleVar(2);
}

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

		PushStyleCompact();

		char buf[64];
		sprintf(buf, "##%sVisible", element->id.data()); // ### operator override ID ignoring the preceding label

		ImGui::Checkbox(buf, &element->visible);

		ImGui::TableNextColumn();

		char bufl[64];
		sprintf(bufl, "##%sLocked", element->id.data()); // ### operator override ID ignoring the preceding label

		if (ImGui::Checkbox(bufl, &element->locked))
		{
			lui::element::update_locked_state(element, element->locked);
		}

		PopStyleCompact();
	}

	void tree::display_element(UIElement* element)
	{
		if (element->is_from_widget)
		{
			return;
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (element->firstChild != nullptr && element->type != UIElementType::UI_WIDGET)
		{
			auto node_flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_SpanFullWidth;
			if (element == properties::element_)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			}

			if (element == properties::element_->parent)
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			}

			if (lui::element::is_descendent_of(element, properties::element_))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
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
					if (child == properties::element_)
					{
						ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					}

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
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
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
			if (ImGui::BeginTable("3ways", 3, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings))
			{
				char buf[64];
				sprintf(buf, "Elements (%i/%i)", lui::core::allocated_elements_, LUI_MAX_ELEMENTS);

				ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_NoHide);

				ImGui::TableSetupColumn(" " ICON_FK_EYE, ImGuiTableColumnFlags_WidthFixed, 20);
				ImGui::TableSetupColumn("  " ICON_FK_LOCK, ImGuiTableColumnFlags_WidthFixed, 20);
				ImGui::TableHeadersRow();

				display_element(lui::core::get_root_element());

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}
}