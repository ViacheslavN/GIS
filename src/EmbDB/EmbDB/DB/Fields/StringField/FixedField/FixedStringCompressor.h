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
			typedef TBaseNodeCompressor<_TKey, CommonLib::CString, _Transaction, TSignedDiffEncoder64, TFixedStringZlibCompressor, StringFieldCompressorParams>  TBase;

			TBPFixedStringLeafCompressor(uint32 nPageSize, CommonLib::alloc_t *pAlloc,  typename TBase::TCompressorParams *pParams = nullptr) : TBase(nPageSize, pAlloc, pParams)
			{

				//this->m_ValueCompressor.init(pValueMemSet, pPageAlloc, pTran->getType());

			}

			template<typename _Transactions  >
			bool  init(TCompressorParams *pParams, _Transactions *pTran)
			{
				this->m_ValueEncoder.init(pParams, pTran->getType());
				return true;
			}

			~TBPFixedStringLeafCompressor()
			{				
				this->m_ValueEncoder.Free();
			}
			void Free()
			{
				this->m_ValueEncoder.Free();
			}

			template<typename _Transactions  >
			static typename TBase::TCompressorParams *LoadCompressorParams(_Transactions *pTran)
			{
				return new StringFieldCompressorParams();
			}


			uint32 GetSplitIndex(const TValueMemSet& vevStrings) const
			{

				uint32 nFreePage = this->m_nPageSize - this->m_KeyEncoder.GetCompressSize();

				return this->m_ValueEncoder.GetSplitIndex(nFreePage, vevStrings);
			}
			/*void SplitIn(uint32 nBegin, uint32 nEnd, TBPFixedStringLeafCompressor *pCompressor, bool bRecalcSrc = true, bool bRecalcDst = true)
			{

				uint32 nSize = nEnd- nBegin;

				this->m_nCount -= nSize;
				pCompressor->m_nCount += nSize;
			    this->recalcKey();
				pCompressor->recalcKey();

				this->m_ValueCompressor.SplitIn( nBegin,  nEnd, &pCompressor->m_ValueCompressor);

			}*/

			void PreSave(const TValueMemSet& vecStrings)
			{
				this->m_ValueEncoder.PreSave(vecStrings);
			}
			bool isHalfEmpty() const
			{
				/*uint32 nNoCompSize = this->m_nCount * (sizeof(TKey) + sizeof(uint16));
				for (uint32 i = 0, sz = this->m_pValueMemSet->size(); i< sz; ++i)
				{

					nNoCompSize += this->m_pValueMemSet->GetAt(i).m_nLen;
				}*/
				


				return this->m_ValueEncoder.GetNoComressSize() < (this->m_nPageSize - this->headSize())/2;
			}

	};

	//typedef  TBaseLeafNodeDiffComp<int64, sFixedStringVal, IDBTransaction, OIDCompressor, TFixedCompress, StringFieldCompressorParams> TBPFixedStringLeafCompress;
 
}

#endif