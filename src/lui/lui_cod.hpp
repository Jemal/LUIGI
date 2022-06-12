#pragma once

#define LUI_MAX_ELEMENTS 100

struct UIAnchorPair
{
	const char* name;
	int value;
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

enum UIAnimationStateFlags
{
	AS_IN_USE = 1 << 0, // pretty sure this flag stands for visible, oh well
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

	AS_USE_GAMETIME = 1 << 26
};


struct UIAnimationState
{
	int flags;

	bool leftAnchor;
	bool topAnchor;
	bool rightAnchor;
	bool bottomAnchor;

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
	float rotation;

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

enum class ui_element_type
{
	base,
	image,
	text,
	widget,
	custom_element
};

template<typename T, size_t Min, size_t Max>
class UIPool
{
public:
	T m_pool[Max];
	int m_poolData[Max];
	int m_firstFree;
	int m_iteration;
	int m_allocatedCount;

	void init()
	{
		memset(this, 0, sizeof(this));

		for (std::uint16_t i = Min; i < Max; ++i)
		{
			this->m_poolData[i] = i + 1;
		}

		this->m_firstFree = 0;
		this->m_iteration = Max + 1;
		this->m_allocatedCount = 0;
	};

	T* allocate()
	{
		if (this->m_firstFree > Max)
		{
			return nullptr;
		}

		auto pool_index = this->m_firstFree;

		assert(pool_index <= Max);

		this->m_firstFree = this->m_poolData[pool_index];
		this->m_poolData[pool_index] = this->m_iteration;

		if (this->m_iteration + 1 >= Max + 1)
		{
			this->m_iteration = Max + 1;
		}
		else
		{
			this->m_iteration++;
		}

		this->m_allocatedCount++;

		return &this->m_pool[pool_index];
	};

	bool free(T* data)
	{
		auto pool_index = ((char*)data - (char*)this) / sizeof(T);

		if (pool_index >= Min && pool_index < Max)
		{
			if (this->m_poolData[pool_index] <= Max)
			{
				__debugbreak();
			}

			this->m_poolData[pool_index] = this->m_firstFree;
			this->m_firstFree = pool_index;

			memset(data, 0, sizeof(T));

			this->m_allocatedCount--;

			return true;
		}

		return false;
	}

	T* at(int index)
	{
		return &this->m_pool[index];
	}

	T* front()
	{
		if (this->m_allocatedCount == 0)
		{
			return nullptr;
		}

		return &this->m_pool[0];
	}

	T* back()
	{
		return &this->m_pool[this->m_allocatedCount];
	}

	int size()
	{
		return this->m_allocatedCount;
	}
};
