#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_FIXED_STRING_LEAF_NODE_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_FIXED_STRING_LEAF_NODE_H_
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CommonLibrary/String.h"
#include "Key.h"
#include "RBMap.h"
#include "embDBInternal.h"
#include "BTBaseNode.h"
#include "FixedStringLeafCompressor.h"
#include "BPTreeLeafNode.h"
namespace embDB
{
	template<typename _TKey,  typename _TLink, typename _TComp, typename _TCompressor>
	class BPTreeLeafFixedStringNode : public  BPTreeLeafNode<_TKey, CommonLib::CString, _TLink, _TComp>
	{
	public:

		typedef  BPTreeLeafNode<_TKey, CommonLib::CString, _TLink, _TComp> TBase;
		typedef _TCompressor TCompressor;
		typedef typename TBase::TKey TKey;
		typedef typename TBase::TValue TValue;
		typedef typename TBase::TComp TComp;
		typedef typename TBase::TLink TLink;
		typedef typename TBase::TLeafMemSet TLeafMemSet;
		typedef typename TLeafMemSet::TTreeNode TTreeNode;


		//typedef BPLeafNodeSimpleCompressor<TKey, TValue, _TComp> TCompressor;


		BPTreeLeafFixedStringNode( CommonLib::alloc_t *pAlloc, bool bMulti) :
		TBase()	,m_leafMemSet(pAlloc, bMulti),  m_pCompressor(0)

		{
		
		}
		~BPTreeLeafFixedStringNode()
		{
			if(m_pCompressor)
				delete m_pCompressor;
		}

		virtual bool init()
		{
			assert(!m_pCompressor);
			/*switch(m_nCompID)
			{
			case SIMPLE_COMPRESSOR:
				m_pCompressor = new BPLeafNodeSimpleCompressor<TKey, TValue, TComp>();
				break;
			}*/
			m_pCompressor = new TCompressor();
			//return m_pCompressor != 0;
			return true;
		}

		virtual bool isLeaf() const {return true;}
		virtual uint32 size() const
		{
			assert(m_pCompressor);
			return  2 * sizeof(TLink)  + sizeof(short) +  m_pCompressor->size();
		}

		virtual uint32 headSize() const
		{
			assert(m_pCompressor);
			return 2 * sizeof(TLink) + sizeof(short) +  m_pCompressor->headSize(); 
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
		virtual bool insert( const TKey& key, const TValue& val)
		{
			TTreeNode *pNode = m_leafMemSet.insert(key, val);
			bool bRet = false;
			if(!m_leafMemSet.isNull(pNode))
			{
				bRet = m_pCompressor->insert(pNode);
			}
			return bRet;
		}
		virtual bool update(const TKey& key, const TValue& newValue)
		{
			return true;
		}
		virtual bool remove(const TKey& key)
		{
			TTreeNode* pNode = m_leafMemSet.findNode(key);
			if(m_leafMemSet.isNull(pNode))
				return false;
			 m_pCompressor->remove(pNode);
			 m_leafMemSet.deleteNode(pNode, true, true);
			return true;
		}
		bool SplitIn(BPTreeLeafNode *pNode, TKey* key)
		{
	
			TLeafMemSet& nodeNewTree = pNode->m_leafMemSet;
			TCompressor* pNewNodeComp = pNode->m_pCompressor;
			TTreeNode* pCurr = m_leafMemSet.tree_maximim(m_leafMemSet.root());
			uint32 nSize = m_leafMemSet.size()/2;

			while(nSize)
			{

				m_leafMemSet.deleteNode(pCurr, true, false);
				m_pCompressor->remove(pCurr);

				TTreeNode* pPrev = pCurr->m_pPrev;

				nodeNewTree.insertNode(pCurr);
				pNewNodeComp->insert(pCurr);
				pCurr = pPrev;
				--nSize;
			}
			m_leafMemSet.deleteNode(pCurr, true, false);
			m_pCompressor->remove(pCurr);
			nodeNewTree.insertNode(pCurr);
			pNode->m_pCompressor->insert(pCurr);
			*key = pCurr->m_key;
			return true;
		}
		bool deleteNode(TTreeNode *pNode, bool bDecSize, bool bRemNode)
		{
			 m_pCompressor->remove(pNode);
			 m_leafMemSet.deleteNode(pNode, bDecSize, bRemNode);
			 return true;
		}

		bool AlignmentOf(BPTreeLeafNode* pNode, bool bFromLeft)
		{
			TLeafMemSet& nodeTree = pNode->m_leafMemSet;
			TCompressor* pNodeComp = pNode->m_pCompressor;

			int nCnt = ((m_leafMemSet.size() + nodeTree.size()))/2 - m_leafMemSet.size();
			//assert(nCnt > 0);
			if(nCnt <= 0)
				return false; //оставим все при своих
			if(bFromLeft)
			{
				TTreeNode* pCurr = nodeTree.maximumNode();
				while(nCnt)
				{
				
					nodeTree.deleteNode(pCurr, true, false);
					pNodeComp->remove(pCurr);

					TTreeNode* pPrev = pCurr->m_pPrev;

					m_leafMemSet.insertNode(pCurr);
					m_pCompressor->insert(pCurr);
					pCurr = pPrev;
					--nCnt;
				}
			}
			else
			{
				TTreeNode* pCurr = nodeTree.minimumNode();
				while(nCnt)
				{
					nodeTree.deleteNode(pCurr, true, false);
					pNodeComp->remove(pCurr);
					TTreeNode* pNext = pCurr->m_pNext;
					m_leafMemSet.insertNode(pCurr);
					m_pCompressor->insert(pCurr);
					pCurr = pNext;
					--nCnt;
				}
			}
			return true;
		}
		bool UnionWith(BPTreeLeafNode* pNode)
		{
			TLeafMemSet& nodeTree = pNode->m_leafMemSet;
			uint32 nSize = nodeTree.size();
			TTreeNode* pCurr = nodeTree.minimumNode();
			assert(m_leafMemSet.size() == m_pCompressor->count());
			
			while(nSize)
			{
				TTreeNode* pNext = pCurr->m_pNext;
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
			//if(!m_leafMemSet.isEmpty())
				return m_leafMemSet.minimumNode()->m_key;
			//return TKey();
		}
		uint32 count() const 
		{
			return m_leafMemSet.size();
		}
	public:
		TCompressor * m_pCompressor;
		TLeafMemSet m_leafMemSet;
	};
	
}
#endif