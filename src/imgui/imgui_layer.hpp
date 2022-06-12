#pragma once

#include "core/layer.hpp"

namespace uie
{
	class imgui_layer : public layer
	{
	public:
		imgui_layer();
		~imgui_layer() = default;

		virtual void on_attach() override;
		virtual void on_detach() override;

		void begin();
		void end();

		void block_events(bool block) { block_events_ = block; }

		void set_theme();

	private:
		bool block_events_ = true;

	};
}