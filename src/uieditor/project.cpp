#include <stdafx.hpp>
#include "app.hpp"
#include "canvas.hpp"
#include "log.hpp"
#include "project.hpp"
#include "widgets.hpp"
#include "renderer/image.hpp"

namespace uieditor
{
	ProjectState project::state_ = PROJECT_NEW;
	std::string project::project_name_ = "";

	std::vector<std::string> project::saved_elements_;

	void project::new_project()
	{
		widgets::populate_widgets_list();

		saved_elements_.clear();

		lui::core::clear_element_pool();

		canvas::size_.x = 1280.0f;
		canvas::size_.y = 720.0f;

		lui::core::create_root();

		renderer::image::images_.clear();

		canvas::background_image_ = nullptr;

		state_ = PROJECT_NEW;

		set_project_name("Untitled", true);
	}

	void project::save_project(std::string name)
	{
		saved_elements_.clear();

		if (!name.contains(".uip"))
		{
			name.append(".uip");
		}

		auto project_path = std::filesystem::path("uieditor\\projects") / name;
		if (utils::io::file_exists(project_path.string()))
		{
			utils::io::delete_file(project_path.string());
		}

		nlohmann::ordered_json json;

		// save canvas settings
		json["Dimensions"]["Width"] = canvas::size_.x;
		json["Dimensions"]["Height"] = canvas::size_.y;

		// elements
		auto element_pool = &json["Elements"];

		for (auto i = 0; i < lui::core::element_pool_.size(); i++)
		{
			auto element = lui::core::element_pool_.at(i);

			if (std::find(saved_elements_.begin(), saved_elements_.end(), element->id) == saved_elements_.end())
			{
				auto element_data = &(*element_pool)[element->name];
				lui::element::serialize(element, element_data, false);
			}
		}

		log::print(log_normal, "Saving project '%s'", name.data());

		state_ = PROJECT_SAVED;

		set_project_name(name, false);

		utils::io::write_file(project_path.string(), json.dump(4));
	}

	void project::load_project(std::string name)
	{
		lui::core::clear_element_pool();

		auto project_path = std::filesystem::path("uieditor\\projects") / name;
		if (!utils::io::file_exists(project_path.string()))
		{
			return;
		}

		auto project_file = utils::io::read_file(project_path.string());
		if (!project_file.empty())
		{
			auto json = nlohmann::ordered_json::parse(project_file);

			// load canvas settings
			canvas::size_.x = json["Dimensions"]["Width"];
			canvas::size_.y = json["Dimensions"]["Height"];

			// load elements
			auto elements = &json["Elements"];
			for (auto& item : elements->items())
			{
				auto name = item.key();
				auto element = item.value();

				lui::element::deserialize(nullptr, name, &element, false);
			}

			state_ = PROJECT_SAVED;

			widgets::populate_widgets_list();

			set_project_name(name, false);
		}
	}

	void project::set_project_name(std::string name, bool modified)
	{
		project_name_ = name;

		auto title = utils::string::va("LUIGI - %s%s", name.data(), modified ? "*" : "");

		SetWindowTextA(app::hwnd_, title);
	}

	void project::set_project_modified()
	{
		if (state_ == PROJECT_SAVED)
		{
			state_ = PROJECT_MODIFIED;

			set_project_name(project_name_, true);
		}
	}
}