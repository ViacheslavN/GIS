#ifndef _EMBEDDED_DATABASE_TREES_SIMPLE_LIST_
#define _EMBEDDED_DATABASE_TREES_SIMPLE_LIST_

namespace embDB
{

template <class _TValue >
class TVectorRO
{
public:
	typedef _TValue     TValue;
 


	TVectorRO(CommonLib::alloc_t* alloc) : data_(0), alloc_(alloc), capacity_(0), size_(0)
	{
		if(!alloc_)
			alloc_ = &m_simple_alloc;
	}
	~TVectorRO()
	{
		clear();
	}
	size_t size() const {return size_;}
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
	void push_back(const TValue& val)
	{
		assert(size_ < capacity_);
		data_[size_] = val;
		size_++;
	}
	void reserve(size_t nSize)
	{
		if(capacity_ > nSize)
			return;

		capacity_ = nSize;
		TValue* tmp = (TValue*)alloc_->alloc(capacity_ * sizeof(TValue));

		if(data_)
		{
			memcpy(tmp, data_, size_* sizeof(TValue));
			alloc_->free(data_);
			data_ = tmp;
		}
		else
		{
			data_ = tmp;
		}
	}

	/*TValue* binary_search(const TKey& key)
	{
		int32 nIndex = search_index(key);
		if(nIndex != -1)
			return &data_[ nIndex ];
		return NULL;
	}*/

	template<class _TComp >
	int32 binary_search_multi(const TValue& val, _TComp& m_comp)
	{
		if(size_ == 0)
			return -1;
		int32 nIndex = -1;
		int32 lb = 0, ub = size_ - 1;
		while( lb <= ub )
		{
			nIndex = ( lb + ub ) >> 1;


			if(m_comp.EQ(val, data_[ lb ]))
			{
				return lb;
			}

			if( m_comp.LE(val, data_[ nIndex ]))
			{
				ub = nIndex - 1;
			}
			else
			{

				if(m_comp.EQ(val, data_[ nIndex ]))
				{

					if (nIndex == lb + 1)
						return nIndex;
					else
						ub = nIndex + 1;

					//return nIndex;
				}
				else 
				{
					lb = nIndex + 1;
				}
			}
		}
		return -1;
	}

	template<class _TComp >
	int32 binary_search(const TValue& val, _TComp& m_comp)
	{
		if(size_ == 0)
			return -1;
		int32 nIndex = -1;
		int32 lb = 0, ub = size_ - 1;
		while( lb <= ub )
		{
			nIndex = ( lb + ub ) >> 1;
			if( m_comp.LE(val, data_[ nIndex ]))
			{
				ub = nIndex - 1;
			}
			else
			{

				if(m_comp.EQ(val, data_[ nIndex ]))
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
	template<class _TComp >
	int32 binary_search_or_less_index(const TValue& val, short& nType, _TComp& m_comp) //находит элемент либо возращает последний меньший
	{

		if(size_ == 0)
			return -1;

		int32 nIndex = -1;
		int32 lb = 0, ub = size_ - 1;
		int32 nLastLessIndex = -1;
		while( lb <= ub )
		{
			nIndex = ( lb + ub ) >> 1;

			if( m_comp.LE(val, data_[ nIndex ]))
			{
				ub = nIndex - 1;
			}
			else
			{

				if(m_comp.EQ(val, data_[ nIndex ]))
				{

					//if (nIndex == lb + 1)
					{
						nType = FIND_KEY;
						return nIndex;
					}
					//else
					//	ub = nIndex + 1;
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


	template<class _TComp >
	int32 binary_search_or_less_index_multi(const TValue& val, short& nType, _TComp& m_comp) //находит элемент либо возращает последний меньший
	{

		if(size_ == 0)
			return -1;

		int32 nIndex = -1;
		int32 lb = 0, ub = size_ - 1;
		int32 nLastLessIndex = -1;
		while( lb <= ub )
		{
			nIndex = ( lb + ub ) >> 1;

			if(m_comp.EQ(val, data_[ lb ]))
			{
				nType = FIND_KEY;
				return lb;
			}


			if( m_comp.LE(val, data_[ nIndex ]))
			{
				ub = nIndex - 1;
			}
			else
			{

				if(m_comp.EQ(val, data_[ nIndex ]))
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

	const TValue& operator [](size_t nIndex) const
	{
		assert(nIndex < size_);
		return data_[nIndex];
	}

	TValue& operator [](size_t nIndex) 
	{
		assert(nIndex < size_);
		return data_[nIndex];
	}

		private:
			TValue*  data_;
			CommonLib::alloc_t* alloc_;
			size_t capacity_;
			size_t size_;
			//TComp  m_comp;
			CommonLib::simple_alloc_t m_simple_alloc;
	};

}
#endif