#pragma once

#include "lui_cod.hpp"
#include "ui_element.hpp"

#define LUI_MAX_ELEMENTS 100

namespace uie
{
	class lui
	{
	public:
		template<typename T, size_t min, size_t max>
		class pool;

		using element_pool = pool<ui_element, 0, 4500>;
		static element_pool element_pool_;

		static ui_element* create_element();

		static int get_element_count();

		static void run_frame();
	};

	template<typename T, size_t min, size_t max>
	class lui::pool
	{
	public:
		void init()
		{
			memset(this, 0, sizeof(this));

			for (std::uint16_t i = min; i < max; ++i)
			{
				this->pool_data_[i] = i + 1;
			}

			this->first_free_ = 0;
			this->iteration_ = max + 1;
			this->allocated_count_ = 0;
		};

		T* allocate()
		{
			if (this->first_free_ > max)
			{
				return nullptr;
			}

			auto pool_index = this->first_free_;

			assert(pool_index <= max);

			this->first_free_ = this->m_poolData[pool_index];
			this->pool_data_[pool_index] = this->iteration_;

			if (this->iteration_ + 1 >= max + 1)
			{
				this->iteration_ = max + 1;
			}
			else
			{
				this->iteration_++;
			}

			this->allocated_count_++;

			return &this->pool_[pool_index];
		};

		bool free(T* data)
		{
			auto pool_index = ((char*)data - (char*)this) / sizeof(T);

			if (pool_index >= min && pool_index < max)
			{
				if (this->pool_data_[pool_index] <= max)
				{
					__debugbreak();
				}

				this->pool_data_[pool_index] = this->first_free_;
				this->first_free_ = pool_index;

				memset(data, 0, sizeof(T));

				this->allocated_count_--;

				return true;
			}

			return false;
		}

		T* at(int index)
		{
			return &this->pool_[index];
		}

		T* front()
		{
			if (this->allocated_count_ == 0)
			{
				return nullptr;
			}

			return &this->pool_[0];
		}

		T* back()
		{
			return &this->pool_[this->allocated_count_];
		}

		int size()
		{
			return this->allocated_count_;
		}

	private:
		T pool_[max];
		int pool_data_[max];
		int first_free_;
		int iteration_;
		int allocated_count_;
	};
}