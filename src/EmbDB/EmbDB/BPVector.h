#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_VECTOR_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_VECTOR_H_
#include "BaseRBTree.h"
namespace embDB
{

	template<class T> inline void swap_elements(T& a, T& b)
	{
		T temp = a;
		a = b;
		b = temp;
	}

	template <class _TValue >
	class TBPVector
	{
	public:
		typedef _TValue     TValue;
 		TBPVector(CommonLib::alloc_t* alloc = NULL) : m_pData(0), m_pAlloc(alloc), m_nCapacity(0), m_nSize(0)
		{
			if(!m_pAlloc)
				m_pAlloc = &m_simple_alloc;
		}
		~TBPVector()
		{
			clear();
		}
		size_t size() const {return m_nSize;}
		void clear()
		{
			if(m_pData)
			{
				m_pAlloc->free(m_pData);
				m_pData = 0;
			}
			m_nSize = 0;
			m_nCapacity = 0;
		}
		bool empty()
		{
			return m_nSize == 0;
		}
		bool push_back(const TValue& val)
		{
			if(m_nSize == m_nCapacity)
			{
				m_nCapacity = 2*m_nSize;
				if(!m_nCapacity)
					m_nCapacity = 2;

				TValue*  pNewdata = (TValue*)m_pAlloc->alloc(m_nCapacity * sizeof(TValue));
				if(!pNewdata)
					return false;
				if(m_pData)
				{
					::memcpy(pNewdata, m_pData, m_nSize* sizeof(TValue));
					m_pAlloc->free(m_pData);
				}
				m_pData = pNewdata;
			
			}
			m_pData[m_nSize] = val;
			m_nSize++;
			return true;
		}

		bool push_back(const TBPVector& vec)
		{
			return copy(vec, m_nSize, 0, vec.size());
		}
		bool insert(const TValue& value, size_t idx)
		{
			if(idx > m_nSize)
				return false;

			if((m_nSize + 1) > m_nCapacity)
			{
				if(!m_nCapacity)
					m_nCapacity = 1;
				reserve(2 * m_nCapacity);
			}
	
			if(m_nSize == idx)
				return push_back(value);
		
			::memmove( m_pData + idx + 1, m_pData + idx, ( m_nSize - idx)*sizeof( TValue ) );
			m_pData[idx] = value;
			m_nSize++;
			return true;
		}


		bool insert(const TBPVector& vec, size_t nPos, size_t nBegin, size_t nEnd)
		{		 
			size_t nLen = nEnd - nBegin;
			if(m_nSize + nPos + nLen >= m_nCapacity)
			{
				if(!reserve((2 * m_nCapacity) > (m_nSize + nPos + nLen) ? 2 * m_nCapacity :  2 * (m_nSize + nPos + nLen) ))
					return false;
			}

			if(m_nSize == nPos)
				return push_back(vec);
			if(nPos < m_nSize)
				mover(nPos, nLen);

			::memcpy( m_pData + nPos, vec.m_pData + nBegin, ( nLen)*sizeof( TValue ) );
			return true;
		}


		bool remove(size_t idx)
		{
			if(idx > m_nSize)
				return false;
			::memmove( m_pData + idx, m_pData + idx + 1, 
				( m_nSize - idx - 1 )*sizeof( TValue ) );
			m_nSize--;
			return true;
		}
	 
		bool reserve(size_t nSize)
		{
			if(m_nCapacity > nSize)
				return true;

			m_nCapacity = m_nCapacity + nSize;
			TValue* tmp = (TValue*)m_pAlloc->alloc(m_nCapacity * sizeof(TValue));
			if(!tmp)
				return false;
			if(m_pData)
			{
				memcpy(tmp, m_pData, m_nSize* sizeof(TValue));
				m_pAlloc->free(m_pData);
				m_pData = tmp;
			}
			else
			{
				m_pData = tmp;
			}
			return true;
		}
		bool resize(size_t nSize)
		{
			if(m_nSize > nSize)
			{
				m_nSize = nSize;
				return true;
			}
			if(!reserve(nSize))
				return false;
			m_nSize = nSize;
			return true;
		}
		bool copy(const TBPVector& vec, size_t nPos, size_t nBegin, size_t nEnd)
		{
			size_t nLen = nEnd - nBegin;
			if(m_nSize + nPos + nLen >= m_nCapacity)
			{
				if(!reserve((2 * m_nCapacity) > (m_nSize + nPos + nLen) ? 2 * m_nCapacity :  2 * (m_nSize + nPos + nLen) ))
					return false;
			}
			memcpy(m_pData + nPos, vec.m_pData + nBegin,  nLen * sizeof(TValue));
		 
			if((nPos + nLen) > m_nSize)
				m_nSize += (nPos + nLen) - m_nSize;
			return true;
		}
		bool mover(size_t nPos, size_t nCnt)
		{
			if( m_nSize + nCnt >= m_nCapacity)
			{
				if(!reserve((2 * m_nCapacity) > m_nSize + nCnt ? 2*m_nCapacity : 2 * (m_nSize + nCnt) ))
					return false;
			}
			memmove(m_pData + nPos + nCnt, m_pData + nPos, (m_nSize - nPos)* sizeof(TValue)); 
			m_nSize +=  nCnt;
			return true;
		}

		bool movel(size_t nPos, size_t nCnt)
		{
		    memmove(m_pData + nPos - nCnt,  m_pData + nPos, (m_nSize  - nCnt)* sizeof(TValue));
			m_nSize -= nCnt;
			return true;
		}
		template<class _TComp >
		int32 binary_search(const TValue& val, _TComp& comp)
		{
			short  nType = 0;
			int32 nIndex = lower_bound(val, nType, comp);
			return nType == FIND_KEY ? nIndex : -1;
		}
	


		template<class _TComp >
		int32 lower_bound(const TValue& val, short& nType, _TComp& comp, int32 nLeft = -1, int32 nRight = -1) 
		{

			if(m_nSize == 0)
				return -1;

			int32 nFirst = nLeft == -1 ? 0 : nLeft;
			int32 nIndex = 0;
			int32 nStep = 0;
			int32 nCount = nRight == -1 ? m_nSize : nRight;
			while (nCount > 0)
			{
				nIndex = nFirst; 
				nStep = nCount >> 1;
				nIndex += nStep;
				if(comp.LE(m_pData[ nIndex ], val))
				{
					nFirst = ++nIndex;
					nCount -= (nStep + 1);
				} 
				else nCount = nStep;
			}
			if(nFirst < (int32)m_nSize && comp.EQ(val, m_pData[ nFirst ]))
			  nType = FIND_KEY;
			return nFirst;
		}
		
		template<class _TComp >
		int32 upper_bound(const TValue& val, const _TComp& comp)  
		{

			if(m_nSize == 0)
				return -1;

			int32 nFirst = 0;
			int32 nIndex = 0;
			int32 nStep = 0;
			int32 nCount = m_nSize;
			while (nCount > 0)
		   {
				nIndex = nFirst; 
				nStep = nCount >> 1;
				nIndex += nStep;
				if(!comp.LE(val, m_pData[ nIndex ]))
				{
					nFirst = ++nIndex;
					nCount -= (nStep + 1);
				} 
				else nCount = nStep;
			}
			return nFirst;
		}



		const TValue& operator [](size_t nIndex) const
		{
			assert(nIndex < m_nSize);
			return m_pData[nIndex];
		}
		TValue& operator [](size_t nIndex) 
		{
			assert(nIndex < m_nSize);
			return m_pData[nIndex];
		}

		TValue& back() 
		{
			assert(m_nSize);
			return m_pData[m_nSize - 1];
		}

		const TValue& back() const
		{
			assert(m_nSize);
			return m_pData[m_nSize - 1];
		}


		TValue& front() 
		{
			assert(m_nSize);
			return m_pData[0];
		}

		const TValue& front() const
		{
			assert(m_nSize);
			return m_pData[0];
		}
	
		void swap(TBPVector& vec)
		{
			//alloc must be compatible

			std::swap(vec.m_pData, m_pData);
			std::swap(vec.m_nCapacity, m_nCapacity);
			std::swap(vec.m_nSize, m_nSize);
		}

		

		template<class _TComp >
		void quick_sort(_TComp& comp)
		{
			if(m_nSize < 2) return;

			TValue* e1;
			TValue* e2;

			int  stack[80];
			int* top = stack; 
			int  limit = m_nSize;
			int  base = 0;

			for(;;)
			{
				int len = limit - base;

				int i;
				int j;
				int pivot;

				if(len > 9)
				{
					// we use base + len/2 as the pivot
					pivot = base + len / 2;
					swap_elements(m_pData[base], m_pData[pivot]);

					i = base + 1;
					j = limit - 1;

					// now ensure that *i <= *base <= *j 
					e1 = &(m_pData[j]); 
					e2 = &(m_pData[i]);
					if(comp.LE(*e1, *e2)) swap_elements(*e1, *e2);

					e1 = &(m_pData[base]); 
					e2 = &(m_pData[i]);
					if(comp.LE(*e1, *e2)) swap_elements(*e1, *e2);

					e1 = &(m_pData[j]); 
					e2 = &(m_pData[base]);
					if(comp.LE(*e1, *e2)) swap_elements(*e1, *e2);

					for(;;)
					{
						do i++; while(comp.LE(m_pData[i], m_pData[base]));
						do j--; while(comp.LE(m_pData[base], m_pData[j]));

						if(i > j)
						{
							break;
						}

						swap_elements(m_pData[i], m_pData[j]);
					}

					swap_elements(m_pData[base], m_pData[j]);

					// now, push the largest sub-array
					if(j - base > limit - i)
					{
						top[0] = base;
						top[1] = j;
						base   = i;
					}
					else
					{
						top[0] = i;
						top[1] = limit;
						limit  = j;
					}
					top += 2;
				}
				else
				{
					// the sub-array is small, perform insertion sort
					j = base;
					i = j + 1;

					for(; i < limit; j = i, i++)
					{
						for(; comp.LE(*(e1 = &(m_pData[j + 1])), *(e2 = &(m_pData[j]))); j--)
						{
							swap_elements(*e1, *e2);
							if(j == base)
							{
								break;
							}
						}
					}
					if(top > stack)
					{
						top  -= 2;
						base  = top[0];
						limit = top[1];
					}
					else
					{
						break;
					}
				}
			}
		}

		void setAlloc(CommonLib::alloc_t* pAlloc)
		{
			m_pAlloc = pAlloc;
			if(!m_pAlloc)
				m_pAlloc = &m_simple_alloc;
		}
		protected:
			TValue*  m_pData;
			CommonLib::alloc_t* m_pAlloc;
			size_t m_nCapacity;
			size_t m_nSize;
			CommonLib::simple_alloc_t m_simple_alloc;
		};
}

#endif