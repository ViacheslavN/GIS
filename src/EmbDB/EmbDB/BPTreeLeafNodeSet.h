#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_LEAF_NODE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_LEAF_NODE_SET_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "Key.h"
#include "RBMap.h"
#include "embDBInternal.h"
#include "BTBaseNode.h"
#include "BPCompressors.h"
#include "BPLeafNodeSimpleCompressor.h"
#include "CompressorParams.h"
namespace embDB
{
	template<typename _TLink,  typename _TCompressor, class _TLeafMemSet >
	class BPTreeLeafNodeSet : public  BPBaseTreeNode
	{
	public:

		typedef _TLeafMemSet TLeafMemSet;
		typedef _TCompressor TCompressor;
		typedef typename TLeafMemSet::TKey TKey;
		typedef _TLink TLink;

		typedef typename TLeafMemSet::TTreeNode TMemSetNode;

		BPTreeLeafNodeSet( CommonLib::alloc_t *pAlloc, bool bMulti) :
		m_leafMemSet(pAlloc, bMulti),  m_pCompressor(0),
		m_nNext(-1), m_nPrev(-1)

		{
		
		}
		~BPTreeLeafNodeSet()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool init(ICompressorParams *pParams = NULL)
		{
			assert(!m_pCompressor);
			/*switch(m_nCompID)
			{
			case SIMPLE_COMPRESSOR:
				m_pCompressor = new BPLeafNodeSimpleCompressor<TKey, TValue, TComp>();
				break;
			}*/
			m_pCompressor = new TCompressor(pParams);
			//return m_pCompressor != 0;
			return true;
		}

		virtual bool isLeaf() const {return true;}
		virtual uint32 size() const
		{
			assert(m_pCompressor);
			return  2 * sizeof(TLink) +  m_pCompressor->size();
		}

		virtual uint32 headSize() const
		{
			assert(m_pCompressor);
			return 2 * sizeof(TLink) +  m_pCompressor->headSize(); 
		}
		virtual uint32 rowSize() const
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
		bool insert( const TKey& key)
		{
			TMemSetNode *pNode = m_leafMemSet.insert(key);
			bool bRet = false;
			if(!m_leafMemSet.isNull(pNode))
			{
				bRet = m_pCompressor->insert(pNode);
			}
			return bRet;
		}
		virtual bool update(const TKey& key)
		{
			return true;
		}
		virtual bool remove(const TKey& key)
		{
			TMemSetNode* pNode = m_leafMemSet.findNode(key);
			if(m_leafMemSet.isNull(pNode))
				return false;
			 m_pCompressor->remove(pNode);
			 m_leafMemSet.deleteNode(pNode, true, true);
			return true;
		}
		bool SplitIn(BPTreeLeafNodeSet *pNode, TKey* key)
		{
	
			TLeafMemSet& nodeNewTree = pNode->m_leafMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			TMemSetNode* pCurr = m_leafMemSet.tree_maximim(m_leafMemSet.root());
			uint32 nSize = m_leafMemSet.size()/2;
			bool bOne = (m_leafMemSet.size() < 3);
			while(nSize)
			{

				m_leafMemSet.deleteNode(pCurr, true, false);
				m_pCompressor->remove(pCurr);

				TMemSetNode* pPrev = pCurr->m_pPrev;

				nodeNewTree.insertNode(pCurr);
				pNewNodeComp->insert(pCurr);
				if(!bOne)
					pCurr = pPrev;
				--nSize;
			}
			if(!bOne)
			{
				m_leafMemSet.deleteNode(pCurr, true, false);
				m_pCompressor->remove(pCurr);
				nodeNewTree.insertNode(pCurr);
				pNode->m_pCompressor->insert(pCurr);
			}
		
			*key = pCurr->m_key;
			return true;
		}
		bool deleteNode(TMemSetNode *pNode, bool bDecSize, bool bRemNode)
		{
			 m_pCompressor->remove(pNode);
			 m_leafMemSet.deleteNode(pNode, bDecSize, bRemNode);
			 return true;
		}

		bool AlignmentOf(BPTreeLeafNodeSet* pNode, bool bFromLeft)
		{
			TLeafMemSet& nodeTree = pNode->m_leafMemSet;
			TCompressor* pNodeComp = pNode->m_pCompressor;

			int nCnt = ((m_leafMemSet.size() + nodeTree.size()))/2 - m_leafMemSet.size();
			//assert(nCnt > 0);
			if(nCnt <= 0)
				return false; //оставим все при своих
			if(bFromLeft)
			{
				TMemSetNode* pCurr = nodeTree.maximumNode();
				while(nCnt)
				{
				
					nodeTree.deleteNode(pCurr, true, false);
					pNodeComp->remove(pCurr);

					TMemSetNode* pPrev = pCurr->m_pPrev;

					m_leafMemSet.insertNode(pCurr);
					m_pCompressor->insert(pCurr);
					pCurr = pPrev;
					--nCnt;
				}
			}
			else
			{
				TMemSetNode* pCurr = nodeTree.minimumNode();
				while(nCnt)
				{
					nodeTree.deleteNode(pCurr, true, false);
					pNodeComp->remove(pCurr);
					TMemSetNode* pNext = pCurr->m_pNext;
					m_leafMemSet.insertNode(pCurr);
					m_pCompressor->insert(pCurr);
					pCurr = pNext;
					--nCnt;
				}
			}
			return true;
		}
		bool UnionWith(BPTreeLeafNodeSet* pNode)
		{
			TLeafMemSet& nodeTree = pNode->m_leafMemSet;
			uint32 nSize = nodeTree.size();
			TMemSetNode* pCurr = nodeTree.minimumNode();
			assert(m_leafMemSet.size() == m_pCompressor->count());
			
			while(nSize)
			{
				TMemSetNode* pNext = pCurr->m_pNext;
				m_leafMemSet.insertNode(pCurr);
				m_pCompressor->insert(pCurr);
				pCurr = pNext;
				--nSize;
			}
			assert(m_leafMemSet.size() == m_pCompressor->count());
			nodeTree.dropTree();
			
			return true;
		}

		TKey& firstKey()
		{
			assert(!m_leafMemSet.isEmpty());
				return m_leafMemSet.minimumNode()->m_key;
		}
		uint32 count() const 
		{
			return m_leafMemSet.size();
		}
		uint32 tupleSize() const
		{
			return m_pCompressor->tupleSize();
		}
		TMemSetNode* lastNode()
		{
			return m_leafMemSet.maximumNode();
		}
		TMemSetNode* firstNode()
		{
			return m_leafMemSet.minimumNode();
		}
		bool isNull(TMemSetNode* pNode )
		{
			return m_leafMemSet.isNull(pNode);
		}
	public:
		TCompressor * m_pCompressor;
		TLeafMemSet m_leafMemSet;
		TLink m_nNext;
		TLink m_nPrev;
	};
	
}
#endif