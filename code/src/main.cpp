#include <stdafx.hpp>
#include "application/application.hpp"

#include "editor/editor_layer.hpp"

namespace uie
{
	application* create_application(const std::string& name)
	{
		auto app = new application(name);

		app->push_layer(new editor_layer());

		return app;
	}
}

int main(int, char**)
{
	auto app = uie::create_application("UIEditor");

	app->run();

	delete app;
}