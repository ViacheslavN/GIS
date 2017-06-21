#pragma once

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"

#include "../../embDBInternal.h"
#include "BTBaseNode.h"
#include "../../CompressorParams.h"
#include "../../Utils/alloc/STLAlloc.h"

namespace embDB
{
	template<typename _TKey,	class _Transaction, class _TCompressor>
		class BPTreeInnerNodeSet : public  BPBaseTreeNode
	{
		typedef _TKey TKey;
		typedef int64 TLink;
		typedef _Transaction Transaction;
		typedef _TCompressor TCompressor;

		typedef STLAllocator<TKey> TKeyAlloc;
		typedef STLAllocator<TLink> TLinkAlloc;
		typedef std::vector<TKey, TKeyAlloc> TKeyMemSet;
		typedef std::vector<TLink, TLinkAlloc> TLinkMemSet;

	public:

		typedef typename _TCompressor::TCompressorParams TInnerCompressorParams;

		BPTreeInnerNodeSet(CommonLib::alloc_t *pAlloc, bool bMulti, uint32 nPageSize) :
			m_nLess(-1), m_innerKeyMemSet(TKeyAlloc(pAlloc)), m_innerLinkMemSet(TLinkAlloc(pAlloc)), m_bMulti(bMulti),
			m_pAlloc(pAlloc), m_bMinSplit(false), m_nPageSize(nPageSize), m_Compressor(nPageSize - sizeof(TLink), pAlloc)
		{

		}

		virtual bool init(TInnerCompressorParams *pParams = NULL, Transaction* pTransaction = NULL)
		{				
			return m_Compressor.init(pParams, pTransaction);
		}
		~BPTreeInnerNodeSet()
		{
	
		}

		virtual bool isLeaf() const { return false; }
		virtual uint32 size() const
		{
			 
			return sizeof(TLink) + m_Compressor.size();
		}
		virtual bool isNeedSplit() const
		{
			return m_Compressor.isNeedSplit();
		}
		virtual uint32 headSize() const
		{
			return sizeof(TLink) + m_Compressor.headSize();
		}
		virtual uint32 rowSize() const
		{
			return m_Compressor.rowSize();
		}
		virtual  bool Save(CommonLib::FxMemoryWriteStream& stream)
		{
			assert(m_nLess != -1);
			stream.write(m_nLess);
			return m_Compressor.Write(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(m_nLess);
			assert(m_nLess != -1);
			return m_Compressor.Load(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		uint32 tupleSize() const
		{
			return m_Compressor.tupleSize();
		}

		template<class TComp>
		TLink findNodeInsert(const TComp& comp, const TKey& key, int32& nIndex)
		{
			nIndex = -1;
			if (!m_bMulti)
			{		
				return lower_bound(comp, key, nIndex);
			}

			auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
			if (it == m_innerKeyMemSet.end())
				return -1;
			if (it == m_innerKeyMemSet.begin()) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			--it;
			nIndex = std::distance(m_innerKeyMemSet.begin(), it);
			return m_innerLinkMemSet[nIndex];
		}

		template<class TComp>
		TLink findNodeRemove(const TComp& comp, const TKey& key, int32& nIndex)
		{
			nIndex = -1;
			if (!m_bMulti)
			{
				return lower_bound(comp, key, nIndex);
			}

			auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
			if (it == m_innerKeyMemSet.end())
				return -1;
			if (it == m_innerKeyMemSet.begin()) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			--it;
			nIndex = std::distance(m_innerKeyMemSet.begin(), it);
			return m_innerLinkMemSet[nIndex];
		}


		

		template<class TComp>
		TLink upper_bound(const TComp& comp, const TKey& key, int32& nIndex)
		{
			auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
			if (it == m_innerKeyMemSet.end())
				return -1;
			if (it == m_innerKeyMemSet.begin()) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			nIndex = std::distance(m_innerKeyMemSet.begin(), it);
			return m_innerLinkMemSet[nIndex];
		}
		template<class TComp>
		TLink lower_bound(const TComp& comp, const TKey& key, int32& nIndex)
		{
			auto it = std::lower_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
			if (it == m_innerKeyMemSet.end())
			{
				nIndex = m_innerKeyMemSet.size() - 1;
				return  m_innerLinkMemSet[nIndex];
			}

			nIndex = std::distance(m_innerKeyMemSet.begin(), it);
			if (comp.EQ(key, m_innerKeyMemSet[nIndex]))
				return m_innerLinkMemSet[nIndex];

			if (nIndex == 0) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			--nIndex;
			return m_innerLinkMemSet[nIndex];
		}

		template<class TComp>
		int insert(const TComp& comp, const TKey& key, TLink nLink)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if (m_innerKeyMemSet.empty())
			{
				m_innerKeyMemSet.push_back(key);
				m_innerLinkMemSet.push_back(nLink);
				nIndex = 0;
			}
			else
			{
				
				if (m_bMulti)
				{
					auto it = std::upper_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
					nIndex = std::distance(m_innerKeyMemSet.begin(), it);
					m_innerKeyMemSet.insert(it, key);
				}
				else
				{
					auto it = std::lower_bound(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), key, comp);
					if (it != m_innerKeyMemSet.end() && comp.EQ(key, (*it)))
						return false;

					nIndex = std::distance(m_innerKeyMemSet.begin(), it);
					m_innerKeyMemSet.insert(it, key);
					
				}

				m_innerLinkMemSet.insert(m_innerLinkMemSet.begin() + nIndex, nLink);
			
			}
			bool bRet = m_Compressor.insert(nIndex, key, nLink, m_innerKeyMemSet, m_innerLinkMemSet);
			return bRet ? nIndex : -1;
		}
		template<class TComp>
		bool remove(const TComp& comp, const TKey& key)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if (m_bMulti)
			{

				nIndex = m_innerKeyMemSet.upper_bound(key, comp);
				if (nIndex && comp.EQ(key, m_innerKeyMemSet[nIndex - 1]))
				{
					nType = FIND_KEY;
					nIndex -= 1;
				}
			}
			else
			{
				nIndex = m_innerKeyMemSet.lower_bound(key, nType, comp);
			}

			if (nType != FIND_KEY)
			{
				return false;
			}
			return removeByIndex(nIndex);
		}


		void removeByIndex(int32 nIndex)
		{
			m_Compressor.remove(nIndex, m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex], m_innerKeyMemSet, m_innerLinkMemSet);
			m_innerKeyMemSet.erase(m_innerKeyMemSet.begin() + nIndex);
			m_innerLinkMemSet.erase(m_innerLinkMemSet.begin() + nIndex);

		}

		bool SplitIn(BPTreeInnerNodeSet *pNode, TKey* pSplitKey)
		{

			TKeyMemSet& newNodeKeySet = pNode->m_innerKeyMemSet;
			TLinkMemSet& newNodeLinkSet = pNode->m_innerLinkMemSet;
			TCompressor& pNewNodeComp = pNode->m_Compressor;

			if (m_bMinSplit)
			{

				//uint32 nNewSize = m_innerKeyMemSet.size() - 2;
				uint32 nLessIndex = m_innerKeyMemSet.size() - 2;
				uint32 nSplitIndex = m_innerKeyMemSet.size() - 1;



				newNodeKeySet.push_back(m_innerKeyMemSet[nSplitIndex]);
				newNodeLinkSet.push_back(m_innerLinkMemSet[nSplitIndex]);


	

				m_Compressor.remove(nSplitIndex, m_innerKeyMemSet[nSplitIndex], m_innerLinkMemSet[nSplitIndex], m_innerKeyMemSet, m_innerLinkMemSet);
				pNewNodeComp.insert(0, newNodeKeySet[0], newNodeLinkSet[0], newNodeKeySet, newNodeLinkSet);

				m_innerKeyMemSet.resize(nSplitIndex);
				m_innerLinkMemSet.resize(nSplitIndex);

				while (true)
				{

					if (!this->isNeedSplit())
						break;

					m_Compressor.remove(nLessIndex, m_innerKeyMemSet[nLessIndex], m_innerLinkMemSet[nLessIndex], m_innerKeyMemSet, m_innerLinkMemSet);


					newNodeKeySet.insert(newNodeKeySet.begin(), m_innerKeyMemSet[nLessIndex]);
					newNodeLinkSet.insert(newNodeLinkSet.begin(), m_innerLinkMemSet[nLessIndex]);


					m_innerKeyMemSet.resize(nLessIndex);
					m_innerLinkMemSet.resize(nLessIndex);

					pNewNodeComp.insert(newNodeKeySet.size() - 1, newNodeKeySet.back(), newNodeLinkSet.back(), newNodeKeySet, newNodeLinkSet);

					--nLessIndex;
				}
				assert(nLessIndex > 0);

				m_Compressor.remove(nLessIndex, m_innerKeyMemSet[nLessIndex], m_innerLinkMemSet[nLessIndex], m_innerKeyMemSet, m_innerLinkMemSet);

				*pSplitKey = m_innerKeyMemSet[nLessIndex];
				pNode->m_nLess = m_innerLinkMemSet[nLessIndex];


				m_innerKeyMemSet.resize(nLessIndex);
				m_innerLinkMemSet.resize(nLessIndex);

			}
			else
			{
				uint32 nSize = m_innerKeyMemSet.size() / 2;


				std::move(std::next(m_innerKeyMemSet.begin(),  nSize + 1), m_innerKeyMemSet.end(), std::inserter(newNodeKeySet, newNodeKeySet.begin()));
				std::move(std::next(m_innerLinkMemSet.begin(),  nSize + 1), m_innerLinkMemSet.end(), std::inserter(newNodeLinkSet, newNodeLinkSet.begin()));



				uint32 nNewSize = nSize;


				*pSplitKey = m_innerKeyMemSet[nNewSize];
				pNode->m_nLess = m_innerLinkMemSet[nNewSize];
				

				assert(pNode->m_nLess != -1);

				m_innerKeyMemSet.resize(nNewSize);
				m_innerLinkMemSet.resize(nNewSize);

				m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
				pNewNodeComp.recalc(newNodeKeySet, newNodeLinkSet);
			}



			return true;
		}

		bool SplitIn(BPTreeInnerNodeSet *pLeftNode, BPTreeInnerNodeSet *pRightNode, TKey* pSplitKey)
		{

			TKeyMemSet& LeftKeySet = pLeftNode->m_innerKeyMemSet;
			TLinkMemSet& LeftLinkSet = pLeftNode->m_innerLinkMemSet;
			TCompressor& pLeftNodeComp = pLeftNode->m_Compressor;

			TKeyMemSet& RightKeySet = pRightNode->m_innerKeyMemSet;
			TLinkMemSet& RightLinkSet = pRightNode->m_innerLinkMemSet;
			TCompressor& pRightNodeComp = pRightNode->m_Compressor;

			//int nSize = m_innerKeyMemSet.size() / 2;


			uint32 nSize = m_bMinSplit ? this->m_innerKeyMemSet.size() - 2 : this->m_innerKeyMemSet.size() / 2;

			std::move(m_innerKeyMemSet.begin(), m_innerKeyMemSet.begin() + nSize, std::inserter(LeftKeySet, LeftKeySet.begin()));
			std::move(m_innerLinkMemSet.begin(), m_innerLinkMemSet.begin() + nSize, std::inserter(LeftLinkSet, LeftLinkSet.begin()));

 
			pLeftNode->m_nLess = m_nLess;

			pLeftNodeComp.recalc(LeftKeySet, LeftLinkSet);

			*pSplitKey = m_innerKeyMemSet[nSize];
			pRightNode->m_nLess = m_innerLinkMemSet[nSize];

			std::move(m_innerKeyMemSet.begin() + nSize + 1, m_innerKeyMemSet.end() , std::inserter(RightKeySet, RightKeySet.begin()));
			std::move(m_innerLinkMemSet.begin() + nSize + 1, m_innerLinkMemSet.end(), std::inserter(RightLinkSet, RightLinkSet.begin()));

			 
			pRightNodeComp.recalc(RightKeySet, RightLinkSet);


			m_innerKeyMemSet.clear();
			m_innerLinkMemSet.clear();
			m_Compressor.clear();
			return true;
		}
		uint32 count() const
		{
			return m_innerLinkMemSet.size();
		}
		TLink link(int32 nIndex)
		{
			return m_innerLinkMemSet[nIndex];
		}

		const TKey& key(int32 nIndex) const
		{
			return m_innerKeyMemSet[nIndex];
		}

		TKey& key(int32 nIndex)
		{
			return m_innerKeyMemSet[nIndex];
		}

		void updateLink(int32 nIndex, TLink nLink)
		{
			m_Compressor.updateValue(nIndex, nLink, m_innerLinkMemSet[nIndex], m_innerLinkMemSet, m_innerKeyMemSet);
			m_innerLinkMemSet[nIndex] = nLink;

		}


		void updateKey(int32 nIndex, const TKey& key)
		{
			m_Compressor.updateKey(nIndex, key, m_innerKeyMemSet[nIndex], m_innerKeyMemSet, m_innerLinkMemSet);
			m_innerKeyMemSet[nIndex] = key;

		}

		bool UnionWith(BPTreeInnerNodeSet* pNode, const TKey* pLessMin, bool bLeft)
		{


			if (bLeft)
			{
				if (pLessMin) //can be if root is empty
				{
					pNode->m_innerKeyMemSet.push_back(*pLessMin);
					pNode->m_innerLinkMemSet.push_back(m_nLess);
				}



				pNode->m_innerKeyMemSet.reserve(pNode->m_innerKeyMemSet.size() + m_innerKeyMemSet.size());
				pNode->m_innerLinkMemSet.reserve(pNode->m_innerLinkMemSet.size() + m_innerLinkMemSet.size());

		 
				std::move(m_innerKeyMemSet.begin(), m_innerKeyMemSet.end(), std::inserter(pNode->m_innerKeyMemSet, pNode->m_innerKeyMemSet.end()));
				std::move(m_innerLinkMemSet.begin(), m_innerLinkMemSet.end(), std::inserter(pNode->m_innerLinkMemSet, pNode->m_innerLinkMemSet.end()));

				
				pNode->m_innerLinkMemSet.swap(m_innerLinkMemSet);
				pNode->m_innerKeyMemSet.swap(m_innerKeyMemSet);

				m_nLess = pNode->m_nLess;

				m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
			}
			else
			{
				if (pLessMin)
				{
					m_innerKeyMemSet.push_back(*pLessMin);
					m_innerLinkMemSet.push_back(pNode->m_nLess);
				}


				m_innerKeyMemSet.reserve(pNode->m_innerKeyMemSet.size() + m_innerKeyMemSet.size());
				m_innerLinkMemSet.reserve(pNode->m_innerLinkMemSet.size() + m_innerLinkMemSet.size());


				std::move(pNode->m_innerKeyMemSet.begin(), pNode->m_innerKeyMemSet.end(), std::inserter(m_innerKeyMemSet, m_innerKeyMemSet.end()));
				std::move(pNode->m_innerLinkMemSet.begin(), pNode->m_innerLinkMemSet.end(), std::inserter(m_innerLinkMemSet, m_innerLinkMemSet.end()));

				m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
			}
			return true;
		}
		bool AlignmentOf(BPTreeInnerNodeSet* pNode, const TKey& LessMin, bool bLeft)
		{
			int nCnt = ((m_innerKeyMemSet.size() + pNode->m_innerKeyMemSet.size())) / 2 - m_innerKeyMemSet.size();
	 
			if (nCnt < 1 && !m_innerKeyMemSet.empty())
				return false;  
			
			uint32 newSize = pNode->m_innerLinkMemSet.size() - nCnt;

			if (bLeft)
			{

				uint32 oldSize = m_innerKeyMemSet.size();


				m_innerKeyMemSet.reserve(m_innerKeyMemSet.size() + nCnt + 1); //1 for less elem
				m_innerLinkMemSet.reserve(m_innerLinkMemSet.size() + nCnt + 1); //1 for less elem

				
				std::move(std::next(pNode->m_innerKeyMemSet.begin(), newSize + 1), pNode->m_innerKeyMemSet.end(), std::inserter(m_innerKeyMemSet, m_innerKeyMemSet.end()));
				std::move(std::next(pNode->m_innerLinkMemSet.begin(), newSize + 1), pNode->m_innerLinkMemSet.end(), std::inserter(m_innerLinkMemSet, m_innerLinkMemSet.end()));
				
				m_innerKeyMemSet.push_back(LessMin);
				m_innerLinkMemSet.push_back(m_nLess);

				std::rotate(m_innerKeyMemSet.begin(), std::next(m_innerKeyMemSet.begin(), oldSize), m_innerKeyMemSet.end());
				std::rotate(m_innerLinkMemSet.begin(), std::next(m_innerLinkMemSet.begin(), oldSize), m_innerLinkMemSet.end());

				m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);

				m_nLess = pNode->m_innerLinkMemSet[newSize];

				pNode->m_innerKeyMemSet.resize(newSize);
				pNode->m_innerLinkMemSet.resize(newSize);

				pNode->m_Compressor.recalc(pNode->m_innerKeyMemSet, pNode->m_innerLinkMemSet);
			}
			else
			{

				m_innerKeyMemSet.reserve(m_innerKeyMemSet.size() + nCnt + 1); //1 for less elem
				m_innerLinkMemSet.reserve(m_innerLinkMemSet.size() + nCnt + 1); //1 for less elem

				m_innerKeyMemSet.push_back(LessMin);
				m_innerLinkMemSet.push_back(pNode->m_nLess);


				std::move(pNode->m_innerKeyMemSet.begin(), std::next(pNode->m_innerKeyMemSet.begin(), nCnt - 1), std::inserter(m_innerKeyMemSet, m_innerKeyMemSet.end()));
				std::move(pNode->m_innerLinkMemSet.begin(), std::next(pNode->m_innerLinkMemSet.begin(), nCnt - 1), std::inserter(m_innerLinkMemSet, m_innerLinkMemSet.end()));
				 

				pNode->m_nLess = pNode->m_innerLinkMemSet[nCnt - 1];



				std::rotate(pNode->m_innerKeyMemSet.begin(), std::next(pNode->m_innerKeyMemSet.begin(), nCnt), pNode->m_innerKeyMemSet.end());
				std::rotate(pNode->m_innerLinkMemSet.begin(), std::next(pNode->m_innerLinkMemSet.begin(), nCnt), pNode->m_innerLinkMemSet.end());
				pNode->m_innerKeyMemSet.resize(newSize);
				pNode->m_innerLinkMemSet.resize(newSize);
 

				m_Compressor.recalc(m_innerKeyMemSet, m_innerLinkMemSet);
				pNode->m_Compressor.recalc(pNode->m_innerKeyMemSet, pNode->m_innerLinkMemSet);
			}
			return true;
		}
		template<class TComp>
		bool isKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			return comp.EQ(key, m_innerKeyMemSet[nIndex]);
		}
		void SetMinSplit(bool bMinSplit)
		{
			m_bMinSplit = bMinSplit;
		}
		virtual void clear()
		{
			m_innerKeyMemSet.clear();
			m_innerLinkMemSet.clear();
			m_Compressor.clear();
		}

		bool IsHaveUnion(BPTreeInnerNodeSet *pNode)
		{
			return this->m_Compressor.IsHaveUnion(pNode->m_Compressor);
		}
		bool IsHaveAlignment(BPTreeInnerNodeSet *pNode)
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
		TLink m_nLess;
		TKeyMemSet m_innerKeyMemSet;
		TLinkMemSet m_innerLinkMemSet;
		bool m_bMulti;
		TCompressor  m_Compressor;
		CommonLib::alloc_t *m_pAlloc;
		bool m_bMinSplit;
		uint32 m_nPageSize;
	};
}
