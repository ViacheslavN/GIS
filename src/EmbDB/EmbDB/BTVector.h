#ifndef _EMBEDDED_DATABASE_B_TREE_NODE_READ_ONLY_VECTOR_H_
#define _EMBEDDED_DATABASE_B_TREE_NODE_READ_ONLY_VECTOR_H_
#include "Key.h"
#include "BaseRBTree.h"
namespace embDB
{
	//размер задаеться один раз

	template<class _TKey, class _TValue>
	class TVectorObj
	{
	public:

		typedef _TKey TKey;
		typedef _TValue TValue;

		TVectorObj(const TKey& key, const TValue& val) : m_key(key), m_val(val)
		{}
		TKey m_key;
		TValue m_val;

	};


	/*template <typename TKey>
	struct TVecComp
	{	
		bool LE(const TKey& _Left, const TKey& _Right){
			return (_Left < _Right);
		}
		bool EQ(const TKey& _Left, const TKey& _Right){
			return (_Left == _Right);
		}
	};*/

	template <class _TKey, class _TValue, class _TComp = comp<_TKey> >
	class BNodeVectorRO
	{
	public:
		typedef _TKey       TKey;
		typedef _TValue     TValue;
		typedef _TComp      TComp;
		typedef TVectorObj<TKey, TValue> TVecObj;

		BNodeVectorRO(CommonLib::alloc_t* alloc) : data_(0), alloc_(alloc), capacity_(0), size_(0)
		{

		}
		~BNodeVectorRO()
		{
			clear();
		}
		uint32 size() const {return size_;}
		void clear()
		{
			if(data_)
				alloc_->free(data_);
			size_ = 0;
			capacity_ = 0;
		}
		bool empty()
		{
			return size_ == 0;
		}
		void push_back(TVecObj& obj)
		{
			assert(size_ < capacity_);
			data_[size_] = obj;
			size_++;
		}
		void push_back(const TKey& key, const TValue& val)
		{
			TVecObj obj(key, val);
			assert(size_ < capacity_);
			data_[size_] = obj;
			size_++;
		}
		void reserve(uint32 nSize)
		{
			if(capacity_ > nSize)
				return;

			capacity_ = nSize;
			TVecObj* tmp = (TVecObj*)alloc_->alloc(capacity_ * sizeof(TVecObj));

			if(data_)
			{
				memcpy(tmp, data_, size_* sizeof(TVecObj));
				alloc_->free(data_);
				data_ = tmp;
			}
			else
			{
				data_ = tmp;
			}
		}
		//в предпологаемом использовании данные всегда отсортированны
		TVecObj* search(const TKey& key)
		{
			int32 nIndex = search_index(key);
			if(nIndex != -1)
				return &data_[ nIndex ];
			return NULL;
		}
		int32 search_index(const TKey& key)
		{

			int32 nIndex = -1;
			int32 lb = 0, ub = size_ - 1;
			while( lb <= ub )
			{
				nIndex = ( lb + ub ) >> 1;

				if( m_comp.LE(key, data_[ nIndex ].m_key))
				{
					ub = nIndex - 1;
				}
				else
				{
					
					if(m_comp.EQ(key, data_[ nIndex ].m_key))
					{
						return nIndex;
					}
					else 
					{
						lb = nIndex + 1;
					}
				}
			}
			return -1;
		}
		int32 search_or_less_index(const TKey& key, short& nType) //находит элемент либо возращает последний меньший
		{

			int32 nIndex = -1;
			int32 lb = 0, ub = size_ - 1;
			int32 nLastLessIndex = -1;
			while( lb <= ub )
			{
				nIndex = ( lb + ub ) >> 1;

				if( m_comp.LE(key, data_[ nIndex ].m_key))
				{
					ub = nIndex - 1;
				}
				else
				{

					if(m_comp.EQ(key, data_[ nIndex ].m_key))
					{
						nType = FIND_KEY;
						return nIndex;
					}
					else 
					{
						nLastLessIndex = nIndex;
						lb = nIndex + 1;
					}
				}
			}
			return nLastLessIndex;
		}
		
		const TVecObj& operator [](uint32 nIndex) const
		{
			assert(nIndex < size_);
			return data_[nIndex];
		}

		 TVecObj& operator [](uint32 nIndex) 
		{
			assert(nIndex < size_);
			return data_[nIndex];
		}


	private:
		TVecObj*  data_;
		CommonLib::alloc_t* alloc_;
		uint32 capacity_;
		uint32 size_;
		TComp  m_comp;
	};
}


#endif