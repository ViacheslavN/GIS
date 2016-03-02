#ifndef _EMBEDDED_DATABASE_FIELD_BASE_LEAF_DIFF_NODE_2_COMPRESS_H_
#define _EMBEDDED_DATABASE_FIELD_BASE_LEAF_DIFF_NODE_2_COMPRESS_H_
#include "BaseLeafNodeComp.h"

namespace embDB
{

	template<class _TKey, class _TValue, class _Transaction = IDBTransaction, class _TKeyCommpressor = TEmptyValueCompress<_TKey>, class _TValueCompressor  = TEmptyValueCompress<_TValue> ,
	class _TLeafCompressorParams = CompressorParamsBaseImp>
	class TBaseLeafNodeDiffComp2 : public TBaseLeafNodeComp<_TKey, _TValue, _Transaction, _TKeyCommpressor, _TValueCompressor, _TLeafCompressorParams>
	{
	public:

		typedef TBaseLeafNodeComp<_TKey, _TValue, _Transaction, _TKeyCommpressor, _TValueCompressor, _TLeafCompressorParams> TBase;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TKeyCommpressor TKeyCommpressor;
		typedef typename TBase::TValueCompressor TValueCompressor;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TKeyMemSet TKeyMemSet;
		typedef typename TBase::TValueMemSet TValueMemSet;

		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;


		TBaseLeafNodeDiffComp2(uint32 nPageSize, Transaction *pTran, CommonLib::alloc_t *pAlloc = 0, TLeafCompressorParams *pParams = NULL,
			TKeyMemSet *pKeyMemset= NULL, TValueMemSet *pValueMemSet = NULL) : TBase(nPageSize, pTran, pAlloc, pParams, pKeyMemset, pValueMemSet)
		{



		}


		virtual ~TBaseLeafNodeDiffComp2(){}
		virtual bool add(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
		{

			if(!vecKeys.empty())
			{
				m_KeyCompressor.AddDiffSymbol(vecKeys[0] - (*m_pKeyMemSet)[m_nCount - 1]);
				m_ValueCompressor.AddDiffSymbol(vecValues[0] - (*m_pValueMemSet)[m_nCount - 1]);
				m_nCount++;
			}


			for (uint32 i = 1, sz = vecKeys.size(); i < sz; ++i)
			{
				m_KeyCompressor.AddDiffSymbol(vecKeys[i] - vecKeys[i - 1]);
				m_ValueCompressor.AddDiffSymbol(vecValues[i] - vecValues[i - 1]);

				m_nCount++;
			}

			return true;
		}
		virtual bool recalc(const TKeyMemSet& vecKeys, const TValueMemSet& vecValues)
		{
			m_nCount = vecKeys.size();
			m_KeyCompressor.clear();
			m_ValueCompressor.clear();
			for (uint32 i = 1, sz = vecKeys.size(); i < sz; 	++i)
			{
				m_KeyCompressor.AddDiffSymbol(vecKeys[i] - vecKeys[i - 1]); 
				m_ValueCompressor.AddDiffSymbol(vecValues[i] - vecValues[i - 1]);
			}

			return true;
		}		
		virtual void recalc()
		{
			m_nCount = m_pKeyMemSet->size();
			m_KeyCompressor.clear();
			m_ValueCompressor.clear();
			for (size_t i = 1, sz = m_pKeyMemSet->size(); i < sz; 	++i)
			{
				m_KeyCompressor.AddDiffSymbol((*m_pKeyMemSet)[i] - (*m_pKeyMemSet)[i - 1]); 
				m_ValueCompressor.AddDiffSymbol((*m_pValueMemSet)[i] - (*m_pValueMemSet)[i - 1]); 
			}


		}

	};
}

#endif