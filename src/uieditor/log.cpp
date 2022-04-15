#include <stdafx.hpp>
#include "log.hpp"

namespace uieditor
{
	ImVector<log_message> log::output;

	void log::print(int type, const char* fmt, ...)
	{
		char va_buffer[0x200] = { 0 };

		va_list ap;
		va_start(ap, fmt);
		vsprintf_s(va_buffer, fmt, ap);
		va_end(ap);

		auto formatted = std::string(va_buffer);
		auto lines = utils::string::split(formatted, '\n');

		for (auto& line : lines)
		{
			print_message(type, line.data());
		}
	}

	void log::print_message(int type, char* message)
	{
		auto string = _strdup(message);

		output.push_back({ type, string });

		::printf(string);
		::printf("\n");
	}

	void log::draw()
	{
		if (ImGui::Begin("Output Log"))
		{
			ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(4, 1)); // Tighten spacing

				ImGuiListClipper clipper;
				clipper.Begin(output.size());

				while (clipper.Step())
				{
					for (auto i = clipper.DisplayStart; i < clipper.DisplayEnd; i++)
					{
						auto line = output[i];

						ImVec4 color;
						auto has_color = line.type == log_message_type::log_error || line.type == log_message_type::log_warning;

						if (line.type == log_message_type::log_error)
						{ 
							color = ImVec4(1.0f, 0.4f, 0.4f, 1.0f); 
						}
						else if (line.type == log_message_type::log_warning)
						{ 
							color = ImVec4(1.0f, 0.8f, 0.6f, 1.0f); 
						}

						if (has_color)
						{
							ImGui::PushStyleColor(ImGuiCol_Text, color);
						}

						ImGui::Text(line.message);

						if (has_color)
						{
							ImGui::PopStyleColor();
						}
					}
				}

				clipper.End();

			ImGui::PopStyleVar();

			if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY())
			{
				ImGui::SetScrollHereY(1.0f);
			}
		}

		ImGui::End();
	}
}