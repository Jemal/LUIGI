#pragma once

namespace uieditor
{
	class project
	{
	public:
		static bool saving_popup_;
		static bool opening_popup_;

		static std::string project_name_;

		static void save_project(std::string name);
		static void load_project(std::string name);

	private:

	};
}