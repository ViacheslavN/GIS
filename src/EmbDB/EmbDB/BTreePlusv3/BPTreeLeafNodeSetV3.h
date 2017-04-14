#pragma once
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
//#include "RBMap.h"
#include "../embDBInternal.h"
#include "../BTBaseNode.h"
#include "../CompressorParams.h"
namespace embDB
{
	template<typename _TKey,
		class _Transaction, class _TCompressor>
		class BPTreeLeafNodeSetv3Base : public  BPBaseTreeNode
	{
	public:

		typedef int64 TLink;
		typedef _TKey TKey;
		typedef _Transaction Transaction;
		typedef _TCompressor TCompressor;
		typedef std::vector<TKey> TVector;

		typedef typename _TCompressor::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetv3Base(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			/*m_leafKeyMemSet(pAlloc),*/ m_Compressor(pAlloc, m_leafKeyMemSet, nPageSize), m_nNext(-1), m_nPrev(-1), m_bMulti(bMulti),
			m_pAlloc(pAlloc), m_bMinSplit(false), m_nPageSize(nPageSize)

		{

		}
		~BPTreeLeafNodeSetv3Base()
		{
			 
		}



		virtual bool isLeaf() const { return true; }
		virtual uint32 size() const
		{
 
			return  2 * sizeof(TLink) + m_Compressor.size();
		}
		virtual bool isNeedSplit() const
		{
			return m_Compressor.isNeedSplit(/*nPageSize - 2 *sizeof(TLink)*/);
		}
		virtual uint32 headSize() const
		{
			return 2 * sizeof(TLink) + m_Compressor.headSize();
		}
		virtual uint32 rowSize() const
		{
			return m_Compressor.rowSize();
		}
		virtual bool IsFree() const
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}

		template<class TComp>
		bool insertImp(TComp& comp, const TKey& key, int32& nIndex, int nInsertLeafIndex = -1)
		{
			nIndex = -1;
			if (m_leafKeyMemSet.empty())
			{
				m_leafKeyMemSet.push_back(key);
				nIndex = 0;
			}
			else
			{
				if (nInsertLeafIndex != -1)
					nIndex = nInsertLeafIndex;
				else
				{
					if (m_bMulti)
					{
						auto it = std::upper_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
						nIndex = std::distance(m_leafKeyMemSet.begin(), it);
						m_leafKeyMemSet.insert(it, key);
					}
					else
					{
						auto it = std::lower_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
						if(it != m_leafKeyMemSet.end() && comp.EQ(*it, key))
						{
							//TO DO logs
							return false;
						}
						nIndex = std::distance(m_leafKeyMemSet.begin(), it);
						m_leafKeyMemSet.insert(it, key);
					}
				}
			}
			return true;
		}
		template<class TComp>
		int insert(TComp& comp, const TKey& key, int nInsertLeafIndex = -1)
		{
			int32 nIndex = 0;
			if (!insertImp(comp, key, nIndex, nInsertLeafIndex))
				return -1;
			if (!m_Compressor.insert(nIndex, key))
				return -1;
			return nIndex;
		}


		template<class TComp>
		uint32 upper_bound(TComp& comp, const TKey& key)
		{
			auto it = std::upper_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
			return std::distance(m_leafKeyMemSet.begin(), it);
		}
		template<class TComp>
		uint32 lower_bound(TComp& comp, const TKey& key, short& nType)
		{
			auto it = std::lower_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
			return std::distance(m_leafKeyMemSet.begin(), it);
		}
		template<class TComp>
		int32 binary_search(TComp& comp, const TKey& key)
		{
			auto it = std::lower_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
			if (it == m_leafKeyMemSet.end())
				return -1;

			if (comp.EQ(*it, key))
				return std::distance(m_leafKeyMemSet.begin(), it);

			return -1;
		}

		bool update(const TKey& key)
		{
			return true;
		}
		template<class TComp>
		bool remove(TComp& comp, const TKey& key)
		{
			int32 nIndex = -1;
	 		if (m_bMulti)
			{
		 
				auto it = std::upper_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
				if (it == m_leafKeyMemSet.end())
					return false;
				if (it != m_leafKeyMemSet.begin())
				--it;
				nIndex = std::distance(m_leafKeyMemSet.begin(), it);
			
			}
			else
			{
				auto it = std::lower_bound(m_leafKeyMemSet.begin(), m_leafKeyMemSet.end(), key, comp);
				if (it == m_leafKeyMemSet.end())
					return false;
				nIndex = std::distance(m_leafKeyMemSet.begin(), it);
			}
			if (!comp.EQ(m_leafKeyMemSet[nIndex], key))
				return false;
 
			removeByIndex(nIndex);
			return true;
		}
		bool removeByIndex(int32 nIndex)
		{
			m_Compressor.remove(nIndex, m_leafKeyMemSet[nIndex]);
			m_leafKeyMemSet.erase(std::next(m_leafKeyMemSet.begin(), nIndex));
			return true;
		}
		template<class TVector, class TVecVal>
		int SplitInVec(TVector& src, TVector& dst, TVecVal* pSplitVal, int32 _nBegin = -1, int32 _nEnd = -1)
		{
			uint32 nBegin = _nBegin == -1 ? src.size() / 2 : _nBegin;
			uint32 nEnd = _nEnd == -1 ? src.size() - 1 : _nEnd;
			dst.insert(dst.begin(),  std::next(src.begin(), nBegin), std::next(src.begin(), nEnd));
			src.resize(src.size() - (nEnd - nBegin));

			if (pSplitVal)
				*pSplitVal = dst[0];
			return (int)nBegin;

		}


		template<class TVector>
		void SplitInVec(TVector& src, TVector& dst, uint32 nBegin, uint32 nCount)
		{
			//dst.copy(src, 0, nBegin, nCount);
			dst.insert(dst.begin(), std::next(src.begin(), nBegin), std::next(src.begin(), nBegin + nCount));
		}

		template<class TVector, class TVecVal>
		int SplitOne(TVector& src, TVector& dst, TVecVal* pSplitVal)
		{
			uint32 nSize = src.size() - 1;
			dst.push_back(src[nSize]);
			src.resize(nSize);
			if (pSplitVal)
				*pSplitVal = dst[0];
			return (int)nSize;

		}




		int  SplitIn(BPTreeLeafNodeSetv3Base *pNode, TKey* pSplitKey)
		{

			TLeafMemSet& newNodeMemSet = pNode->m_leafKeyMemSet;
			TCompressor& NewNodeComp = pNode->m_Compressor;


			if (m_bMinSplit)
			{
				m_pCompressor->remove(m_leafKeyMemSet.size() - 1, m_leafKeyMemSet.back());
				int nSplitIndex = SplitOne(m_leafKeyMemSet, newNodeMemSet, pSplitKey);

				NewNodeComp.insert(0, newNodeMemSet[0]);
				return nSplitIndex;
			}
			else
			{
				int nSplitIndex = SplitInVec(m_leafKeyMemSet, newNodeMemSet, pSplitKey);
				m_Compressor.SplitIn(0, nSplitIndex, NewNodeComp);
				return nSplitIndex;
			}


		}


		int  SplitIn(BPTreeLeafNodeSetv3Base *pLeftNode, BPTreeLeafNodeSetv3Base *pRightNode, TKey* pSplitKey)
		{

			TVector& leftNodeMemSet = pLeftNode->m_leafKeyMemSet;
			TCompressor& pleftNodeComp = pLeftNode->m_pCompressor;

			TVector& rightNodeMemSet = pRightNode->m_leafKeyMemSet;
			TCompressor& pRightNodeComp = pRightNode->m_pCompressor;


			uint32 nSize = m_leafKeyMemSet.size() / 2;

			if (pSplitKey)
				*pSplitKey = m_leafKeyMemSet[nSize];

			SplitInVec(m_leafKeyMemSet, leftNodeMemSet, 0, nSize);
			SplitInVec(m_leafKeyMemSet, rightNodeMemSet, nSize, m_leafKeyMemSet.size());

			pleftNodeComp.recalc(leftNodeMemSet);
			pRightNodeComp.recalc(rightNodeMemSet);
			return nSize;

		}

		uint32 count() const
		{
			return m_leafKeyMemSet.size();
		}
		uint32 tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		const TKey& key(uint32 nIndex) const
		{
			return m_leafKeyMemSet[nIndex];
		}
		TKey& key(uint32 nIndex)
		{
			return m_leafKeyMemSet[nIndex];
		}

		template<class TVector>
		bool UnionVec(TVector& dstVec, TVector& srcVec, bool bLeft, int *nCheckIndex = 0)
		{
			if (bLeft)
			{
				if (nCheckIndex)
					*nCheckIndex += srcVec.size();
				srcVec.insert(srcVec.end(), dstVec.begin(), dstVec.end());
				srcVec.swap(dstVec);
			}
			else
			{
				dstVec.insert(dstVec.end(), srcVec.begin(), srcVec.end());
				srcVec.clear();
			}
			return true;
		}


		bool UnionWith(BPTreeLeafNodeSetv3Base* pNode, bool bLeft,
			int *nCheckIndex = 0)
		{
			UnionVec(m_leafKeyMemSet, pNode->m_leafKeyMemSet, bLeft, nCheckIndex);

			/*if(!bLeft)
			m_pCompressor->add(pNode->m_leafKeyMemSet);

			if(bLeft)*/
			this->m_pCompressor->recalc(this->m_leafKeyMemSet);
			return true;
		}


		template<class TVector>
		bool AlignmentOfVec(TVector& dstVec, TVector& srcVec, bool bFromLeft, int *nCheckIndex = 0)
		{

			int nCnt = ((dstVec.size() + srcVec.size())) / 2 - dstVec.size();
			if (nCnt <= 0)
				return false; //оставим все при своих
			if (bFromLeft)
			{
				if (nCheckIndex)
					*nCheckIndex += nCnt;

				uint32 newSize = srcVec.size() - nCnt;
				dstVec.insert(srcVec, 0, newSize, srcVec.size());
				srcVec.resize(newSize);
			}
			else
			{
				dstVec.copy(srcVec, dstVec.size(), 0, nCnt);
				srcVec.movel(nCnt, nCnt);
			}
			return true;
		}

		bool AlignmentOf(BPTreeLeafNodeSetv3Base* pNode, bool bFromLeft)
		{
			TVector& nodeMemset = pNode->m_leafKeyMemSet;
			TCompressor& pNodeComp = pNode->m_Compressor;
			if (!AlignmentOfVec(m_leafKeyMemSet, nodeMemset, bFromLeft))
				return false;
			pNodeComp.recalc(nodeMemset);
			m_Compressor.recalc(m_leafKeyMemSet);

			return true;
		}
		template<class TComp>
		bool isKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			return comp.EQ(key, m_leafKeyMemSet[nIndex]);
		}

		void SetMinSplit(bool bMinSplit)
		{
			m_bMinSplit = bMinSplit;
		}
		virtual void clear()
		{
			m_leafKeyMemSet.clear();
			delete m_pCompressor;
			m_pCompressor = nullptr;

		}
		bool IsHaveUnion(BPTreeLeafNodeSetv3Base *pNode)
		{

			return this->m_Compressor.IsHaveUnion(pNode.m_Compressor);
		}
		bool IsHaveAlignment(BPTreeLeafNodeSetv3Base *pNode)
		{
			return this->m_Compressor.IsHaveAlignment(pNode.m_Compressor);
		}
		bool isHalfEmpty() const
		{
			return this->m_Compressor.isHalfEmpty();
		}
		virtual  void PreSave()
		{

		}

	public:
		TCompressor  m_Compressor;
		TVector m_leafKeyMemSet;
		TLink m_nNext;
		TLink m_nPrev;
		bool m_bMulti;
		CommonLib::alloc_t *m_pAlloc;
		bool m_bMinSplit;
		uint32 m_nPageSize;
		//TComporator m_comp;
	};

	template<typename _TKey,
		class _Transaction, class _TCompressor>
		class BPTreeLeafNodeSetv3 : public  BPTreeLeafNodeSetv3Base <_TKey, _Transaction, _TCompressor>
	{
	public:

		typedef BPTreeLeafNodeSetv3Base <_TKey, _Transaction, _TCompressor> TBase;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::Transaction Transaction;


		typedef typename TBase::TCompressor TCompressor;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetv3(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			TBase(pAlloc, bMulti, nPageSize), m_Compressor(pAlloc, this->m_nPageSize - 2 * sizeof(TLink), this->m_leafKeyMemSet)
		{}


		bool init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL)
		{
		//	this->m_pCompressor = new TCompressor(this->m_nPageSize - 2 * sizeof(TLink), pTransaction, this->m_pAlloc, pParams, &this->m_leafKeyMemSet);
			this->m_Compressor.init(pParams, pTransaction);
			return true;
		}


		virtual  bool Save(CommonLib::FxMemoryWriteStream& stream)
		{
			stream.write(this->m_nNext);
			stream.write(this->m_nPrev);
			return this->m_Compressor.Write(this->m_leafKeyMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(this->m_nNext);
			stream.read(this->m_nPrev);
			return this->m_Compressor.Load(this->m_leafKeyMemSet, stream);
		}


		bool IsUnion(BPTreeLeafNodeSetv3 *pNode)		{

			return this->m_Compressor.IsUnion(pNode->m_Compressor);
		}
		bool IsAlignment(BPTreeLeafNodeSetv3 *pNode)
		{
			return this->m_Compressor.IsAlignment(pNode->m_Compressor);
		}
	};

}
