#include <stdafx.hpp>
#include "app.hpp"
#include "properties.hpp"
#include "widgets.hpp"

namespace uieditor
{
	std::string widgets::new_widget_name_ = ""s;
	UIElement* widgets::widget_element_ = nullptr;
	std::vector<std::string> widgets::widgets_ = {};

	namespace
	{
		// add these to lui/element.cpp
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

			if (element != widgets::widget_element_)
			{
				(*element_data)["Anchors"][0] = element->currentAnimationState.leftAnchor;
				(*element_data)["Anchors"][1] = element->currentAnimationState.topAnchor;
				(*element_data)["Anchors"][2] = element->currentAnimationState.rightAnchor;
				(*element_data)["Anchors"][3] = element->currentAnimationState.bottomAnchor;

				(*element_data)["Position"][0] = element->currentAnimationState.leftPx;
				(*element_data)["Position"][1] = element->currentAnimationState.topPx;
				(*element_data)["Position"][2] = element->currentAnimationState.rightPx;
				(*element_data)["Position"][3] = element->currentAnimationState.bottomPx;
			}

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
	
		void load_element_data(UIElement* element, std::string name, nlohmann::ordered_json* element_data, std::string widget_name)
		{
			if (widget_name.empty())
			{
				lui::element::create_element();
			}

			auto new_element = lui::core::element_pool_.at(lui::core::element_pool_.size());

			if (!widget_name.empty())
			{
				new_element->name = widget_name;
			}
			else
			{
				if (element == nullptr)
				{
					element = properties::element_;
				}

				lui::element::add_element(element, new_element);

				new_element->name = name;

				new_element->currentAnimationState.leftAnchor = (*element_data)["Anchors"][0];
				new_element->currentAnimationState.topAnchor = (*element_data)["Anchors"][1];
				new_element->currentAnimationState.rightAnchor = (*element_data)["Anchors"][2];
				new_element->currentAnimationState.bottomAnchor = (*element_data)["Anchors"][3];

				new_element->currentAnimationState.leftPx = (*element_data)["Position"][0];
				new_element->currentAnimationState.topPx = (*element_data)["Position"][1];
				new_element->currentAnimationState.rightPx = (*element_data)["Position"][2];
				new_element->currentAnimationState.bottomPx = (*element_data)["Position"][3];
			}

			new_element->type = lui::element::string_to_type((*element_data)["Type"]);
			new_element->priority = (*element_data)["Priority"];

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

					load_element_data(new_element, element_name, &val, "");
				}
			}
		}
	}

	void widgets::populate_widgets_list()
	{
		auto files = utils::io::list_files("./uieditor/widgets/");
		for (auto& file : files)
		{
			file.erase(0, strlen("./uieditor/widgets/"));
			file.erase(file.length() - 4);
		
			widgets_.push_back(file);
		}
	}

	void widgets::save_widget(UIElement* element, std::string filepath)
	{
		if (utils::io::file_exists(filepath))
		{
			utils::io::delete_file(filepath);
		}

		nlohmann::ordered_json widget_data;

		widget_data["Dimensions"]["Width"] = element->right - element->left;
		widget_data["Dimensions"]["Height"] = element->bottom - element->top;

		auto widget = &widget_data["Widget"];

		save_element_data(element, widget);

		utils::io::write_file(filepath, widget_data.dump(4));
	}

	void widgets::load_widget(std::string name)
	{
		auto widget_path = std::filesystem::path("uieditor\\widgets") / name.append(".uiw");
		if (!utils::io::file_exists(widget_path.string()))
		{
			return;
		}

		name.erase(name.length() - 4);

		auto widget_file = utils::io::read_file(widget_path.string());
		if (!widget_file.empty())
		{
			auto json_data = nlohmann::ordered_json::parse(widget_file);

			lui::element::add_and_select_element(true);

			auto new_element = properties::element_;

			new_element->currentAnimationState.rightPx = new_element->currentAnimationState.leftPx + json_data["Dimensions"]["Width"];
			new_element->currentAnimationState.bottomPx = new_element->currentAnimationState.topPx + json_data["Dimensions"]["Height"];

			lui::element::invalidate_layout(new_element);

			auto widget_data = &json_data["Widget"];

			// load elements
			load_element_data(new_element, "", widget_data, name);
		}
	}
}