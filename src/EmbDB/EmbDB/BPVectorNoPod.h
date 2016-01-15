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
			if(this->m_pData)
			{
				_clear(this->m_pData, this->m_nSize);
				this->m_pData = NULL;
			}
			this->m_nSize = 0;
			this->m_nCapacity = 0;
		}
		size_t size() const {return this->m_nSize;}

		void _clear(TValue*  pData, size_t nSize)
		{
			for (size_t i = 0; i < nSize; ++i)
			{
				pData[i].~TValue();
				
			}
			this->m_pAlloc->free(pData);
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
			if(this->m_pData)
			{
				 
				this->m_pAlloc->free(this->m_pData);

			}
			this->m_nSize = 0;
			this->m_nCapacity = 0;
		}
		bool empty()
		{
			return this->m_nSize == 0;
		}
		bool push_back(const TValue& val)
		{
			if(this->m_nSize == this->m_nCapacity)
			{
				this->m_nCapacity = 2*this->m_nSize;
				if(!this->m_nCapacity)
					this->m_nCapacity = 2;

				TValue*  pNewdata = (TValue*)this->m_pAlloc->alloc(this->m_nCapacity * sizeof(TValue));
				if(!pNewdata)
					return false;
				if(this->m_pData)
				{
					//::memcpy(pNewdata, m_pData, m_nSize* sizeof(TValue));
					//m_pAlloc->free(m_pData);
					_move(this->m_pData, this->m_nSize, pNewdata, this->m_nCapacity);
					_clear(this->m_pData, this->m_nSize);
				}
			//	else
			//		_init(pNewdata, m_nCapacity);
				this->m_pData = pNewdata;
		

			}
			new ((TValue*)&this->m_pData[this->m_nSize]) TValue(val);
			//m_pData[m_nSize] = val;
			this->m_nSize++;
			return true;
		}

		bool push_back(const TBPVectorNoPOD& vec)
		{
			return copy(vec, this->m_nSize, 0, vec.size());
		}
		bool insert(const TValue& value, size_t idx)
		{
			if(idx > this->m_nSize)
				return false;

			if((this->m_nSize + 1) > this->m_nCapacity)
			{
				if(!this->m_nCapacity)
					this->m_nCapacity = 1;
				reserve(2 * this->m_nCapacity);
			}

			if(this->m_nSize == idx)
				return push_back(value);

			//::memmove( m_pData + idx + 1, m_pData + idx, ( m_nSize - idx)*sizeof( TValue ) );
			mover(idx, 1);
			this->m_pData[idx] = value;
			this->m_nSize++;
			return true;
		}


		bool insert(const TBPVectorNoPOD& vec, size_t nPos, size_t nBegin, size_t nEnd)
		{		 
			size_t nLen = nEnd - nBegin;
			if(this->m_nSize + nPos + nLen >= this->m_nCapacity)
			{
				if(!reserve((2 * this->m_nCapacity) > (this->m_nSize + nPos + nLen) ? 2 * this->m_nCapacity :  2 * (this->m_nSize + nPos + nLen) ))
					return false;
			}

			if(this->m_nSize == nPos)
				return push_back(vec);
			if(nPos < this->m_nSize)
				mover(nPos, nLen);
		
			//::memcpy( m_pData + nPos, vec.m_pData + nBegin, ( nLen)*sizeof( TValue ) );
			copy(vec, nPos, 0, nLen);
			return true;
		}


		bool remove(size_t idx)
		{
			if(idx > this->m_nSize - 1)
				return false;
			
			for (size_t i = idx; i < this->m_nSize - 1; ++i )
			{
				this->m_pData[i] = this->m_pData[i + 1];
			}

			this->m_pData[this->m_nSize - 1].~TValue();
			this->m_nSize--;
			return true;
		}

		bool reserve(size_t nSize)
		{
			if(this->m_nCapacity > nSize)
				return true;

			this->m_nCapacity = nSize;
			TValue* tmp = (TValue*)this->m_pAlloc->alloc(this->m_nCapacity * sizeof(TValue));
			if(!tmp)
				return false;

			if(this->m_pData)
			{
				//::memcpy(pNewdata, m_pData, m_nSize* sizeof(TValue));
				//m_pAlloc->free(m_pData);
				_move(this->m_pData, this->m_nSize, tmp, this->m_nCapacity);
				_clear(this->m_pData, this->m_nSize);
			}
			//else
			//	_init(tmp, m_nCapacity);
			this->m_pData = tmp;
			return true;
		}
		bool resize(size_t nSize)
		{
			if(this->m_nSize > nSize)
			{
				this->m_nSize = nSize;
				return true;
			}
			if(!reserve(nSize))
				return false;
			this->m_nSize = nSize;
			return true;
		}
		bool copy(const TBPVectorNoPOD& vec, size_t nPos, size_t nBegin, size_t nEnd)
		{
			size_t nLen = nEnd - nBegin;
			if(this->m_nSize + nPos + nLen >= this->m_nCapacity)
			{
				if(!reserve((2 * this->m_nCapacity) > (this->m_nSize + nPos + nLen) ? 2 * this->m_nCapacity :  2 * (this->m_nSize + nPos + nLen) ))
					return false;
			}
			//memcpy(m_pData + nPos, vec.m_pData + nBegin,  nLen * sizeof(TValue));

			TValue* pDstData = this->m_pData + nPos;
			TValue* pSrcData = vec.m_pData + nBegin;
			for (size_t i = 0; i < nLen; ++i)
			{
				if((i + nPos) < this->m_nSize)
					pDstData[i] = pSrcData[i];
				else
					new ((TValue*)&pDstData[i]) TValue(pSrcData[i]);
			}

			if((nPos + nLen) > this->m_nSize)
				this->m_nSize += (nPos + nLen) - this->m_nSize;
			return true;
		}
		bool mover(size_t nPos, size_t nCnt)
		{
			if(nCnt > this->m_nSize)
			{
				assert(nCnt < this->m_nSize);
				return false;
			}
			if( this->m_nSize + nCnt >= this->m_nCapacity)
			{
				if(!reserve((2 * this->m_nCapacity) > this->m_nSize + nCnt ? 2*this->m_nCapacity : 2 * (this->m_nSize + nCnt) ))
					return false;
			}
			//memmove(m_pData + nPos + nCnt, m_pData + nPos, (m_nSize)* sizeof(TValue));
			//TValue* pDstData = m_pData + m_nCapacity;
			//TValue* pSrcData = m_pData + nPos;
			//TValue* pDstData = m_pData + nNewSize -1;

			size_t nNewSize = this->m_nSize + nCnt;

			size_t nDstIdx = nNewSize - 1;
			size_t nSrcIdx = nDstIdx  -nCnt;

			size_t nLen = this->m_nSize - nPos + nCnt;
			
			for (size_t i = 0; i < nLen; ++i)
			{
				//m_pData[nDstIdx - i] = m_pData[nSrcIdx - i];
				if(nDstIdx - i < this->m_nSize)
					this->m_pData[nDstIdx - i]  = this->m_pData[nSrcIdx - i];
				else
				//	m_pData[nDstIdx - i].~TValue();

				new ((TValue*)&this->m_pData[nDstIdx - i] ) TValue(this->m_pData[nSrcIdx - i]);
			}

			this->m_nSize +=  nCnt;
			return true;
		}

		bool movel(size_t nPos, size_t nCnt)
		{
			//memmove(m_pData + nPos - nCnt,  m_pData + nPos, (m_nSize  - nCnt)* sizeof(TValue));

			TValue* pDstData = this->m_pData + nPos - nCnt;
			TValue* pSrcData = this->m_pData + nPos;
			for (size_t i = 0, sz = this->m_nSize  - nCnt; i < sz; ++i )
			{
				//pDstData[i].~TValue();
				//new ((TValue*)&pDstData[i]) TValue(pSrcData[i]);
				//pSrcData[i].~TValue();
				pDstData[i] = pSrcData[i];
			}

			for (size_t i = this->m_nSize - nCnt; i < this->m_nSize; ++i )
				this->m_pData[i].~TValue();
	
			this->m_nSize -= nCnt;
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

			if(this->m_nSize == 0)
				return -1;

			int32 nFirst = nLeft == -1 ? 0 : nLeft;
			int32 nIndex = 0;
			int32 nStep = 0;
			int32 nCount = nRight == -1 ? this->m_nSize : nRight;
			while (nCount > 0)
			{
				nIndex = nFirst; 
				nStep = nCount >> 1;
				nIndex += nStep;
				if(comp.LE(this->m_pData[ nIndex ], val))
				{
					nFirst = ++nIndex;
					nCount -= (nStep + 1);
				} 
				else nCount = nStep;
			}
			if(nFirst < (int32)this->m_nSize && comp.EQ(val, this->m_pData[ nFirst ]))
				nType = FIND_KEY;
			return nFirst;
		}

		template<class _TComp >
		int32 upper_bound(const TValue& val, const _TComp& comp)  
		{

			if(this->m_nSize == 0)
				return -1;

			int32 nFirst = 0;
			int32 nIndex = 0;
			int32 nStep = 0;
			int32 nCount = this->m_nSize;
			while (nCount > 0)
			{
				nIndex = nFirst; 
				nStep = nCount >> 1;
				nIndex += nStep;
				if(!comp.LE(val, this->m_pData[ nIndex ]))
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
			assert(nIndex < this->m_nSize);
			return this->m_pData[nIndex];
		}
		TValue& operator [](size_t nIndex) 
		{
			assert(nIndex < this->m_nSize);
			return this->m_pData[nIndex];
		}

		TValue& back() 
		{
			assert(this->m_nSize);
			return this->m_pData[this->m_nSize - 1];
		}

		const TValue& back() const
		{
			assert(this->m_nSize);
			return this->m_pData[this->m_nSize - 1];
		}


		TValue& front() 
		{
			assert(this->this->m_nSize);
			return this->m_pData[0];
		}

		const TValue& front() const
		{
			assert(this->m_nSize);
			return this->m_pData[0];
		}

		void swap(TBPVectorNoPOD& vec)
		{
			//alloc must be compatible
		

			std::swap(vec.m_pData, this->m_pData);
			std::swap(vec.m_nCapacity, this->m_nCapacity);
			std::swap(vec.m_nSize, this->m_nSize);
		}
	private:
	
	};
}

#endif