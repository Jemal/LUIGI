#pragma once

#include "application/layer.hpp"

namespace uie
{
	class editor_layer : public layer
	{
	public:
		editor_layer();
		~editor_layer() = default;

		virtual void on_attach() override;
		virtual void on_detach() override;

		virtual void on_imgui_render() override;

	private:

	};
}