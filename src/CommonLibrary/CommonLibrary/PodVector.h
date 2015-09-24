#ifndef _LIB_COMMON_POD_VECTOR_
#define _LIB_COMMON_POD_VECTOR_

#include "alloc_t.h"
namespace CommonLib
{

	template<class _TValue>
	class TPodVector
	{	
		public:

			typedef _TValue TValue;
			typedef TPodVector<TValue> TSelfType;

			TPodVector(uint32 nSize = 0, alloc_t *pAlloc = NULL) : m_nSize(0), m_pAlloc(pAlloc), m_pData(NULL), m_nCapacity(nSize)
			{
				if(!m_pAlloc)
					m_pAlloc = &m_alloc;
				if(m_nCapacity != 0)
				{
					m_pData = (TValue *)m_pAlloc->alloc(m_nCapacity);
				}
			}

			TPodVector(const TSelfType& vec) : m_nSize(vec.m_nSize),  m_pData(NULL), m_nCapacity(vec.m_nCapacity), m_pAlloc(vec.m_pAlloc)
			{
				 if(vec.m_pAlloc != &vec.m_alloc)
					 m_pAlloc = m_pAlloc;
				 else
					 m_pAlloc = &m_alloc;
				if(m_nSize != 0)
				{
					m_pData = (TValue *)m_pAlloc->alloc(m_nCapacity);
					memcpy(m_pData, vec.m_pData, m_nSize);
				}
			}

			const TSelfType& operator = (const TSelfType& vec)
			{
				if(m_pAlloc != vec.m_pAlloc && vec.m_pAlloc != &vec.m_alloc)
				{
					free();
					m_pAlloc = vec.m_pAlloc;
				}
				m_nSize = vec.size();
				if(m_nSize != 0)
				{
					resize(vec.size());
					memcpy(m_pData, vec.m_pData, sizeof(TValue) * m_nSize);
				}
				
				return *this;
			}

			~TPodVector()
			{
				free();
			}

			TValue& operator [](uint32 nIndex)
			{
				assert(nIndex < m_nSize);
				return m_pData[nIndex];
			}
			const TValue& operator [](uint32 nIndex) const
			{
				assert(nIndex < m_nSize);
				return m_pData[nIndex];
			}

			const TValue* begin() const
			{
				return m_pData;
			}

			TValue* begin()
			{
				return m_pData;
			}

			void reserve(uint32 nSize, bool bClear = false)
			{
				if(nSize > m_nCapacity)
				{
					m_nCapacity = nSize;
					TValue* pTmp = (TValue*)m_pAlloc->alloc(m_nCapacity * sizeof(TValue));

					if(m_pData)
					{
						if(!bClear)
							memcpy(pTmp, m_pData, m_nSize* sizeof(TValue));
						m_pAlloc->free(m_pData);
					}
					m_pData = pTmp;
				}
				if(bClear)
					m_nSize = 0;
			}
			void resize(uint32 nSize)
			{
				if(nSize > m_nSize)
				{
					if(m_pData)
						m_pAlloc->free(m_pData);

					m_pData = (TValue *)m_pAlloc->alloc(nSize * sizeof(TValue));
					m_nCapacity = nSize;
				}

				m_nSize = nSize;
			}
 
			uint32 size() const {return m_nSize;}
			uint32 capacity() const{return m_nCapacity;}
			bool empty() const {return m_nSize == 0;}
			void clear()
			{
				m_nSize = 0;
			}
			void free()
			{
				if(m_pData)
				{
					m_pAlloc->free(m_pData);
					m_nSize = 0;
					m_nCapacity = 0;
					m_pData = NULL;
				}
			}

			void push_back(const TValue& value)
			{
				if(m_nSize == m_nCapacity)
					reserve( m_nSize != 0 ? m_nSize *2 : 2);
				m_pData[m_nSize] = value;
				m_nSize++;
			}

			void push_back(const TSelfType& vec)
			{
				size_t nNewSize = m_nSize + vec.size();
				 if(nNewSize >= m_nCapacity)
					 reserve(2 * m_nCapacity > nNewSize ? 2* m_nCapacity : nNewSize);

				 memcpy(m_pData + m_nSize, vec.m_pData,  vec.size() * sizeof(TValue));

				 m_nSize += vec.size();
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
			
		private:
			TValue *m_pData;
			alloc_t *m_pAlloc;
			simple_alloc_t m_alloc;
			uint32 m_nSize;
			uint32 m_nCapacity;
	};
}



#endif