#include <stdafx.hpp>
#include "tree.hpp"
#include "log.hpp"
#include "properties.hpp"

namespace uieditor
{
	UIElement* properties::element = nullptr;
	char properties::element_name[32] = {};
	char properties::element_text[256] = {};
	bool properties::link_width_height = false;

	float properties::input_fast_step = 10.0f;

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

		if (combo_property("Image", element->currentAnimationState.image == nullptr ? "Select..." : element->currentAnimationState.image->name.data()))
		{
			for (auto i = 0; i < renderer::image::loaded_images.size(); i++)
			{
				auto* image = &renderer::image::loaded_images.at(i);

				ImGui::PushID(i);

				const bool is_selected = (selected_image == i);

				if (ImGui::Selectable(image->name.data()))
				{
					selected_image = i;

					element->currentAnimationState.image = image;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}
	}

	void properties::draw_text_properties()
	{
		static int selected_font = 0;

		if (combo_property("Font", element->currentAnimationState.font == nullptr ? "Select..." : element->currentAnimationState.font->name.data()))
		{
			for (auto i = 0; i < renderer::font::loaded_fonts.size(); i++)
			{
				auto* font = &renderer::font::loaded_fonts.at(i);

				ImGui::PushID(i);

				const bool is_selected = (selected_font == i);

				if (ImGui::Selectable(font->name.data()))
				{
					selected_font = i;

					element->currentAnimationState.font = font;
				}

				if (is_selected)
				{
					ImGui::SetItemDefaultFocus();
				}

				ImGui::PopID();
			}

			ImGui::EndCombo();
		}

		slider_property("Font Scale:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.textScale, 0.0f, 1.0f);

		text_property("Text:", uieditor::properties::element_text, IM_ARRAYSIZE(uieditor::properties::element_text));

		if (element->text.data() != uieditor::properties::element_text)
		{
			element->text = uieditor::properties::element_text;
		}
	}

	void properties::draw()
	{
		if (ImGui::Begin("Properties", 0, ImGuiWindowFlags_::ImGuiWindowFlags_MenuBar))
		{
			if (element != nullptr)
			{
				if (ImGui::BeginMenuBar())
				{
					ImGui::Text("Width: %.0f", element->right - element->left);

					ImGui::PushStyleColor(ImGuiCol_Button, link_width_height ? ImGui::GetStyleColorVec4(ImGuiCol_Button) : ImVec4(0, 0, 0, 0));

					if (ImGui::ImageButton(renderer::engine::globals.link_icon, ImVec2(16, 16), ImVec2(0, 0), ImVec2(1, 1), 2))
					{
						link_width_height = link_width_height == false;
					}

					ImGui::PopStyleColor();
					
					ImGui::Text("Height : %.0f", element->bottom - element->top);

					ImGui::EndMenuBar();
				}

				if (ImGui::BeginTable("label_property", 2, ImGuiTableFlags_NoSavedSettings))
				{
					ImGui::TableSetupColumn("##Label", ImGuiTableColumnFlags_WidthFixed, 100);

					if (combo_property("Anchors:", lui::element::anchors_to_string(lui::element::anchors_to_int(element))))
					{
						for (auto i = 0; i < UIAnchorType::ANCHOR_COUNT; i++)
						{
							if (ImGui::Selectable(lui::element::anchors_to_string(i)))
							{
								element->currentAnimationState.leftAnchor = (i & UIAnchorType::ANCHOR_LEFT) != 0;
								element->currentAnimationState.topAnchor = (i & UIAnchorType::ANCHOR_TOP) != 0;
								element->currentAnimationState.rightAnchor = (i & UIAnchorType::ANCHOR_RIGHT) != 0;
								element->currentAnimationState.bottomAnchor = (i & UIAnchorType::ANCHOR_BOTTOM) != 0;

								lui::element::invalidate_layout(element);
							}
						}

						ImGui::EndCombo();
					}

					if (input_property("Left:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.leftPx, 1.0f, input_fast_step))
					{
						lui::element::invalidate_layout(element);
					}

					if (input_property("Right:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.rightPx, 1.0f, input_fast_step))
					{
						lui::element::invalidate_layout(element);
					}

					if (input_property("Top:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.topPx, 1.0f, input_fast_step))
					{
						lui::element::invalidate_layout(element);
					}

					if (input_property("Bottom:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.bottomPx, 1.0f, input_fast_step))
					{
						lui::element::invalidate_layout(element);
					}

					color_property("Color:", &element->currentAnimationState.red);

					slider_property("Alpha:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.alpha, 0.0f, 1.0f);

					if (element->type == UIElementType::UI_IMAGE)
					{
						draw_image_properties();
					}
					else if (element->type == UIElementType::UI_TEXT)
					{
						draw_text_properties();
					}

					if (input_property("Rotation:", ImGuiDataType_::ImGuiDataType_Float, &element->currentAnimationState.rotation, 1.0f, input_fast_step))
					{
						lui::element::invalidate_layout(element);
					}

					auto uses_stencil = (element->currentAnimationState.flags & AS_STENCIL) != 0;

					if (bool_property("Stencil", &uses_stencil))
					{
						if (uses_stencil)
						{
							element->currentAnimationState.flags |= AS_STENCIL;
						}
						else
						{
							element->currentAnimationState.flags &= ~AS_STENCIL;
						}
					}

					ImGui::EndTable();
				}
			}

		}

		ImGui::End();
	}
}