#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_VECTOR_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_VECTOR_H_
#include "BaseRBTree.h"
namespace embDB
{
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
				m_pAlloc->free(m_pData);
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
		bool insert(const TValue& value, size_t idx)
		{
			if(idx > m_nSize)
				return false;

			if((m_nSize + 1) > m_nCapacity)
				reserve(2 * m_nCapacity);
	
			if(m_nSize == idx)
				return push_back(value);
		
			::memmove( m_pData + idx + 1, m_pData + idx, ( m_nSize - idx)*sizeof( TValue ) );
			m_pData[idx] = value;
			m_nSize++;
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

			m_nCapacity = nSize;
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
			memcpy(m_pData + nPos, vec.m_pData + nBegin,  nLen);
			m_nSize += nLen;
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
		int32 lower_bound(const TValue& val, short& nType, _TComp& comp) 
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
				nStep = nCount / 2;
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
		int32 upper_bound(const TValue& val,_TComp& comp)  
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
				nStep = nCount / 2;
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

			private:
				TValue*  m_pData;
				CommonLib::alloc_t* m_pAlloc;
				size_t m_nCapacity;
				size_t m_nSize;
				CommonLib::simple_alloc_t m_simple_alloc;
		};
}

#endif