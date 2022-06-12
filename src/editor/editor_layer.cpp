#include <stdafx.hpp>

#include "lui/lui.hpp"
#include "renderer/texture.hpp"

#include "editor_layer.hpp"

namespace uie
{
	bool show_demo_window_ = false;

	editor_layer::editor_layer() : layer("EditorLayer")
	{
		lui::element_pool_.init();
	}

	void editor_layer::on_attach()
	{

	}

	void editor_layer::on_detach()
	{

	}

	void editor_layer::on_render()
	{
		ImGui::DockSpaceOverViewport();

		ImGui::BeginMainMenuBar();

		if (ImGui::BeginMenu("File"))
		{
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Help"))
		{
			ImGui::MenuItem("ImGui Demo", NULL, &show_demo_window_);

			ImGui::EndMenu();
		}

		ImGui::EndMainMenuBar();

		if (show_demo_window_)
		{
			ImGui::ShowDemoWindow();
		}

		canvas_panel_.render();
		hierarchy_panel_.render();
		log_panel_.render();
		properties_panel_.render();
	}
}