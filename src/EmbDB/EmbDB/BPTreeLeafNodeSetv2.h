#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_V2_LEAF_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "IDBTransactions.h"
#include "BTBaseNode.h"
#include "CompressorParams.h"
namespace embDB
{
	template<typename _TKey, typename typename _TLink, typename _TComp,
	class _Transaction, class _TCompressor>
	class BPTreeLeafNodeSetv2 : public  BPBaseTreeNode
	{
	public:

		typedef _TLink TLink;
		typedef _TKey TKey;
		typedef _Transaction Transaction;
		typedef _TComp		 TComporator;
		typedef _TCompressor TCompressor;
		typedef  TBPVector<TKey> TLeafMemSet;

		BPTreeLeafNodeSetv2( CommonLib::alloc_t *pAlloc, bool bMulti) :
		m_leafMemSet(pAlloc),  m_pCompressor(0),	m_nNext(-1), m_nPrev(-1), m_bMulti(bMulti)

		{
		
		}
		~BPTreeLeafNodeSetv2()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool init(ICompressorParams *pParams = NULL)
		{
			assert(!m_pCompressor);
			m_pCompressor = new TCompressor(pParams);
			return true;
		}

		virtual bool isLeaf() const {return true;}
		virtual size_t size() const
		{
			assert(m_pCompressor);
			return  2 * sizeof(TLink) +  m_pCompressor->size();
		}

		virtual size_t headSize() const
		{
			assert(m_pCompressor);
			return 2 * sizeof(TLink) +  m_pCompressor->headSize(); 
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
			stream.write(m_nNext);
			stream.write(m_nPrev);
			return m_pCompressor->Write(m_leafMemSet, stream);
		}
		virtual bool Load(CommonLib::FxMemoryReadStream& stream)
		{
			stream.read(m_nNext);
			stream.read(m_nPrev); 
			return m_pCompressor->Load(m_leafMemSet, stream);
		}
		bool insert(const TKey& key)
		{
			uint32 nIndex = -1;
			short nType = 0;

			if(m_leafMemSet.empty())
			{
				m_leafMemSet.push_back(key);
			}
			else
			{
				if(m_bMulti)
					nIndex = m_leafMemSet.upper_bound(key, m_comp);
				else
				{
					nIndex = m_leafMemSet.lower_bound(key, nType, m_comp);
					if(nType == FIND_KEY)
					{
						//TO DO logs
						return false;
					}
				}

				m_leafMemSet.insert(key, nIndex);
			}
			bool bRet = m_pCompressor->insert(key);
			return bRet;
		}

		uint32 upper_bound(const TKey& key)
		{
			return  m_leafMemSet.upper_bound(key, m_comp);
		}
		uint32 lower_bound(const TKey& key, short& nType)
		{
			return m_leafMemSet.lower_bound(key, nType, m_comp);
		}
		uint32 binary_search(const TKey& key)
		{
			return m_leafMemSet.binary_search(key, m_comp);
		}

		virtual bool update(const TKey& key)
		{
			return true;
		}
		virtual bool remove(const TKey& key)
		{
			uint32 nIndex = -1;
			short nType = 0;
			if(m_bMulti)
			{
				nIndex = m_leafMemSet.upper_bound(nIndex,  m_comp);
				if(nIndex && m_comp.EQ(key, m_leafMemSet[nIndex - 1]))
				{
					nType = FIND_KEY;
					nIndex -= 1;
				}
			}
			else
				nIndex = m_leafMemSet.lower_bound(nIndex, nType, m_comp);
					
			if(nType != FIND_KEY)
			{
				return false;
			}

			m_pCompressor->remove(m_leafMemSet[nIndex]);
			m_leafMemSet.remove(nIndex);
			return true;
		}
		bool SplitIn(BPTreeLeafNodeSetv2 *pNode, TKey* pSplitKey)
		{
	
			TLeafMemSet& newNodeMemSet = pNode->m_leafMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			size_t nSize = m_leafMemSet.size()/2;
		 
			//bool bOne = (m_leafMemSet.size() < 3);
			newNodeMemSet.copy(m_leafMemSet, 0, nSize, m_leafMemSet.size());
			int32 nNewSize = nSize;
			while(nSize < m_leafMemSet.size())
			{						 
				m_pCompressor->remove(m_leafMemSet[nSize]);
				pNewNodeComp->insert(m_leafMemSet[nSize]);
				++nSize;
			}	
			m_leafMemSet.resize(nNewSize);
			*pSplitKey = newNodeMemSet[0];
			return true;
		}
		size_t count() const 
		{
			return m_leafMemSet.size();
		}
		size_t tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		const TKey& key(uint32 nIndex) const
		{
			return m_leafMemSet[nIndex];
		}
		TKey& key(uint32 nIndex)
		{
			return m_leafMemSet[nIndex];
		}
	public:
		TCompressor * m_pCompressor;
		TLeafMemSet m_leafMemSet;
		TLink m_nNext;
		TLink m_nPrev;
		bool m_bMulti;
		TComporator m_comp;
	};
	
}
#endif