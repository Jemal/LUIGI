#include <stdafx.hpp>
#include "core/application.hpp"

#include "editor/editor_layer.hpp"

#include "version.hpp"

namespace uie
{
	application* create_application(std::string name)
	{
		auto app = new application(name);

		app->push_layer(new editor_layer());

		return app;
	}
}

int main(int, char**)
{
	auto app = uie::create_application("LUIGI - " __TIMESTAMP__ " build " REVISION_NUMBER_STR);

	app->run();

	delete app;
}