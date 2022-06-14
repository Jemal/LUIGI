#pragma once

namespace uie
{
	class ui_element
	{
	public:
		class animation_state
		{
		public:
			enum anchor_type;
			struct position;
			enum flags;

			int flags()
			{
				return this->flags_;
			}
			void flags(int value)
			{
				this->flags_ |= value;
			}

			struct position
			{
				struct axis
				{
					float offsets[2]; // pixel position based on anchors
					float global[2]; // global position based on pixel position and parent position
					float anchors[2]; // anchors with percentage
				};

				axis x; // left/right
				axis y; // top/bottom
			} position;

		private:
			int flags_;

			float rotation_;

			float red_;
			float green_;
			float blue_;
			float alpha_;

			union
			{
				void* font_;
				void* image_;
			};
		};

		enum class type
		{
			base,
			image,
			text,
			widget,
			custom_element
		};

		std::uint32_t id_;
		std::string name_;

		type type_;

		bool visible_;
		bool locked_;

		ui_element* parent_;
		ui_element* first_child_;
		ui_element* last_child_;
		ui_element* prev_sibling_;
		ui_element* next_sibling_;

		struct
		{
			animation_state current;
			animation_state* prev;
			animation_state* next;
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

		void calculate_global_rectangle(animation_state* animation_state);
		void set_dimensions(ui_element* root);

		bool update_layout(ui_element* root, int delta_time);
		void default_layout(ui_element* root, int delta_time);

		static void ui_image_render(ui_element* element, ui_element* root, float red, float green, float blue, float alpha);
	};

	enum ui_element::animation_state::anchor_type
	{
		none = 0,
		all = 15,

		top = 2,
		top_left = 3,
		top_right = 6,
		top_left_right = 7,

		bottom = 8,
		bottom_left = 9,
		bottom_right = 12,
		bottom_left_right = 13,

		top_bottom = 10,
		top_bottom_left = 11,
		top_bottom_right = 14,

		left = 1,
		right = 4,
		left_right = 5,

		count = 16
	};

	enum ui_element::animation_state::flags
	{
		as_in_use = BIT(0),
		as_layout_cached = BIT(1),
		as_stencil = BIT(2),
		as_focused = BIT(3),
		as_left_offset = BIT(4),
		as_left_anchor = BIT(5),
		as_top_offset = BIT(6),
		as_top_anchor = BIT(7),
		as_right_offset = BIT(8),
		as_right_anchor = BIT(9),
		as_bottom_offset = BIT(8),
		as_bottom_anchor = BIT(9),
		as_rotation = BIT(10),
		as_red = BIT(11),
		as_green = BIT(12),
		as_blue = BIT(13),
		as_alpha = BIT(14),
		as_material = BIT(15),
		as_font = BIT(16),
		as_tween_ease_in = BIT(17),
		as_tween_ease_out = BIT(18)
	};
}