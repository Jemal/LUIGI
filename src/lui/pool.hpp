#pragma once

namespace lui
{
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
				++this->m_iteration;
			}

			this->m_allocatedCount = pool_index;

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
}