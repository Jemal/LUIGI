#pragma once

namespace uieditor
{
	enum ProjectState
	{
		PROJECT_NEW,
		PROJECT_SAVED,
		PROJECT_MODIFIED
	};

	class project
	{
	public:
		static ProjectState state_;
		static std::string project_name_;

		static void new_project();
		static void save_project(std::string name);
		static void load_project(std::string name);

		static void set_project_name(std::string name, bool modified);

		static void set_project_modified();

	private:

	};
}