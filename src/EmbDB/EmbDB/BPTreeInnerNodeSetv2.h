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
	template<typename _TKey,typename typename _TLink, typename _TComp,
	class _Transaction, class _TCompressor>
	class BPTreeInnerNodeSetv2 :   public  BPBaseTreeNode
	{
		typedef _TKey TKey;
		typedef _TLink TLink;
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
			stream.write(m_nLess);
			return m_pCompressor->Write(m_innerKeyMemSet, m_innerLinkMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			 stream.read(m_nLess); 
			return m_pCompressor->Load(m_innerKeyMemSet, m_innerLinkMemSet,  stream);
		}
		size_t tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		TLink upper_bound(const TKey& key, int32& nIndex )
		{
			nIndex = m_innerKeyMemSet.upper_bound(key, m_comp);
			if(nIndex == 0) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}
			return m_innerLinkMemSet[nIndex - 1]; 
		}
		TLink lower_bound(const TKey& key, int32& nIndex )
		{
			short nType = 0;
			nIndex = m_innerKeyMemSet.lower_bound(key, nType, m_comp);
			if(nType == FIND_KEY)
				return m_innerLinkMemSet[nIndex];

			if(nIndex == 0) //меньше всех ключей
			{
				nIndex = -1;
				return m_nLess;
			}

			return m_innerLinkMemSet.[nIndex - 1]; 
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

			m_pCompressor->remove(m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex]);
			m_innerKeyMemSet.remove(nIndex);
			m_innerLinkMemSet.remove(nIndex);
			assert(m_pCompressor->count() == m_innerKeyMemSet.size());
			return true;
		}

		bool SplitIn(BPTreeInnerNodeSetv2 *pNode, TKey* pSplitKey)
		{

			TKeyMemSet& newNodeKeySet = pNode->m_innerKeyMemSet;
			TLinkMemSet& newNodeLinkSet = pNode->m_innerLinkMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			size_t nSize = m_innerKeyMemSet.size()/2;

			//bool bOne = (m_leafMemSet.size() < 3);
			newNodeKeySet.copy(m_innerKeyMemSet, 0, nSize,  m_innerKeyMemSet.size());
			newNodeLinkSet.copy(m_innerLinkMemSet, 0, nSize,  m_innerLinkMemSet.size());
			size_t nNewSize = nSize;
			while(nSize < m_innerKeyMemSet.size())
			{						 
				m_pCompressor->remove(m_innerKeyMemSet[nSize], m_innerLinkMemSet[nSize]);
				pNewNodeComp->insert(m_innerKeyMemSet[nSize], m_innerLinkMemSet[nSize]);
				++nSize;
			}	
			m_innerKeyMemSet.resize(nNewSize);
			m_innerLinkMemSet.resize(nNewSize);
			*pSplitKey = newNodeKeySet[0];
			return true;
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