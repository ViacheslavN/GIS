#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_FIXED_STRING_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_FIXED_STRING_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "../../../BTreePlus/BPTreeLeafNodeMap.h"
 

#include "utils/alloc/PageAlloc.h"
#include "FixedStringCompressor.h"
#include "FixedStringZLibCompressor.h"
namespace embDB
{
	template<typename _TKey, typename _Transaction>
	class TFixedStringLeafNode : public  BPTreeLeafNodeMap<_TKey, CommonLib::CString, _Transaction, 
		TBPFixedStringLeafCompressor<_TKey, _Transaction> >
	{
	public:
		typedef   BPTreeLeafNodeMap<_TKey, CommonLib::CString, _Transaction,
			 TBPFixedStringLeafCompressor<_TKey, _Transaction> > TBase;

		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TKeyMemSet TKeyMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
		typedef typename TBase::TValueMemSet TValueMemSet;

		TFixedStringLeafNode( CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
		TBase(pAlloc, bMulti, nPageSize), m_pPageAlloc(NULL)
		{

		}
		~TFixedStringLeafNode()
		{
		
			this->m_Compressor.Free();
		}


		virtual bool init(TLeafCompressorParams *pParams , Transaction* pTransaction)
		{
			 
			this->m_Compressor.init(pParams, pTransaction);
			return true;
		}

		bool insertString(const TKey& key, const CommonLib::CString& sString)
		{

		}
		void SetPageAlloc(CommonLib::alloc_t *pPageAlloc)
		{
			assert(pPageAlloc != NULL);
			m_pPageAlloc = pPageAlloc;
		}
		int SplitIn(TFixedStringLeafNode *pNode, TKey* pSplitKey)
		{
			TCompressor& pNewNodeComp = pNode->m_Compressor;
			int nSplitIndex = this->m_Compressor.GetSplitIndex(this->m_ValueMemSet);
			assert(nSplitIndex != 0);
			uint32 nSize = (uint32)this->m_ValueMemSet.size();
			
			this->SplitInVec(this->m_ValueMemSet, pNode->m_ValueMemSet, nSplitIndex, nSize - nSplitIndex);
			this->SplitInVec(this->m_KeyMemSet, pNode->m_KeyMemSet, nSplitIndex, nSize - nSplitIndex);
			//this->m_Compressor.SplitIn(nSplitIndex, nSize, pNewNodeComp);

			this->m_ValueMemSet.resize(nSplitIndex);
			this->m_KeyMemSet.resize(nSplitIndex);


			*pSplitKey = pNode->m_KeyMemSet[0];

			this->m_Compressor.recalc(m_KeyMemSet, m_ValueMemSet);
			pNewNodeComp.recalc(pNode->m_KeyMemSet, pNode->m_ValueMemSet);
			return nSplitIndex;
		
		}

		int  SplitIn(TFixedStringLeafNode *pLeftNode, TFixedStringLeafNode *pRightNode, TKey* pSplitKey)
		{

			int nSplitIndex = this->m_Compressor.GetSplitIndex(m_ValueMemSet);			
			return TBase::SplitIn(pLeftNode, pRightNode, pSplitKey, nSplitIndex);
		}

		virtual  void PreSave()
		{
			this->m_Compressor.PreSave(m_ValueMemSet);
		}
	public:
		CommonLib::alloc_t *m_pPageAlloc;


	};

}
#endif