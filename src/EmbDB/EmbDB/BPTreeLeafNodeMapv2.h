#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_MAP_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeSetv2.h"
namespace embDB
{
	template<typename _TKey, typename _TValue,
	class _Transaction, class _TCompressor,
	class _TKeyMemSet = TBPVector<_TKey>,
	class _TValueMemSet = TBPVector<_TValue> >
	class BPTreeLeafNodeMapv2 : public  BPTreeLeafNodeSetv2Base<_TKey, _Transaction, _TCompressor, _TKeyMemSet>
	{
	public:
		typedef BPTreeLeafNodeSetv2Base<_TKey, _Transaction, _TCompressor, _TKeyMemSet> TBase;
		typedef _TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
		typedef _TValueMemSet TValueMemSet;

		BPTreeLeafNodeMapv2( CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			TBase(pAlloc, bMulti, nPageSize), m_leafValueMemSet(pAlloc)
		{

		}
		~BPTreeLeafNodeMapv2()
		{
 
		}


		virtual bool init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL )
		{
			assert(!this->m_pCompressor);
			this->m_pCompressor = new TCompressor(this->m_nPageSize - 2 *sizeof(TLink), pTransaction, this-> m_pAlloc, pParams, &this->m_leafKeyMemSet, &this->m_leafValueMemSet);
			return true;
		}
		
		virtual  bool Save(	CommonLib::FxMemoryWriteStream& stream) 
		{
			stream.write(this->m_nNext);
			stream.write(this->m_nPrev);
			return this->m_pCompressor->Write(this->m_leafKeyMemSet, this->m_leafValueMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(this->m_nNext);
			stream.read(this->m_nPrev); 
			return this->m_pCompressor->Load(this->m_leafKeyMemSet, this->m_leafValueMemSet, stream);
		}
		template<class TComp>
		int insert(TComp& comp, const TKey& key, const TValue& value, int nInsertInIndex = -1)
		{
			int32 nIndex = 0;
			bool bRet =  this->insertImp(comp, key, nIndex, nInsertInIndex);
			if(!bRet)
				return -1;

			this->m_leafValueMemSet.insert(value, nIndex);
			if(!this->m_pCompressor->insert(nIndex, key, value))
				return -1;
			return nIndex;
		}


	/*	int GetSplitIndex()
		{
			if(!this->m_bMinSplit)
				return  this->m_leafKeyMemSet.size()/2;


			int nIndex = this->m_leafKeyMemSet.size() - 1;

			while(true)
			{


				this->m_pCompressor->remove(nIndex, this->m_leafKeyMemSet[nIndex], this->m_leafValueMemSet[nIndex]);
				if(!isNeedSplit())
					break;
				--nIndex;
			}

			assert(nIndex > 0);
			return nIndex;
		}*/

		int SplitIn(BPTreeLeafNodeMapv2 *pNode, TKey* pSplitKey)
		{
 			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			if(this->m_bMinSplit)
			{

				int nSplitIndex = this->m_leafKeyMemSet.size() - 1;
				
				while(true)
				{
				
					this->m_pCompressor->remove(nSplitIndex, this->m_leafKeyMemSet[nSplitIndex], this->m_leafValueMemSet[nSplitIndex]);
					if(!this->isNeedSplit())
						break;
					--nSplitIndex;
				}

				assert(nSplitIndex > 0);
				int nCount = this->m_leafKeyMemSet.size() - nSplitIndex;
				if(nCount == 1)
				{
					this->SplitOne(this->m_leafKeyMemSet, pNode->m_leafKeyMemSet, pSplitKey);
					this->SplitOne(this->m_leafValueMemSet, pNode->m_leafValueMemSet, (TValue*)NULL);
					pNewNodeComp->insert(0, pNode->m_leafKeyMemSet[0], pNode->m_leafValueMemSet[0]);
				}
				else
				{
					uint32 nSize = this->m_leafValueMemSet.size();
					this->SplitInVec(this->m_leafKeyMemSet, pNode->m_leafKeyMemSet, pSplitKey, nSplitIndex);
					this->SplitInVec(this->m_leafValueMemSet, pNode->m_leafValueMemSet, (TValue*)NULL, nSplitIndex);
					this->m_pCompressor->SplitIn(nSplitIndex, nSize, pNewNodeComp, false, true);
				}
				return nSplitIndex;
			}
			else
			{

				int nSplitIndex = this->SplitInVec(this->m_leafKeyMemSet, pNode->m_leafKeyMemSet, pSplitKey);
				this->SplitInVec(this->m_leafValueMemSet, pNode->m_leafValueMemSet, (TValue*)NULL);
				this->m_pCompressor->SplitIn(0, nSplitIndex, pNewNodeComp);
				return nSplitIndex;
			}
		}


		int  SplitIn(BPTreeLeafNodeMapv2 *pLeftNode, BPTreeLeafNodeMapv2 *pRightNode, TKey* pSplitKey, int nSplitIndex = -1)
		{

			TLeafMemSet& leftKeyMemSet = pLeftNode->m_leafKeyMemSet;
			TValueMemSet& leftValueMemSet = pLeftNode->m_leafValueMemSet;
			TCompressor* pleftNodeComp = pLeftNode->m_pCompressor;

			TLeafMemSet& rightKeyMemSet = pRightNode->m_leafKeyMemSet;
			TValueMemSet& rightValueMemSet = pRightNode->m_leafValueMemSet;
			TCompressor* pRightNodeComp = pRightNode->m_pCompressor;

			if(nSplitIndex == -1)
				nSplitIndex = this->m_leafKeyMemSet.size()/2;

			if(pSplitKey)
				*pSplitKey = this->m_leafKeyMemSet[nSplitIndex];

			this->SplitInVec(this->m_leafKeyMemSet, leftKeyMemSet, 0, nSplitIndex);
			this->SplitInVec(this->m_leafValueMemSet, leftValueMemSet, 0, nSplitIndex);

			this->SplitInVec(this->m_leafKeyMemSet, rightKeyMemSet, nSplitIndex, this->m_leafKeyMemSet.size());
			this->SplitInVec(this->m_leafValueMemSet, rightValueMemSet, nSplitIndex, this->m_leafValueMemSet.size());

			pleftNodeComp->recalc(leftKeyMemSet, leftValueMemSet);
			pRightNodeComp->recalc(rightKeyMemSet, rightValueMemSet);
			return nSplitIndex;



		}
		
		const TValue& value(uint32 nIndex) const
		{
			return this->m_leafValueMemSet[nIndex];
		}
		TValue& value(uint32 nIndex)
		{
			return this->m_leafValueMemSet[nIndex];
		}

		bool UnionWith(BPTreeLeafNodeMapv2* pNode, bool bLeft, int *nCheckIndex = 0)
		{
			if(!bLeft)
				this->m_pCompressor->add(pNode->m_leafKeyMemSet, pNode->m_leafValueMemSet);
			this->UnionVec(this->m_leafKeyMemSet, pNode->m_leafKeyMemSet, bLeft, nCheckIndex);
			this->UnionVec(this->m_leafValueMemSet, pNode->m_leafValueMemSet, bLeft);

			if(bLeft)
				this->m_pCompressor->recalc(this->m_leafKeyMemSet, this->m_leafValueMemSet);
		
			return true;
		}
		bool AlignmentOf(BPTreeLeafNodeMapv2* pNode, bool bFromLeft)
		{
			if(!this->AlignmentOfVec(this->m_leafKeyMemSet, pNode->m_leafKeyMemSet, bFromLeft))
				return false;
						
			this->AlignmentOfVec(this->m_leafValueMemSet, pNode->m_leafValueMemSet, bFromLeft);

			pNode->recalc();
			recalc();

			return true;
		}
		void recalc()
		{
			this->m_pCompressor->recalc(this->m_leafKeyMemSet, this->m_leafValueMemSet);
		}

		bool removeByIndex(int32 nIndex)
		{
			this->m_pCompressor->remove(nIndex, this->m_leafKeyMemSet[nIndex], this->m_leafValueMemSet[nIndex]);
			bool bRet = this->m_leafKeyMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				return false;
			}
			 bRet = this->m_leafValueMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				return false;
			}
			return true;
		}
		virtual void clear()
		{
			this->m_leafKeyMemSet.clear();
			this->m_leafValueMemSet.clear();
			delete this->m_pCompressor;
			this->m_pCompressor = NULL;

		}
		

	public:
		TValueMemSet m_leafValueMemSet;
	
	};

}
#endif