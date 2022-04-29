#pragma once

namespace uieditor
{
	class project
	{
	public:
		static std::string project_name_;

		static void save_project(std::string name);
		static void load_project(std::string name);

	private:

	};
}