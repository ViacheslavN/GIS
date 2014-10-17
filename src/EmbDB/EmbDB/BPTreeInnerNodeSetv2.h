#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_INNER_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_INNER_NODE_SET_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "IDBTransactions.h"
#include "BTBaseNode.h"
#include "CompressorParams.h"
#include "BPVector.h"
namespace embDB
{
	template<typename _TKey, typename _TComp,
	class _Transaction, class _TCompressor>
	class BPTreeInnerNodeSetv2 :   public  BPBaseTreeNode
	{
		typedef _TKey TKey;
		typedef int64 TLink;
		typedef _Transaction Transaction;
		typedef _TComp		 TComporator;
		typedef _TCompressor TCompressor;
		typedef  TBPVector<TKey> TKeyMemSet;
		typedef  TBPVector<TLink> TLinkMemSet;

	public:
		
		BPTreeInnerNodeSetv2( CommonLib::alloc_t *pAlloc,  bool bMulti) :
		  m_pCompressor(0), m_nLess(-1),  m_innerKeyMemSet(pAlloc), m_innerLinkMemSet(pAlloc), m_bMulti(bMulti)
		{
			
		}

		virtual bool init(ICompressorParams *pParams = NULL)
		{
			assert(!m_pCompressor);
			m_pCompressor = new TCompressor(pParams);
			return true;
		}
		~BPTreeInnerNodeSetv2()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool isLeaf() const {return false;}
		virtual size_t size() const
		{
			assert(m_pCompressor);
			return sizeof(TLink) +  m_pCompressor->size();
		}
		virtual size_t headSize() const
		{
			assert(m_pCompressor);
			return sizeof(TLink) +  m_pCompressor->headSize(); 
		}
		virtual size_t rowSize() const
		{
			assert(m_pCompressor);
			return m_pCompressor->rowSize();
		}
		virtual bool IsFree() const 
		{
			return !(m_nFlag & (ROOT_NODE | BUSY_NODE));
		}
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
		size_t tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		TLink upper_bound(const TKey& key, int32& nIndex )
		{
			nIndex = m_innerKeyMemSet.upper_bound(key, m_comp);
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
		TLink lower_bound(const TKey& key, short& nType, int32& nIndex )
		{
			nIndex = m_innerKeyMemSet.lower_bound(key, nType, m_comp);
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
	

		TLink findNodeForBTree(const TKey& key)
		{
			assert(m_nLess != -1);
			
			int32 nIndex = -1;
			short nType = 0;
			if(m_bMulti)
			{
				nIndex = m_innerKeyMemSet.upper_bound(key, m_comp);
				if(nIndex == 0) //меньше всех ключей
					return m_nLess;
				return m_innerLinkMemSet[nIndex - 1]; 
			}
		
			nIndex  = m_innerKeyMemSet.lower_bound(key, nType, m_comp);
			if(nType == FIND_KEY)
				return m_innerLinkMemSet.[nIndex];

			if(nIndex == 0) //меньше всех ключей
				return m_nLess;
			
			return m_innerLinkMemSet.[nIndex - 1];
		}

		virtual bool insert(const TKey& key, TLink nLink)
		{
			uint32 nIndex = -1;
			short nType = 0;

			if(m_innerKeyMemSet.empty())
			{
				m_innerKeyMemSet.push_back(key);
				m_innerLinkMemSet.push_back(nLink);
			}
			else
			{
				short nType = 0;
				if(m_bMulti)
					nIndex = m_innerKeyMemSet.upper_bound(key, m_comp);
				else
				{
					nIndex = m_innerKeyMemSet.lower_bound(key, nType, m_comp);
					if(nType == FIND_KEY)
					{
						//TO LOGs
						return false;
					}
				}
			
			
				m_innerKeyMemSet.insert(key, nIndex);
				m_innerLinkMemSet.insert(nLink, nIndex);
			}
			
		

			bool bRet = m_pCompressor->insert(key, nLink);
	
			return bRet;
		}
		virtual bool remove(const TKey& key)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if(m_bMulti)
			{
				
				nIndex = m_innerKeyMemSet.upper_bound(key, m_comp);
				if(nIndex && m_comp.EQ(key, m_innerKeyMemSet[nIndex - 1]))
				{
					nType = FIND_KEY;
					nIndex -= 1;
				}
			}
			else
			{
				nIndex = m_innerKeyMemSet.lower_bound(key, nType, m_comp);
			}
	
			if(nType != FIND_KEY)
			{
				return false;
			}
			return removeByIndex(nIndex);
		}


		bool removeByIndex(int32 nIndex)
		{
			m_pCompressor->remove(m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex]);
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
			size_t nSize = m_innerKeyMemSet.size()/2;

			//bool bOne = (m_leafMemSet.size() < 3);
			newNodeKeySet.copy(m_innerKeyMemSet, 0,  nSize + 1,  m_innerKeyMemSet.size());
			newNodeLinkSet.copy(m_innerLinkMemSet, 0, nSize + 1,  m_innerLinkMemSet.size());
			size_t nNewSize = nSize;
			nSize++;
			while(nSize   < m_innerKeyMemSet.size())
			{						 
				m_pCompressor->remove(m_innerKeyMemSet[nSize], m_innerLinkMemSet[nSize]);
				pNewNodeComp->insert(m_innerKeyMemSet[nSize], m_innerLinkMemSet[nSize]);
				++nSize;
			}	

			*pSplitKey = m_innerKeyMemSet[nNewSize];
			pNode->m_nLess = m_innerLinkMemSet[nNewSize];

			m_pCompressor->remove(m_innerKeyMemSet[nNewSize], m_innerLinkMemSet[nNewSize]);
		

			assert(pNode->m_nLess  != -1);

			m_innerKeyMemSet.resize(nNewSize);
			m_innerLinkMemSet.resize(nNewSize);
			
			return true;
		}
		size_t count() const
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
			m_innerLinkMemSet[nIndex] = nLink;
			m_pCompressor->update(m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex]);
		}


		void updateKey(int32 nIndex, const TKey& key)
		{
			m_innerKeyMemSet[nIndex] = key;
			m_pCompressor->update(m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex]);
		}

		bool UnionWith(BPTreeInnerNodeSetv2* pNode,  const TKey& LessMin, bool bLeft)
		{
		

			if(bLeft)
			{
				pNode->m_innerKeyMemSet.push_back(LessMin);
				pNode->m_innerLinkMemSet.push_back(m_nLess);
		

				pNode->m_innerKeyMemSet.push_back(m_innerKeyMemSet);
				pNode->m_innerLinkMemSet.push_back(m_innerLinkMemSet);
							 

				pNode->m_innerLinkMemSet.swap(m_innerLinkMemSet);
				pNode->m_innerKeyMemSet.swap(m_innerKeyMemSet);

				m_nLess = pNode->m_nLess;

				m_pCompressor->recalc(m_innerLinkMemSet, m_innerKeyMemSet);
			}
			else
			{
				m_innerKeyMemSet.push_back(LessMin);
				m_innerLinkMemSet.push_back(pNode->m_nLess);



				m_innerKeyMemSet.push_back(pNode->m_innerKeyMemSet);
				m_innerLinkMemSet.push_back(pNode->m_innerLinkMemSet);

				m_pCompressor->recalc(m_innerLinkMemSet, m_innerKeyMemSet);
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
				m_pCompressor->insert(LessMin, m_nLess);

				size_t oldSize = m_innerKeyMemSet.size();

				m_innerKeyMemSet.mover(0, nCnt );
				m_innerLinkMemSet.mover(0, nCnt);

				m_innerKeyMemSet[nCnt - 1] = LessMin;
				m_innerLinkMemSet[nCnt - 1] = m_nLess;

				size_t newSize = pNode->m_innerLinkMemSet.size() -nCnt;


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
		bool isKey(const TKey& key, uint32 nIndex)
		{
			return m_comp.EQ(key, m_innerKeyMemSet[nIndex]);
		}

	public:
		TLink m_nLess;
		TKeyMemSet m_innerKeyMemSet;
		TLinkMemSet m_innerLinkMemSet;
		bool m_bMulti;
		TComporator m_comp;
		TCompressor *m_pCompressor;
	};	
}

#endif