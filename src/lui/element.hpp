#pragma once

enum UIElementType
{
	UI_ELEMENT = 0,
	UI_IMAGE = 1,
	UI_TEXT = 2,
	UI_TYPE_COUNT = 3
};

enum UIAnchorType
{
	ANCHOR_NONE = 0,
	ANCHOR_ALL = 15,

	ANCHOR_TOP = 2,
	ANCHOR_TOP_LEFT = 3,
	ANCHOR_TOP_RIGHT = 6,
	ANCHOR_TOP_LEFT_RIGHT = 7,

	ANCHOR_BOTTOM = 8,
	ANCHOR_BOTTOM_LEFT = 9,
	ANCHOR_BOTTOM_RIGHT = 12,
	ANCHOR_BOTTOM_LEFT_RIGHT = 13,

	ANCHOR_TOP_BOTTOM = 10,
	ANCHOR_TOP_BOTTOM_LEFT = 11,
	ANCHOR_TOP_BOTTOM_RIGHT = 14,

	ANCHOR_LEFT = 1,
	ANCHOR_RIGHT = 4,
	ANCHOR_LEFT_RIGHT = 5,

	ANCHOR_COUNT = 16
};

enum AnimationStateFlags
{
	AS_IN_USE = 1 << 0,
	AS_LAYOUT_CACHED = 1 << 1,
	AS_STENCIL = 1 << 2,
	AS_FOCUS = 1 << 3,

	AS_LEFT_PX = 1 << 4,
	AS_LEFT_PT = 1 << 5,

	AS_TOP_PX = 1 << 6,
	AS_TOP_PT = 1 << 7,

	AS_RIGHT_PX = 1 << 8,
	AS_RIGHT_PT = 1 << 9,

	AS_BOTTOM_PX = 1 << 10,
	AS_BOTTOM_PT = 1 << 11,

	AS_ZROT = 1 << 13,

	AS_RED = 1 << 14,
	AS_GREEN = 1 << 15,
	AS_BLUE = 1 << 16,
	AS_ALPHA = 1 << 17,

	AS_MATERIAL = 1 << 18,
	AS_FONT = 1 << 19,
	AS_TEXT_SCALE = 1 << 20,
	AS_TEXT_STYLE = 1 << 21,
	AS_ALIGNMENT = 1 << 22,
	AS_SCALE = 1 << 23,

	AS_EASE_IN = 1 << 24,
	AS_EASE_OUT = 1 << 25,

	AS_USE_GAMETIME = 1 << 26,
	AS_DEBUG_DRAW_RECT = 1 << 27
};

struct UIAnimationState
{
	unsigned int flags;

	bool leftAnchor;
	bool topAnchor;
	bool rightAnchor;
	bool bottomAnchor;

	float leftPct;
	float topPct;
	float rightPct;
	float bottomPct;

	float leftPx;
	float topPx;
	float rightPx;
	float bottomPx;

	float globalLeft;
	float globalTop;
	float globalRight;
	float globalBottom;

	float red;
	float green;
	float blue;
	float alpha;

	float rotation;

	int alignment;

	renderer::image_t* image;
	renderer::font_t* font;
};

struct UIElement
{
	std::string id; // used for imgui
	std::string name; // element name (would be the .id property in LUI)

	std::string text;

	UIAnimationState currentAnimationState;
	UIAnimationState* prevAnimationState;
	UIAnimationState* nextAnimationState;

	UIElement* parent;
	UIElement* prevSibling;
	UIElement* nextSibling;
	UIElement* firstChild;
	UIElement* lastChild;

	int child_count;

	void(*layoutFunction)(UIElement*, UIElement*, int, int);
	void(*renderFunction)(UIElement*, UIElement*, float, float, float, float);

	unsigned int priority;

	float left;
	float top;
	float right;
	float bottom;

	int type;

	struct
	{
		float textDimLeft;
		float textDimTop;
		float textDimRight;
		float textDimBottom;

		float textLeft;
		float textTop;
	};
};

namespace lui
{
	class element
	{
	public:
		static void add_element(UIElement* element, UIElement* child);
		static void create_element();
		static UIElement* add_and_select_element(bool from_canvas);

		static std::string type_to_string(int type);

		static bool is_descendent_of(UIElement* element, UIElement* child);

		static void invalidate_layout(UIElement* element);

		static void remove_element(UIElement* element);
		static void remove_children(UIElement* element);

		static void remove_from_parent(UIElement* element);
		static void set_parent(UIElement* element, UIElement* parent);
		static void inherit_parent_flags(UIElement* element, UIElement* parent);

		static int anchors_to_int(UIElement* element);
		static const char* anchors_to_string(int anchor_value);

		static void calculate_global_rectangle(UIElement* element, UIAnimationState* animation_state);
		static void get_text_dimensions(UIElement* root, const char* text, renderer::font_t* font, float font_height, float* left, float* top, float* right, float* bottom, float wrap_width);
		static void set_dimensions(UIElement* element, UIElement* root);

		static void default_layout(UIElement* element, UIElement* root, int delta_game_time, int delta_system_time);
		static void layout(UIElement* element, UIElement* root, int delta_game_time, int delta_system_time);
		static bool update_layout(UIElement* element, UIElement* root, int delta_game_time, int delta_system_time);

		static void ui_image_render(UIElement* element, UIElement* root, float red, float green, float blue, float alpha);
		static void ui_text_render(UIElement* element, UIElement* root, float red, float green, float blue, float alpha);

		static void render(UIElement* element, UIElement* root, float red, float green, float blue, float alpha);

		static void context_menu(UIElement* element, bool from_canvas);

	private:

	};
}