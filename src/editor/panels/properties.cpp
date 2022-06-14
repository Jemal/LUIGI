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

	std::vector<std::string, int> anchor_pair =
	{
		{ "None", ui_element::animation_state::anchor_type::none },
		{ "All", ui_element::animation_state::anchor_type::all },
		{ "Left", ui_element::animation_state::anchor_type::left },
		{ "Top", ui_element::animation_state::anchor_type::top },
		{ "Right", ui_element::animation_state::anchor_type::right },
		{ "Bottom", ui_element::animation_state::anchor_type::bottom },
		{ "Left-Right", ui_element::animation_state::anchor_type::left_right },
		{ "Top-Left", ui_element::animation_state::anchor_type::top_left },
		{ "Top-Right", ui_element::animation_state::anchor_type::top_right },
		{ "Top-Left-Right", ui_element::animation_state::anchor_type::top_left_right },
		{ "Top-Bottom", ui_element::animation_state::anchor_type::top_bottom },
		{ "Top-Bottom-Left", ui_element::animation_state::anchor_type::top_bottom_left },
		{ "Top-Bottom-Right", ui_element::animation_state::anchor_type::top_bottom_right },
		{ "Bottom-Left", ui_element::animation_state::anchor_type::bottom_left },
		{ "Bottom-Right", ui_element::animation_state::anchor_type::bottom_right },
		{ "Bottom-Left-Right", ui_element::animation_state::anchor_type::bottom_left_right }
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
			for (auto& kv : anchor_pair)
			{
				if (ImGui::Selectable(kv.))
				{
					selected_element_->states_.current_.position.x.anchors[0] = (kv.second & ui_element::anchor_type::left) != 0;
					selected_element_->states_.current_.position.y.anchors[0] = (kv.second & ui_element::anchor_type::top) != 0;
					selected_element_->states_.current_.position.x.anchors[1] = (kv.second & ui_element::anchor_type::right) != 0;
					selected_element_->states_.current_.position.y.anchors[1] = (kv.second & ui_element::anchor_type::bottom) != 0;

					selected_element_->invalidate_layout();
				}
			}

			ImGui::EndCombo();
		}

		if (input_property("Left:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.position.x.offsets[0], 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (input_property("Right:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.position.x.offsets[1], 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (input_property("Top:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.position.y.offsets[0], 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		if (input_property("Bottom:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.position.y.offsets[1], 1.0f, input_fast_step_))
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

		if (selected_element_->type_ == ui_element::type::image)
		{
			draw_image_properties();
		}
		else if (selected_element_->type_ == ui_element::type::text)
		{
			draw_text_properties();
		}

		if (input_property("Rotation:", ImGuiDataType_::ImGuiDataType_Float, &selected_element_->states_.current_.rotation, 1.0f, input_fast_step_))
		{
			selected_element_->invalidate_layout();
		}

		auto uses_stencil = (selected_element_->states_.current_.flags & stencil) != 0;

		if (bool_property("Stencil", &uses_stencil))
		{
			if (uses_stencil)
			{
				selected_element_->states_.current_.flags |= stencil;
			}
			else
			{
				selected_element_->states_.current_.flags &= ~stencil;
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