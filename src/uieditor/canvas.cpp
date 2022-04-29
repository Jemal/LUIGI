#include <stdafx.hpp>
#include "app.hpp"
#include "canvas.hpp"
#include "tree.hpp"
#include "log.hpp"
#include "project.hpp"
#include "properties.hpp"
#include "renderer/image.hpp"

namespace uieditor
{
	float canvas::zoom_pct_ = 1.0f;
	renderer::image_t* canvas::background_image_ = nullptr;

	ImDrawList* canvas::draw_list_ = nullptr;

	bool canvas::clicked_in_element_ = false;
	bool canvas::hovering_element_ = false;

	int canvas::click_mode_ = UIAnchorType::ANCHOR_NONE;
	int canvas::hover_mode_ = UIAnchorType::ANCHOR_NONE;

	UIElement* canvas::hovered_element_ = nullptr;

	ImVec4 canvas::region_ = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	ImVec2 canvas::size_ = ImVec2(1280.0f, 720.0f);
	ImVec2 canvas::mouse_pos_ = ImVec2(0.0f, 0.0f);

	bool canvas::in_focus_ = false;

	void canvas::push_stencil(float left, float top, float right, float bottom)
	{
		draw_list_->PushClipRect(ImVec2(left + region_.x, region_.y + top), ImVec2(right + region_.x, bottom + region_.y), true);
	}

	void canvas::pop_stencil()
	{
		draw_list_->PopClipRect();
	}

	void canvas::draw_image(ID3D11ShaderResourceView* texture, float x, float y, float w, float h, float angle, ImVec4 color)
	{
		x *= zoom_pct_;
		y *= zoom_pct_;
		w *= zoom_pct_;
		h *= zoom_pct_;

		ImVec2 center((region_.x + x) + (w / 2), (region_.y + y) + (h / 2));

		auto radians = static_cast<float>(angle * M_PI / 180.0f);
		auto cos_a = cosf(radians);
		auto sin_a = sinf(radians);

		ImVec2 pos[4] =
		{
			center + ImRotate(ImVec2(-w * 0.5f, -h * 0.5f), cos_a, sin_a),
			center + ImRotate(ImVec2(+w * 0.5f, -h * 0.5f), cos_a, sin_a),
			center + ImRotate(ImVec2(+w * 0.5f, +h * 0.5f), cos_a, sin_a),
			center + ImRotate(ImVec2(-w * 0.5f, +h * 0.5f), cos_a, sin_a)
		};

		ImVec2 uvs[4] =
		{
			ImVec2(0.0f, 0.0f),
			ImVec2(1.0f, 0.0f),
			ImVec2(1.0f, 1.0f),
			ImVec2(0.0f, 1.0f)
		};

		draw_list_->AddImageQuad(texture, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(color));
	}

	void canvas::draw_text(UIElement* element, float x, float y, float red, float green, float blue, float alpha, const char* text, renderer::font_t* font, float font_height, float wrap_width, int alignment)
	{
		x *= zoom_pct_;
		y *= zoom_pct_;
		wrap_width *= zoom_pct_;

		auto color = ImGui::GetColorU32(ImVec4(red, green, blue, alpha));

		auto font_scale = (font_height / font->size) * zoom_pct_;

		auto size = font->handle->Ascent * font_scale;
		auto dist = (font->size - font->handle->Ascent) * font_scale;

		// text placement has been giving me the biggest headache ever
		// it isnt placed the same way CoD does, im probably missing something obvious or doing something wrong but ye
		// if anyone ever looks at this feel free to fix :)

		auto bruh = size + font_scale;

		draw_list_->AddText(font->handle, size, ImVec2((region_.x + x) - font_scale, (region_.y + y) - bruh), color, text, 0, wrap_width);
		//draw_list_->AddText(font->handle, size, ImVec2((region_.x + x) - font_scale, region_.y + (element->top + (dist / 2.0f))), color, text, 0, wrap_width);
	}

	void canvas::draw_grid()
	{
		auto color = IM_COL32(20, 255, 50, 100);
		auto step = (size_.x * zoom_pct_) / app::grid_step_;

		for (auto x = fmodf(0.0f, step); x < (size_.x * zoom_pct_); x += step)
		{
			draw_list_->AddLine(ImVec2(region_.x + x, region_.y), ImVec2(region_.x + x, region_.w), color);
		}
		for (auto y = fmodf(0.0f, step); y < (size_.y * zoom_pct_); y += step)
		{
			draw_list_->AddLine(ImVec2(region_.x, region_.y + y), ImVec2(region_.z, region_.y + y), color);
		}
	}

	void canvas::highlight_selected_element(UIElement* element, bool hovered)
	{
		auto thickness = 1.0f;

		auto scaled_left = element->left * zoom_pct_;
		auto scaled_top = element->top * zoom_pct_;
		auto scaled_right = element->right * zoom_pct_;
		auto scaled_bottom = element->bottom * zoom_pct_;

		auto width = scaled_right - scaled_left;
		auto height = scaled_bottom - scaled_top;

		auto element_left = width >= 0.0f ? scaled_left : scaled_right;
		auto element_top = height >= 0.0f ? scaled_top : scaled_bottom;
		auto element_right = width >= 0.0f ? scaled_right : scaled_left;
		auto element_bottom = height >= 0.0f ? scaled_bottom : scaled_top;

		auto left = region_.x + element_left;
		auto top = region_.y + element_top;
		auto right = region_.x + element_right;
		auto bottom = region_.y + element_bottom;

		auto anchor_color = IM_COL32(69, 141, 248, 50);
		auto hover_color = hovered ? IM_COL32(248, 141, 69, 255) : IM_COL32(69, 141, 248, 255);
		auto white_color = IM_COL32(244, 244, 244, 255);

		if (!hovered && in_focus_)
		{
			if (hover_mode_ == UIAnchorType::ANCHOR_NONE)
			{
				::SetCursor(clicked_in_element_ && ImGui::IsMouseDown(ImGuiMouseButton_Left) ? ::LoadCursor(NULL, IDC_SIZEALL) : ::LoadCursor(NULL, IDC_ARROW));
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_TOP_LEFT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
				draw_list_->AddTriangleFilled(ImVec2(left, top), ImVec2(left + 20.0f, top), ImVec2(left, top + 20.f), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_TOP_RIGHT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
				draw_list_->AddTriangleFilled(ImVec2(right - 20.0f, top), ImVec2(right, top), ImVec2(right, top + 20.f), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_TOP)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				draw_list_->AddRectFilled(ImVec2(left, top), ImVec2(right, top + 15.0f), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_BOTTOM_LEFT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
				draw_list_->AddTriangleFilled(ImVec2(left, bottom), ImVec2(left + 20.0f, bottom), ImVec2(left, bottom - 20.f), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_BOTTOM_RIGHT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
				draw_list_->AddTriangleFilled(ImVec2(right - 20.0f, bottom), ImVec2(right, bottom), ImVec2(right, bottom - 20.f), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_BOTTOM)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				draw_list_->AddRectFilled(ImVec2(left, bottom), ImVec2(right, bottom - 15.0f), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_LEFT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
				draw_list_->AddRectFilled(ImVec2(left, top), ImVec2(left + 15.0f, bottom), anchor_color);
			}
			else if (hover_mode_ == UIAnchorType::ANCHOR_RIGHT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
				draw_list_->AddRectFilled(ImVec2(right, top), ImVec2(right - 15.0f, bottom), anchor_color);
			}
		}

		draw_list_->AddRect(ImVec2(left, top), ImVec2(right, bottom), hover_color, 0.0f, 0, 1.0f);

		// help debug the text area
		if (element->type == UI_TEXT)
		{
			scaled_left = element->textLeft * zoom_pct_;
			scaled_top = element->textTop * zoom_pct_;
			scaled_right = element->textDimRight * zoom_pct_;
			scaled_bottom = element->textDimBottom * zoom_pct_;

			width = scaled_right - scaled_left;
			height = scaled_bottom - scaled_top;

			element_left = width >= 0.0f ? scaled_left : scaled_right;
			element_top = height >= 0.0f ? scaled_top : scaled_bottom;
			element_right = width >= 0.0f ? scaled_right : scaled_left;
			element_bottom = height >= 0.0f ? scaled_bottom : scaled_top;

			left = region_.x + element_left;
			top = region_.y + element_top;
			right = region_.x + element_right;
			bottom = region_.y + element_bottom;

			//draw_list_->AddRect(ImVec2(left, top), ImVec2(right, bottom), white_color, 0.0f, 0, 1.0f);
		}
	}

	void canvas::update_mode_for_anchors(int* mode, UIElement* element, ImVec2 mouse_pos)
	{
		auto scaled_left = element->left * zoom_pct_;
		auto scaled_top = element->top * zoom_pct_;
		auto scaled_right = element->right * zoom_pct_;
		auto scaled_bottom = element->bottom * zoom_pct_;

		auto width = scaled_right - scaled_left;
		auto height = scaled_bottom - scaled_top;

		// check if we flipped the our element
		auto left = width >= 0.0f ? scaled_left : scaled_right;
		auto top = height >= 0.0f ? scaled_top : scaled_bottom;
		auto right = width >= 0.0f ? scaled_right : scaled_left;
		auto bottom = height >= 0.0f ? scaled_bottom : scaled_top;

		auto radius = 20.0f;

		auto clicked_left = mouse_pos.x >= left && mouse_pos.x <= left + radius;
		auto clicked_top = mouse_pos.y >= top && mouse_pos.y <= top + radius;
		auto clicked_right = mouse_pos.x <= right && mouse_pos.x >= right - radius;
		auto clicked_bottom = mouse_pos.y <= bottom && mouse_pos.y >= bottom - radius;

		auto clicked_middle_x = mouse_pos.x >= left + radius && mouse_pos.x <= right - radius;
		auto clicked_middle_y = mouse_pos.y >= top + radius && mouse_pos.y <= bottom - radius;

		if (clicked_left && clicked_top)
		{
			*mode = UIAnchorType::ANCHOR_TOP_LEFT;
		}
		else if (clicked_right && clicked_top)
		{
			*mode = UIAnchorType::ANCHOR_TOP_RIGHT;
		}
		else if (clicked_middle_x && clicked_top)
		{
			*mode = UIAnchorType::ANCHOR_TOP;
		}
		else if (clicked_left && clicked_bottom)
		{
			*mode = UIAnchorType::ANCHOR_BOTTOM_LEFT;
		}
		else if (clicked_right && clicked_bottom)
		{
			*mode = UIAnchorType::ANCHOR_BOTTOM_RIGHT;
		}
		else if (clicked_middle_x && clicked_bottom)
		{
			*mode = UIAnchorType::ANCHOR_BOTTOM;
		}
		else if (clicked_left && clicked_middle_y)
		{
			*mode = UIAnchorType::ANCHOR_LEFT;
		}
		else if (clicked_right && clicked_middle_y)
		{
			*mode = UIAnchorType::ANCHOR_RIGHT;
		}
		else
		{
			*mode = UIAnchorType::ANCHOR_NONE;
		}
	}

	bool canvas::clicked_in_children_bounds(UIElement* element, ImVec2 mouse_pos, bool hover)
	{
		auto child = element->lastChild;
		while (child)
		{
			auto scaled_left = child->left * zoom_pct_;
			auto scaled_top = child->top * zoom_pct_;
			auto scaled_right = child->right * zoom_pct_;
			auto scaled_bottom = child->bottom * zoom_pct_;

			auto child_width = scaled_right - scaled_left;
			auto child_height = scaled_bottom - scaled_top;

			auto in_child_bounds_x = child_width >= 0.0f ? mouse_pos.x >= scaled_left && mouse_pos.x <= scaled_right : mouse_pos.x >= scaled_right && mouse_pos.x <= scaled_left;
			auto in_child_bounds_y = child_height >= 0.0f ? mouse_pos.y >= scaled_top && mouse_pos.y <= scaled_bottom : mouse_pos.y >= scaled_bottom && mouse_pos.y <= scaled_top;

			if (in_child_bounds_x && in_child_bounds_y)
			{
				if (hover)
				{
					hovered_element_ = child;
				}
				else
				{
					properties::element_ = child;
				}

				return clicked_in_element_bounds(child, mouse_pos, hover);
			}

			if (child->prevSibling == NULL)
			{
				child = child->lastChild;
			}
			else
			{
				child = child->prevSibling;
			}
		}

		return false;
	}

	bool canvas::clicked_in_element_bounds(UIElement* element, ImVec2 mouse_pos, bool hover)
	{
		auto scaled_left = element->left * zoom_pct_;
		auto scaled_top = element->top * zoom_pct_;
		auto scaled_right = element->right * zoom_pct_;
		auto scaled_bottom = element->bottom * zoom_pct_;

		auto in_bounds_x = mouse_pos.x >= scaled_left && mouse_pos.x <= scaled_right;
		auto in_bounds_y = mouse_pos.y >= scaled_top && mouse_pos.y <= scaled_bottom;

		auto in_child_bounds = false;

		if (in_bounds_x && in_bounds_y)
		{
			in_child_bounds = element->firstChild != nullptr ? clicked_in_children_bounds(element, mouse_pos, hover) : true;

			// if we didnt click within a child then just select the parent
			if (!in_child_bounds)
			{
				if (hover)
				{
					hovered_element_ = element;
				}
				else
				{
					properties::element_ = element;
				}

				return true;
			}
		}

		return in_child_bounds;
	}

	void canvas::handle_element_transform(UIElement* element)
	{
		auto io = &ImGui::GetIO();

		hover_mode_ = click_mode_;

		if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 10.0f))
		{
			auto delta = io->MouseDelta;
			if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
			{
				delta.x /= 2.0f;
				delta.y /= 2.0f;
			}

			delta.x /= zoom_pct_;
			delta.y /= zoom_pct_;

			if (click_mode_ == UIAnchorType::ANCHOR_TOP_LEFT)
			{
				properties::element_->currentAnimationState.leftPx += delta.x;
				properties::element_->currentAnimationState.topPx += delta.y;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_TOP_RIGHT)
			{
				properties::element_->currentAnimationState.rightPx += delta.x;
				properties::element_->currentAnimationState.topPx += delta.y;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_TOP)
			{
				properties::element_->currentAnimationState.topPx += delta.y;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_BOTTOM_LEFT)
			{
				properties::element_->currentAnimationState.leftPx += delta.x;
				properties::element_->currentAnimationState.bottomPx += delta.y;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_BOTTOM_RIGHT)
			{
				properties::element_->currentAnimationState.rightPx += delta.x;
				properties::element_->currentAnimationState.bottomPx += delta.y;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_BOTTOM)
			{
				properties::element_->currentAnimationState.bottomPx += delta.y;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_LEFT)
			{
				properties::element_->currentAnimationState.leftPx += delta.x;
			}
			else if (click_mode_ == UIAnchorType::ANCHOR_RIGHT)
			{
				properties::element_->currentAnimationState.rightPx += delta.x;
			}
			else
			{
				properties::element_->currentAnimationState.leftPx += delta.x;
				properties::element_->currentAnimationState.rightPx += delta.x;
				properties::element_->currentAnimationState.topPx += delta.y;
				properties::element_->currentAnimationState.bottomPx += delta.y;
			}

			lui::element::invalidate_layout(properties::element_);
		}
	}

	void canvas::draw()
	{
		if (ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_MenuBar))
		{
			if (ImGui::BeginMenuBar())
			{
				ImGui::TextColored(ImVec4(1.0f, 1.0f, 0.0f, 1.0f), properties::element_->name.data());

				ImGui::Text("(%.0fx%.0f)", properties::element_->right - properties::element_->left, properties::element_->bottom - properties::element_->top);

				ImGui::EndMenuBar();
			}

			auto io = &ImGui::GetIO();
			auto root = lui::core::get_root_element();

			auto content_region_min = ImGui::GetWindowContentRegionMin();
			content_region_min.x += ImGui::GetWindowPos().x;
			content_region_min.y += ImGui::GetWindowPos().y;

			auto content_region_max = ImGui::GetWindowContentRegionMax();
			content_region_max.x += ImGui::GetWindowPos().x;
			content_region_max.y += ImGui::GetWindowPos().y;

			auto size = ImVec2(size_.x * zoom_pct_, size_.y * zoom_pct_);

			auto canvas_window_size = ImVec2((content_region_max.x - content_region_min.x), (content_region_max.y - content_region_min.y));
			auto cursor_screen_pos = ImGui::GetCursorScreenPos();

			auto region_min = ImVec2(cursor_screen_pos.x + ((canvas_window_size.x - size.x) / 2.0f), cursor_screen_pos.y + ((canvas_window_size.y - size.y) / 2.0f));
			auto region_max = ImVec2(region_min.x + size.x, region_min.y + size.y);

			region_ = ImVec4(region_min.x, region_min.y, region_max.x, region_max.y);

			// the implementation of zooming the canvas is super messy
			// everything works fine currently but you cant scroll the canvas if zoom > 100%
			auto button_size = ImVec2(content_region_max.x - content_region_min.x, content_region_max.y - content_region_min.y);
			if (button_size.y <= 0.0f)
			{
				button_size.y = 1.0f;
			}

			ImGui::InvisibleButton("canvas", button_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
			//ImGui::SetItemUsingMouseWheel();

			auto is_hovered = in_focus_ = ImGui::IsItemHovered();
			auto is_active = ImGui::IsItemActive();
			mouse_pos_ = ImVec2(io->MousePos.x - region_.x, io->MousePos.y - region_.y);

			draw_list_ = ImGui::GetWindowDrawList();

			// remove anti aliasing lines from draw list
			if ((draw_list_->Flags & ImDrawListFlags_AntiAliasedLines) != 0)
			{
				draw_list_->Flags &= ~ImDrawListFlags_AntiAliasedLines;
			}

			lui::element::context_menu(properties::element_, true);

			if (is_hovered)
			{
				auto wheel = ImGui::GetIO().MouseWheel;
				if (wheel)
				{
					zoom_pct_ += wheel * 0.05f;
					zoom_pct_ = std::clamp(zoom_pct_, 0.25f, 3.0f);
				}

				hovering_element_ = clicked_in_element_bounds(root, mouse_pos_, true);

				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					clicked_in_element_ = clicked_in_element_bounds(root, mouse_pos_, false);

					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
					{
						if (properties::element_->parent != nullptr)
						{
							properties::element_ = properties::element_->parent;
						}
					}

					hovered_element_ = properties::element_;

					update_mode_for_anchors(&click_mode_, properties::element_, mouse_pos_);
				}

				update_mode_for_anchors(&hover_mode_, properties::element_, mouse_pos_);
			}
			else
			{
				clicked_in_element_ = false;
				hovering_element_ = false;

				click_mode_ = UIAnchorType::ANCHOR_NONE;
				hover_mode_ = UIAnchorType::ANCHOR_NONE;
			}

			if (is_active)
			{
				if (clicked_in_element_ && hovered_element_)
				{
					// dont want to move/resize root
					if (properties::element_ != root)
					{
						handle_element_transform(properties::element_);
					}
				}
			}

			// Draw border and background color
			draw_list_->AddRectFilled(ImVec2(region_.x, region_.y), ImVec2(region_.z, region_.w), IM_COL32(20, 20, 20, 255));
			draw_list_->AddRect(ImVec2(region_.x - 1, region_.y - 1), ImVec2(region_.z + 1, region_.w + 1), IM_COL32(255, 255, 255, 100));

			// keep everything drawn inside the canvas
			draw_list_->PushClipRect(ImVec2(region_.x, region_.y), ImVec2(region_.z, region_.w), true);

			if (app::show_background_)
			{
				if (background_image_)
				{
					draw_image(background_image_->texture, 0.0f, 0.0f, size_.x, size_.y, 0.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}

			// layout and render our elements
			lui::core::frame();

			if (app::show_grid_)
			{
				draw_grid();
			}

			// remove canvas clip
			draw_list_->PopClipRect();

			if (properties::element_ != root)
			{
				highlight_selected_element(properties::element_, false);
			}

			if (properties::element_ != hovered_element_ && hovered_element_ != root && hovered_element_ != NULL)
			{
				highlight_selected_element(hovered_element_, true);
			}

			// draw zoom pct
			{
				auto font = &renderer::font::fonts_.at(0);

				auto x = cursor_screen_pos.x;
				auto y = content_region_max.y - font->handle->FontSize;
				auto zoom = utils::string::va("Zoom: %g%%", zoom_pct_ * 100.0f);

				draw_list_->AddText(font->handle, font->handle->FontSize, ImVec2(x, y), IM_COL32(255, 255, 255, 255), zoom);
			}
		}

		ImGui::End();
	}
}