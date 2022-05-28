#pragma once

#include "window.hpp"

namespace uie
{
	class application
	{
	public:
		application(const std::string& name);
		~application();

		void run();

	private:
		std::unique_ptr<window> window_;
	};
}