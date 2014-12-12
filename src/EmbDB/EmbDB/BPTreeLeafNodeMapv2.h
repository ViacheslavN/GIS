#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeSetv2.h"
namespace embDB
{
	template<typename _TKey, typename _TValue,  /* typename _TComp,*/
	class _Transaction, class _TCompressor>
	class BPTreeLeafNodeMapv2 : public  BPTreeLeafNodeSetv2Base<_TKey,/* _TComp,*/ _Transaction, _TCompressor>
	{
	public:
		typedef BPTreeLeafNodeSetv2Base<_TKey, /*_TComp, */_Transaction, _TCompressor> TBase;
		typedef _TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
//		typedef typename TBase::TComporator	 TComporator;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef  TBPVector<TValue> TValueMemSet;

		BPTreeLeafNodeMapv2( CommonLib::alloc_t *pAlloc, bool bMulti) :
			TBase(pAlloc, bMulti), m_leafValueMemSet(pAlloc)
		{

		}
		~BPTreeLeafNodeMapv2()
		{
 
		}
		virtual  bool Save(	CommonLib::FxMemoryWriteStream& stream) 
		{
			stream.write(m_nNext);
			stream.write(m_nPrev);
			return m_pCompressor->Write(m_leafKeyMemSet, m_leafValueMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(m_nNext);
			stream.read(m_nPrev); 
			return m_pCompressor->Load(m_leafKeyMemSet,m_leafValueMemSet, stream);
		}
		template<class TComp>
		bool insert(TComp& comp, const TKey& key, const TValue& value)
		{
			int32 nIndex = 0;
			bool bRet =  insertImp(comp, key, nIndex);
			if(!bRet)
				return false;

			m_leafValueMemSet.insert(value, nIndex);
			return  m_pCompressor->insert(key, value);

		}
		int SplitIn(BPTreeLeafNodeMapv2 *pNode, TKey* pSplitKey)
		{
 			TCompressor* pNewNodeComp = pNode->m_pCompressor;

			int nSplitIndex = SplitInVec(m_leafKeyMemSet, pNode->m_leafKeyMemSet, pSplitKey);
			SplitInVec(m_leafValueMemSet, pNode->m_leafValueMemSet, (TValue*)NULL);

			recalc();
			pNode->recalc();
			return nSplitIndex;
		}
		
		const TValue& value(uint32 nIndex) const
		{
			return m_leafValueMemSet[nIndex];
		}
		TValue& value(uint32 nIndex)
		{
			return m_leafValueMemSet[nIndex];
		}

		bool UnionWith(BPTreeLeafNodeMapv2* pNode, bool bLeft, int *nCheckIndex = 0)
		{
			m_pCompressor->add(pNode->m_leafKeyMemSet, pNode->m_leafValueMemSet);
			UnionVec(m_leafKeyMemSet, pNode->m_leafKeyMemSet, bLeft, nCheckIndex);
			UnionVec(m_leafValueMemSet, pNode->m_leafValueMemSet, bLeft);
			return true;
		}
		bool AlignmentOf(BPTreeLeafNodeMapv2* pNode, bool bFromLeft)
		{
			if(!AlignmentOfVec(m_leafKeyMemSet, pNode->m_leafKeyMemSet, bFromLeft))
				return false;
						
			AlignmentOfVec(m_leafValueMemSet, pNode->m_leafValueMemSet, bFromLeft);

			pNode->recalc();
			recalc();

			return true;
		}
		void recalc()
		{
			m_pCompressor->recalc(m_leafKeyMemSet, m_leafValueMemSet);
		}
	public:
		TValueMemSet m_leafValueMemSet;
	
	};

}
#endif