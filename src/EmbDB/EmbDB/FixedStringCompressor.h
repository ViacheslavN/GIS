#ifndef _EMBEDDED_DATABASE_FIXED_STRING_LEAF_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_LEAF_COMPRESSOR_H_
#include "BPVector.h"
#include "StringVal.h"
#include "StringCompressorParams.h"
#include "BaseLeafNodeCompDiff.h"
#include "FixedStringACCompressor.h"
#include "FixedStringZLibCompressor.h"
namespace embDB
{
	class TFixedCompress
	{
	public:

 
		typedef  TBPVector<sFixedStringVal> TValueMemSet;
		TFixedCompress(CommonLib::alloc_t *pAlloc, StringFieldCompressorParams *pParams) : m_nCount(0), m_pAlloc(pAlloc), m_pParams(pParams), m_nStringDataSize(0)
		{

		}
		~TFixedCompress()
		{
			Free();
		}

		void init(TValueMemSet* pVecValues)
		{
			m_pValueMemset = pVecValues;
		}
		void AddSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& value, const TValueMemSet& vecValues)
		{
			m_nCount++;
			m_nStringDataSize += value.m_nLen;
		}
		void RemoveSymbol(uint32 nSize,  int nIndex, const sFixedStringVal& value, const TValueMemSet& vecValues)
		{
			m_nCount--;
			m_nStringDataSize -= value.m_nLen;
		}



		uint32 GetCompressSize() const
		{
			return m_nStringDataSize;
		}

		bool compress( const TValueMemSet& vecValues, CommonLib::IWriteStream *pStream)
		{
			assert(m_nCount == vecValues.size());

			for (uint32 i = 0, sz = vecValues.size(); i < sz; ++i)
			{
				pStream->write(vecValues[i].m_pBuf, vecValues[i].m_nLen);
			}
			return true;
		}
		bool decompress(uint32 nSize, TValueMemSet& vecValues, CommonLib::FxMemoryReadStream *pStream)
		{
			m_nStringDataSize = 0;
			sFixedStringVal sString;
			for (uint32 i = 0, sz = nSize; i < sz; ++i)
			{
				

				sString.m_nLen  = (uint32)strlen((const char*)pStream->buffer() + pStream->pos()) + 1;
				m_nStringDataSize += sString.m_nLen;
				sString.m_pBuf = (byte*)m_pAlloc->alloc(sString.m_nLen);
				memcpy(sString.m_pBuf, pStream->buffer() + pStream->pos(), sString.m_nLen);
				
				pStream->seek(sString.m_nLen, CommonLib::soFromCurrent);
				vecValues.push_back(sString);
			}

			m_nCount = nSize;
			return true;
		}
		void clear()
		{
			m_nCount = 0;
			m_nStringDataSize = 0;

		}

		uint32 count() const
		{
			return m_nCount;
		}
		void Free()
		{
			for (uint32 i = 0; i < m_pValueMemset->size(); ++i )
			{
				sFixedStringVal& val = (*m_pValueMemset)[i];
				m_pAlloc->free(val.m_pBuf);
			}
		}
	private:
		uint32 m_nCount;
		CommonLib::alloc_t* m_pAlloc;
		StringFieldCompressorParams *m_pParams;
		uint32 m_nStringDataSize;
		TValueMemSet* m_pValueMemset;
	};


	template<class _TKey, class _Transaction>
	class TBPFixedStringLeafCompressor : public TBaseLeafNodeDiffComp<_TKey, sFixedStringVal, _Transaction, OIDCompressor, TFixedStringZlibCompressor, StringFieldCompressorParams> 
	{
		public:
			typedef TBaseLeafNodeDiffComp<_TKey, sFixedStringVal, _Transaction, OIDCompressor, TFixedStringZlibCompressor, StringFieldCompressorParams>  TBase;

			TBPFixedStringLeafCompressor(uint32 nPageSize, _Transaction *pTran, CommonLib::alloc_t *pAlloc,  typename TBase::TLeafCompressorParams *pParams,
				typename TBase::TKeyMemSet *pKeyMemset, typename TBase::TValueMemSet *pValueMemSet, CommonLib::alloc_t *pPageAlloc = NULL) : TBase(nPageSize, pTran, pAlloc, pParams, pKeyMemset, pValueMemSet)
			{

				this->m_ValueCompressor.init(pValueMemSet, pPageAlloc, pTran->getType());

			}
			~TBPFixedStringLeafCompressor()
			{				
				this->m_ValueCompressor.Free();
			}
			void Free()
			{
				this->m_ValueCompressor.Free();
			}

			template<typename _Transactions  >
			static typename TBase::TLeafCompressorParams *LoadCompressorParams(_Transactions *pTran)
			{
				return new StringFieldCompressorParams();
			}


			uint32 GetSplitIndex() const
			{

				uint32 nFreePage = this->m_nPageSize - this->m_KeyCompressor.GetCompressSize();

				return this->m_ValueCompressor.GetSplitIndex(nFreePage);
			}
			void SplitIn(uint32 nBegin, uint32 nEnd, TBPFixedStringLeafCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
			{

				uint32 nSize = nEnd- nBegin;

				this->m_nCount -= nSize;
				pCompressor->m_nCount += nSize;
			    this->recalcKey();
				pCompressor->recalcKey();

				this->m_ValueCompressor.SplitIn( nBegin,  nEnd, &pCompressor->m_ValueCompressor);

			}

			void PreSave()
			{
				this->m_ValueCompressor.PreSave();
			}
			bool isHalfEmpty() const
			{
				uint32 nNoCompSize = this->m_nCount * (sizeof(TKey) + sizeof(uint16));
				for (uint32 i = 0, sz = this->m_pValueMemSet->size(); i< sz; ++i)
				{

					nNoCompSize += this->m_pValueMemSet->GetAt(i).m_nLen;
				}
				


				return nNoCompSize < (this->m_nPageSize - this->headSize())/2;
			}

	};

	//typedef  TBaseLeafNodeDiffComp<int64, sFixedStringVal, IDBTransaction, OIDCompressor, TFixedCompress, StringFieldCompressorParams> TBPFixedStringLeafCompress;
 
}

#endif