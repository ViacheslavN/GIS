#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeSetv2.h"
namespace embDB
{
	template<typename _TKey, typename _TValue,  /* typename _TComp,*/
	class _Transaction, class _TCompressor,
	class _TKeyMemSet = TBPVector<_TKey>,
	class _TValueMemSet = TBPVector<_TValue> >
	class BPTreeLeafNodeMapv2 : public  BPTreeLeafNodeSetv2Base<_TKey,/* _TComp,*/ _Transaction, _TCompressor, _TKeyMemSet>
	{
	public:
		typedef BPTreeLeafNodeSetv2Base<_TKey, /*_TComp, */_Transaction, _TCompressor, _TKeyMemSet> TBase;
		typedef _TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef _TValueMemSet TValueMemSet;

		BPTreeLeafNodeMapv2( CommonLib::alloc_t *pAlloc, bool bMulti) :
			TBase(pAlloc, bMulti), m_leafValueMemSet(pAlloc)
		{

		}
		~BPTreeLeafNodeMapv2()
		{
 
		}


		virtual bool init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL )
		{
			assert(!m_pCompressor);
			m_pCompressor = new TCompressor(pTransaction, m_pAlloc, pParams, &m_leafKeyMemSet, &m_leafValueMemSet);
			return true;
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
		int insert(TComp& comp, const TKey& key, const TValue& value, int nInsertInIndex = -1)
		{
			int32 nIndex = 0;
			bool bRet =  insertImp(comp, key, nIndex, nInsertInIndex);
			if(!bRet)
				return -1;

			m_leafValueMemSet.insert(value, nIndex);
			if(!m_pCompressor->insert(nIndex, key, value))
				return -1;
			return nIndex;
		}
		int SplitIn(BPTreeLeafNodeMapv2 *pNode, TKey* pSplitKey)
		{
 			TCompressor* pNewNodeComp = pNode->m_pCompressor;

			int nSplitIndex = SplitInVec(m_leafKeyMemSet, pNode->m_leafKeyMemSet, pSplitKey);
			SplitInVec(m_leafValueMemSet, pNode->m_leafValueMemSet, (TValue*)NULL);
			m_pCompressor->SplitIn(0, nSplitIndex, pNewNodeComp);
			//recalc();
			//pNode->recalc();
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

		bool removeByIndex(int32 nIndex)
		{
			m_pCompressor->remove(nIndex, m_leafKeyMemSet[nIndex], m_leafValueMemSet[nIndex]);
			bool bRet = m_leafKeyMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				return false;
			}
			 bRet = m_leafValueMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				return false;
			}
			return true;
		}
	public:
		TValueMemSet m_leafValueMemSet;
	
	};

}
#endif