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
		  m_pCompressor(0), m_nLess(-1),  m_innerKeyMemSet(pAlloc), m_innerLinkMemSet(pAlloc)
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
			return m_pCompressor->Load(m_innerMemSet, m_innerLinkMemSet,  stream);
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
				if(m_bMulti)
					nIndex = m_innerKeyMemSet.upper_bound(nIndex, nType, TComporator());
				else
				{
					nIndex = m_innerKeyMemSet.upper_bound(nIndex, nType, TComporator());
					if(nType == FIND_KEY)
					{
						//TO DO logs
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
				nIndex = m_innerKeyMemSet.upper_bound(nIndex, nType, TComporator());
			else
			{
				nIndex = m_innerKeyMemSet.upper_bound(nIndex, nType, TComporator());
			
			}
			if(nType != FIND_KEY)
			{
				return false;
			}

			m_pCompressor->remove(m_innerKeyMemSet[nIndex], m_innerLinkMemSet[nIndex]);
			m_innerKeyMemSet.remove(nIndex);
			m_innerLinkMemSet.remove(nIndex);
			assert(m_pCompressor->count() == m_innerMemSet.size());
			return true;
		}

		bool SplitIn(BPTreeInnerNodeSetv2 *pNode, int32& nMediankey)
		{

			TLeafMemSet& newNodeKeySet = pNode->m_innerKeyMemSet;
			TLeafMemSet& newNodeLinkSet = pNode->m_innerLinkMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			size_t nSize = m_leafMemSet.size()/2;

			//bool bOne = (m_leafMemSet.size() < 3);
			newNodeKeySet.copy(m_innerKeyMemSet, nSize,  m_innerKeyMemSet.size());
			newNodeLinkSet.copy(m_innerLinkMemSet, nSize,  m_innerLinkMemSet.size());
			nMediankey = nSize;
			while(nSize < m_leafMemSet.size())
			{						 
				m_pCompressor->remove(m_innerKeyMemSet[nSize], m_innerLinkMemSet[nSize]);
				pNewNodeComp->insert(m_innerKeyMemSet[nSize], m_innerLinkMemSet[nSize]);
				++nSize;
			}	
			m_innerKeyMemSet.resize(nSize - 1);
			m_innerLinkMemSet.resize(nSize - 1);
			return true;
		}
	public:

		TKeyMemSet m_innerKeyMemSet;
		TLinkMemSet m_innerLinkMemSet;
		bool m_bMulti;
		m_pCompressor;
	};	
}