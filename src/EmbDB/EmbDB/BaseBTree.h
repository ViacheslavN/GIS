#ifndef _EMBEDDED_DATABASE_B_TREE_H_
#define _EMBEDDED_DATABASE_B_TREE_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BTreeNode.h"
#include "IDBTransactions.h"
#include "simple_stack.h"
#include "CacheNodes.h"
#include "DBMagicSymbol.h"
namespace embDB
{
	template <class _TNodeElem, class _TComp,
	class _TBreeNode = BTreeNode<_TNodeElem, _TComp> >
	class TBaseBTree
	{
		public:
			typedef _TNodeElem       TNodeElem ;
			typedef _TComp   TComp;
			typedef RBSet<TNodeElem , TComp>   TMemSet;
			typedef typename TMemSet::TTreeNode TTreeNode;
			typedef _TBreeNode TBTreeNode;

			TBaseBTree(size_t nNodeSize, int64 nPageBTreeInfo, IDBTransactions* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize);
			~TBaseBTree();
			bool insert(const TNodeElem& key);
			bool  update(const TNodeElem& key);
			TNodeElem * search(const TNodeElem& key); //for tests
			bool setRootPage(int64 nPageBTreeInfo, bool bSave);
			int64 getRootAddr();
			int64 getRootPageIofoTree() const {return m_nPageBTreeInfo;}
			//void setStorage(IStorage *pStorage){m_pStorage = pStorage;}
			bool commit();

			
		#include "BTreeIteratorImpl.h"

			save_iterator begin()
			{
				if(!m_pRoot)
				{
					m_pRoot = getNode(m_nRootAddr, true);
					if(!m_pRoot)
						return save_iterator(this, -1, TNodeElem());
				}
				TBTreeNode* pBNode = m_pRoot;
				while(pBNode)
				{
					if(pBNode->m_nLess == -1) //уходим в лево
						break;
					pBNode = getNode(pBNode->m_nLess);
				}
				if(!pBNode)
					return save_iterator(this, -1, TNodeElem ());
				TBTreeNode::TMemSet& nodeTree = pBNode->m_memset;
				TTreeNode* pNode = nodeTree.tree_minimum(nodeTree.root());
				if(!pNode)
				{
					return save_iterator(this,  -1, TNodeElem ());
				}
				return save_iterator(this, pBNode->m_nPageAddr, pNode->key_);
			}
		private:
			TBTreeNode* findNode( TBTreeNode *node, const TNodeElem  &key);
			bool splitNode(const TNodeElem  &key, TBTreeNode *pNode, TBTreeNode *pNewNode, TTreeNode** pMedian, bool bNotInsert = false);
			TBTreeNode *newNode(bool bIsRoot = false);
			TBTreeNode *getNode(int64 nAddr, bool bIsRoot = false, bool bNotMove = false);
			bool InsertInNode(TBTreeNode*pNode, const TNodeElem& key);
			bool loadBTreeInfo();
			bool saveBTreeInfo();
			bool isChangeRoot(){return m_bChangeRoot;}
		private:
			TBTreeNode *m_pRoot; 
			size_t m_nNodeSize;
			int64 m_nRootAddr;
			int64 m_nPageBTreeInfo;
			
			CommonLib::alloc_t* m_pAlloc;
			IDBTransactions* m_pTransaction;
			typedef RBSet<TBTreeNode*> TChangeNode;
			TChangeNode m_ChangeNode;
			size_t m_nChacheSize;
			typedef TSimpleCache<int64, TBTreeNode> TNodesCache;
			TNodesCache m_Chache;
			bool m_bChangeRoot;
	};

	#include "BaseBTree.cpp"
}

#endif