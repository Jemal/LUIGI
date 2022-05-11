#include <stdafx.hpp>
#include "app.hpp"
#include "log.hpp"
#include "settings.hpp"

namespace uieditor
{
	bool settings::show_settings_ = false;
	bool settings::show_add_font_modal_ = false;

	std::string settings::new_font_name_ = ""s;

	std::vector<color_t> settings::colors_;
	std::vector<std::unique_ptr<renderer::font_t>> settings::saved_fonts_;

	void settings::save_color_settings()
	{
		auto path = "uieditor\\config\\colors.json";
		if (!utils::io::file_exists(path))
		{
			log::print(log_error, "Failed to save color settings for project");
			return;
		}

		nlohmann::ordered_json color_settings;

		auto i = 0;

		for (auto& color : colors_)
		{
			color_settings[i]["Name"] = color.name;
			color_settings[i]["r"] = color.r;
			color_settings[i]["g"] = color.g;
			color_settings[i]["b"] = color.b;

			i++;
		}

		utils::io::write_file(path, color_settings.dump(4));
	}

	void settings::load_color_settings()
	{
		auto path = "uieditor\\config\\colors.json";
		if (!utils::io::file_exists(path))
		{
			log::print(log_error, "Failed to load colors settings for project");
			return;
		}

		auto color_settings = utils::io::read_file(path);
		if (!color_settings.empty())
		{
			auto data = nlohmann::ordered_json::parse(color_settings);

			for (auto& item : data.items())
			{
				auto values = item.value();

				auto color_name = values["Name"];
				auto color_r = values["r"];
				auto color_g = values["g"];
				auto color_b = values["b"];

				// add it to the saved fonts so we can view it in settings
				{
					color_t new_color;
					new_color.name = color_name;
					new_color.r = color_r;
					new_color.g = color_g;
					new_color.b = color_b;

					colors_.push_back(new_color);
				}
			}
		}
	}

	void settings::save_font_settings()
	{
		auto path = "uieditor\\config\\fonts.json";
		if (!utils::io::file_exists(path))
		{
			log::print(log_error, "Failed to save font settings for project");
			return;
		}

		nlohmann::ordered_json font_settings;

		auto i = 0;

		for (auto& font : saved_fonts_)
		{
			font_settings[i]["Name"] = font->name;
			font_settings[i]["Path"] = font->path;
			font_settings[i]["Size"] = font->size;

			i++;
		}

		utils::io::write_file(path, font_settings.dump(4));
	}

	void settings::load_font_settings()
	{
		auto path = "uieditor\\config\\fonts.json";
		if (!utils::io::file_exists(path))
		{
			log::print(log_error, "Failed to load font settings for project");
			return;
		}

		auto font_settings = utils::io::read_file(path);
		if (!font_settings.empty())
		{
			auto data = nlohmann::json::parse(font_settings);

			for (auto& item : data.items())
			{
				auto values = item.value();

				auto font_name = values["Name"];
				auto font_path = values["Path"];
				auto font_size = values["Size"];

				if (font_name.is_null() || font_path.is_null() || font_size.is_null())
				{
					return;
				}

				renderer::font::register_font(font_name, font_path, font_size);

				// add it to the saved fonts so we can view it in settings
				{
					renderer::font_t new_font;
					new_font.name = font_name;
					new_font.path = font_path;
					new_font.size = font_size;

					settings::saved_fonts_.push_back(std::make_unique<renderer::font_t>(new_font));
				}
			}
		}
	}

	void settings::add_new_saved_font_entry(std::string path)
	{
		renderer::font_t new_font;
		new_font.name = new_font_name_;
		new_font.path = path;
		new_font.size = 18;

		settings::saved_fonts_.push_back(std::make_unique<renderer::font_t>(new_font));
	}

	void settings::draw_color_settings()
	{
		if (ImGui::BeginTabItem("Colors"))
		{
			if (ImGui::BeginTable("Color Settings", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 125);
				ImGui::TableSetupColumn("Value");
				ImGui::TableHeadersRow();

				for (auto i = 0; i < colors_.size(); i++)
				{
					auto color = &colors_.at(i);

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::Text(color->name.data());

					ImGui::TableNextColumn();

					char buf[64];
					sprintf(buf, "##%s", color->name.data());

					ImGui::PushItemWidth(-1);

					ImGui::ColorEdit3(buf, &color->r);

					ImGui::PopItemWidth();
				}

				ImGui::EndTable();
			}

			ImGui::EndTabItem();
		}
	}

	void settings::draw_font_settings()
	{
		if (ImGui::BeginTabItem("Fonts"))
		{
			if (ImGui::BeginTable("FontSettings", 3, ImGuiTableFlags_BordersV | ImGuiTableFlags_BordersOuterH | ImGuiTableFlags_Resizable | ImGuiTableFlags_RowBg | ImGuiTableFlags_NoBordersInBody | ImGuiTableFlags_NoSavedSettings))
			{
				ImGui::TableSetupColumn("Name", ImGuiTableColumnFlags_WidthFixed, 125);
				ImGui::TableSetupColumn("Path");
				ImGui::TableSetupColumn("Size", ImGuiTableColumnFlags_WidthFixed, 45);
				ImGui::TableHeadersRow();

				for (auto i = 0; i < saved_fonts_.size(); i++)
				{
					auto font = saved_fonts_.at(i).get();

					ImGui::TableNextRow();
					ImGui::TableNextColumn();

					ImGui::Text(font->name.data());

					ImGui::Spacing();

					ImGui::TableNextColumn();

					ImGui::Text(font->path.data());

					ImGui::TableNextColumn();

					char buf[64];
					sprintf(buf, "##%sSize", font->name.data());

					ImGui::PushItemWidth(-1);

					ImGui::DragInt(buf, &font->size, 1, 1, 200);

					ImGui::PopItemWidth();
				}

				ImGui::EndTable();
			}

			if (ImGui::Button("Add"))
			{
				new_font_name_ = ""s;
				show_add_font_modal_ = true;
			}

			ImGui::SameLine();

			if (ImGui::Button("Save"))
			{
				save_font_settings();
			}

			if (ImGui::IsItemHovered())
			{
				ImGui::BeginTooltip();
				ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
				ImGui::TextUnformatted("Restart is required to apply changes");
				ImGui::PopTextWrapPos();
				ImGui::EndTooltip();
			}

			ImGui::PopItemWidth();

			ImGui::EndTabItem();
		}
	}

	void settings::handle_new_font()
	{
		if (show_add_font_modal_)
		{
			ImGui::OpenPopup("Font Name");
			show_add_font_modal_ = false;
		}

		if (ImGui::BeginPopupModal("Font Name", nullptr, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_AlwaysAutoResize))
		{
			ImGui::InputText("##NewFontName", &new_font_name_);

			ImGui::Separator();

			auto ok_size = ImGui::GetButtonSize("Ok");
			auto cancel_size = ImGui::GetButtonSize("Cancel");

			float buttons_width = ok_size.x + cancel_size.x + ImGui::GetStyle().ItemSpacing.x;
			ImGui::SetCursorPosX(ImGui::GetCursorPosX() + ImGui::GetWindowWidth() / 2.0f - buttons_width / 2.0f - ImGui::GetStyle().WindowPadding.x);

			if (ImGui::Button("Ok", ok_size))
			{
				app::file_dialog_mode_ = FILE_DIALOG_FONT;

				ImGui::CloseCurrentPopup();
			}

			ImGui::SameLine();
			if (ImGui::Button("Cancel", cancel_size))
			{
				new_font_name_ = ""s;

				ImGui::CloseCurrentPopup();
			}

			ImGui::EndPopup();
		}
	}

	void settings::draw()
	{
		if (!show_settings_)
		{
			return;
		}

		auto center = ImGui::GetMainViewport()->GetCenter();
		ImGui::SetNextWindowPos(ImGui::GetMainViewport()->GetCenter(), ImGuiCond_Appearing, ImVec2(0.5f, 0.5f));
		ImGui::SetNextWindowSize(ImVec2(800.0f, 500.0f), ImGuiCond_Appearing);

		if (ImGui::Begin("Settings", &show_settings_, ImGuiWindowFlags_NoSavedSettings | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoDocking))
		{
			ImGui::PushItemWidth(ImGui::GetFontSize() * -12);

			if (ImGui::BeginTabBar("MySettings"))
			{
				draw_color_settings();

				draw_font_settings();

				ImGui::EndTabBar();
			}

			ImGui::End();
		}

		handle_new_font();
	}
}