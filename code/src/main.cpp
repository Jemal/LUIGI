#include <stdafx.hpp>
#include "application/application.hpp"

namespace uie
{
	application* create_application(const std::string& name)
	{
		auto app = new application(name);

		// push layers?

		return app;
	}
}

int main(int, char**)
{
	auto app = uie::create_application("UIEditor");

	app->run();

	delete app;
}