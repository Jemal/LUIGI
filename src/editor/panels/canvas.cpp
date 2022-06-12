#include <stdafx.hpp>
#include "editor/editor_layer.hpp"

#ifndef IMGUI_DEFINE_MATH_OPERATORS
#define IMGUI_DEFINE_MATH_OPERATORS
#endif

#include "imgui_internal.h"

#include "renderer/texture.hpp"

#define M_PI	3.14159265358979323846

namespace uie
{
	std::string context_ = "";

	std::vector<std::string> projects_ = {
		"MenuTemplate",
		"UIButton",
		"Amogus"
	};

	ImDrawList* draw_list_ = nullptr;

	ImVec4 canvas::region_ = ImVec4(0.0f, 0.0f, 0.0f, 0.0f);
	ImVec2 canvas::size_ = ImVec2(1280.0f, 720.0f);
	ImVec2 canvas::mouse_pos_ = ImVec2(0.0f, 0.0f);
	ImVec2 canvas::scroll_pos_ = ImVec2(0.0f, 0.0f);

	float zoom_pct_ = 1.0f;

	ImVec2 canvas::get_size()
	{
		return size_;
	}

	void canvas::draw_image(ID3D11ShaderResourceView* texture, float x, float y, float w, float h, float angle, ImVec4 color)
	{
		x *= zoom_pct_;
		y *= zoom_pct_;
		w *= zoom_pct_;
		h *= zoom_pct_;

		if (texture)
		{
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
		else
		{
			draw_list_->AddRectFilled(ImVec2(region_.x + x, region_.y + y), ImVec2(region_.x + x + w, region_.y + y + h), ImGui::GetColorU32(color));
		}
	}

	canvas::canvas()
	{

	}

	void canvas::set_context()
	{

	}

	void canvas::render()
	{
		ImGui::Begin("Canvas", NULL, ImGuiWindowFlags_NoScrollbar);

		ui_tab_bar();

		ImGui::End();
	}

	void canvas::ui_tab_bar()
	{
		ImGui::BeginTabBar("##CanvasTabs");

		for (auto project : projects_)
		{
			if (ImGui::BeginTabItem(project.data()))
			{
				if (project != context_)
				{
					printf("setting context to %s\n", project.data());
					context_ = project;
				}

				auto io = &ImGui::GetIO();
				auto root = lui::element_pool_.front();

				ImGui::BeginChild("##CANVAS_CHILD");

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

				region_min.x += scroll_pos_.x;
				region_min.y += scroll_pos_.y;

				auto region_max = ImVec2(region_min.x + size.x, region_min.y + size.y);

				region_ = ImVec4(region_min.x, region_min.y, region_max.x, region_max.y);

				// canvas button
				{
					auto button_size = ImVec2(content_region_max.x - content_region_min.x, content_region_max.y - content_region_min.y);
					if (button_size.y <= 0.0f)
					{
						button_size.y = 1.0f;
					}

					ImGui::InvisibleButton("##CANVAS_BUTTON", button_size, ImGuiButtonFlags_MouseButtonLeft | ImGuiButtonFlags_MouseButtonRight);
					//ImGui::Button("canvas", button_size);
				}

				draw_list_ = ImGui::GetWindowDrawList();

				// Draw border and background color
				draw_list_->AddRectFilled(ImVec2(region_.x, region_.y), ImVec2(region_.z, region_.w), IM_COL32(0, 0, 0, 255));

				ImGui::EndChild();

				lui::run_frame();

				ImGui::EndTabItem();
			}
		}

		ImGui::EndTabBar();
	}
}