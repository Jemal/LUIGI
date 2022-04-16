#include <stdafx.hpp>
#include "canvas.hpp"
#include "tree.hpp"
#include "log.hpp"
#include "properties.hpp"
#include "renderer/image.hpp"

namespace uieditor
{
	bool canvas::show_element_highlight = true;
	bool canvas::show_background = false;
	bool canvas::show_grid = false;
	float canvas::grid_step = 24.0f;

	renderer::image_t* canvas::background = nullptr;

	ImDrawList* canvas::draw_list = nullptr;

	bool canvas::clicked_in_element = false;

	int canvas::click_mode = UIAnchorType::ANCHOR_NONE;
	int canvas::hover_mode = UIAnchorType::ANCHOR_NONE;

	ImVec2 canvas::region_min = ImVec2(0.0f, 0.0f);
	ImVec2 canvas::region_max = ImVec2(0.0f, 0.0f);
	ImVec2 canvas::size = ImVec2(1280.0f, 720.0f);
	ImVec2 canvas::mouse_pos = ImVec2(0.0f, 0.0f);

	void canvas::push_stencil(float left, float top, float right, float bottom)
	{
		draw_list->PushClipRect(ImVec2(left + region_min.x, region_min.y + top), ImVec2(right + region_min.x, bottom + region_min.y), true);
	}

	void canvas::pop_stencil()
	{
		draw_list->PopClipRect();
	}

	void canvas::draw_image(ID3D11ShaderResourceView* texture, float x, float y, float w, float h, float angle, ImVec4 color)
	{
		ImVec2 center((region_min.x + x) + (w / 2), (region_min.y + y) + (h / 2));

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

		draw_list->AddImageQuad(texture, pos[0], pos[1], pos[2], pos[3], uvs[0], uvs[1], uvs[2], uvs[3], ImGui::GetColorU32(color));
	}

	void canvas::draw_text(float x, float y, float red, float green, float blue, float alpha, const char* text, renderer::font_t* font, float scale, float wrap_width, int alignment)
	{
		auto color = ImGui::GetColorU32(ImVec4(red, green, blue, alpha));

		auto size = font->handle->FontSize * scale;

		draw_list->AddText(font->handle, font->handle->FontSize * scale, ImVec2(region_min.x + x, region_min.y + (y - size)), color, text, 0, wrap_width);
	}

	void canvas::draw_grid()
	{
		auto color = IM_COL32(20, 255, 50, 75);
		auto step = size.x / grid_step;

		for (auto x = fmodf(0.0f, step); x < size.x; x += step)
		{
			draw_list->AddLine(ImVec2(region_min.x + x, region_min.y), ImVec2(region_min.x + x, region_max.y), color);
		}
		for (auto y = fmodf(0.0f, step); y < size.y; y += step)
		{
			draw_list->AddLine(ImVec2(region_min.x, region_min.y + y), ImVec2(region_max.x, region_min.y + y), color);
		}
	}

	void canvas::highlight_selected_element(UIElement* element)
	{
		auto thickness = 2.0f;

		auto width = element->right - element->left;
		auto height = element->bottom - element->top;

		auto element_left = width >= 0.0f ? element->left : element->right;
		auto element_top = height >= 0.0f ? element->top : element->bottom;
		auto element_right = width >= 0.0f ? element->right : element->left;
		auto element_bottom = height >= 0.0f ? element->bottom : element->top;

		auto left = region_min.x + element_left - thickness;
		auto top = region_min.y + element_top - thickness;
		auto right = region_min.x + element_right + thickness;
		auto bottom = region_min.y + element_bottom + thickness;

		auto anchor_color = IM_COL32(69, 141, 248, 255);
		auto white_color = IM_COL32(244, 244, 244, 255);
		auto hover_color = IM_COL32(244, 244, 244, 50);

		auto canvas_in_focus = ImGui::IsWindowFocused();

		if (canvas_in_focus)
		{
			if (hover_mode == UIAnchorType::ANCHOR_NONE)
			{
				::SetCursor(clicked_in_element && ImGui::IsMouseDown(ImGuiMouseButton_Left) ? ::LoadCursor(NULL, IDC_SIZEALL) : ::LoadCursor(NULL, IDC_ARROW));
			}
			else if (hover_mode == UIAnchorType::ANCHOR_TOP_LEFT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
				draw_list->AddTriangleFilled(ImVec2(left, top), ImVec2(left + 20.0f, top), ImVec2(left, top + 20.f), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_TOP_RIGHT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
				draw_list->AddTriangleFilled(ImVec2(right - 20.0f, top), ImVec2(right, top), ImVec2(right, top + 20.f), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_TOP)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				draw_list->AddRectFilled(ImVec2(left, top), ImVec2(right, top + 15.0f), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_BOTTOM_LEFT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENESW));
				draw_list->AddTriangleFilled(ImVec2(left, bottom), ImVec2(left + 20.0f, bottom), ImVec2(left, bottom - 20.f), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_BOTTOM_RIGHT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENWSE));
				draw_list->AddTriangleFilled(ImVec2(right - 20.0f, bottom), ImVec2(right, bottom), ImVec2(right, bottom - 20.f), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_BOTTOM)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZENS));
				draw_list->AddRectFilled(ImVec2(left, bottom), ImVec2(right, bottom - 15.0f), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_LEFT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
				draw_list->AddRectFilled(ImVec2(left, top), ImVec2(left + 15.0f, bottom), hover_color);
			}
			else if (hover_mode == UIAnchorType::ANCHOR_RIGHT)
			{
				::SetCursor(::LoadCursor(NULL, IDC_SIZEWE));
				draw_list->AddRectFilled(ImVec2(right, top), ImVec2(right - 15.0f, bottom), hover_color);
			}
		}

		draw_list->AddRect(ImVec2(left, top), ImVec2(right, bottom), anchor_color, 0.0f, 0, thickness);

		if (canvas_in_focus)
		{
			left += width >= 0.0f ? 1.0f : -1.0f;
			top += height >= 0.0f ? 1.0f : -1.0f;

			auto middle_x = left + ((right - left) / 2.0f);
			auto middle_y = top + ((bottom - top) / 2.0f);

			auto grab_size = 4.0f;

			draw_list->AddRectFilled(ImVec2(left - grab_size, top - grab_size), ImVec2(left + grab_size, top + grab_size), white_color); // top-left
			draw_list->AddRect(ImVec2(left - grab_size, top - grab_size), ImVec2(left + grab_size, top + grab_size), anchor_color, 0.0f, 0, thickness); // top-left

			draw_list->AddRectFilled(ImVec2(right - grab_size, top - grab_size), ImVec2(right + grab_size, top + grab_size), white_color, 0.0f); // top-right
			draw_list->AddRect(ImVec2(right - grab_size, top - grab_size), ImVec2(right + grab_size, top + grab_size), anchor_color, 0.0f, 0, thickness); // top-right

			draw_list->AddRectFilled(ImVec2(left - grab_size, bottom - grab_size), ImVec2(left + grab_size, bottom + grab_size), white_color, 0.0f); // bottom-left
			draw_list->AddRect(ImVec2(left - grab_size, bottom - grab_size), ImVec2(left + grab_size, bottom + grab_size), anchor_color, 0.0f, 0, thickness); // bottom-left

			draw_list->AddRectFilled(ImVec2(right - grab_size, bottom - grab_size), ImVec2(right + grab_size, bottom + grab_size), white_color, 0.0f); // bottom-right
			draw_list->AddRect(ImVec2(right - grab_size, bottom - grab_size), ImVec2(right + grab_size, bottom + grab_size), anchor_color, 0.0f, 0, thickness); // bottom-right

			draw_list->AddRectFilled(ImVec2(left - grab_size, middle_y - grab_size), ImVec2(left + grab_size, middle_y + grab_size), white_color, 0.0f); // middle-left
			draw_list->AddRect(ImVec2(left - grab_size, middle_y - grab_size), ImVec2(left + grab_size, middle_y + grab_size), anchor_color, 0.0f, 0, thickness); // middle-left

			draw_list->AddRectFilled(ImVec2(right - grab_size, middle_y - grab_size), ImVec2(right + grab_size, middle_y + grab_size), white_color, 0.0f); // middle-right
			draw_list->AddRect(ImVec2(right - grab_size, middle_y - grab_size), ImVec2(right + grab_size, middle_y + grab_size), anchor_color, 0.0f, 0, thickness); // middle-left

			draw_list->AddRectFilled(ImVec2(middle_x - grab_size, top - grab_size), ImVec2(middle_x + grab_size, top + grab_size), white_color, 0.0f); // middle-top
			draw_list->AddRect(ImVec2(middle_x - grab_size, top - grab_size), ImVec2(middle_x + grab_size, top + grab_size), anchor_color, 0.0f, 0, thickness); // middle-left

			draw_list->AddRectFilled(ImVec2(middle_x - grab_size, bottom - grab_size), ImVec2(middle_x + grab_size, bottom + grab_size), white_color, 0.0f); // middle-bottom
			draw_list->AddRect(ImVec2(middle_x - grab_size, bottom - grab_size), ImVec2(middle_x + grab_size, bottom + grab_size), anchor_color, 0.0f, 0, thickness); // middle-left
		}
	}

	bool canvas::clicked_in_children_bounds(UIElement* element, ImVec2 mouse_pos)
	{
		auto child = element->lastChild;
		while (child)
		{
			auto child_width = child->right - child->left;
			auto child_height = child->bottom - child->top;

			auto in_child_bounds_x = child_width >= 0.0f ? mouse_pos.x >= child->left && mouse_pos.x <= child->right : mouse_pos.x >= child->right && mouse_pos.x <= child->left;
			auto in_child_bounds_y = child_height >= 0.0f ? mouse_pos.y >= child->top && mouse_pos.y <= child->bottom : mouse_pos.y >= child->bottom && mouse_pos.y <= child->top;

			if (in_child_bounds_x && in_child_bounds_y)
			{
				auto child_anchor_int = lui::element::anchors_to_int(child);
				auto has_valid_parent = child->parent != nullptr && child->parent != lui::core::get_root_element();

				properties::element = child;

				clicked_in_element_bounds(child, mouse_pos);

				return true;
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

	bool canvas::clicked_in_element_bounds(UIElement* element, ImVec2 mouse_pos)
	{
		auto in_bounds_x = mouse_pos.x >= element->left && mouse_pos.x <= element->right;
		auto in_bounds_y = mouse_pos.y >= element->top && mouse_pos.y <= element->bottom;

		auto in_child_bounds = false;

		if (in_bounds_x && in_bounds_y)
		{
			if (element->firstChild != nullptr)
			{
				in_child_bounds = clicked_in_children_bounds(element, mouse_pos);
			}
			else
			{
				in_child_bounds = true;
			}

		}

		return in_child_bounds;
	}

	void canvas::update_mode_for_anchors(int* mode, UIElement* element, ImVec2 mouse_pos)
	{
		auto width = element->right - element->left;
		auto height = element->bottom - element->top;

		auto left = width >= 0.0f ? element->left : element->right;
		auto top = height >= 0.0f ? element->top : element->bottom;
		auto right = width >= 0.0f ? element->right : element->left;
		auto bottom = height >= 0.0f ? element->bottom : element->top;

		auto middle_x = left + ((right - left) / 2.0f);
		auto middle_y = top + ((bottom - top) / 2.0f);

		auto radius = 20.0f;

		auto clicked_left = mouse_pos.x >= left && mouse_pos.x <= left + radius;
		auto clicked_top = mouse_pos.y >= top && mouse_pos.y <= top + radius;
		auto clicked_right = mouse_pos.x <= right && mouse_pos.x >= right - radius;
		auto clicked_bottom = mouse_pos.y <= bottom && mouse_pos.y >= bottom - radius;

		auto clicked_middle_x = mouse_pos.x >= middle_x - radius && mouse_pos.x <= middle_x + radius;
		auto clicked_middle_y = mouse_pos.y >= middle_y - radius && mouse_pos.y <= middle_y + radius;

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

	void canvas::draw()
	{
		ImGui::SetNextWindowSize(ImVec2(size.x + 15.0f, size.y + 30.0f), ImGuiCond_::ImGuiCond_Always);

		if (ImGui::Begin("Canvas", nullptr, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar))
		{
			auto io = &ImGui::GetIO();
			auto root = lui::core::get_root_element();
			auto is_element_root = properties::element == root;

			auto content_region_min = ImGui::GetWindowContentRegionMin();
			content_region_min.x += ImGui::GetWindowPos().x;
			content_region_min.y += ImGui::GetWindowPos().y;
			
			auto content_region_max = ImGui::GetWindowContentRegionMax();
			content_region_max.x += ImGui::GetWindowPos().x;
			content_region_max.y += ImGui::GetWindowPos().y;

			auto canvas_window_size = ImVec2(content_region_max.x - content_region_min.x, content_region_max.y - content_region_min.y);
			auto cursor_screen_pos = ImGui::GetCursorScreenPos();

			region_min = ImVec2(cursor_screen_pos.x + ((canvas_window_size.x - size.x) / 2.0f), cursor_screen_pos.y + ((canvas_window_size.y - size.y) / 2.0f));
			region_max = ImVec2(region_min.x + size.x, region_min.y + size.y);

			// This will catch our interactions
			ImGui::InvisibleButton("canvas", size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);

			auto is_hovered = ImGui::IsItemHovered();
			auto is_active = ImGui::IsItemActive();
			mouse_pos = ImVec2(io->MousePos.x - region_min.x, io->MousePos.y - region_min.y);

			draw_list = ImGui::GetWindowDrawList();

			lui::element::context_menu(properties::element);

			if (is_hovered)
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					clicked_in_element = clicked_in_element_bounds(root, mouse_pos);

					if (ImGui::IsKeyDown(ImGuiKey_LeftCtrl))
					{
						if (properties::element->parent != nullptr)
						{
							properties::element = properties::element->parent;
						}
					}

					update_mode_for_anchors(&click_mode, properties::element, mouse_pos);
				}

				update_mode_for_anchors(&hover_mode, properties::element, mouse_pos);
			}

			if (is_active && clicked_in_element)
			{
				// dont want to move/resize root
				if (!is_element_root)
				{
					hover_mode = click_mode;

					if (ImGui::IsMouseDragging(ImGuiMouseButton_Left, 10.0f))
					{
						auto delta = io->MouseDelta;
						if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
						{
							delta.x /= 2.0f;
							delta.y /= 2.0f;
						}

						auto keep_current_aspect_ratio = properties::link_width_height;

						if (click_mode == UIAnchorType::ANCHOR_TOP_LEFT)
						{
							properties::element->currentAnimationState.leftPx += delta.x;
							properties::element->currentAnimationState.topPx += delta.y;
						}
						else if (click_mode == UIAnchorType::ANCHOR_TOP_RIGHT)
						{
							properties::element->currentAnimationState.rightPx += delta.x;
							properties::element->currentAnimationState.topPx += delta.y;
						}
						else if (click_mode == UIAnchorType::ANCHOR_TOP)
						{
							properties::element->currentAnimationState.topPx += delta.y;
						}
						else if (click_mode == UIAnchorType::ANCHOR_BOTTOM_LEFT)
						{
							properties::element->currentAnimationState.leftPx += keep_current_aspect_ratio ? (delta.x + delta.y) / 2.0f : delta.x;
							properties::element->currentAnimationState.bottomPx += keep_current_aspect_ratio ? ((delta.x + delta.y) / 2.0f) : delta.y;
						}
						else if (click_mode == UIAnchorType::ANCHOR_BOTTOM_RIGHT)
						{
							properties::element->currentAnimationState.rightPx += keep_current_aspect_ratio ? (delta.x + delta.y) / 2.0f : delta.x;
							properties::element->currentAnimationState.bottomPx += keep_current_aspect_ratio ? (delta.x + delta.y) / 2.0f : delta.y;
						}
						else if (click_mode == UIAnchorType::ANCHOR_BOTTOM)
						{
							properties::element->currentAnimationState.bottomPx += delta.y;
						}
						else if (click_mode == UIAnchorType::ANCHOR_LEFT)
						{
							properties::element->currentAnimationState.leftPx += delta.x;
						}
						else if (click_mode == UIAnchorType::ANCHOR_RIGHT)
						{
							properties::element->currentAnimationState.rightPx += delta.x;
						}
						else
						{
							properties::element->currentAnimationState.leftPx += delta.x;
							properties::element->currentAnimationState.rightPx += delta.x;
							properties::element->currentAnimationState.topPx += delta.y;
							properties::element->currentAnimationState.bottomPx += delta.y;
						}
					}
					else
					{
						auto wheel = io->MouseWheel;
						if (ImGui::IsKeyDown(ImGuiKey_LeftShift))
						{
							wheel *= 5.0f;
						}

						properties::element->currentAnimationState.topPx -= wheel;
						properties::element->currentAnimationState.bottomPx += wheel;
					}

					lui::element::invalidate_layout(properties::element);
				}
			}

			if (!is_hovered)
			{
				clicked_in_element = false;

				click_mode = UIAnchorType::ANCHOR_NONE;
				hover_mode = UIAnchorType::ANCHOR_NONE;
			}

			// Draw border and background color
			{
				draw_list->AddRectFilled(region_min, region_max, IM_COL32(20, 20, 20, 255));
				draw_list->AddRect(ImVec2(region_min.x - 1, region_min.y - 1), ImVec2(region_max.x + 1, region_max.y + 1), IM_COL32(255, 255, 255, 100));
			}

			// keep everything drawn inside the canvas
			draw_list->PushClipRect(region_min, region_max, true);

			if (show_background)
			{
				if (background)
				{
					draw_image(background->texture, 0.0f, 0.0f, size.x, size.y, 0.0f, ImVec4(1.0f, 1.0f, 1.0f, 1.0f));
				}
			}

			// layout and render our elements
			lui::core::frame();

			if (!is_element_root)
			{
				highlight_selected_element(properties::element);
			}

			if (show_grid)
			{
				draw_grid();
			}

			draw_list->PopClipRect();
		}

		ImGui::End();
	}
}