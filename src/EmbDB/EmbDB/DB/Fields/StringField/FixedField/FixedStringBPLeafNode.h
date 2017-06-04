#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_FIXED_STRING_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_FIXED_STRING_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeMapv2.h"
#include "../StringVal.h"
#include "FixedStringLeafCompressor.h"
#include "utils/alloc/PageAlloc.h"
#include "FixedStringCompressor.h"
#include "FixedStringZLibCompressor.h"
namespace embDB
{
	template<typename _TKey, typename _Transaction>
	class TFixedStringLeafNode : public  BPTreeLeafNodeMapv2<_TKey, sFixedStringVal, _Transaction, 
		TBPFixedStringLeafCompressor<_TKey, _Transaction> /*BPFixedStringLeafNodeCompressor<_TKey, _Transaction>*/ >
	{
	public:
		typedef   BPTreeLeafNodeMapv2<_TKey, sFixedStringVal, _Transaction, 
			 TBPFixedStringLeafCompressor<_TKey, _Transaction> /*BPFixedStringLeafNodeCompressor<_TKey, _Transaction>*/ > TBase;
		typedef sFixedStringVal TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
		typedef TBPVector<sFixedStringVal>		TValueMemSet;

		TFixedStringLeafNode( CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
		TBase(pAlloc, bMulti, nPageSize), m_pPageAlloc(NULL)
		{

		}
		~TFixedStringLeafNode()
		{
			if(this->m_pCompressor)
				this->m_pCompressor->Free();
		}


		virtual bool init(TLeafCompressorParams *pParams , Transaction* pTransaction)
		{
			assert(!this->m_pCompressor);
			this->m_pCompressor = new TCompressor(this->m_nPageSize - 2* sizeof(TLink), pTransaction, m_pAlloc, pParams, &this->m_leafKeyMemSet, &this->m_leafValueMemSet, m_pPageAlloc);
			return true;
		}

		bool insertString(const TKey& key, const CommonLib::CString& sString)
		{

		}
		void SetPageAlloc(CommonLib::alloc_t *pPageAlloc)
		{
			assert(pPageAlloc != NULL);
			assert(this->m_pCompressor == NULL);
			m_pPageAlloc = pPageAlloc;
		}
		int SplitIn(TFixedStringLeafNode *pNode, TKey* pSplitKey)
		{
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			
			//uint32 nFreeSize = m_nPageSize - 2 *sizeof(TLink);



			int nSplitIndex = this->m_pCompressor->GetSplitIndex();
			assert(nSplitIndex != 0);
			uint32 nSize = this->m_leafValueMemSet.size();
			
			this->SplitInVec(this->m_leafValueMemSet, pNode->m_leafValueMemSet, (TValue*)NULL, nSplitIndex);
			this->SplitInVec(this->m_leafKeyMemSet, pNode->m_leafKeyMemSet, pSplitKey, nSplitIndex);
			this->m_pCompressor->SplitIn(nSplitIndex, nSize, pNewNodeComp);
		

			return nSplitIndex;
		
		}

		int  SplitIn(TFixedStringLeafNode *pLeftNode, TFixedStringLeafNode *pRightNode, TKey* pSplitKey)
		{

			int nSplitIndex = this->m_pCompressor->GetSplitIndex();
			
			
			return TBase::SplitIn(pLeftNode, pRightNode, pSplitKey, nSplitIndex);
		}

		virtual  void PreSave()
		{
			this->m_pCompressor->PreSave();
		}
	public:
		CommonLib::alloc_t *m_pPageAlloc;


	};

}
#endif