#pragma once

#include "layer.hpp"
#include "window.hpp"

#include "imgui/imgui_layer.hpp"

namespace uie
{
	class application
	{
	public:
		application(const std::string& name);
		~application();

		void push_layer(layer* layer);
		void push_overlay(layer* layer);

		static application& get() { return *instance_; }

		window& get_window() { return *window_; };

		void run();

	private:
		bool running_ = true;
		bool minimized_ = false;

		static application* instance_;

		std::unique_ptr<window> window_;

		static LRESULT WINAPI wnd_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam);

		imgui_layer* imgui_layer_;
		layer_stack layer_stack_;
	};
}