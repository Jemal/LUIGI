#include <stdafx.hpp>
#include "app.hpp"
#include "canvas.hpp"
#include "tree.hpp"
#include "log.hpp"
#include "project.hpp"
#include "properties.hpp"
#include "settings.hpp"

namespace uieditor
{
	UIElement* properties::element_ = nullptr;

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
		if (button_property("Image", element_->currentAnimationState.image == nullptr ? "Select..." : element_->currentAnimationState.image->name.data()))
		{
			app::file_dialog_mode_ = FILE_DIALOG_IMAGE;
		}
	}

	void properties::draw_text_properties()
	{
		static int selected_font = 0;

		if (combo_property("Font", element_->currentAnimationState.font == nullptr ? "Select..." : element_->currentAnimationState.font->name.data()))
		{
			for (auto i = 0; i < renderer::font::fonts_.size(); i++)
			{
				auto* font = renderer::font::fonts_.at(i).get();

				ImGui::PushID(i);

				auto is_selected = (selected_font == i);

				if (ImGui::Selectable(font->name.data()))
				{
					selected_font = i;

					element_->currentAnimationState.font = font;

					lui::element::invalidate_layout(element_);
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}

		if (text_property("Text:", &element_->text))
		{
			lui::element::invalidate_layout(element_);
		}
	}

	void properties::draw_canvas_properties()
	{
		if (input_property("Width:", ImGuiDataType_::ImGuiDataType_Float, &canvas::size_.x, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		if (input_property("Height:", ImGuiDataType_::ImGuiDataType_Float, &canvas::size_.y, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		auto uses_stencil = (element_->currentAnimationState.flags & AS_STENCIL) != 0;
		if (bool_property("Stencil", &uses_stencil))
		{
			if (uses_stencil)
			{
				element_->currentAnimationState.flags |= AS_STENCIL;
			}
			else
			{
				element_->currentAnimationState.flags &= ~AS_STENCIL;
			}

			lui::element::invalidate_layout(element_);
		}
	}

	void properties::draw_element_properties()
	{
		if (combo_property("Anchors:", lui::element::anchors_to_string(lui::element::anchors_to_int(element_))))
		{
			for (auto i = 0; i < UIAnchorType::ANCHOR_COUNT; i++)
			{
				auto anchor = anchor_pair[i];

				if (ImGui::Selectable(anchor.name))
				{
					element_->currentAnimationState.leftAnchor = (anchor.value & UIAnchorType::ANCHOR_LEFT) != 0;
					element_->currentAnimationState.topAnchor = (anchor.value & UIAnchorType::ANCHOR_TOP) != 0;
					element_->currentAnimationState.rightAnchor = (anchor.value & UIAnchorType::ANCHOR_RIGHT) != 0;
					element_->currentAnimationState.bottomAnchor = (anchor.value & UIAnchorType::ANCHOR_BOTTOM) != 0;

					lui::element::invalidate_layout(element_);
				}
			}

			ImGui::EndCombo();
		}

		if (input_property("Left:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.leftPx, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		if (input_property("Right:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.rightPx, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		if (input_property("Top:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.topPx, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		if (input_property("Bottom:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.bottomPx, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		if (color_property("Color:", &element_->currentAnimationState.red))
		{
			lui::element::invalidate_layout(element_);
		}

		ImGui::SameLine();

		if (ImGui::Button("Saved##ColorSets"))
		{
			show_color_sets_ = true;
		}

		if (slider_property("Alpha:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.alpha, 0.0f, 1.0f))
		{
			lui::element::invalidate_layout(element_);
		}

		if (element_->type == UIElementType::UI_IMAGE)
		{
			draw_image_properties();
		}
		else if (element_->type == UIElementType::UI_TEXT)
		{
			draw_text_properties();
		}

		if (input_property("Rotation:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.rotation, 1.0f, input_fast_step_))
		{
			lui::element::invalidate_layout(element_);
		}

		auto uses_stencil = (element_->currentAnimationState.flags & AS_STENCIL) != 0;

		if (bool_property("Stencil", &uses_stencil))
		{
			if (uses_stencil)
			{
				element_->currentAnimationState.flags |= AS_STENCIL;
			}
			else
			{
				element_->currentAnimationState.flags &= ~AS_STENCIL;
			}

			lui::element::invalidate_layout(element_);
		}
	}

	void properties::draw()
	{
		if (ImGui::Begin("Properties"))
		{
			if (element_ != nullptr)
			{
				auto root = lui::core::get_root_element();
				auto is_selected_element_root = element_ == root;

				ImGui::Text("Type: %s", lui::element::type_to_string(element_->type).data());
				
				ImGui::SetNextItemOpen(true, ImGuiCond_Appearing);

				if (ImGui::TreeNode(is_selected_element_root ? "Global Properties" : "Base Properties"))
				{
					if (ImGui::BeginTable("label_property", 2, ImGuiTableFlags_NoSavedSettings))
					{
						ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, 100);

						if (is_selected_element_root)
						{
							draw_canvas_properties();
						}
						else
						{
							draw_element_properties();
						}

						ImGui::EndTable();
					}

					ImGui::TreePop();
				}
			}
		}

		if (show_color_sets_)
		{
			ImGui::OpenPopup("Color Sets");
			show_color_sets_ = false;
		}

		if (ImGui::BeginPopup("Color Sets"))
		{
			float sz = ImGui::GetTextLineHeight();
			for (int i = 0; i < settings::colors_.size(); i++)
			{
				if (ImGui::BeginMenu("Colors"))
				{
					auto color = &settings::colors_.at(i);

					const char* name = color->name.data();
					ImVec2 p = ImGui::GetCursorScreenPos();

					ImVec4 value(color->r, color->g, color->b, 1.0f);

					ImGui::GetWindowDrawList()->AddRectFilled(p, ImVec2(p.x + sz, p.y + sz), ImGui::GetColorU32(value));
					ImGui::Dummy(ImVec2(sz, sz));
					ImGui::SameLine();
					ImGui::MenuItem(name);

					ImGui::EndMenu();
				}

				if (ImGui::BeginMenu("Swatches"))
				{
					ImGui::EndMenu();
				}
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}
}