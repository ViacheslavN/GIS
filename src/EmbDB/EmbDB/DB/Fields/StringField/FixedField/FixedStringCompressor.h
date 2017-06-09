#ifndef _EMBEDDED_DATABASE_FIXED_STRING_LEAF_COMPRESSOR_H_
#define _EMBEDDED_DATABASE_FIXED_STRING_LEAF_COMPRESSOR_H_
#include "BPVector.h"
#include "../StringVal.h"
#include "../StringCompressorParams.h"
#include "../../../BTreePlus/BaseNodeCompressor.h"
#include "../../BaseFieldEncoders.h"
#include "FixedStringACCompressor.h"
#include "FixedStringZLibCompressor.h"
#include "DB/GlobalParams.h"
namespace embDB
{

	template<class _TKey, class _Transaction>
	class TBPFixedStringLeafCompressor : public TBaseNodeCompressor<_TKey, CommonLib::CString, _Transaction, TSignedDiffEncoder64, TFixedStringZlibCompressor, StringFieldCompressorParams>
	{

		typedef STLAllocator<CommonLib::CString> TValueAlloc;
		typedef std::vector<CommonLib::CString, TValueAlloc> TValueMemSet;


		public:
			typedef TBaseLeafNodeDiffComp<_TKey, sFixedStringVal, _Transaction, TSignedDiffEncoder64, TFixedStringZlibCompressor, StringFieldCompressorParams>  TBase;

			TBPFixedStringLeafCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc,  typename TBase::TLeafCompressorParams *pParams) : TBase(nPageSize, pAlloc, pParams)
			{

				//this->m_ValueCompressor.init(pValueMemSet, pPageAlloc, pTran->getType());

			}

			template<typename _Transactions  >
			bool  init(TCompressorParams *pParams, _Transactions *pTran)
			{
				this->m_ValueCompressor.init(pParams->m_StringCoding; pTran->getType());
				return true;
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