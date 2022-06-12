#include <stdafx.hpp>
#include "properties.hpp"

#include "imgui.h"

#include "lui/lui.hpp"

#include "utils/string.hpp"

namespace uie
{
	ui_element* properties::selected_element_;

	float properties::input_fast_step_ = 10.0f;

	bool properties::show_color_sets_ = true;

	UIAnchorPair anchor_pair[ANCHOR_COUNT] =
	{
		{ "None", ANCHOR_NONE },
		{ "All", ANCHOR_ALL },
		{ "Left", ANCHOR_LEFT },
		{ "Top", ANCHOR_TOP },
		{ "Right", ANCHOR_RIGHT },
		{ "Bottom", ANCHOR_BOTTOM },
		{ "Left-Right", ANCHOR_LEFT_RIGHT },
		{ "Top-Left", ANCHOR_TOP_LEFT },
		{ "Top-Right", ANCHOR_TOP_RIGHT },
		{ "Top-Left-Right", ANCHOR_TOP_LEFT_RIGHT },
		{ "Top-Bottom", ANCHOR_TOP_BOTTOM },
		{ "Top-Bottom-Left", ANCHOR_TOP_BOTTOM_LEFT },
		{ "Top-Bottom-Right", ANCHOR_TOP_BOTTOM_RIGHT },
		{ "Bottom-Left", ANCHOR_BOTTOM_LEFT },
		{ "Bottom-Right", ANCHOR_BOTTOM_RIGHT },
		{ "Bottom-Left-Right", ANCHOR_BOTTOM_LEFT_RIGHT },
	};

	properties::properties()
	{
	}

	void properties::set_context()
	{

	}

	void properties::begin_property(const char* label)
	{
		ImGui::TableNextColumn();
		const ImVec2 cursorPos = ImGui::GetCursorPos();
		ImGui::SetCursorPos({ cursorPos.x + 4, cursorPos.y + 4 });
		ImGui::TextUnformatted(label);

		ImGui::TableNextColumn();
		ImGui::SetNextItemWidth(250);
	}

	bool properties::bool_property(const char* label, bool* value)
	{
		begin_property(label);

		return ImGui::Checkbox(utils::string::va("##%s", label), value);
	}

	bool properties::input_property(const char* label, ImGuiDataType_ type, void* data, float step, float fast_step)
	{
		begin_property(label);

		return ImGui::InputScalar(utils::string::va("##%s", label), type, data, &step, &fast_step, "%.2f");
	}

	bool properties::slider_property(const char* label, ImGuiDataType_ type, void* data, float min, float max)
	{
		begin_property(label);

		return ImGui::SliderScalar(utils::string::va("##%s", label), type, data, &min, &max, "%.2f");
	}

	bool properties::button_property(const char* label, const char* preview_value)
	{
		begin_property(label);

		return ImGui::Button(preview_value);
	}

	bool properties::combo_property(const char* label, const char* preview_value)
	{
		begin_property(label);

		return ImGui::BeginCombo(utils::string::va("##%s", label), preview_value);
	}

	bool properties::color_property(const char* label, float* color)
	{
		begin_property(label);

		return ImGui::ColorEdit3(utils::string::va("##%s", label), color, ImGuiColorEditFlags_NoInputs);
	}

	bool properties::text_property(const char* label, char* buf, size_t buf_size)
	{
		begin_property(label);

		return ImGui::InputText(utils::string::va("##%s", label), buf, buf_size);
	}

	bool properties::text_property(const char* label, std::string* string)
	{
		begin_property(label);

		return ImGui::InputText(utils::string::va("##%s", label), string);
	}

	void properties::draw_image_properties()
	{
		/*if (button_property("Image", element_->states_.current_.image == nullptr?"Select...":element_->states_.current_.image->name.data()))
		{
			app::file_dialog_mode_ = FILE_DIALOG_IMAGE;
		}*/
	}

	void properties::draw_text_properties()
	{

	}

	void properties::draw_canvas_properties()
	{
		/*if (input_property("Width:", ImGuiDataType_::ImGuiDataType_Float, &canvas::size_.x, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		if (input_property("Height:", ImGuiDataType_::ImGuiDataType_Float, &canvas::size_.y, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		auto uses_stencil = (element_->states_.current_.flags & AS_STENCIL) != 0;
		if (bool_property("Stencil", &uses_stencil))
		{
			if (uses_stencil)
			{
				element_->states_.current_.flags |= AS_STENCIL;
			}
			else
			{
				element_->states_.current_.flags &= ~AS_STENCIL;
			}

			lui::element::invalidate_layout(element_);
		}*/
	}

	void properties::draw_element_properties()
	{
		if (combo_property("Anchors:", selected_element_->anchors_to_string(selected_element_->anchors_to_int())))
		{
			for (auto i = 0; i < UIAnchorType::ANCHOR_COUNT; i++)
			{
				auto anchor = anchor_pair[i];

				if (ImGui::Selectable(anchor.name))
				{
					selected_element_->states_.current_.leftAnchor = (anchor.value & UIAnchorType::ANCHOR_LEFT) != 0;
					selected_element_->states_.current_.topAnchor = (anchor.value & UIAnchorType::ANCHOR_TOP) != 0;
					selected_element_->states_.current_.rightAnchor = (anchor.value & UIAnchorType::ANCHOR_RIGHT) != 0;
					selected_element_->states_.current_.bottomAnchor = (anchor.value & UIAnchorType::ANCHOR_BOTTOM) != 0;

					selected_element_->invalidate_layout();
				}
			}

			ImGui::EndCombo();
		}

		if (input_property("Left:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.leftPx, 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (input_property("Right:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.rightPx, 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (input_property("Top:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.topPx, 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (input_property("Bottom:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.bottomPx, 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (color_property("Color:", &selected_element_->states_.current_.red))
		{
			selected_element_->invalidate_layout();
		}

		ImGui::SameLine();

		if (ImGui::Button("Saved##ColorSets"))
		{
			show_color_sets_ = true;
		}

		if (slider_property("Alpha:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.alpha, 0.0f, 1.0f))
		{
			selected_element_->invalidate_layout();
		}

		if (selected_element_->type_ == ui_element_type::image)
		{
			draw_image_properties();
		}
		else if (selected_element_->type_ == ui_element_type::text)
		{
			draw_text_properties();
		}

		if (input_property("Rotation:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.rotation, 1.0f, input_fast_step_))
		{
			//lui::element::invalidate_layout(element_);
		}

		auto uses_stencil = (selected_element_->states_.current_.flags & AS_STENCIL) != 0;

		if (bool_property("Stencil", &uses_stencil))
		{
			if (uses_stencil)
			{
				selected_element_->states_.current_.flags |= AS_STENCIL;
			}
			else
			{
				selected_element_->states_.current_.flags &= ~AS_STENCIL;
			}

			selected_element_->invalidate_layout();
		}
	}

	void properties::render()
	{
		ImGui::Begin("Properties");

		if (ImGui::CollapsingHeader("Base Properties"))
		{
			if (ImGui::BeginTable("label_property", 2, ImGuiTableFlags_NoSavedSettings))
			{
				ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, 100);

				if (selected_element_)
				{
					draw_element_properties();
				}

				ImGui::EndTable();
			}
		}

		ImGui::End();
	}
}