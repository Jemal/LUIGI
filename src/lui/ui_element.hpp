#pragma once

namespace uie
{
	class ui_element
	{
	public:
		std::uint32_t id_;
		std::string name_;

		ui_element_type type_;

		bool visible_;
		bool locked_;

		ui_element* parent_;
		ui_element* first_child_;
		ui_element* last_child_;
		ui_element* prev_sibling_;
		ui_element* next_sibling_;

		struct
		{
			UIAnimationState current_;
			UIAnimationState* prev_;
			UIAnimationState* next_;
		} states_;

		void invalidate_layout();

		// add on to this function for more types
		void setup();

		int anchors_to_int();
		const char* anchors_to_string(int anchor_value);

		void add_element(ui_element* child);

		bool is_descendent_of(ui_element* parent);

		void(*layout_callback_)(ui_element* root, int delta_time);
		void(*render_callback_)(ui_element*, ui_element*, float, float, float, float);

		static void layout(ui_element* element, ui_element* root, int delta_time);
		static void render(ui_element* element, ui_element* root, float red, float green, float blue, float alpha);

	private:
		float left_;
		float top_;
		float right_;
		float bottom_;

		int priority_;

		void set_parent(ui_element* parent);
		void inherit_parent_flags();

		void calculate_global_rectangle(UIAnimationState* animation_state);
		void set_dimensions(ui_element* root);

		bool update_layout(ui_element* root, int delta_time);
		void default_layout(ui_element* root, int delta_time);

		static void ui_image_render(ui_element* element, ui_element* root, float red, float green, float blue, float alpha);
	};

	typedef UIPool<ui_element, 0, LUI_MAX_ELEMENTS> UIElementPool;
}