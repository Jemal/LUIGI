#include <stdafx.hpp>
#include "editor/editor_layer.hpp"

#include "imgui/imgui_font.hpp"

/*
	clean up this file
*/

namespace uie
{
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

	void display_element_tree(ui_element* element)
	{
		//lui::element::context_menu(element, false);

		if (ImGui::IsItemClicked() && !ImGui::IsItemToggledOpen() && ImGui::IsItemHovered())
		{
			properties::selected_element_ = element;
		}

		ImGui::TableNextColumn();

		PushStyleCompact();

		char buf[64];
		sprintf(buf, "##%iVisible", element->id_); // ### operator override ID ignoring the preceding label

		ImGui::Checkbox(buf, &element->visible_);

		ImGui::TableNextColumn();

		char bufl[64];
		sprintf(bufl, "##%iLocked", element->id_); // ### operator override ID ignoring the preceding label

		if (ImGui::Checkbox(bufl, &element->locked_))
		{
			//lui::element::update_locked_state(element, element->locked);
		}

		PopStyleCompact();
	}

	void display_element(ui_element* element)
	{
		/*if (element->is_from_widget)
		{
			return;
		}*/

		if (!element)
		{
			return;
		}

		ImGui::TableNextRow();
		ImGui::TableNextColumn();

		if (element->first_child_ != nullptr && element->type_ != ui_element_type::widget)
		{
			auto node_flags = ImGuiTreeNodeFlags_None | ImGuiTreeNodeFlags_SpanFullWidth;
			if (element == properties::selected_element_)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			}

			if (element == properties::selected_element_->parent_)
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			}

			if (element->is_descendent_of(properties::selected_element_))
			{
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			}

			char buf[64];
			sprintf(buf, "%s###Element", element->name_.data()); // ### operator override ID ignoring the preceding label

			auto open = ImGui::TreeNodeEx(buf, node_flags);

			display_element_tree(element);

			if (open)
			{
				auto child = element->first_child_;
				while (child)
				{
					if (child == properties::selected_element_)
					{
						ImGui::SetNextItemOpen(true, ImGuiCond_Once);
					}

					if (!element)
					{
						break;
					}

					display_element(child);

					child = child->next_sibling_;
				}

				ImGui::TreePop();
			}
		}
		else
		{
			auto node_flags = ImGuiTreeNodeFlags_Leaf | ImGuiTreeNodeFlags_Bullet | ImGuiTreeNodeFlags_NoTreePushOnOpen | ImGuiTreeNodeFlags_SpanFullWidth;
			if (element == properties::selected_element_)
			{
				node_flags |= ImGuiTreeNodeFlags_Selected;
				ImGui::SetNextItemOpen(true, ImGuiCond_Once);
			}

			char buf[64];
			sprintf(buf, "%s###Element", element->name_.data()); // ### operator override ID ignoring the preceding label

			ImGui::TreeNodeEx(buf, node_flags);

			display_element_tree(element);
		}
	}

	hierarchy::hierarchy()
	{

	}

	void hierarchy::set_context()
	{
		// each time we change the context we want to switch to the needed element pool context
	}

	void hierarchy::render()
	{
		ImGui::Begin("Hierarchy");

		if (ImGui::Button("Create Element"))
		{
			auto child = lui::create_element();
			if (properties::selected_element_)
			{
				properties::selected_element_->add_element(child);
			}
		}

		ImGui::SameLine();

		if (ImGui::Button("Create Image"))
		{
			auto child = lui::create_element();
			child->type_ = ui_element_type::image;
			child->setup();

			if (properties::selected_element_)
			{
				properties::selected_element_->add_element(child);
			}

			properties::selected_element_ = child;
		}

		if (ImGui::BeginTable("3ways", 3, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings))
		{
			char buf[64];
			sprintf(buf, "Elements (%i/%i)", lui::element_pool_.size(), LUI_MAX_ELEMENTS);

			ImGui::TableSetupColumn(buf, ImGuiTableColumnFlags_NoHide);

			ImGui::TableSetupColumn(" " ICON_FK_EYE, ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableSetupColumn("  " ICON_FK_LOCK, ImGuiTableColumnFlags_WidthFixed, 20);
			ImGui::TableHeadersRow();

			display_element(lui::element_pool_.front());

			ImGui::EndTable();
		}

		ImGui::End();
	}
}