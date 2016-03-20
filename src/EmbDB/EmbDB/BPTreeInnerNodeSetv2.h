#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_INNER_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_INNER_NODE_SET_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "embDBInternal.h"

#include "CompressorParams.h"
#include "BPVector.h"
#include "BTBaseNode.h"
namespace embDB
{
	template<typename _TKey,/* typename _TComp,*/
	class _Transaction, class _TCompressor,
	class _TKeyMemSet = TBPVector<_TKey> >
	class BPTreeInnerNodeSetv2 :   public  BPBaseTreeNode 
	{
		typedef _TKey TKey;
		typedef int64 TLink;
		typedef _Transaction Transaction;
		//typedef _TComp		 TComporator;
		typedef _TCompressor TCompressor;
		typedef _TKeyMemSet TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;


		
				
	public:

		typedef typename _TCompressor::TInnerCompressorParams TInnerCompressorParams;
		
		BPTreeInnerNodeSetv2( CommonLib::alloc_t *pAlloc,  bool bMulti, uint32 nPageSize) :
		  m_pCompressor(0), m_nLess(-1),  m_innerKeyMemSet(pAlloc), m_innerLinkMemSet(pAlloc), m_bMulti(bMulti),
			  m_pAlloc(pAlloc), m_bOneSplit(false), m_nPageSize(nPageSize)
		{
			
		}

		virtual bool init(TInnerCompressorParams *pParams = NULL, Transaction* pTransaction = NULL )
		{
			assert(!m_pCompressor);
			m_pCompressor = new TCompressor(m_nPageSize - sizeof(TLink), &m_innerKeyMemSet, &m_innerLinkMemSet, m_pAlloc, pParams);
			return true;
		}
		~BPTreeInnerNodeSetv2()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool isLeaf() const {return false;}
		virtual uint32 size() const
		{
			assert(m_pCompressor);
			return sizeof(TLink) +  m_pCompressor->size();
		}
		virtual bool isNeedSplit() const
		{
			assert(m_pCompressor);
			return m_pCompressor->isNeedSplit();
		}
		virtual uint32 headSize() const
		{
			assert(m_pCompressor);
			return sizeof(TLink) +  m_pCompressor->headSize(); 
		}
		virtual uint32 rowSize() const
		{
			assert(m_pCompressor);
			return m_pCompressor->rowSize();
		}
		/*virtual bool IsFree() const 
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}*/
		virtual  bool Save(	CommonLib::FxMemoryWriteStream& stream) 
		{
			assert(m_nLess != -1);
			stream.write(m_nLess);
			return m_pCompressor->Write(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			 stream.read(m_nLess); 
			 assert(m_nLess != -1);
			return m_pCompressor->Load(m_innerKeyMemSet, m_innerLinkMemSet,  stream);
		}
		uint32 tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}

		template<class TComp>
		TLink upper_bound(const TComp& comp, const TKey& key, int32& nIndex )
		{
			nIndex = m_innerKeyMemSet.upper_bound(key, comp);
			if(nIndex == -1)
				return -1;
			if(nIndex == 0) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			nIndex--;
			return m_innerLinkMemSet[nIndex]; 
		}
		template<class TComp>
		TLink lower_bound(const TComp& comp, const TKey& key, short& nType, int32& nIndex )
		{
			nIndex = m_innerKeyMemSet.lower_bound(key, nType, comp);
			if(nIndex == -1)
				return -1;
			if(nType == FIND_KEY)
				return m_innerLinkMemSet[nIndex];

			if(nIndex == 0) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			//	if(nIndex == m_innerKeyMemSet.size())
			nIndex--;
			//nIndex--;
			return m_innerLinkMemSet[nIndex]; 
		}
		
		template<class TComp>
		int insert(const TComp& comp, const TKey& key, TLink nLink)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if(m_innerKeyMemSet.empty())
			{
				m_innerKeyMemSet.push_back(key);
				m_innerLinkMemSet.push_back(nLink);
				nIndex = 0;
			}
			else
			{
				short nType = 0;
				if(m_bMulti)
					nIndex = m_innerKeyMemSet.upper_bound(key, comp);
				else
				{
					nIndex = m_innerKeyMemSet.lower_bound(key, nType, comp);
					if(nType == FIND_KEY)
					{
						//TO LOGs
						return false;
					}
				}
			
			
				m_innerKeyMemSet.insert(key, nIndex);
				m_innerLinkMemSet.insert(nLink, nIndex);
			}
			
		

			bool bRet = m_pCompressor->insert(nIndex, key, nLink);
	
			return bRet ? nIndex : -1;
		}
		template<class TComp>
		bool remove(const TComp& comp, const TKey& key)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if(m_bMulti)
			{
				
				nIndex = m_innerKeyMemSet.upper_bound(key, comp);
				if(nIndex && comp.EQ(key, m_innerKeyMemSet[nIndex - 1]))
				{
					nType = FIND_KEY;
					nIndex -= 1;
				}
			}
			else
			{
				nIndex = m_innerKeyMemSet.lower_bound(key, nType, comp);
			}
	
			if(nType != FIND_KEY)
			{
				return false;
			}
			return removeByIndex(nIndex);
		}


		bool removeByIndex(int32 nIndex)
		{
			m_pCompressor->remove(nIndex, m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex]);
			bool bRet = m_innerKeyMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				assert(false);
				return false;
			}
			bRet =  m_innerLinkMemSet.remove(nIndex);
			if(!bRet)
			{
				//TO DO Logs
				assert(false);
				return false;
			}
		
			return true;
		}

		bool SplitIn(BPTreeInnerNodeSetv2 *pNode, TKey* pSplitKey)
		{

			TKeyMemSet& newNodeKeySet = pNode->m_innerKeyMemSet;
			TLinkMemSet& newNodeLinkSet = pNode->m_innerLinkMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;

			if(m_bOneSplit)
			{
				uint32 nNewSize = m_innerKeyMemSet.size() - 2;
				uint32 nLessIndex = m_innerKeyMemSet.size() - 2;
				uint32 nSplitIndex = m_innerKeyMemSet.size() - 1;


				newNodeKeySet.push_back(m_innerKeyMemSet[nSplitIndex]);
				newNodeLinkSet.push_back(m_innerLinkMemSet[nSplitIndex]);


				*pSplitKey =  m_innerKeyMemSet[nLessIndex];
				pNode->m_nLess = m_innerLinkMemSet[nLessIndex];

	
				m_pCompressor->remove(nSplitIndex, m_innerKeyMemSet[nSplitIndex], m_innerLinkMemSet[nSplitIndex]);

				m_innerKeyMemSet.resize(nNewSize + 1);
				m_innerLinkMemSet.resize(nNewSize + 1);

				m_pCompressor->remove(nLessIndex, m_innerKeyMemSet[nLessIndex],  m_innerLinkMemSet[nLessIndex]);
				

				pNewNodeComp->insert(0, newNodeKeySet[0], newNodeLinkSet[0]);


				m_innerKeyMemSet.resize(nNewSize);
				m_innerLinkMemSet.resize(nNewSize);

			}
			else
			{
				uint32 nSize = m_innerKeyMemSet.size()/2;

				newNodeKeySet.copy(m_innerKeyMemSet, 0,  nSize + 1,  m_innerKeyMemSet.size());
				newNodeLinkSet.copy(m_innerLinkMemSet, 0, nSize + 1,  m_innerLinkMemSet.size());

				m_pCompressor->SplitIn(nSize + 1, m_innerKeyMemSet.size(), pNewNodeComp);

				uint32 nNewSize = nSize;


				*pSplitKey = m_innerKeyMemSet[nNewSize];
				pNode->m_nLess = m_innerLinkMemSet[nNewSize];

				m_pCompressor->remove(nNewSize, m_innerKeyMemSet[nNewSize], m_innerLinkMemSet[nNewSize]);


				assert(pNode->m_nLess  != -1);

				m_innerKeyMemSet.resize(nNewSize);
				m_innerLinkMemSet.resize(nNewSize);
				m_pCompressor->recalc(m_innerKeyMemSet, m_innerLinkMemSet);


				pNewNodeComp->recalc(newNodeKeySet, newNodeLinkSet);
			}

		
			
			return true;
		}

		bool SplitIn(BPTreeInnerNodeSetv2 *pLeftNode, BPTreeInnerNodeSetv2 *pRightNode, TKey* pSplitKey)
		{

			TKeyMemSet& LeftKeySet = pLeftNode->m_innerKeyMemSet;
			TLinkMemSet& LeftLinkSet = pLeftNode->m_innerLinkMemSet;
			TCompressor* pLeftNodeComp = pLeftNode->m_pCompressor;

			TKeyMemSet& RightKeySet = pRightNode->m_innerKeyMemSet;
			TLinkMemSet& RightLinkSet = pRightNode->m_innerLinkMemSet;
			TCompressor* pRightNodeComp = pRightNode->m_pCompressor;

			int nSize = m_innerKeyMemSet.size()/2;

			LeftKeySet.copy(m_innerKeyMemSet, 0,  0,  nSize);
			LeftLinkSet.copy(m_innerLinkMemSet, 0,  0,  nSize);
			pLeftNode->m_nLess = m_nLess;
		 
			pLeftNodeComp->recalc(LeftKeySet, LeftLinkSet);

			*pSplitKey = m_innerKeyMemSet[nSize];
			pRightNode->m_nLess = m_innerLinkMemSet[nSize];


			RightKeySet.copy(m_innerKeyMemSet, 0,  nSize + 1,  m_innerKeyMemSet.size());
			RightLinkSet.copy(m_innerLinkMemSet, 0,  nSize + 1,  m_innerKeyMemSet.size());
			pRightNodeComp->recalc(RightKeySet, RightLinkSet);


			m_innerKeyMemSet.clear();
			m_innerLinkMemSet.clear();
			m_pCompressor->clear();
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
			m_pCompressor->update(nIndex, m_innerKeyMemSet[nIndex], nLink);
			m_innerLinkMemSet[nIndex] = nLink;
			
		}


		void updateKey(int32 nIndex, const TKey& key)
		{
			m_pCompressor->update(nIndex, key, m_innerLinkMemSet[nIndex]);
			m_innerKeyMemSet[nIndex] = key;
			
		}

		bool UnionWith(BPTreeInnerNodeSetv2* pNode,  const TKey* pLessMin, bool bLeft)
		{
		

			if(bLeft)
			{
				if(pLessMin) //can be if root is empty
				{
					pNode->m_innerKeyMemSet.push_back(*pLessMin);
					pNode->m_innerLinkMemSet.push_back(m_nLess);
				}
	
		

				pNode->m_innerKeyMemSet.push_back(m_innerKeyMemSet);
				pNode->m_innerLinkMemSet.push_back(m_innerLinkMemSet);
							 

				pNode->m_innerLinkMemSet.swap(m_innerLinkMemSet);
				pNode->m_innerKeyMemSet.swap(m_innerKeyMemSet);

				m_nLess = pNode->m_nLess;

				m_pCompressor->recalc(m_innerKeyMemSet, m_innerLinkMemSet);
			}
			else
			{
				if(pLessMin)
				{
					m_innerKeyMemSet.push_back(*pLessMin);
					m_innerLinkMemSet.push_back(pNode->m_nLess);
				}
		



				m_innerKeyMemSet.push_back(pNode->m_innerKeyMemSet);
				m_innerLinkMemSet.push_back(pNode->m_innerLinkMemSet);

				m_pCompressor->recalc(m_innerKeyMemSet, m_innerLinkMemSet);
			}
			return true;
		}
		bool AlignmentOf(BPTreeInnerNodeSetv2* pNode,  const TKey& LessMin, bool bLeft)
		{
			int nCnt = ((m_innerKeyMemSet.size() + pNode->m_innerKeyMemSet.size() ))/2 - m_innerKeyMemSet.size();
			//assert(nCnt > 0);
			if(nCnt < 2 && !m_innerKeyMemSet.empty())
				return false; //оставим все при своих

	 
			if(bLeft)
			{
				

				uint32 oldSize = m_innerKeyMemSet.size();

				m_innerKeyMemSet.mover(0, nCnt );
				m_innerLinkMemSet.mover(0, nCnt);

				m_innerKeyMemSet[nCnt - 1] = LessMin;
				m_innerLinkMemSet[nCnt - 1] = m_nLess;

				//m_pCompressor->insert(nCnt - 1, LessMin, m_nLess);

				uint32 newSize = pNode->m_innerLinkMemSet.size() -nCnt;


				m_innerKeyMemSet.copy(pNode->m_innerKeyMemSet, 0, newSize + 1, pNode->m_innerKeyMemSet.size());
				m_innerLinkMemSet.copy(pNode->m_innerLinkMemSet, 0, newSize + 1, pNode->m_innerLinkMemSet.size());

				m_pCompressor->recalc(m_innerKeyMemSet, m_innerLinkMemSet);

				m_nLess = pNode->m_innerLinkMemSet[newSize];

				pNode->m_innerKeyMemSet.resize(newSize);
				pNode->m_innerLinkMemSet.resize(newSize);

				pNode->m_pCompressor->recalc(pNode->m_innerKeyMemSet, pNode->m_innerLinkMemSet);
			}
			else
			{
				m_innerKeyMemSet.push_back(LessMin);
				m_innerLinkMemSet.push_back(pNode->m_nLess);

				m_innerKeyMemSet.copy(pNode->m_innerKeyMemSet, m_innerKeyMemSet.size(), 0, nCnt - 1);
				m_innerLinkMemSet.copy(pNode->m_innerLinkMemSet, m_innerLinkMemSet.size(), 0, nCnt - 1);

				pNode->m_nLess = pNode->m_innerLinkMemSet[nCnt - 1];

				pNode->m_innerKeyMemSet.movel(nCnt , nCnt );
				pNode->m_innerLinkMemSet.movel(nCnt , nCnt );

				m_pCompressor->recalc(m_innerKeyMemSet, m_innerLinkMemSet);
				pNode->m_pCompressor->recalc(pNode->m_innerKeyMemSet, pNode->m_innerLinkMemSet);
			}
			return true;
		}
		template<class TComp>
		bool isKey(TComp& comp, const TKey& key, uint32 nIndex)
		{
			return comp.EQ(key, m_innerKeyMemSet[nIndex]);
		}
		void SetOneSplit(bool bOneSplit )
		{
			m_bOneSplit = bOneSplit;
		}
		virtual void clear()
		{
			m_innerKeyMemSet.clear();
			m_innerLinkMemSet.clear();
			delete m_pCompressor;
			m_pCompressor = NULL;

		}



	 


		bool IsHaveUnion(BPTreeInnerNodeSetv2 *pNode)
		{
			return m_pCompressor->IsHaveUnion(pNode->m_pCompressor);
		}
		bool IsHaveAlignment(BPTreeInnerNodeSetv2 *pNode)
		{
			return m_pCompressor->IsHaveAlignment(pNode->m_pCompressor);
		}
		bool isHalfEmpty() const
		{
			return m_pCompressor->isHalfEmpty();
		}

	public:
		TLink m_nLess;
		TKeyMemSet m_innerKeyMemSet;
		TLinkMemSet m_innerLinkMemSet;
		bool m_bMulti;
		//TComporator m_comp;
		TCompressor *m_pCompressor;
		CommonLib::alloc_t *m_pAlloc;
		bool m_bOneSplit;
		uint32 m_nPageSize;
	};	
}

#endif