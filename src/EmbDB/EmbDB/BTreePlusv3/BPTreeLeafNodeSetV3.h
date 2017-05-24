#pragma once
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
//#include "RBMap.h"
#include "../embDBInternal.h"
#include "../BTBaseNode.h"
#include "../CompressorParams.h"
#include "../STLAlloc.h"
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
		typedef STLAllocator<TKey> TAlloc;
		typedef std::vector<TKey, TAlloc> TKeyMemSet;

		typedef typename _TCompressor::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetv3Base(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			m_KeyMemSet(TAlloc(pAlloc)), m_Compressor(nPageSize - 2*sizeof(TLink) , pAlloc), m_nNext(-1), m_nPrev(-1), m_bMulti(bMulti),
			m_pAlloc(pAlloc), m_bMinSplit(false), m_nPageSize(nPageSize)

		{

		}
		~BPTreeLeafNodeSetv3Base()
		{
			 
		}

		virtual bool init(TLeafCompressorParams *pParams = NULL, Transaction* pTransaction = NULL)
		{
			return m_Compressor.init(pParams, pTransaction);
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
			if (m_KeyMemSet.empty())
			{
				m_KeyMemSet.push_back(key);
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
						auto it = std::upper_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
						nIndex = std::distance(m_KeyMemSet.begin(), it);
						m_KeyMemSet.insert(it, key);
					}
					else
					{
						auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
						if(it != m_KeyMemSet.end() && comp.EQ(*it, key))
						{
							//TO DO logs
							return false;
						}
						nIndex = std::distance(m_KeyMemSet.begin(), it);
						m_KeyMemSet.insert(it, key);
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
			if (!m_Compressor.insert(nIndex, key, m_KeyMemSet))
				return -1;
			return nIndex;
		}


		template<class TComp>
		uint32 upper_bound(TComp& comp, const TKey& key)
		{
			auto it = std::upper_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
			return std::distance(m_KeyMemSet.begin(), it);
		}
		template<class TComp>
		uint32 lower_bound(TComp& comp, const TKey& key)
		{
			auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
			return std::distance(m_KeyMemSet.begin(), it);
		}
		template<class TComp>
		int32 binary_search(TComp& comp, const TKey& key)
		{
			auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
			if (it == m_KeyMemSet.end())
				return -1;

			if (comp.EQ(*it, key))
				return std::distance(m_KeyMemSet.begin(), it);

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
		 
				auto it = std::upper_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
				if (it == m_KeyMemSet.end())
					return false;
				if (it != m_KeyMemSet.begin())
				--it;
				nIndex = std::distance(m_KeyMemSet.begin(), it);
			
			}
			else
			{
				auto it = std::lower_bound(m_KeyMemSet.begin(), m_KeyMemSet.end(), key, comp);
				if (it == m_KeyMemSet.end())
					return false;
				nIndex = std::distance(m_KeyMemSet.begin(), it);
			}
			if (!comp.EQ(m_KeyMemSet[nIndex], key))
				return false;
 
			removeByIndex(nIndex);
			return true;
		}
		bool removeByIndex(int32 nIndex)
		{
			m_Compressor.remove(nIndex, m_KeyMemSet[nIndex], m_KeyMemSet);
			m_KeyMemSet.erase(std::next(m_KeyMemSet.begin(), nIndex));
			return true;
		}
		template<class TVector, class TVecVal>
		int SplitInVec(TVector& src, TVector& dst, TVecVal* pSplitVal, int32 _nBegin = -1, int32 _nEnd = -1)
		{
			uint32 nBegin = _nBegin == -1 ? src.size() / 2 : _nBegin;
			uint32 nEnd = _nEnd == -1 ? src.size() : _nEnd;

			std::move(std::next(src.begin(), nBegin), std::next(src.begin(), nEnd), std::inserter(dst, dst.begin()));
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
			TKeyMemSet& newNodeMemSet = pNode->m_KeyMemSet;
			TCompressor& NewNodeComp = pNode->m_Compressor;

			if (m_bMinSplit)
			{
				m_Compressor.remove(m_KeyMemSet.size() - 1, m_KeyMemSet.back(), m_KeyMemSet);
				int nSplitIndex = SplitOne(m_KeyMemSet, newNodeMemSet, pSplitKey);

				NewNodeComp.insert(0, newNodeMemSet[0], newNodeMemSet);
				return nSplitIndex;
			}
			else
			{
				int nSplitIndex = SplitInVec(m_KeyMemSet, newNodeMemSet, pSplitKey);
				m_Compressor.recalc(m_KeyMemSet);
				NewNodeComp.recalc(newNodeMemSet);
				return nSplitIndex;
			}
		}
		int  SplitIn(BPTreeLeafNodeSetv3Base *pLeftNode, BPTreeLeafNodeSetv3Base *pRightNode, TKey* pSplitKey)
		{

			TKeyMemSet& leftNodeMemSet = pLeftNode->m_KeyMemSet;
			TCompressor& pleftNodeComp = pLeftNode->m_Compressor;

			TKeyMemSet& rightNodeMemSet = pRightNode->m_KeyMemSet;
			TCompressor& pRightNodeComp = pRightNode->m_Compressor;


			uint32 nSize = m_KeyMemSet.size() / 2;

			if (pSplitKey)
				*pSplitKey = m_KeyMemSet[nSize];

			SplitInVec(m_KeyMemSet, leftNodeMemSet, 0, nSize);
			SplitInVec(m_KeyMemSet, rightNodeMemSet, nSize, m_KeyMemSet.size() - nSize);

			pleftNodeComp.recalc(leftNodeMemSet);
			pRightNodeComp.recalc(rightNodeMemSet);
			return nSize;

		}

		uint32 count() const
		{
			return m_KeyMemSet.size();
		}
		uint32 tupleSize() const
		{
			return m_Compressor.tupleSize();
		}
		const TKey& key(uint32 nIndex) const
		{
			return m_KeyMemSet[nIndex];
		}
		TKey& key(uint32 nIndex)
		{
			return m_KeyMemSet[nIndex];
		}

		template<class TVector>
		bool UnionVec(TVector& dstVec, TVector& srcVec, bool bLeft, int *nCheckIndex = 0)
		{
			if (bLeft)
			{
				if (nCheckIndex)
					*nCheckIndex += srcVec.size();

				srcVec.reserve(srcVec.size() + dstVec.size());
				std::move(dstVec.begin(), dstVec.end(), std::inserter(srcVec, srcVec.end()));
				srcVec.swap(dstVec);
			}
			else
			{
				dstVec.reserve(srcVec.size() + dstVec.size());
				std::move(srcVec.begin(), srcVec.end(), std::inserter(dstVec, dstVec.end()));
				srcVec.clear();
			}
			return true;
		}


		bool UnionWith(BPTreeLeafNodeSetv3Base* pNode, bool bLeft,
			int *nCheckIndex = 0)
		{
			UnionVec(m_KeyMemSet, pNode->m_KeyMemSet, bLeft, nCheckIndex);

 
			this->m_Compressor.recalc(this->m_KeyMemSet);
			return true;
		}


		template<class TVector>
		bool AlignmentOfVec(TVector& dstVec, TVector& srcVec, bool bFromLeft, int *nCheckIndex = 0)
		{

			int nCnt = ((dstVec.size() + srcVec.size())) / 2 - dstVec.size();
			if (nCnt <= 0)
				return false; //оставим все при своих

			uint32 newSize = srcVec.size() - nCnt;
			dstVec.reserve(dstVec.size() + nCnt);
			if (bFromLeft)
			{
				if (nCheckIndex)
					*nCheckIndex += nCnt;
				uint32 oldSize = dstVec.size();
				
				std::move(std::next(srcVec.begin(), newSize), srcVec.end(), std::inserter(dstVec, dstVec.end()));
				std::rotate(dstVec.begin(), std::next(dstVec.begin(), oldSize), dstVec.end());
				srcVec.resize(newSize);
			}
			else
			{
				std::move(srcVec.begin(), std::next(srcVec.begin(), nCnt), std::inserter(dstVec, dstVec.end()));
				std::rotate(srcVec.begin(), std::next(srcVec.begin(), nCnt), srcVec.end());
				srcVec.resize(newSize);
			}
			return true;
		}

		bool AlignmentOf(BPTreeLeafNodeSetv3Base* pNode, bool bFromLeft)
		{
			auto& nodeMemset = pNode->m_KeyMemSet;
			TCompressor& pNodeComp = pNode->m_Compressor;
			if (!AlignmentOfVec(m_KeyMemSet, nodeMemset, bFromLeft))
				return false;
			pNodeComp.recalc(nodeMemset);
			m_Compressor.recalc(m_KeyMemSet);

			return true;
		}
		template<class TComp>
		bool isKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			return comp.EQ(key, m_KeyMemSet[nIndex]);
		}

		void SetMinSplit(bool bMinSplit)
		{
			m_bMinSplit = bMinSplit;
		}
		virtual void clear()
		{
			m_KeyMemSet.clear();
			m_Compressor.clear();
		}
		bool IsHaveUnion(BPTreeLeafNodeSetv3Base *pNode)
		{

			return this->m_Compressor.IsHaveUnion(pNode->m_Compressor);
		}
		bool IsHaveAlignment(BPTreeLeafNodeSetv3Base *pNode)
		{
			return this->m_Compressor.IsHaveAlignment(pNode->m_Compressor);
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
		TKeyMemSet m_KeyMemSet;
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
		typedef typename TBase::TKeyMemSet TLeafMemSet;
		typedef typename TBase::TLeafCompressorParams TLeafCompressorParams;

		BPTreeLeafNodeSetv3(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			TBase(pAlloc, bMulti, nPageSize) 
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
			return this->m_Compressor.Write(this->m_KeyMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(this->m_nNext);
			stream.read(this->m_nPrev);
			return this->m_Compressor.Load(this->m_KeyMemSet, stream);
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
