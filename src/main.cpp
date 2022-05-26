#include <stdafx.hpp>

#include <utils/entt.hpp>

struct AnimationState
{
	// pixel position based on anchors
	float leftPx;
	float topPx;
	float rightPx;
	float bottomPx;

	// anchors with percentage
	float leftPct;
	float topPct;
	float rightPct;
	float bottomPct;

	// global position based on pixel position and parent position
	float globalLeft;
	float globalTop;
	float globalRight;
	float globalBottom;

	// pitch, yaw, roll
	float xRot;
	float yRot;
	float zRot;

	// color
	float red;
	float green;
	float blue;
	float alpha;

	union
	{
		void* font;
		void* image;
	};
};

struct UIElement
{
	std::uint32_t id;
	std::string name;

	AnimationState currentAnimationState;
};

static entt::registry lui_registry_;

entt::entity create_element(entt::registry& reg)
{
	auto entity = reg.create();

	reg.emplace<UIElement>(entity, static_cast<std::uint32_t>(reg.size() - 1), "element");

	return entity;
}

void update_elements(entt::registry& reg)
{
	auto view = reg.view<UIElement>();

	view.each([](UIElement& element)
	{
		// update element name based on id
		element.name.append(std::to_string(element.id));
	
		printf("%s\n", element.name.data());
	});
}

int main()
{
	for (auto i = 0; i < 20; ++i)
	{
		auto element = create_element(lui_registry_);
	}

	update_elements(lui_registry_);

	return 0;
}
