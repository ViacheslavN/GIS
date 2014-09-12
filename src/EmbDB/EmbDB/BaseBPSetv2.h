#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_V2_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeNodeSetV2.h"
#include "IDBTransactions.h"
#include "simple_stack.h"
#include "CacheNodes.h"
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPInnerNodeSimpleCompressorV2.h"
#include "BPLeafNodeSetSimpleCompressorV2.h"
#include "BPTree.h"
#include "CompressorParams.h"
#include "BPIteratorSetV2.h"
namespace embDB
{


	/*template <class _TKey>
	class CKeyFunctor
	{
		_TKey inc(const _TKey& key);
		_TKey dec(const _TKey& key);
	};
	*/

	/*enum eBPTState
	{
		eBPTNoChange = 1,
		eBPTChangeLeafNode = 2,
		eBPTNewLeafNode =4,
		eBPTDeleteLeafNode = 8,
		eBPTNewInnerNode =16,
		eBPTDeleteInnerNode =32,
		eBPTNewRootNode =64
	};*/


	template <	class _TKey, class _TComp, class _Transaction,
	class _TInnerCompess = BPInnerNodeSimpleCompressorV2<_TKey>,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressorV2<_TKey>,  
	class _TInnerNode = BPTreeInnerNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeSetv2<_TKey,  _TComp, _Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
	class TBPlusTreeSetV2 /*: public IBPTree<_TKey, _TValue>*/
	{
	public:

		enum eBPTState
		{
			eBPTNoChange = 1,
			eBPTChangeLeafNode = 2,
			eBPTNewLeafNode =4,
			eBPTDeleteLeafNode = 8,
			eBPTNewInnerNode =16,
			eBPTDeleteInnerNode =32,
			eBPTNewRootNode =64
		};


		TBPlusTreeSetV2(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
		  m_nPageBTreeInfo(nPageBTreeInfo), m_pTransaction(pTransaction), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
		 ,m_bChangeRoot(false), m_nRootAddr(-1), m_bMulti(bMulti)
		 ,m_Chache(pAlloc)
		 ,m_ChangeNode(pAlloc)
		 ,m_pRoot(NULL)
		 ,m_nRTreeStaticAddr(-1)
		 ,m_nPageInnerCompInfo(-1)
		 ,m_nPageLeafPageCompInfo(-1)
		 ,m_bCheckCRC32(bCheckCRC32)
		 ,m_BTreeInfo(bCheckCRC32)
		 ,m_nStateTree(eBPTNoChange)
		{}
		~TBPlusTreeSetV2()
		{
			TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.value().pListEl->obj_;
				assert(pBNode != m_pRoot);
				delete pBNode;
				it.next();
			}
			if(m_pRoot)
			{
				delete m_pRoot;
			}
		}
		//typedef _Traits Traits;
		typedef _TKey      TKey;
		typedef _TComp	   TComp;
		typedef int64     TLink;
		typedef _Transaction  Transaction;
		typedef _TInnerCompess  TInnerCompess;
		typedef _TLeafCompess  TLeafCompess;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode	TLeafNode;
		typedef _TBTreeNode TBTreeNode;
		//typedef _TIerator iterator;
 
		typedef CompressorParamsBase<Transaction> TCompressorParamsBase;
		typedef BPTreeStatistics<int64, _Transaction, _TKey> BPTreeStatisticsInfo;
 
/*
		typedef TBPSetIteratorV2<TKey, TComp,Transaction, TInnerCompess, TLeafCompess,
		TInnerNode, TLeafNode, TBTreeNode> iterator;*/
		  

		void setTransactions(Transaction *pTransaction)
		{
			m_pTransaction  = pTransaction;
		}

		void clear(bool bNotSetFreePage = true)
		{
			TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.value().pListEl->obj_;
				delete pBNode;
				it.next();
			}
			m_Chache.m_set.clear();
			m_Chache.clear();
			if(m_pRoot)
			{
				if(bNotSetFreePage)
				{
					//TO DO нужно удалять или как то пометить свободные файловые страницы
				}
				

				deleteNode(m_pRoot);
				m_pRoot = newNode(true, true);
				m_pRoot->m_nParent = -1;
				m_pRoot->setFlags(ROOT_NODE, true);
				m_pRoot->Save(m_pTransaction);
				m_BTreeInfo.clear();
				m_BTreeInfo.Save(m_pTransaction);
				saveBTreeInfo();
			}
			m_nStateTree |=(eBPTDeleteLeafNode | eBPTDeleteInnerNode |  eBPTNewRootNode);
		}

		void setRootPage(int64 nPageBTreeInfo)
		{
			m_nPageBTreeInfo = nPageBTreeInfo;
		}
		bool  loadBTreeInfo()
		{
			if(m_nPageBTreeInfo == -1)
			{
				m_pTransaction->error(_T("BTREE: Error Load  BTreeInfoPage: -1"));
				return false;
			}
			CFilePage * pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo);
			if(!pPage)
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Error load BTreeInfoPage: %I64d"), (int64)m_nPageBTreeInfo);
				m_pTransaction->error(sMsg);
				return false;
			}

			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());

			sFilePageHeader header(stream, m_bCheckCRC32 && !pPage->isCheck());
			if(m_bCheckCRC32 && !pPage->isCheck() && !header.isValid())
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Page %I64d Error CRC for info page"), (int64)m_nPageBTreeInfo);
				m_pTransaction->error(sMsg);
				return false;
			}
			pPage->setCheck(true);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_INFO_PAGE)
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Page %I64d is not BTreeInfoPage"), (int64)m_nPageBTreeInfo);
				m_pTransaction->error(sMsg);
				return false;
			}
			m_nRootAddr = stream.readInt64();
			m_nRTreeStaticAddr = stream.readInt64();
			m_bMulti = stream.readBool();
			m_nPageInnerCompInfo = stream.readInt64();
			m_nPageLeafPageCompInfo = stream.readInt64();
			if(m_nRTreeStaticAddr != -1)
			{
				m_BTreeInfo.setPage(m_nRTreeStaticAddr);
				m_BTreeInfo.Load(m_pTransaction);
			}
			return true;
		}


		bool saveBTreeInfo()
		{
			CFilePage * pPage = NULL;
			if(m_nPageBTreeInfo == -1)
			{
				pPage = m_pTransaction->getNewPage();
				if(pPage)
					m_nPageBTreeInfo = pPage->getAddr();
				if(m_nPageBTreeInfo == -1)
				{
					CommonLib::str_t sMsg;
					sMsg.format(_T("BTREE: Error save BTreeInfoPage: -1"), (int64)m_nPageBTreeInfo);
					m_pTransaction->error(sMsg);
					return false;
				}
			}
			else
			{
				pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo, false);
				if(!pPage)
				{
					CommonLib::str_t sMsg;
					sMsg.format(_T("BTREE: Error save BTreeInfoPage: %I64d is not load"), (int64)m_nPageBTreeInfo);
					m_pTransaction->error(sMsg);
					return false;
				}
			}
					
			CommonLib::FxMemoryWriteStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_INFO_PAGE);
			//stream.write((int64)DB_BTREE_INFO_PAGE); 
			stream.write(m_nRootAddr);
			stream.write(m_nRTreeStaticAddr);
			stream.write(m_bMulti);
			stream.write(m_nPageInnerCompInfo);
			stream.write(m_nPageLeafPageCompInfo);
			if(m_bCheckCRC32)
				header.writeCRC32(stream);
			m_pTransaction->saveFilePage(pPage);
			pPage->setCheck(true);
			return true;
		}
		TLink getPageBTreeInfo() const 
		{
			return m_nPageBTreeInfo;
		}
		void setPageBTreeInfo(TLink PageBTreeInfo)   
		{
			m_nPageBTreeInfo = PageBTreeInfo;
		}
		void setCompPageInfo(int64 nInnerPageInfo, int64 nLeafPageInfo)
		{
			m_nPageInnerCompInfo = nInnerPageInfo;
			m_nPageLeafPageCompInfo = nLeafPageInfo;
			saveBTreeInfo();
		}
		bool commit()
		{
			if(!m_pRoot)
				return true;
			TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.value().pListEl->obj_;
				if(pBNode->getFlags() & CHANGE_NODE)
				{
					pBNode->Save(m_pTransaction);
					pBNode->setFlags(CHANGE_NODE, false);
				}
				it.next();
			}
			if(m_pRoot->getFlags() & CHANGE_NODE)
			{
				m_pRoot->Save(m_pTransaction);
				m_pRoot->setFlags(CHANGE_NODE, false);
			}
			m_BTreeInfo.Save(m_pTransaction);
			m_nStateTree = eBPTNoChange;
			return true;
		}
		
		TBTreeNode *newNode(bool bIsRoot, bool bIsLeaf)
		{
				TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, -1, m_bMulti, bIsLeaf, m_bCheckCRC32, (ICompressorParams *)m_InnerCompParams.get(),
					(ICompressorParams *)m_LeafCompParams.get() );
				pNode->Load(m_pTransaction);
				m_BTreeInfo.AddNode(1, bIsLeaf);
				if(bIsRoot)
					pNode->setFlags(ROOT_NODE, true);
				else
					m_Chache.AddElem(pNode->m_nPageAddr, pNode);
				return pNode;
		}

	TBTreeNode* getNode(TLink nAddr, bool bIsRoot = false, bool bNotMove = false, bool bCheckCache = false)
	{

		   if(nAddr == -1)
				return NULL;
			if(nAddr == m_nRootAddr && m_pRoot)
			{
				return m_pRoot;
			}
			TBTreeNode *pBNode = m_Chache.GetElem(nAddr, bNotMove);
			if(!pBNode)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(nAddr);
				assert(pFilePage);
				if(!pFilePage)
				{
					return NULL;
				}
				pBNode = new TBTreeNode(-1, m_pAlloc, nAddr, m_bMulti, false, m_bCheckCRC32, (ICompressorParams *)m_InnerCompParams.get(), (ICompressorParams *)m_LeafCompParams.get());
				if(!pBNode->LoadFromPage(pFilePage, m_pTransaction))
				{
					return NULL;
				}
				if(bCheckCache)
				{
					if(m_Chache.size() > m_nChacheSize)
					{
						TBTreeNode *pDelNode = m_Chache.remove_back();
						if(pDelNode)
						{
							if(pDelNode->getFlags() & CHANGE_NODE)
								pDelNode->Save(m_pTransaction);
							delete pDelNode;
						}
					}
				}
				if(bIsRoot)
					pBNode->setFlags(ROOT_NODE, true);
				else
					m_Chache.AddElem(pBNode->m_nPageAddr, pBNode, bNotMove);
			}
			return pBNode;
	}
	bool deleteNode(TBTreeNode* pNode)
	{
		m_BTreeInfo.AddNode(-1, pNode->isLeaf());
		m_Chache.remove(pNode->m_nPageAddr);
		m_pTransaction->dropFilePage(pNode->m_nPageAddr);
		m_ChangeNode.remove(pNode);
		delete pNode;
		return true;
	}
	void ClearChache()
	{
		TChangeNode::iterator it = m_ChangeNode.begin();
		for(; !it.isNull(); ++it)
		{
			TBTreeNode* pChNode = *it;
			pChNode->setFlags(BUSY_NODE, false);
		}
		m_ChangeNode.clear();

		if(m_Chache.size() <= m_nChacheSize)
			return;
		for (size_t i = 0, sz = m_Chache.size(); i < sz - m_nChacheSize; i++)
		{
			TBTreeNode* pDelNode = m_Chache.remove_back();//remove(pChNode->m_nPageAddr);
			if(pDelNode)
			{
				if( pDelNode->getFlags() & CHANGE_NODE)
					pDelNode->Save(m_pTransaction);
				delete pDelNode;
			}
		}
	}

	bool init()
	{
		if(m_pRoot || m_nRootAddr != -1)
			return false;
		if(m_nPageBTreeInfo == -1)
			return false;
		return createRootPage();
	}
	bool createRootPage()
	{
		m_pRoot = newNode(true, true);
		m_nRootAddr = m_pRoot->m_nPageAddr; 
		CFilePage *pFilePage = m_pTransaction->getNewPage();
		if(!pFilePage)
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("BTREE: Error create new static page"));
			m_pTransaction->error(sMsg);
			return false;
		}
		m_nRTreeStaticAddr = pFilePage->getAddr();
		m_BTreeInfo.setPage(m_nRTreeStaticAddr);
		if(!saveBTreeInfo())
			return false;
		m_pRoot->Save(m_pTransaction);
		m_BTreeInfo.Save(m_pTransaction);
		return true;
	}
	bool checkRoot()
	{
		if(m_pRoot)
			return true;

		if(m_nPageBTreeInfo != -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
		{
			if(!createRootPage())
				return false;
		}
		else
		{
			m_pRoot = getNode(m_nRootAddr, true); 
			m_nRootAddr = m_pRoot->m_nPageAddr; 
		}	
		if(!m_pRoot)
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("BTREE: Error load root page: %I64d"), (int64)m_nRootAddr);
			m_pTransaction->error(sMsg);
			return false;
		}
		return true;
	}


	TBTreeNode* findLeafNodeForInsert(const TKey& key)
	{
		if(!m_pRoot)
		{
			if(!checkRoot())
				return NULL;
		}
		if(m_pRoot-> isLeaf())
		{
			return m_pRoot;
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
					return pNode;
					break;
				}
				nParentAddr = pNode->m_nPageAddr;
				nNextAddr= pNode->findNodeInsert(key);
			}
		}

		return NULL;
	}

	bool insert(const TKey& key)
	{
		/*if(!m_pRoot)
		{
			if(!checkRoot())
				return false;
		}
			
		bool bRet = true;
		
			if(m_pRoot-> isLeaf())
			{
				bRet = InsertInLeafNode(m_pRoot, key);
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
						bRet = InsertInLeafNode(pNode, key);
						break;
					}
					nParentAddr = pNode->m_nPageAddr;
					nNextAddr= pNode->findNodeInsert(key);
				}
			}*/
		
			bool bRet = false;
			TBTreeNode* pNode = findLeafNodeForInsert(key);
			if(pNode)
			{
				bRet = InsertInLeafNode(pNode, key);
			}
			ClearChache();
			if(bRet)
				m_BTreeInfo.AddKey(1);
			return bRet;	
		}
		template<class TIterator, class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, TKey* pKey = NULL)
		{

			TIterator it = last();
			if(!it.m_pCurLeafNode)
				return false;
			
			TKey key;
			if(it.m_nIndex == -1)
				key = keyFunctor.begin();
			else
				key = keyFunctor.inc(it.key());

			if(pKey)
				*pKey = key;
			bool bRet = InsertInLeafNode(pBNode, key);
			ClearChache();
			if(bRet)
				m_BTreeInfo.AddKey(1);
			return bRet;	
		}
		bool InsertInLeafNode(TBTreeNode *pNode, const TKey& key)
		{
			assert(pNode->isLeaf());
			if(!pNode->insertInLeaf(key))
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Error insert"));
				m_pTransaction->error(sMsg);
				return false;
			}
			pNode->setFlags(CHANGE_NODE |BUSY_NODE, true);
			
			m_ChangeNode.insert(pNode);
			m_nStateTree |= eBPTChangeLeafNode;
		
			return CheckLeafNode(pNode);
		}
		bool CheckLeafNode(TBTreeNode *pNode)
		{
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
						CommonLib::str_t sMsg;
						sMsg.format(_T("BTREE: Error create new root node"));
						m_pTransaction->error(sMsg);
						return false;
					}
					bNewRoot = true;
				}

				m_nStateTree |= eBPTNewLeafNode;

				TBTreeNode* pNewLeafNode = newNode(false, true);

				splitLeafNode(pNode, pNewLeafNode, pParentNode);

				pNewLeafNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);

				m_ChangeNode.insert(pNewLeafNode);
				m_ChangeNode.insert(pParentNode);
				if(bNewRoot)
				{
					m_nStateTree |= eBPTNewRootNode;
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
		bool splitLeafNode(TBTreeNode *pNode, TBTreeNode *pNewNode, TBTreeNode *pParentNode)
		{
			assert(pNewNode->isLeaf());
			assert(pNode->isLeaf());
			TKey splitKey;
			pNode->splitIn(pNewNode, &splitKey);
			if(pNode->m_nParent == -1)
			{

				pNode->m_nParent = pParentNode->m_nPageAddr;
				pParentNode->setLess(pNode->m_nPageAddr);
			 
			}
			if(pNode->next() != -1)
			{
				pNewNode->m_LeafNode.m_nNext = pNode->m_LeafNode.m_nNext;
				TBTreeNode *pNextNode = getNode(pNode->m_LeafNode.m_nNext);
				if(pNextNode)
				{
					pNextNode->m_LeafNode.m_nPrev = pNewNode->m_nPageAddr;
				}
				pNextNode->setFlags(CHANGE_NODE, true);
			}
			pNode->m_LeafNode.m_nNext = pNewNode->m_nPageAddr;
			pNewNode->m_LeafNode.m_nPrev = pNode->m_nPageAddr;

			pNewNode->m_nParent = pParentNode->m_nPageAddr;
			pParentNode->insertInInnerNode(splitKey, pNewNode->m_nPageAddr);
			return true;
		}

	bool splitInnerNode(TBTreeNode *pNode)
	{
		TBTreeNode* pNodeNewRight = newNode(false, false);
		pNodeNewRight->setFlags(CHANGE_NODE|BUSY_NODE, true);

		TBTreeNode* pNodeParent = getNode(pNode->m_nParent);
		// Split node and get median
		TKey nMedianKey;
		if (!pNode->splitIn(pNodeNewRight, &nMedianKey))
		{
			return false;
		}
		pNodeNewRight->m_nParent = pNode->m_nParent;
		m_ChangeNode.insert(pNode);
		m_ChangeNode.insert(pNodeNewRight);

		while ( pNodeParent != 0 )
		{
			// Add median to the parent
			pNodeParent->insertInInnerNode(nMedianKey, pNodeNewRight->m_nPageAddr);
			if (pNodeParent->size() > m_pTransaction->getPageSize())
			{
				pNodeParent->setFlags(CHANGE_NODE|BUSY_NODE, true);

				pNodeNewRight = newNode(false, false);
				if(!pNodeNewRight)
				{
					CommonLib::str_t sMsg;
					sMsg.format(_T("BTREE: Error create new right B Node"));
					m_pTransaction->error(sMsg);
					return false;
				}

				pNodeNewRight->setFlags(CHANGE_NODE|BUSY_NODE, true);
				if (!pNodeParent->splitIn(pNodeNewRight, &nMedianKey))
				{
					return false;
				}
				pNodeNewRight->m_nParent = pNodeParent->m_nParent;
				m_ChangeNode.insert(pNodeNewRight);
				m_ChangeNode.insert(pNodeParent);

				pNode = pNodeParent;

				// Move up
				pNodeParent = getNode(pNodeParent->m_nParent);
				m_nStateTree |= eBPTNewInnerNode;
			}
			else
			{
				pNodeParent->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pNodeParent);
		
				pNodeNewRight->m_nParent = pNodeParent->m_nPageAddr;
				break;
			}
		}
		if(!pNodeParent)
		{
			TBTreeNode* pNodeNewRoot = newNode(true, false);
			if(!pNodeNewRoot)
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Error create new root node"));
				m_pTransaction->error(sMsg);
				return false;
			}
			m_nStateTree |= eBPTNewRootNode;
			//pNodeNewRoot->m_innerMemSet.insert(nMedianKey, pNodeNewRight->m_nPageAddr);
			pNodeNewRoot->insertInInnerNode(nMedianKey, pNodeNewRight->m_nPageAddr);

			pNode->m_nParent = pNodeNewRoot->m_nPageAddr;
			pNodeNewRoot->setLess(pNode->m_nPageAddr);
			assert(pNodeNewRoot->m_InnerNode.m_nLess != -1);
			pNodeNewRight->m_nParent = pNodeNewRoot->m_nPageAddr;
			m_nRootAddr = pNodeNewRoot->m_nPageAddr;
			m_pRoot->setFlags(ROOT_NODE|BUSY_NODE, false);
			m_Chache.AddElem(m_pRoot->m_nPageAddr, m_pRoot);
			m_pRoot = pNodeNewRoot;
			m_pRoot->setFlags(ROOT_NODE, true);
			m_pRoot->setFlags(CHANGE_NODE, true);
			//m_ChangeNode.insert(pNodeNewRoot);
			saveBTreeInfo();
		}
		return true;
	}
	template<class TIterator>
	TIterator find(const TKey& key)
	{
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, -1);

		if(!m_pRoot)
		{
			m_pRoot= getNode(m_nRootAddr, true); 
		}
		if(!m_pRoot)
			return TIterator(this, NULL, -1);

		short nType = 0;
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->binary_search(key));
		}
		int32 nIndex = -1;
		int64 nNextAddr = m_pRoot->inner_lower_bound(key, nType, nIndex);
		int64 nParent = m_pRoot->addr();

		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNode* pNode = getNode(nNextAddr);
			if(!pNode)
			{

				ClearChache();
				return TIterator(this,  NULL,-1);
				break;
			}
			pNode->m_nParent = nParent;
			pNode->m_nFoundIndex = nIndex;
			nType = 0;
			if(pNode->isLeaf())
			{
				ClearChache();
				return TIterator(this, pNode, pNode->binary_search(key));
				break;
			}
			nNextAddr = pNode->inner_lower_bound(key, nType, nIndex);
			nParent = pNode->addr();
		}
		ClearChache();
		return TIterator(this, NULL,-1);
	}

	template<class TIterator>
	TIterator begin()
	{
		TBTreeNode* pFindBTNode = NULL;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, -1);

		if(!m_pRoot)
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot)
			return TIterator(this, NULL, NULL);
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot, 0);
		}
		int64 nNextAddr = m_pRoot->less();
		int64 nParent = m_pRoot->addr();
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNode* pNode = getNode(nNextAddr);
			pNode->m_nParent = nParent;
			pNode->m_nFoundIndex = -1;
			if(pNode->isLeaf())
			{
				pFindBTNode = pNode;
				break;
			}
			nNextAddr = pNode->less();
			nParent = pNode->addr();
		}
		ClearChache();
		return TIterator(this, pFindBTNode, 0);
	}
	template<class TIterator>
	TIterator last()
	{
		TBTreeNode* pFindBTNode = NULL;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, 0);

		if(!m_pRoot)
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot)
			return TIterator(this, NULL, 0);
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->count()  - 1);
		}
		int64 nNextAddr = m_pRoot->backLink();
		int64 nParent = m_pRoot->addr();
		int32 nIndex = m_pRoot->count() - 1;
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNode* pNode = getNode(nNextAddr);
			pNode->m_nParent = nParent;
			pNode->m_nFoundIndex = nIndex;
			if(pNode->isLeaf())
			{
				pFindBTNode = pNode;
				break;
			}
			nNextAddr = pNode->backLink();
			nParent = pNode->addr();
			nIndex = pNode->count() - 1;
		}
		ClearChache();
		return TIterator(this, pFindBTNode, pFindBTNode ? pFindBTNode->count() - 1 : -1);
	}
	template<class TIterator>
	TIterator upper_bound(const TKey& key)
	{
		 
		int32 nIndex = 0;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, 0);

		if(!m_pRoot)
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot)
			return TIterator(this, NULL, 0);

	 
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot, m_pRoot->leaf_upper_bound(key));
		}
		nIndex = -1;
		int64 nNextAddr = m_pRoot->inner_upper_bound(key, nIndex);
		int64 nParent = m_pRoot->addr();
	 
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNode* pNode = getNode(nNextAddr);
			pNode->m_nParent = nParent;
			pNode->m_nFoundIndex = nIndex;
			if(pNode->isLeaf())
			{
				ClearChache();
				return TIterator(this, pNode, pNode->leaf_upper_bound(key));
				break;
			}
			nNextAddr = pNode->inner_upper_bound(key, nIndex);
			nParent = pNode->addr();
		}
		ClearChache();
		return TIterator(this, NULL,-1);
	}
	template<class TIterator>
	TIterator lower_bound(const TKey& key)
	{
		TBTreeNode* pFindBTNode = NULL;
		int32 nIndex = 0;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, 0);

		if(!m_pRoot)
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot)
			return TIterator(this, NULL, 0);

		short nType = 0;
		if(m_pRoot->isLeaf())
		{
			
			return TIterator(this, m_pRoot, m_pRoot->leaf_lower_bound(key, nType));
		}
		nIndex = -1;
		int64 nNextAddr = m_pRoot->inner_lower_bound(key, nType, nIndex);
		int64 nParent = m_pRoot->addr();

		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNode* pNode = getNode(nNextAddr);
			pNode->m_nParent = nParent;
			pNode->m_nFoundIndex = nIndex;
			if(pNode->isLeaf())
			{
				ClearChache();
				return TIterator(this, pNode, pNode->leaf_lower_bound(key, nType));
				break;
			}
			nNextAddr = pNode->inner_lower_bound(key, nType, nIndex);
			nParent = pNode->addr();
		}
		ClearChache();
		return TIterator(this, NULL,-1);
	}
	/*template<class TIterator>
	bool remove(const TKey& key)
	{
		iterator it = find(key);
		if(it.isNull())
			return false;
		return remove(it);
	}*/
	template<class TIterator>
	bool remove(TIterator it)
	{
		if(it.isNull())
			return false;

		TBTreeNode *pNode = it.m_pCurNode;
		TLeafNode *pLeafNode = it.m_pCurLeafNode;
		TKey key = it.key();

		pLeafNode->removeByIndex(it.m_nIndex);
		pNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pNode);

		int nFoundIndex = pNode->m_nFoundIndex;

		TBTreeNode *pParentNode = getNode(pNode->m_nParent);
		if(pLeafNode->size() >  m_pTransaction->getPageSize()/2)
		{

			if(pNode->m_nFoundIndex != -1 && pParentNode->isKey(key, pNode->m_nFoundIndex))
			{
				TBTreeNode* pIndexNode = getNode(pParentNode->link(pNode->m_nFoundIndex ));
				pParentNode->updateKey(pNode->m_nFoundIndex , pIndexNode->key(0));
				pParentNode->setFlags(CHANGE_NODE, true);
				m_ChangeNode.insert(pParentNode);
			}

			return removeUP(key, pParentNode);
			return true;
		}
		if(pLeafNode->getFlags() & ROOT_NODE)
		{
			return true;
		}


		assert(pParentNode);
		assert(!pParentNode->isLeaf());


		TBTreeNode* pDonorNode = NULL;
		bool bLeft = false;
		
		if(pParentNode->less() == pNode->addr())
		{
			pDonorNode = getNode(pParentNode->link(0));
			pDonorNode->m_nFoundIndex = 0;
			bLeft = false;
		}
		else
		{

			TBTreeNode* pLeafNodeRight = NULL;
			TBTreeNode* pLeafNodeLeft = NULL;

			if(pNode->m_nFoundIndex == 0)
			{
				pLeafNodeLeft = getNode(pParentNode->less());
				pLeafNodeLeft->m_nFoundIndex = -1;
				if(pParentNode->count() > 1)
				{
					pLeafNodeRight = getNode(pParentNode->link(1));
					pLeafNodeRight->m_nFoundIndex = 1;
				}
			}
			else
			{
				pLeafNodeLeft = getNode(pParentNode->link(pNode->m_nFoundIndex - 1));
				pLeafNodeLeft->m_nFoundIndex = pNode->m_nFoundIndex - 1;
				if((int32)pParentNode->count() > pNode->m_nFoundIndex + 1)
				{
					pLeafNodeRight = getNode(pParentNode->link(pNode->m_nFoundIndex + 1));
					pLeafNodeRight->m_nFoundIndex = pNode->m_nFoundIndex + 1;
				}
			}
 
			assert(pLeafNodeLeft != NULL || pLeafNodeRight != NULL);
			assert(pLeafNodeLeft != pNode && (pLeafNodeRight != pNode) && (pLeafNodeLeft != pLeafNodeRight));

			int nLeftsize =  pLeafNodeLeft ? pLeafNodeLeft->rowSize() : -1;
			int nRightSize = pLeafNodeRight ? pLeafNodeRight->rowSize() : -1;
			if(nLeftsize > nRightSize)
			{
				pDonorNode = pLeafNodeLeft;
					bLeft = true;
			}
			else
			{
				pDonorNode = pLeafNodeRight;
				bLeft = false;
			}
		}
		assert(pDonorNode);
		size_t nSumSize = pDonorNode->rowSize() + pLeafNode->rowSize() + pLeafNode->headSize();
		int nCnt = ((pLeafNode->count() + pDonorNode->count()))/2 - pLeafNode->count();

		bool bUnion = false;
		bool bAlignment = false;

		if(nSumSize <  m_pTransaction->getPageSize()/2)	
			bUnion = true;
		else if(nCnt > 0)
			bAlignment = true;
		else
		{
			if(nSumSize < m_pTransaction->getPageSize())
			{
				bUnion = true;
			}
		}
		if(bUnion)  
		{
			bool bRet = true;
			if(pDonorNode->m_nFoundIndex == -1)
			{
				bRet = UnionLeafNode(pParentNode, pDonorNode, pNode, false);
				nFoundIndex = -1;
			}
			else
			{
				bRet = UnionLeafNode(pParentNode, pNode, pDonorNode, bLeft);
				nFoundIndex = pNode->m_nFoundIndex;
			}
			if(!bRet)
			{
				return false;
			}
		}
		else if(bAlignment)
		{
			if(!AlignmentLeafNode(pParentNode, pNode,  pDonorNode,  bLeft))
				return false;

			nFoundIndex = pNode->m_nFoundIndex;
		}
		if(nFoundIndex != -1 && pParentNode->isKey(key, nFoundIndex))
		{
			TBTreeNode* pIndexNode = getNode(pParentNode->link(nFoundIndex));
			pParentNode->updateKey(nFoundIndex, pIndexNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
			m_ChangeNode.insert(pParentNode);
		}
		return removeUP(key, pParentNode);
	 
	}

	bool UnionLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode* pDonorNode, bool bLeft)
	{
		pLeafNode->UnionWith(pDonorNode, bLeft);
		size_t nNodeSize = pLeafNode->size() ;
		/*if(nNodeSize > m_pTransaction->getPageSize())
		{
			//pLeafNode->splitIn()
			return false;
		}*/
	
		if(bLeft)
		{

			TBTreeNode* pPrevNode = getNode( pDonorNode->prev());
			if(pPrevNode)
			{
				assert(pPrevNode->isLeaf());
				pLeafNode->setPrev(pPrevNode->m_nPageAddr);
				pPrevNode->setNext(pLeafNode->m_nPageAddr);
				pPrevNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pPrevNode);
			}
			else
				pLeafNode->setPrev(-1);
		}
		else
		{

			TBTreeNode* pNextNode = getNode( pDonorNode->m_LeafNode.m_nNext);
			if(pNextNode)
			{
				assert(pNextNode->isLeaf());
				pLeafNode->setNext(pNextNode->m_nPageAddr);
				pNextNode->setPrev(pLeafNode->m_nPageAddr);
				pNextNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pNextNode);
			}
			else
				pLeafNode->m_LeafNode.m_nNext = -1;
		}

		pParentNode->removeByIndex(pDonorNode->m_nFoundIndex);
		if(bLeft && pLeafNode->m_nFoundIndex != -1)
		{
			pLeafNode->m_nFoundIndex--;
			assert(pLeafNode->m_nFoundIndex >= 0);
			pParentNode->updateKey(pLeafNode->m_nFoundIndex, pLeafNode->key(0));
 		}
 
			
		deleteNode(pDonorNode);
		pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pParentNode);
		return true;

	}

	bool AlignmentLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode* pDonorNode , bool bLeft)
	{
		if(!pLeafNode->AlignmentOf(pDonorNode, bLeft))
			return false;
		if(bLeft)
		{
			pParentNode->updateKey(pLeafNode->m_nFoundIndex, pLeafNode->key(0));
			pLeafNode->m_nFoundIndex = -1;
		}
		else
			pParentNode->updateKey(pDonorNode->m_nFoundIndex, pDonorNode->key(0));
		pLeafNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pLeafNode);
		pDonorNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pDonorNode);
		pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pParentNode);
		return true;
	}


	bool removeUP(const TKey& key, TBTreeNode* pCheckNode)
	{
			 
		while(pCheckNode)
		{
			
			TBTreeNode*  pParentNode = getNode(pCheckNode->m_nParent);
			if(!pParentNode)
			{
				if(!pCheckNode->count())
				{
					TBTreeNode* pNode = getNode(pCheckNode->less());
					assert(pNode);//Должна быть
					m_Chache.remove(pNode->addr());
					m_ChangeNode.remove(m_pRoot);
					deleteNode(m_pRoot);
					m_pRoot = pNode;
					m_pRoot->m_nParent = -1;
					m_nRootAddr = m_pRoot->addr();
					pNode->setFlags(CHANGE_NODE|ROOT_NODE, true);
					m_ChangeNode.insert(pNode);
					saveBTreeInfo();
					m_nStateTree |= (eBPTDeleteInnerNode | eBPTNewRootNode);
					
				}

				return true;
			}
			

			if(pCheckNode->m_nFoundIndex != -1 && pParentNode->isKey(key, pCheckNode->m_nFoundIndex ))
			{
				TBTreeNode* pIndexNode = getNode(pCheckNode->link(pCheckNode->m_nFoundIndex ));
				pParentNode->updateKey(pCheckNode->m_nFoundIndex, pIndexNode->key(0));
				pParentNode->setFlags(CHANGE_NODE, true);
				m_ChangeNode.insert(pParentNode);
			}
			if(!(pCheckNode->getFlags() & CHANGE_NODE))
					break;
			if(pCheckNode->size() >  m_pTransaction->getPageSize()/2)
			{
				pCheckNode = getNode(pCheckNode->m_nParent);
				continue;
			}

			TBTreeNode* pDonorNode = NULL;
			bool bLeft = false;

			if(pParentNode->less() == pCheckNode->addr())
			{
				pDonorNode = getNode(pParentNode->link(0));
				pDonorNode->m_nFoundIndex = 0;
				bLeft = false;
			}
			else
			{

				TBTreeNode* pLeafNodeRight = NULL;
				TBTreeNode* pLeafNodeLeft = NULL;

				if(pCheckNode->m_nFoundIndex == 0)
				{
					pLeafNodeLeft = getNode(pParentNode->less());
					pLeafNodeLeft->m_nFoundIndex = -1;
					if(pParentNode->count() > 1)
					{
						pLeafNodeRight = getNode(pParentNode->link(1));
						pLeafNodeRight->m_nFoundIndex = 1;
					}
				}
				else
				{
					pLeafNodeLeft = getNode(pParentNode->link(pCheckNode->m_nFoundIndex - 1));
					pLeafNodeLeft->m_nFoundIndex = pCheckNode->m_nFoundIndex - 1;
					if((int32)pParentNode->count() > pCheckNode->m_nFoundIndex + 1)
					{
						pLeafNodeRight = getNode(pParentNode->link(pCheckNode->m_nFoundIndex + 1));
						pLeafNodeRight->m_nFoundIndex = pCheckNode->m_nFoundIndex + 1;
					}
				}

				assert(pLeafNodeLeft != NULL || pLeafNodeRight != NULL);
				assert(pLeafNodeLeft != pCheckNode && (pLeafNodeRight != pCheckNode) && (pLeafNodeLeft != pLeafNodeRight));


				int nLeftsize =  pLeafNodeLeft ? pLeafNodeLeft->rowSize() : -1;
				int nRightSize = pLeafNodeRight ? pLeafNodeRight->rowSize() : -1;
				if(nLeftsize > nRightSize)
				{
					pDonorNode = pLeafNodeLeft;
					bLeft = true;
				}
				else
				{
					pDonorNode = pLeafNodeRight;
					bLeft = false;
				}
			}
			assert(pDonorNode);
		
			size_t nSumSize = pCheckNode->rowSize() + pDonorNode->rowSize() + pDonorNode->headSize();
			nSumSize += pCheckNode->tupleSize(); //insert less	
			
			
			bool bUnion = false;
			bool bAlignment = false;
			int nCnt = ((pCheckNode->count() + pDonorNode->count()))/2 - pCheckNode->count();
			if(nSumSize <   m_pTransaction->getPageSize()/2)	
				bUnion = true;
			else if(nCnt > 0)
				bAlignment = true;
			else
			{
				if(nSumSize < m_pTransaction->getPageSize())
				{
					bUnion = true;
				}
			}
			if(bUnion) 
			{
				if(pDonorNode->addr() == pParentNode->less())
				{
					UnionInnerNode(pParentNode, pDonorNode, pCheckNode, false);
					pCheckNode = pDonorNode;
				}
				else
					UnionInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft);
				/*if(bLess)
				{
					pCheckNode = pParentNode;
					continue;
				}*/
			}
			else if(bAlignment)
			{
				if(AlignmentInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft))
				{
					pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
					m_ChangeNode.insert(pParentNode);
				}

			}
			pCheckNode = getNode(pCheckNode->m_nParent);
		}
		ClearChache();
		return true;
	}


	bool UnionInnerNode(TBTreeNode* pParentNode, TBTreeNode* pNode, TBTreeNode* pDonorNode, bool bLeft)
	{
		pNode->UnioInnerWith(pDonorNode, bLeft ? getMinimumKey(getNode(pNode->less())) :   getMinimumKey(getNode(pDonorNode->less())),  bLeft);
 
		pParentNode->removeByIndex(pDonorNode->m_nFoundIndex);
		if(bLeft && pNode->m_nFoundIndex != -1)
		{
			pNode->m_nFoundIndex--;
			assert(pNode->m_nFoundIndex >= 0);
			pParentNode->updateKey(pNode->m_nFoundIndex, pNode->key(0));
		}
		deleteNode(pDonorNode);
		assert(pParentNode->count() != 0 || pParentNode->less() != -1);
		pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pParentNode);
		m_nStateTree |= (eBPTDeleteInnerNode);
		return true;
	}


	const TKey& getMinimumKey(TBTreeNode* pNode)
	{
		while(!pNode->isLeaf())
		{
			pNode = getNode(pNode->less());
			assert(pNode);
		}
		assert(pNode);
		return pNode->key(0);
	}

	bool AlignmentInnerNode(TBTreeNode* pParentNode, TBTreeNode *pNode, TBTreeNode* pDonorNode, bool bLeft)
	{	


		if(!pNode->AlignmentInnerNodeOf(pDonorNode, bLeft ? getMinimumKey(getNode(pNode->less())) :   getMinimumKey(getNode(pDonorNode->less())),  bLeft))
			return false;

		if(!bLeft) //Нода донор справа
		{
			const TKey& minKey = getMinimumKey(getNode(pDonorNode->less()));
			pParentNode->updateKey(pDonorNode->m_nFoundIndex, minKey);
			pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
			m_ChangeNode.insert(pParentNode);
		}
		else
		{
			const TKey& minKey = getMinimumKey(getNode(pNode->less()));
			pParentNode->updateKey(pNode->m_nFoundIndex, minKey);
			pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
			m_ChangeNode.insert(pParentNode);
		}
	 
		pNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pNode);
		pDonorNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pDonorNode);
		return true;
	}

		BPTreeStatisticsInfo m_BTreeInfo;
	protected:
		TBTreeNode *m_pRoot; 
		TLink m_nRootAddr;
		TLink m_nRTreeStaticAddr;
		TLink m_nPageBTreeInfo;

		TLink m_nPageInnerCompInfo;
		TLink m_nPageLeafPageCompInfo;

		CommonLib::alloc_t* m_pAlloc;
		Transaction* m_pTransaction;
		typedef RBSet<TBTreeNode*> TChangeNode;
		TChangeNode m_ChangeNode;
		size_t m_nChacheSize;
		typedef TSimpleCache<TLink, TBTreeNode> TNodesCache;
		TNodesCache m_Chache;
		bool m_bChangeRoot;
		bool m_bMulti;
		bool m_bCheckCRC32;

		std::auto_ptr<TCompressorParamsBase*> m_LeafCompParams;
		std::auto_ptr<TCompressorParamsBase*> m_InnerCompParams;
		uint32 m_nStateTree;
	};



	template <class _TKey,	class _TComp, class _Transaction,
	class _TInnerCompess = BPInnerNodeSimpleCompressorV2<_TKey>,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressorV2<_TKey>,  
	class _TInnerNode = BPTreeInnerNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeSetv2<_TKey,  _TComp, _Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeSetv2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
	class TBPSetV2 : public TBPlusTreeSetV2<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>
	{
	public:
		typedef TBPlusTreeSetV2<_TKey, _TComp, _Transaction, _TInnerCompess, 
			_TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode > TBase;


		typedef typename TBase::TKey      TKey;
		typedef typename TBase::TComp	   TComp;
		typedef typename TBase::TLink     TLink;
		typedef typename TBase::Transaction  Transaction;
		typedef typename TBase::TInnerCompess  TInnerCompess;
		typedef typename TBase::TLeafCompess  TLeafCompess;
		typedef typename TBase::TInnerNode TInnerNode;
		typedef typename TBase::TLeafNode	TLeafNode;
		typedef typename TBase::TBTreeNode TBTreeNode;

		typedef TBPSetIteratorV2<TKey, TComp,Transaction, TInnerCompess, TLeafCompess,
			TInnerNode, TLeafNode, TBTreeNode> iterator;

		TBPSetV2(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti, bCheckCRC32 )
			{

			}

			iterator find(const TKey& key)  
			{
				return TBase::find<iterator>(key);
			}
			iterator begin()
			{
				return TBase::begin<iterator>();
			}

			iterator last()
			{
				return TBase::last<iterator>();
			}
			iterator upper_bound(const TKey& key)
			{
				return TBase::upper_bound<iterator>(key);
			}
			iterator lower_bound(const TKey& key)
			{
				return TBase::lower_bound<iterator>(key);
			}

			bool remove(const TKey& key)
			{
				iterator it = find(key);
				if(it.isNull())
					return false;
				return TBase::remove<iterator>(it);
			}
	};
	 
}
#endif