#include <stdafx.hpp>

#include "renderer/texture.hpp"

#include <imgui.h>
#include "editor_layer.hpp"

namespace uie
{
	editor_layer::editor_layer() : layer("EditorLayer")
	{

	}

	void editor_layer::on_attach()
	{

	}

	void editor_layer::on_detach()
	{

	}

	void editor_layer::on_imgui_render()
	{
		ImGui::DockSpaceOverViewport();

		if (ImGui::Begin("Hello World"))
		{
			ImGui::Text("UIEditor");

			static auto white = texture::create("textures/white.png");
			if (white)
			{
				ImGui::ImageButton(white->get_data(), ImVec2(100.0f, 100.0f));
			}
		}

		ImGui::End();
	}
}