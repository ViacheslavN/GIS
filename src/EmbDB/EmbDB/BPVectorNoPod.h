#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_VECTOR_NO_POD_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_VECTOR_NO_POD_H_
#include "BPVector.h"
namespace embDB
{

 
	template <class _TValue >
	class TBPVectorNoPOD : protected TBPVector<_TValue>
	{
	public:
		typedef _TValue     TValue;
		typedef TBPVector<_TValue> TBase;
		TBPVectorNoPOD(CommonLib::alloc_t* alloc = NULL) : TBase(alloc)
		{

		}
		~TBPVectorNoPOD()
		{
			if(m_pData)
			{
				_clear(m_pData, m_nSize);
				m_pData = NULL;
			}
			m_nSize = 0;
			m_nCapacity = 0;
		}
		size_t size() const {return m_nSize;}

		void _clear(TValue*  pData, size_t nSize)
		{
			for (size_t i = 0; i < nSize; ++i)
			{
				pData[i].~TValue();
				
			}
			m_pAlloc->free(pData);
		}
		void _move(TValue*  pDataSrc, size_t nSizeSrc, TValue* pDataDst, size_t nSizeDst)
		{
			size_t i = 0;
			for (; i < nSizeSrc; ++i)
			{
				new ((TValue*)&pDataDst[i]) TValue(pDataSrc[i]);
			}
		/*	for (; i < nSizeDst; ++i )
			{
				new ((TValue*)&pDataDst[i]) TValue();
			}*/
		}
		void _init(TValue*  pData, size_t nSize)
		{	
			for (size_t i = 0; i < nSize; ++i)
			{
				new ((TValue*)&pData[i]) TValue();
			}
		}
		void clear()
		{
			if(m_pData)
			{
				 
				m_pAlloc->free(m_pData);

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
					//::memcpy(pNewdata, m_pData, m_nSize* sizeof(TValue));
					//m_pAlloc->free(m_pData);
					_move(m_pData, m_nSize, pNewdata, m_nCapacity);
					_clear(m_pData, m_nSize);
				}
			//	else
			//		_init(pNewdata, m_nCapacity);
				m_pData = pNewdata;
		

			}
			new ((TValue*)&m_pData[m_nSize]) TValue(val);
			//m_pData[m_nSize] = val;
			m_nSize++;
			return true;
		}

		bool push_back(const TBPVectorNoPOD& vec)
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

			//::memmove( m_pData + idx + 1, m_pData + idx, ( m_nSize - idx)*sizeof( TValue ) );
			mover(idx, 1);
			m_pData[idx] = value;
			m_nSize++;
			return true;
		}


		bool insert(const TBPVectorNoPOD& vec, size_t nPos, size_t nBegin, size_t nEnd)
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
		
			//::memcpy( m_pData + nPos, vec.m_pData + nBegin, ( nLen)*sizeof( TValue ) );
			copy(vec, nPos, 0, nLen);
			return true;
		}


		bool remove(size_t idx)
		{
			if(idx > m_nSize - 1)
				return false;
			
			for (size_t i = idx; i < m_nSize - 1; ++i )
			{
				m_pData[i] = m_pData[i + 1];
			}

			m_pData[m_nSize - 1].~TValue();
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
				//::memcpy(pNewdata, m_pData, m_nSize* sizeof(TValue));
				//m_pAlloc->free(m_pData);
				_move(m_pData, m_nSize, tmp, m_nCapacity);
				_clear(m_pData, m_nSize);
			}
			//else
			//	_init(tmp, m_nCapacity);
			m_pData = tmp;
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
		bool copy(const TBPVectorNoPOD& vec, size_t nPos, size_t nBegin, size_t nEnd)
		{
			size_t nLen = nEnd - nBegin;
			if(m_nSize + nPos + nLen >= m_nCapacity)
			{
				if(!reserve((2 * m_nCapacity) > (m_nSize + nPos + nLen) ? 2 * m_nCapacity :  2 * (m_nSize + nPos + nLen) ))
					return false;
			}
			//memcpy(m_pData + nPos, vec.m_pData + nBegin,  nLen * sizeof(TValue));

			TValue* pDstData = m_pData + nPos;
			TValue* pSrcData = vec.m_pData + nBegin;
			for (size_t i = 0; i < nLen; ++i)
			{
				if((i + nPos) < m_nSize)
					pDstData[i] = pSrcData[i];
				else
					new ((TValue*)&pDstData[i]) TValue(pSrcData[i]);
			}

			if((nPos + nLen) > m_nSize)
				m_nSize += (nPos + nLen) - m_nSize;
			return true;
		}
		bool mover(size_t nPos, size_t nCnt)
		{
			if(nCnt > m_nSize)
			{
				assert(nCnt < m_nSize);
				return false;
			}
			if( m_nSize + nCnt >= m_nCapacity)
			{
				if(!reserve((2 * m_nCapacity) > m_nSize + nCnt ? 2*m_nCapacity : 2 * (m_nSize + nCnt) ))
					return false;
			}
			//memmove(m_pData + nPos + nCnt, m_pData + nPos, (m_nSize)* sizeof(TValue));
			//TValue* pDstData = m_pData + m_nCapacity;
			//TValue* pSrcData = m_pData + nPos;
			//TValue* pDstData = m_pData + nNewSize -1;

			size_t nNewSize = m_nSize + nCnt;

			size_t nDstIdx = nNewSize - 1;
			size_t nSrcIdx = nDstIdx  -nCnt;

			size_t nLen = m_nSize - nPos + nCnt;
			
			for (size_t i = 0; i < nLen; ++i)
			{
				//m_pData[nDstIdx - i] = m_pData[nSrcIdx - i];
				if(nDstIdx - i < m_nSize)
					m_pData[nDstIdx - i]  = m_pData[nSrcIdx - i];
				else
				//	m_pData[nDstIdx - i].~TValue();

				new ((TValue*)&m_pData[nDstIdx - i] ) TValue(m_pData[nSrcIdx - i]);
			}

			m_nSize +=  nCnt;
			return true;
		}

		bool movel(size_t nPos, size_t nCnt)
		{
			//memmove(m_pData + nPos - nCnt,  m_pData + nPos, (m_nSize  - nCnt)* sizeof(TValue));

			TValue* pDstData = m_pData + nPos - nCnt;
			TValue* pSrcData = m_pData + nPos;
			for (size_t i = 0, sz = m_nSize  - nCnt; i < sz; ++i )
			{
				//pDstData[i].~TValue();
				//new ((TValue*)&pDstData[i]) TValue(pSrcData[i]);
				//pSrcData[i].~TValue();
				pDstData[i] = pSrcData[i];
			}

			for (size_t i = m_nSize - nCnt; i < m_nSize; ++i )
				m_pData[i].~TValue();
	
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

		void swap(TBPVectorNoPOD& vec)
		{
			//alloc must be compatible
		

			std::swap(vec.m_pData, m_pData);
			std::swap(vec.m_nCapacity, m_nCapacity);
			std::swap(vec.m_nSize, m_nSize);
		}
	private:
	
	};
}

#endif