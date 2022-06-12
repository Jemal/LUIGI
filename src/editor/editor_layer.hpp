#pragma once

#include "core/layer.hpp"

#include "panels/canvas.hpp"
#include "panels/hierarchy.hpp"
#include "panels/log.hpp"
#include "panels/properties.hpp"

namespace uie
{
	class editor_layer : public layer
	{
	public:
		editor_layer();
		~editor_layer() = default;

		virtual void on_attach() override;
		virtual void on_detach() override;

		virtual void on_render() override;

	private:
		canvas canvas_panel_;
		hierarchy hierarchy_panel_;
		log log_panel_;
		properties properties_panel_;
	};
}