#include <stdafx.hpp>
#include "canvas.hpp"
#include "tree.hpp"
#include "log.hpp"
#include "properties.hpp"
#include "misc/filedialog/ImGuiFileDialog.h"

namespace uieditor
{
	UIElement* properties::element_ = nullptr;
	char properties::element_name_[32] = {};
	char properties::element_text_[256] = {};

	float properties::input_fast_step_ = 10.0f;

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

		return ImGui::ColorEdit3(utils::string::va("##%s", label), color);
	}

	bool properties::text_property(const char* label, char* buf, size_t buf_size)
	{
		begin_property(label);

		return ImGui::InputText(utils::string::va("##%s", label), buf, buf_size);
	}

	void properties::draw_image_properties()
	{
		static int selected_image = 0;

		/*if (combo_property("Image", element_->currentAnimationState.image == nullptr ? "Select..." : element_->currentAnimationState.image->name.data()))
		{
			for (auto i = 0; i < renderer::image::images_.size(); i++)
			{
				auto* image = &renderer::image::images_.at(i);

				ImGui::PushID(i);

				const bool is_selected = (selected_image == i);

				if (ImGui::Selectable(image->name.data()))
				{
					selected_image = i;

					element_->currentAnimationState.image = image;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}*/

		if (button_property("Image", element_->currentAnimationState.image == nullptr ? "Select..." : element_->currentAnimationState.image->name.data()))
		{
			ImGuiFileDialog::Instance()->OpenModal("SelectedImageDlg", "Select Image", ".png", "uieditor/images/");
		}
	}

	void properties::draw_text_properties()
	{
		static int selected_font = 0;

		if (combo_property("Font", element_->currentAnimationState.font == nullptr ? "Select..." : element_->currentAnimationState.font->name.data()))
		{
			for (auto i = 0; i < renderer::font::fonts_.size(); i++)
			{
				auto* font = &renderer::font::fonts_.at(i);

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

		text_property("Text:", element_text_, IM_ARRAYSIZE(element_text_));

		if (element_->text.data() != element_text_)
		{
			element_->text = element_text_;

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
	}
	
	void properties::draw_element_properties()
	{
		if (combo_property("Anchors:", lui::element::anchors_to_string(lui::element::anchors_to_int(element_))))
		{
			for (auto i = 0; i < UIAnchorType::ANCHOR_COUNT; i++)
			{
				if (ImGui::Selectable(lui::element::anchors_to_string(i)))
				{
					element_->currentAnimationState.leftAnchor = (i & UIAnchorType::ANCHOR_LEFT) != 0;
					element_->currentAnimationState.topAnchor = (i & UIAnchorType::ANCHOR_TOP) != 0;
					element_->currentAnimationState.rightAnchor = (i & UIAnchorType::ANCHOR_RIGHT) != 0;
					element_->currentAnimationState.bottomAnchor = (i & UIAnchorType::ANCHOR_BOTTOM) != 0;

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

		color_property("Color:", &element_->currentAnimationState.red);

		slider_property("Alpha:", ImGuiDataType_::ImGuiDataType_Float, &element_->currentAnimationState.alpha, 0.0f, 1.0f);

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
		}
	}

	void properties::draw()
	{
		auto file_dialog = ImGuiFileDialog::Instance();

		if (ImGui::Begin("Properties"))
		{
			if (element_ != nullptr)
			{
				if (ImGui::BeginTable("label_property", 2, ImGuiTableFlags_NoSavedSettings))
				{
					ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, 100);

					auto root = lui::core::get_root_element();
					if (element_ == root)
					{
						draw_canvas_properties();
					}
					else
					{
						draw_element_properties();
					}

					ImGui::EndTable();
				}

				//ImVec2 center = ImGui::GetMainViewport()->GetCenter();
				//ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
				//ImGui::SetNextWindowSize(ImVec2(800.0f, 350.0f), ImGuiCond_Appearing);

				if (file_dialog->Display("SelectedImageDlg", ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse))
				{
					if (file_dialog->IsOk())
					{
						auto filename = file_dialog->GetCurrentFileName();
						auto filepath = utils::string::va("%s\\%s", file_dialog->GetCurrentRelativePath().data(), filename.data());

						auto* image = renderer::image::register_handle(filename, filepath);
						if (element_ != nullptr && image)
						{
							element_->currentAnimationState.image = image;
						}
					}

					file_dialog->Close();
				}
			}
		}

		ImGui::End();
	}
}