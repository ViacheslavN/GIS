#pragma once
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeLeafNodeSetv3.h"
namespace embDB
{
	template<typename _TKey, typename _TValue,
		class _Transaction, class _TCompressor>
		class BPTreeLeafNodeMapv3 : public  BPTreeLeafNodeSetv3Base<_TKey, _Transaction, _TCompressor>
	{
	public:
		typedef BPTreeLeafNodeSetv3Base<_TKey, _Transaction, _TCompressor> TBase;
		typedef _TValue TValue;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;
		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TKeyMemSet TKeyMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;
		typedef typename TBase::TAlloc TAlloc;
		typedef std::vector<TValue, TAlloc> TValueMemSet;

		BPTreeLeafNodeMapv3(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			TBase(pAlloc, bMulti, nPageSize), m_ValueMemSet(TAlloc(pAlloc))
		{

		}
		~BPTreeLeafNodeMapv3()
		{

		}

		virtual  bool Save(CommonLib::FxMemoryWriteStream& stream)
		{
			stream.write(this->m_nNext);
			stream.write(this->m_nPrev);
			return this->m_Compressor.Write(this->m_KeyMemSet, this->m_ValueMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(this->m_nNext);
			stream.read(this->m_nPrev);
			return this->m_Compressor.Load(this->m_KeyMemSet, this->m_ValueMemSet, stream);
		}
		template<class TComp>
		int insert(TComp& comp, const TKey& key, const TValue& value, int nInsertInIndex = -1)
		{
			int32 nIndex = 0;
			bool bRet = this->insertImp(comp, key, nIndex, nInsertInIndex);
			if (!bRet)
				return -1;

			this->m_ValueMemSet.insert( std::next(this->m_ValueMemSet.begin(), nIndex), value);
			if (!this->m_Compressor.insert(nIndex, key, value, this->m_KeyMemSet, this->m_ValueMemSet))
				return -1;
			return nIndex;
		}

		int SplitIn(BPTreeLeafNodeMapv3 *pNode, TKey* pSplitKey)
		{
			TCompressor& pNewNodeComp = pNode->m_Compressor;
			if (this->m_bMinSplit)
			{

				int nSplitIndex = this->m_KeyMemSet.size() - 1;

				while (true)
				{

					this->m_Compressor.remove(nSplitIndex, this->m_KeyMemSet[nSplitIndex], this->m_ValueMemSet[nSplitIndex], this->m_KeyMemSet, this->m_ValueMemSet);

					pNode->m_KeyMemSet.insert(pNode->m_KeyMemSet.begin(), this->m_KeyMemSet[nSplitIndex]);
					pNode->m_ValueMemSet.insert(pNode->m_ValueMemSet.begin(), this->m_ValueMemSet[nSplitIndex]);

					this->m_KeyMemSet.resize(nSplitIndex);
					this->m_ValueMemSet.resize(nSplitIndex);

					pNewNodeComp.insert(pNode->m_KeyMemSet.size() - 1, pNode->m_KeyMemSet.back(), pNode->m_ValueMemSet.back(), pNode->m_KeyMemSet, pNode->m_ValueMemSet);


					if (!this->isNeedSplit())
						break;

					--nSplitIndex;
				}
				if (pSplitKey)
					*pSplitKey = pNode->m_KeyMemSet[0];

				return nSplitIndex;
			}
			else
			{

				int nSplitIndex = this->SplitInVec(this->m_KeyMemSet, pNode->m_KeyMemSet, pSplitKey);
				this->SplitInVec(this->m_ValueMemSet, pNode->m_ValueMemSet, (TValue*)NULL);
				pNewNodeComp.recalc(pNode->m_KeyMemSet, pNode->m_ValueMemSet);
				return nSplitIndex;
			}
		}



		int  SplitIn(BPTreeLeafNodeMapv3 *pLeftNode, BPTreeLeafNodeMapv3 *pRightNode, TKey* pSplitKey)
		{

			TKeyMemSet& leftNodeMemSet = pLeftNode->m_KeyMemSet;
			TValueMemSet& leftValueMemSet = pLeftNode->m_ValueMemSet;
			TCompressor& pleftNodeComp = pLeftNode->m_Compressor;

			TKeyMemSet& rightNodeMemSet = pRightNode->m_KeyMemSet;
			TValueMemSet& rightValueMemSet = pRightNode->m_ValueMemSet;
			TCompressor& pRightNodeComp = pRightNode->m_Compressor;


			uint32 nSize = m_bMinSplit? this->m_KeyMemSet.size() - 1 : this->m_KeyMemSet.size() / 2;

			if (pSplitKey)
				*pSplitKey = this->m_KeyMemSet[nSize];

			this->SplitInVec(this->m_KeyMemSet, leftNodeMemSet, 0, nSize);
			this->SplitInVec(this->m_KeyMemSet, rightNodeMemSet, nSize, this->m_KeyMemSet.size() - nSize);

			this->SplitInVec(this->m_ValueMemSet, leftValueMemSet, 0, nSize);
			this->SplitInVec(this->m_ValueMemSet, rightValueMemSet, nSize, this->m_ValueMemSet.size() - nSize);

			pleftNodeComp.recalc(leftNodeMemSet, leftValueMemSet);
			pRightNodeComp.recalc(rightNodeMemSet, rightValueMemSet);
			return nSize;

		}

		const TValue& value(uint32 nIndex) const
		{
			return this->m_ValueMemSet[nIndex];
		}
		TValue& value(uint32 nIndex)
		{
			return this->m_ValueMemSet[nIndex];
		}

		bool UnionWith(BPTreeLeafNodeMapv3* pNode, bool bLeft, int *nCheckIndex = 0)
		{

			this->UnionVec(this->m_KeyMemSet, pNode->m_KeyMemSet, bLeft, nCheckIndex);
			this->UnionVec(this->m_ValueMemSet, pNode->m_ValueMemSet, bLeft);
 
			this->m_Compressor.recalc(this->m_KeyMemSet, this->m_ValueMemSet);

			return true;
		}
		bool AlignmentOf(BPTreeLeafNodeMapv3* pNode, bool bFromLeft)
		{
			if (!this->AlignmentOfVec(this->m_KeyMemSet, pNode->m_KeyMemSet, bFromLeft))
				return false;

			this->AlignmentOfVec(this->m_ValueMemSet, pNode->m_ValueMemSet, bFromLeft);

			pNode->recalc();
			recalc();

			return true;
		}
		void recalc()
		{
			this->m_Compressor.recalc(this->m_KeyMemSet, this->m_ValueMemSet);
		}

		bool removeByIndex(int32 nIndex)
		{
			this->m_Compressor.remove(nIndex, this->m_KeyMemSet[nIndex], this->m_ValueMemSet[nIndex], this->m_KeyMemSet, this->m_ValueMemSet);
			m_KeyMemSet.erase(std::next(m_KeyMemSet.begin(), nIndex));
			m_ValueMemSet.erase(std::next(m_ValueMemSet.begin(), nIndex));
		 	return true;
		}
		virtual void clear()
		{
			this->m_KeyMemSet.clear();
			this->m_ValueMemSet.clear();
			this->m_Compressor.clear();
 		}


	public:
		TValueMemSet m_ValueMemSet;

	};

}
