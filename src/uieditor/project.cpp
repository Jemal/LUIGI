#include <stdafx.hpp>
#include "app.hpp"
#include "canvas.hpp"
#include "log.hpp"
#include "project.hpp"
#include "renderer/image.hpp"
#include "utils/json.hpp"

namespace uieditor
{
	ProjectState project::state_ = PROJECT_NEW;
	std::string project::project_name_ = "";

	namespace
	{
		static std::vector<std::string> saved_elements_;

		void save_element_data(UIElement* element, nlohmann::ordered_json* element_data)
		{
			if (std::find(saved_elements_.begin(), saved_elements_.end(), element->id) != saved_elements_.end())
			{
				return;
			}

			saved_elements_.push_back(element->id);

			(*element_data)["Type"] = lui::element::type_to_string(element->type);
			(*element_data)["Priority"] = element->priority;

			(*element_data)["Anchors"][0] = element->currentAnimationState.leftAnchor;
			(*element_data)["Anchors"][1] = element->currentAnimationState.topAnchor;
			(*element_data)["Anchors"][2] = element->currentAnimationState.rightAnchor;
			(*element_data)["Anchors"][3] = element->currentAnimationState.bottomAnchor;

			(*element_data)["Position"][0] = element->currentAnimationState.leftPx;
			(*element_data)["Position"][1] = element->currentAnimationState.topPx;
			(*element_data)["Position"][2] = element->currentAnimationState.rightPx;
			(*element_data)["Position"][3] = element->currentAnimationState.bottomPx;

			(*element_data)["Color"]["r"] = element->currentAnimationState.red;
			(*element_data)["Color"]["g"] = element->currentAnimationState.green;
			(*element_data)["Color"]["b"] = element->currentAnimationState.blue;

			(*element_data)["Alpha"] = element->currentAnimationState.alpha;

			(*element_data)["Rotation"] = element->currentAnimationState.rotation;
			(*element_data)["Stencil"] = (element->currentAnimationState.flags & AS_STENCIL) != 0;

			if (element->type == UI_IMAGE)
			{
				if (element->currentAnimationState.image != NULL)
				{
					(*element_data)["Image"]["Name"] = element->currentAnimationState.image->name;
					(*element_data)["Image"]["Path"] = element->currentAnimationState.image->path;
				}
				else
				{
					(*element_data)["Image"] = nullptr;
					(*element_data)["Path"] = nullptr;
				}
			}
			else if (element->type == UI_TEXT && element->currentAnimationState.font != NULL)
			{
				(*element_data)["Alignment"] = element->currentAnimationState.alignment;
				(*element_data)["Font"] = element->currentAnimationState.font->name;
				(*element_data)["Text"] = element->text.data();
			}

			// children
			for (auto i = 0; i < element->child_count; i++)
			{
				auto child = element->firstChild;
				while (child)
				{
					auto children_data = &(*element_data)["Children"][child->name];

					save_element_data(child, children_data);

					child = child->nextSibling;
				}
			}
		}

		void load_element_data(UIElement* element, std::string name, nlohmann::ordered_json* element_data)
		{
			lui::element::create_element();

			auto new_element = lui::core::element_pool_.at(lui::core::element_pool_.size());

			if (element == nullptr)
			{
				element = new_element;
			}
			else
			{
				lui::element::add_element(element, new_element);
			}

			new_element->name = name;

			new_element->type = lui::element::string_to_type((*element_data)["Type"]);
			new_element->priority = (*element_data)["Priority"];

			new_element->currentAnimationState.leftAnchor = (*element_data)["Anchors"][0];
			new_element->currentAnimationState.topAnchor = (*element_data)["Anchors"][1];
			new_element->currentAnimationState.rightAnchor = (*element_data)["Anchors"][2];
			new_element->currentAnimationState.bottomAnchor = (*element_data)["Anchors"][3];

			new_element->currentAnimationState.leftPx = (*element_data)["Position"][0];
			new_element->currentAnimationState.topPx = (*element_data)["Position"][1];
			new_element->currentAnimationState.rightPx = (*element_data)["Position"][2];
			new_element->currentAnimationState.bottomPx = (*element_data)["Position"][3];

			new_element->currentAnimationState.red = (*element_data)["Color"]["r"];
			new_element->currentAnimationState.green = (*element_data)["Color"]["g"];
			new_element->currentAnimationState.blue = (*element_data)["Color"]["b"];

			new_element->currentAnimationState.alpha = (*element_data)["Alpha"];
			new_element->currentAnimationState.rotation = (*element_data)["Rotation"];

			auto uses_stencil = (*element_data)["Stencil"];
			if (uses_stencil)
			{
				new_element->currentAnimationState.flags |= AS_STENCIL;
			}

			if (new_element->type == UI_IMAGE)
			{
				auto image_name = (*element_data)["Image"]["Name"];
				auto image_path = (*element_data)["Image"]["Path"];

				if (!image_name.is_null())
				{
					auto image = renderer::image::register_handle(image_name, image_path);
					if (image)
					{
						new_element->currentAnimationState.image = image;
					}
				}

				new_element->renderFunction = lui::element::ui_image_render;
			}
			else if (new_element->type == UI_TEXT)
			{
				new_element->currentAnimationState.alignment = (*element_data)["Alignment"];

				auto font_name = (*element_data)["Font"];
				new_element->currentAnimationState.font = renderer::font::get_font_handle(font_name);

				new_element->text = std::string((*element_data)["Text"]);

				new_element->renderFunction = lui::element::ui_text_render;
			}

			lui::element::invalidate_layout(element);
			//lui::element::invalidate_layout(new_element);

			auto children_data = &(*element_data)["Children"];
			if (!children_data->is_null())
			{
				for (auto& item : children_data->items())
				{
					auto element_name = item.key();
					auto val = item.value();

					load_element_data(new_element, element_name, &val);
				}
			}
		}
	}

	void project::new_project()
	{
		saved_elements_.clear();

		lui::core::clear_element_pool();

		canvas::size_.x = 1280.0f;
		canvas::size_.y = 720.0f;

		lui::core::create_root();

		renderer::image::images_.clear();

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

		nlohmann::ordered_json project_data;

		// save canvas settings
		project_data["Canvas"]["Width"] = canvas::size_.x;
		project_data["Canvas"]["Height"] = canvas::size_.y;

		// elements
		auto element_pool = &project_data["Elements"];

		for (auto i = 0; i < lui::core::element_pool_.size(); i++)
		{
			auto element = lui::core::element_pool_.at(i);

			if (std::find(saved_elements_.begin(), saved_elements_.end(), element->id) == saved_elements_.end())
			{
				auto element_data = &(*element_pool)[element->name];
				save_element_data(element, element_data);
			}
		}

		log::print(log_normal, "Saving project '%s'", name.data());

		state_ = PROJECT_SAVED;

		set_project_name(name, false);

		utils::io::write_file(project_path.string(), project_data.dump(4));
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
			auto json_data = nlohmann::ordered_json::parse(project_file);

			// load canvas settings
			canvas::size_.x = json_data["Canvas"]["Width"];
			canvas::size_.y = json_data["Canvas"]["Height"];

			// load elements
			auto elements = &json_data["Elements"];
			for (auto& item : elements->items())
			{
				auto element_name = item.key();
				auto element = item.value();

				load_element_data(nullptr, element_name, &element);
			}

			state_ = PROJECT_SAVED;

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