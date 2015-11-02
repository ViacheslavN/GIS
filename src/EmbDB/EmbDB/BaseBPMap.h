#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_MAP_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeNodeMap.h"
//#include "IDBTransactions.h"
#include "simple_stack.h"
 
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPIteratorMap.h"
#include "BPTree.h"
#include "BaseBPSet.h"
#include "BPMapTraits.h"
namespace embDB
{



	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet = RBMap<_TKey, _TLink, _TComp>,	
	class _TLeafMemSet = RBMap<_TKey, _TValue, _TComp>,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<_TInnerMemSet> ,
	class _TLeafCompess = BPLeafNodeMapSimpleCompressor<_TLeafMemSet>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeMap< _TLink, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeMap<_Transaction, _TInnerNode, _TLeafNode> >
	class TBPlusTreeMap : public TBPlusTreeSet<_TKey, _TLink, _TComp,_Transaction, 
			_TInnerMemSet, _TLeafMemSet, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{
	public:



		typedef TBPlusTreeSet<_TKey, _TLink, _TComp,_Transaction, _TInnerMemSet, _TLeafMemSet, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode> TBase;

		TBPlusTreeMap(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true):
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32)
		{}
		~TBPlusTreeMap()
		{
	
		}

		typedef  _TKey      TKey;
		typedef  _TValue    TValue;
		typedef  _TComp	   TComp;
		typedef  _TLink     TLink;
		typedef	 _Transaction  Transaction;

		typedef  _TBTreeNode  TBTreeNode;
		typedef  _TInnerMemSet TInnerMemSet;  
		typedef  _TLeafMemSet TLeafMemSet; 
		//typedef  TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeftMemSetNode;
		typedef TBase::BPTreeStatisticsInfo BPTreeStatisticsInfo;
		typedef TBPTIteratorMap<TKey, TValue,TLink, TComp,  Transaction,
			TInnerMemSet,TLeafMemSet, _TInnerCompess,  _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode > iterator;


		bool insert(const TKey& key, const TValue& val)
		{

			bool bRet = true;
			if(!m_pRoot)
			{
				if(m_nPageBTreeInfo != -1)
					loadBTreeInfo();
				if(m_nRootAddr == -1)
				{
					m_pRoot = newNode(true, true);
					m_nRootAddr = m_pRoot->m_nPageAddr; 
					FilePagePtr pFilePage(m_pTransaction->getNewPage());
					if(!pFilePage.get())
					{
						m_pTransaction->error(_T("BTREE: Error create new root page"));
						return false;
					}
					m_nRTreeStaticAddr = pFilePage->getAddr();
					m_BTreeInfo.setPage(m_nRTreeStaticAddr);
					saveBTreeInfo();
				}
				else
				{
					m_pRoot = getNode(m_nRootAddr, true); 
					m_nRootAddr = m_pRoot->m_nPageAddr; 
				}	

			}
			if(!m_pRoot)
			{
				m_pTransaction->error(_T("BTREE: Error load root page: %I64d"), (int64)m_nRootAddr);
				return false;
			}
			if(m_pRoot-> isLeaf())
			{
				bRet = InsertInLeafNode(m_pRoot, key, val);
			}
			else
			{
				int64 nNextAddr = m_pRoot->findNodeInsert(key);
				int64 nParentAddr =  m_pRoot->m_nPageAddr;
				while (nNextAddr != -1)
				{
					TBTreeNode* pNode = getNode(nNextAddr);
					pNode->m_nParent = nParentAddr;
					if(pNode->isLeaf())
					{
						bRet = InsertInLeafNode(pNode, key, val);
						break;
					}
					nParentAddr = pNode->m_nPageAddr;
					nNextAddr= pNode->findNodeInsert(key);
				}
			}
			TChangeNode::iterator it = m_ChangeNode.begin();
			for(; !it.isNull(); ++it)
			{
				TBTreeNode* pChNode = *it;
				pChNode->setFlags(BUSY_NODE, false);
			}
			m_ChangeNode.destroyTree();
			ClearChache();
			if(bRet)
				m_BTreeInfo.AddKey(1);
			return bRet;	
		}
		bool InsertInLeafNode(TBTreeNode *pNode, const TKey& key, const TValue& val)
		{
			assert(pNode->isLeaf());
			if(!pNode->insertInLeaf(key, val))
			{
				m_pTransaction->error(_T("BTREE: Error insert"));
				return false;
			}
			pNode->setFlags(CHANGE_NODE |BUSY_NODE, true);

			m_ChangeNode.insert(pNode);
			if(pNode->size() > m_pTransaction->getPageSize())
			{

				TBTreeNode* pParentNode = getNode(pNode->m_nParent);
				bool bNewRoot = false;
				if(!pParentNode)
				{
					//тут все просто создаеться новый рутовый элеинт
					pParentNode  = newNode(true, false);
					if(!pParentNode)
					{
						m_pTransaction->error(_T("BTREE: Error create new root node"));
						return false;
					}
					bNewRoot = true;
				}

				TBTreeNode* pNewLeafNode = newNode(false, true);

				splitLeafNode(pNode, pNewLeafNode, pParentNode);

				pNewLeafNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);

				m_ChangeNode.insert(pNewLeafNode);
				m_ChangeNode.insert(pParentNode);
				if(bNewRoot)
				{
					m_nRootAddr = pParentNode->m_nPageAddr;
					m_pRoot->setFlags(ROOT_NODE|BUSY_NODE, false);
					m_Chache.AddElem(m_pRoot->m_nPageAddr, m_pRoot);
					m_pRoot = pParentNode;
					m_pRoot->setFlags(ROOT_NODE, true);
					m_pRoot->setFlags(CHANGE_NODE|BUSY_NODE, true);
					//m_Chache.remove(m_pRoot);
					saveBTreeInfo();
					return true;
				}

				if(pParentNode->size() > m_pTransaction->getPageSize())
				{
					if(!splitInnerNode(pParentNode))
					{
						return false;
					}
				}

			}
			return true;
		}

		iterator begin()
		{
			TBTreeNode* pFindBTNode = 0;
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return iterator(this, NULL, NULL);

			if(!m_pRoot)
				m_pRoot = getNode(m_nRootAddr, true); 
			if(!m_pRoot)
				return iterator(this, NULL, NULL);
			if(m_pRoot->isLeaf())
			{
				return iterator(this, m_pRoot, NULL);
			}
			int64 nNextAddr = m_pRoot->less();
			for (;;)
			{
				if( nNextAddr == -1)
				{
					break;
				}
				TBTreeNode* pNode = getNode(nNextAddr);
				if(pNode->isLeaf())
				{
					//return iterator(this, pNode, NULL);
					pFindBTNode = pNode;
					break;
				}
				nNextAddr = pNode->less();
			}
			ClearChache();
			return iterator(this, pFindBTNode, NULL);
		}
		iterator find(const TKey& key, bool bFirst = false)
		{
			TBTreeNode* pFindBTNode = 0;
			TLeftMemSetNode *pFindRBNode= 0;

			findNode(key, bFirst, &pFindBTNode, &pFindRBNode);
			return iterator(this, pFindBTNode, pFindRBNode);
		}

		bool update(const TKey& key, const TValue& value )
		{
			iterator it = find(key);
			if(it.isNull())
				return false;
			it.value() = value; //TO DO set node change....
			return true;
		}


		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, const TValue& value, TKey* pKey = NULL)
		{

			if(!m_pRoot)
			{
				if(!checkRoot())
					return false;
			}
			TBTreeNode *pBNode = getLastLeafNode();
			assert(pBNode);
			assert(pBNode->isLeaf());
			TLeftMemSetNode* pMemNode = pBNode->lastLeftMemSetNode();
			bool bRet = true;
			if(pBNode->isLeamMemsetNodeNull(pMemNode))
			{
				TKey key = keyFunctor.begin();
				if(pKey)
					*pKey = key;
				bRet = InsertInLeafNode(pBNode, key, value);
			}
			else
			{
				TKey key = keyFunctor.inc(pMemNode->m_key);
				if(pKey)
					*pKey = key;
				bRet = InsertInLeafNode(pBNode, key, value);
			}
		
			TChangeNode::iterator it = m_ChangeNode.begin();
			for(; !it.isNull(); ++it)
			{
				TBTreeNode* pChNode = *it;
				pChNode->setFlags(BUSY_NODE, false);
			}
			m_ChangeNode.clear();
			ClearChache();
			if(bRet)
				m_BTreeInfo.AddKey(1);
			return bRet;	
		}

	};

	/*
	template<class _TKey, class _TComp ,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<RBMap<_TKey, int64, _TComp> > ,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressor<_TLeafMemSet> ,
	class _Transaction = IDBTransactions, 
	class _TInnerMemSet = RBMap<_TKey, int64, _TComp>,	
	class _TLeafMemSet = RBSet<_TKey, _TComp>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeSet<int64, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeSet< _Transaction, _TInnerNode, _TLeafNode> 
	>
	*/

	/*
	template <class _TKey, class _TValue, class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet = RBMap<_TKey, _TLink, _TComp>,	
	class _TLeafMemSet = RBMap<_TKey, _TValue, _TComp>,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<_TInnerMemSet> ,
	class _TLeafCompess = BPLeafNodeMapSimpleCompressor<_TLeafMemSet>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeMap< _TLink, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeMap<_Transaction, _TInnerNode, _TLeafNode> >
	class TBPlusTreeMap
	
	*/
	template<class _TKey, class _TValue, class _TComp ,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<RBMap<_TKey, int64, _TComp> > ,
	class _TLeafCompess = BPLeafNodeMapSimpleCompressor<RBMap<_TKey, _TValue, _TComp> > ,
	class _Transaction = IDBTransaction, 
	class _TInnerMemSet = RBMap<_TKey, int64, _TComp>,	
	class _TLeafMemSet = RBMap<_TKey, _TValue, _TComp>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeMap< int64, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeMap<_Transaction, _TInnerNode, _TLeafNode> >

	class TBPMap : public TBPlusTreeMap<_TKey, _TValue, int64, _TComp, _Transaction, 
		_TInnerMemSet, _TLeafMemSet, _TInnerCompess, _TLeafCompess,
		_TInnerNode, _TLeafNode,	_TBTreeNode	>
	{
	public:

		typedef  _TBTreeNode  TBTreeNode;
		typedef  _TInnerMemSet TInnerMemSet;  
		typedef  _TLeafMemSet TLeafMemSet; 
		typedef  _TInnerNode TInnerNode; 
		typedef  _TLeafNode TLeafNode; 
		typedef  _TBTreeNode TBTreeNode; 
		typedef typename TLeafMemSet::TTreeNode TLeftMemSetNode;

		typedef TBPlusTreeMap<_TKey, _TValue, int64, _TComp, _Transaction,
			_TInnerMemSet, _TLeafMemSet, _TInnerCompess , _TLeafCompess,
			BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>, 	BPTreeLeafNodeMap<int64, _TLeafCompess, _TLeafMemSet>,	
			BPTreeNodeMap< _Transaction, _TInnerNode, _TLeafNode> > TMapBase;


		TBPMap(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false) :
			TMapBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti)
		{

		}
	};
}
#endif