#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_V2_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeNodeSetV2.h"
#include "IDBTransactions.h"
#include "simple_stack.h"
//#include "CacheNodes.h"
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPInnerNodeSimpleCompressorV2.h"
#include "BPLeafNodeSetSimpleCompressorV2.h"
#include "BPTree.h"
#include "CompressorParams.h"
#include "BPIteratorSetV2.h"
#include "CommonLibrary/delegate.h"
#include "CacheMRU.h"
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


	template <	class _TKey,  class _TComp,  class _Transaction,
	class _TInnerCompess = BPInnerNodeSimpleCompressorV2<_TKey>,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressorV2<_TKey>,  
	class _TInnerNode = BPTreeInnerNodeSetv2<_TKey, /*_TComp,*/ _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeSetv2<_TKey,  /*_TComp, */_Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeSetv2<_TKey, /*_TComp,*/ _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
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
		 ,m_Cache(pAlloc)
		// ,m_ChangeNode(pAlloc)
		 ,m_pRoot(NULL)
		 ,m_nRTreeStaticAddr(-1)
		 ,m_nPageInnerCompInfo(-1)
		 ,m_nPageLeafPageCompInfo(-1)
		 ,m_bCheckCRC32(bCheckCRC32)
		 ,m_BTreeInfo(bCheckCRC32)
		 ,m_nStateTree(eBPTNoChange)
		{

			m_NodeRemove.reset(new TRemoveNodeDelegate(this, &TBPlusTreeSetV2::deleteNodeRef));

		}
		~TBPlusTreeSetV2()
		{
			TNodesCache::iterator it =	m_Cache.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.object();
				assert(pBNode != m_pRoot.get());
				//delete pBNode;
				pBNode->setParent(NULL);
				it.next();
			}
			it = m_Cache.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode  = it.object();
				assert(pBNode != m_pRoot.get());
				delete pBNode;
				it.next();
			}
			if(m_pRoot.get())
			{
				delete m_pRoot.release();
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
		typedef IRefCntPtr<TBTreeNode> TBTreeNodePtr;
 
		typedef CompressorParamsBase<Transaction> TCompressorParamsBase;
		typedef BPTreeStatistics<int64, _Transaction, _TKey> BPTreeStatisticsInfo;
		typedef CommonLib::delegateimpl1_t<TBPlusTreeSetV2, RefCounter*> TRemoveNodeDelegate;
		std::auto_ptr<TRemoveNodeDelegate> m_NodeRemove;
/*
		typedef TBPSetIteratorV2<TKey, TComp,Transaction, TInnerCompess, TLeafCompess,
		TInnerNode, TLeafNode, TBTreeNode> iterator;*/
		
	

		void setTransactions(Transaction *pTransaction)
		{
			m_pTransaction  = pTransaction;
		}

		void clear(bool bNotSetFreePage = true)
		{
			TNodesCache::TCacheSet::iterator it =	m_Cache.m_set.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.value().pListEl->obj_;
				delete pBNode;
				it.next();
			}
			//m_Chache.m_set.clear();
			//m_Chache.clear();
			if(m_pRoot.get())
			{
				if(bNotSetFreePage)
				{
					//TO DO нужно удалять или как то пометить свободные файловые страницы
				}
				

				deleteNode(m_pRoot.get());
				m_pRoot = newNode(true, true);
				m_pRoot->setParent(NULL);
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
			FilePagePtr pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo);
			if(!pPage.get())
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
			FilePagePtr pPage (NULL);
			if(m_nPageBTreeInfo == -1)
			{
				pPage = m_pTransaction->getNewPage();
				if(pPage.get())
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
				if(!pPage.get())
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
			if(!m_pRoot.get())
				return true;
			TNodesCache::iterator it =	m_Cache.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.object();
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
		
	TBTreeNodePtr newNode(bool bIsRoot, bool bIsLeaf)
	{
		TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, -1, m_bMulti, bIsLeaf, m_bCheckCRC32, (ICompressorParams *)m_InnerCompParams.get(),
				(ICompressorParams *)m_LeafCompParams.get() );
			pNode->Load(m_pTransaction);
			m_BTreeInfo.AddNode(1, bIsLeaf);
			if(bIsRoot)
				pNode->setFlags(ROOT_NODE, true);
			else
				m_Cache.AddElem(pNode->m_nPageAddr, pNode);
		return TBTreeNodePtr(pNode);
	}
	
	TBTreeNodePtr getNode(TLink nAddr, bool bIsRoot = false, bool bNotMove = false, bool bCheckCache = false)
	{

		   if(nAddr == -1)
				return TBTreeNodePtr(NULL);
			if(nAddr == m_nRootAddr && m_pRoot.get())
			{
				return m_pRoot;
			}
			TBTreeNode *pBNode = m_Cache.GetElem(nAddr, bNotMove);
			if(!pBNode)
			{
				FilePagePtr pFilePage = m_pTransaction->getFilePage(nAddr);
				assert(pFilePage.get());
				if(!pFilePage.get())
				{
					return TBTreeNodePtr(NULL);
				}
				pBNode = new TBTreeNode(-1, m_pAlloc, nAddr, m_bMulti, false, m_bCheckCRC32, (ICompressorParams *)m_InnerCompParams.get(), (ICompressorParams *)m_LeafCompParams.get());
				if(!pBNode->LoadFromPage(pFilePage.get(), m_pTransaction))
				{
					delete pBNode;
					return TBTreeNodePtr(NULL);
				}
				pBNode->m_pRemFunk = m_NodeRemove.get();
				if(bCheckCache)
				{
					if(m_Cache.size() > m_nChacheSize)
					{
						TBTreeNode *pDelNode = m_Cache.remove_back();
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
					m_Cache.AddElem(pBNode->m_nPageAddr, pBNode, bNotMove);
			}
			return TBTreeNodePtr(pBNode);
	}
	bool deleteNode(TBTreeNode* pNode)
	{
		m_BTreeInfo.AddNode(-1, pNode->isLeaf());
		m_pTransaction->dropFilePage(pNode->m_nPageAddr);
		pNode->setFlags(REMOVE_NODE, true);
		m_Cache.remove(pNode->m_nPageAddr);
		//delete pNode;
		return true;
	}
	void deleteNodeRef(RefCounter *pRefPtr)
	{
		TBTreeNode* pNode = (TBTreeNode*)pRefPtr;
		if(pNode->getFlags() & REMOVE_NODE)
		{
			delete pNode;
			return;
		}

		ClearChache();
		
	
	}
	void ClearChache()
	{
	
		if(m_Cache.size() <= m_nChacheSize)
			return;
		for (size_t i = 0, sz = m_Cache.size(); i < sz - m_nChacheSize; i++)
		{
			TBTreeNode* pDelNode = m_Cache.remove_back();//remove(pChNode->m_nPageAddr);
			if(!pDelNode)
				break;
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
		if(m_pRoot.get() || m_nRootAddr != -1)
			return false;
		if(m_nPageBTreeInfo == -1)
			return false;
		return createRootPage();
	}
	bool createRootPage()
	{
		m_pRoot = newNode(true, true);
		m_nRootAddr = m_pRoot->m_nPageAddr; 
		FilePagePtr pFilePage = m_pTransaction->getNewPage();
		if(!pFilePage.get())
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
		if(m_pRoot.get())
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
		if(!m_pRoot.get())
		{
			CommonLib::str_t sMsg;
			sMsg.format(_T("BTREE: Error load root page: %I64d"), (int64)m_nRootAddr);
			m_pTransaction->error(sMsg);
			return false;
		}
		return true;
	}


	TBTreeNodePtr findLeafNodeForInsert(const TKey& key)
	{
		if(!m_pRoot.get())
		{
			if(!checkRoot())
				return TBTreeNodePtr(NULL);
		}
		if(m_pRoot-> isLeaf())
		{
			return TBTreeNodePtr(m_pRoot);
		}
		else
		{
			int64 nNextAddr = m_pRoot->findNodeInsert(m_comp, key);
			TBTreeNodePtr pParent =  m_pRoot;
			while (nNextAddr != -1)
			{
				TBTreeNodePtr pNode = getNode(nNextAddr);
				pNode->setParent(pParent.get(), -1);
				if(pNode->isLeaf())
				{
					return TBTreeNodePtr(pNode);
					break;
				}
				pParent = pNode;
				nNextAddr= pNode->findNodeInsert(m_comp, key);
			}
		}

		return TBTreeNodePtr(NULL);
	}
	/*template<class TIterator>
	TIterator insertRetIT(const TKey& key, int& nRet)
	{

			bool bRet = false;
			TBTreeNodePtr pNode = findLeafNodeForInsert(key);
			int nIndex = 0;
			if(pNode.get())
			{
				pNode = InsertInLeafNode(pNode.get(), key, &nIndex);
			}
			ClearChache();
			if(pNode.get())
				m_BTreeInfo.AddKey(1);
			nRet =  pNode.get() ? 1 : -1;	
		   return TIterator(this, pNode.get(), nIndex);
	}*/


	template<class TIterator>
	bool insert(const TKey& key, TIterator* pFromIterator = NULL,  TIterator* pRetIterator = NULL)
	{

		bool bRet = false;
		int nIndex = -1;
		TBTreeNodePtr pNode;
		if(pFromIterator)
		{
			pNode = InsertInLeafNode(pFromIterator->m_pCurNode.get(), key, &nIndex,  pFromIterator->m_nIndex + 1);
		}
		else
		{
			pNode = findLeafNodeForInsert(key);
			if(pNode.get())
			{
				pNode = InsertInLeafNode(pNode.get(), key, &nIndex);
			}
		}		
		ClearChache();
		if(pNode.get())
			m_BTreeInfo.AddKey(1);

		bRet = pNode.get() ? true : false;	
		if(pRetIterator)
		{
			*pRetIterator = TIterator(this, pNode.get(), nIndex);
		}

		return bRet;	
	}
	
	
	template<class TIterator, class TKeyFunctor>
	bool insertLast(TKeyFunctor& keyFunctor, TKey* pKey = NULL,  TIterator* pRetIterator = NULL)
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
			TBTreeNodePtr pNode = InsertInLeafNode(pBNode, key);
			ClearChache();
			if(pNode.get())
				m_BTreeInfo.AddKey(1);
			return pNode.get() ? true : false;	
		}
		TBTreeNodePtr InsertInLeafNode(TBTreeNode *pNode, const TKey& key, int *pInIndex = NULL, int nInsertLeafIndex = -1)
		{
			assert(pNode->isLeaf());
			int nIndex = pNode->insertInLeaf(m_comp, key, nInsertLeafIndex);
			if(nIndex == -1)
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Error insert"));
				m_pTransaction->error(sMsg);
				return TBTreeNodePtr(NULL);
			}
			pNode->setFlags(CHANGE_NODE, true);
			
			//m_ChangeNode.insert(pNode);
			m_nStateTree |= eBPTChangeLeafNode;
		
			if(pInIndex)
				*pInIndex = nIndex;
			return CheckLeafNode(pNode, pInIndex);
		}
		TBTreeNodePtr CheckLeafNode(TBTreeNode *pNode, int *pInIndex = NULL)
		{
			TBTreeNodePtr pRetNode(pNode);
			if(pNode->size() > m_pTransaction->getPageSize())
			{

				TBTreeNodePtr pParentNode = getNode(pNode->parentAddr());
				bool bNewRoot = false;
				if(!pParentNode.get())
				{
					//тут все просто создаеться новый рутовый элеинт
					pParentNode  = newNode(true, false);
					if(!pParentNode.get())
					{
						CommonLib::str_t sMsg;
						sMsg.format(_T("BTREE: Error create new root node"));
						m_pTransaction->error(sMsg);
						return TBTreeNodePtr(NULL);
					}
					bNewRoot = true;
				}

				m_nStateTree |= eBPTNewLeafNode;

				TBTreeNodePtr pNewLeafNode = newNode(false, true);


				int nSplitIndex = splitLeafNode(pNode, pNewLeafNode.get(), pParentNode.get());
				if(pInIndex)
				{
					if(*pInIndex > nSplitIndex)
					{
						*pInIndex = *pInIndex - nSplitIndex;
						pRetNode = pNewLeafNode.get();
					}
				}

				pNewLeafNode->setFlags(CHANGE_NODE, true);
				pParentNode->setFlags(CHANGE_NODE, true);

				//m_ChangeNode.insert(pNewLeafNode);
				//m_ChangeNode.insert(pParentNode);
				if(bNewRoot)
				{
					m_nStateTree |= eBPTNewRootNode;
					m_nRootAddr = pParentNode->m_nPageAddr;
					m_pRoot->setFlags(ROOT_NODE, false);
					m_Cache.AddElem(m_pRoot->m_nPageAddr, m_pRoot.get());
					m_pRoot = pParentNode;
					m_pRoot->setFlags(ROOT_NODE, true);
					m_pRoot->setFlags(CHANGE_NODE, true);
					//m_Chache.remove(m_pRoot);
					saveBTreeInfo();
					return pRetNode;
				}

				if(pParentNode->size() > m_pTransaction->getPageSize())
				{
					if(!splitInnerNode(pParentNode.get()))
					{
						return TBTreeNodePtr(NULL);
					}
				}

			}
			return pRetNode;
		}
		int splitLeafNode(TBTreeNode *pNode, TBTreeNode *pNewNode, TBTreeNode *pParentNode)
		{
			assert(pNewNode->isLeaf());
			assert(pNode->isLeaf());
			TKey splitKey;
			int nIndex = pNode->splitIn(pNewNode, &splitKey);
			if(pNode->parentAddr() == -1)
			{

				pNode->setParent(pParentNode, -1);
				pParentNode->setLess(pNode->m_nPageAddr);
			 
			}
			if(pNode->next() != -1)
			{
				pNewNode->m_LeafNode.m_nNext = pNode->m_LeafNode.m_nNext;
				TBTreeNodePtr pNextNode = getNode(pNode->m_LeafNode.m_nNext);
				if(pNextNode.get())
				{
					pNextNode->m_LeafNode.m_nPrev = pNewNode->m_nPageAddr;
				}
				pNextNode->setFlags(CHANGE_NODE, true);
			}
			pNode->m_LeafNode.m_nNext = pNewNode->m_nPageAddr;
			pNewNode->m_LeafNode.m_nPrev = pNode->m_nPageAddr;

			//pNewNode->m_nParent = pParentNode->m_nPageAddr;
			int nInsertIndex = pParentNode->insertInInnerNode(m_comp, splitKey, pNewNode->m_nPageAddr);
			pNewNode->setParent(pParentNode, nInsertIndex);
		
			return nIndex;
		}

	bool splitInnerNode(TBTreeNode *pInNode)
	{

		TBTreeNodePtr pNode(pInNode);
		TBTreeNodePtr pNodeNewRight = newNode(false, false);
		pNodeNewRight->setFlags(CHANGE_NODE, true);

		TBTreeNodePtr pNodeParent = getNode(pNode->parentAddr());
		// Split node and get median
		TKey nMedianKey;
		if (!pNode->splitIn(pNodeNewRight.get(), &nMedianKey))
		{
			
			return false;
		}
		SetParentInChildCacheOnly(pNodeNewRight.get());
		//pNodeNewRight->m_nParent = pNode->m_nParent;
		
		
		while ( pNodeParent.get() != 0 )
		{
			// Add median to the parent
			int nIndex = pNodeParent->insertInInnerNode(m_comp, nMedianKey, pNodeNewRight->m_nPageAddr);
			pNodeNewRight->setParent(pNodeParent.get(), nIndex);
			if (pNodeParent->size() > m_pTransaction->getPageSize())
			{
				pNodeParent->setFlags(CHANGE_NODE, true);

				pNodeNewRight = newNode(false, false);
				if(!pNodeNewRight.get())
				{
					CommonLib::str_t sMsg;
					sMsg.format(_T("BTREE: Error create new right B Node"));
					m_pTransaction->error(sMsg);
					return false;
				}

				pNodeNewRight->setFlags(CHANGE_NODE, true);
				if (!pNodeParent->splitIn(pNodeNewRight.get(), &nMedianKey))
				{
					return false;
				}
				//pNodeNewRight->m_nParent = pNodeParent->m_nParent;
				//pNodeNewRight->setParent(pNodeParent->parentNodePtr(), pNodeParent->parentAddr());
				SetParentInChildCacheOnly(pNodeNewRight.get());
				pNode = pNodeParent;

				// Move up
				pNodeParent = getNode(pNodeParent->parentAddr());
				m_nStateTree |= eBPTNewInnerNode;
			}
			else
			{
				pNodeParent->setFlags(CHANGE_NODE, true);
				//m_ChangeNode.insert(pNodeParent);
		
				//pNodeNewRight->m_nParent = pNodeParent->m_nPageAddr;
				//pNodeNewRight->setParent(pNodeParent.get());
				break;
			}
		}
		if(!pNodeParent.get())
		{
			TBTreeNodePtr pNodeNewRoot = newNode(true, false);
			if(!pNodeNewRoot.get())
			{
				CommonLib::str_t sMsg;
				sMsg.format(_T("BTREE: Error create new root node"));
				m_pTransaction->error(sMsg);
				return false;
			}
			m_nStateTree |= eBPTNewRootNode;
			//pNodeNewRoot->m_innerMemSet.insert(nMedianKey, pNodeNewRight->m_nPageAddr);
			int nIndex = pNodeNewRoot->insertInInnerNode(m_comp, nMedianKey, pNodeNewRight->m_nPageAddr);

			//pNode->m_nParent = pNodeNewRoot->m_nPageAddr;
			pNode->setParent(pNodeNewRoot.get(), -1);
			pNodeNewRoot->setLess(pNode->m_nPageAddr);
			assert(pNodeNewRoot->m_InnerNode.m_nLess != -1);

			//pNodeNewRight->m_nParent = pNodeNewRoot->m_nPageAddr;
			pNodeNewRight->setParent(pNodeNewRoot.get(), nIndex);

			m_nRootAddr = pNodeNewRoot->m_nPageAddr;
			m_pRoot->setFlags(ROOT_NODE, false);
			m_Cache.AddElem(m_pRoot->m_nPageAddr, m_pRoot.get());
			m_pRoot = pNodeNewRoot;
			m_pRoot->setFlags(ROOT_NODE, true);
			m_pRoot->setFlags(CHANGE_NODE, true);
			//m_ChangeNode.insert(pNodeNewRoot);
			saveBTreeInfo();
		}
		return true;
	}

	void SetParentInChildCacheOnly(TBTreeNode *pNode)
	{
		assert(!pNode->isLeaf());
		TBTreeNode *pLessNode = m_Cache.GetElem(pNode->less(), false);
		if(pLessNode)
			pLessNode->setParent(pNode, -1);
		for (size_t i = 0,  sz = pNode->count(); i < sz; ++i)
		{
			TBTreeNode *pChildNode = m_Cache.GetElem(pNode->link(i), false);
			if(pChildNode)
				pChildNode->setParent(pNode, i);
		}
	}

	
	template<class TIterator, class TComp>
	TIterator find(const TComp& comp, const TKey& key)
	{
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, -1);

		if(!m_pRoot.get())
		{
			m_pRoot = getNode(m_nRootAddr, true); 
		}
		if(!m_pRoot.get())
			return TIterator(this, NULL, -1);

		short nType = 0;
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot.get(), m_pRoot->binary_search(comp, key));
		}
		int32 nIndex = -1;
		int64 nNextAddr = m_pRoot->inner_lower_bound(comp, key, nType, nIndex);
		//int64 nParent = m_pRoot->addr();
		TBTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			
			TBTreeNodePtr pNode = getNode(nNextAddr);
			if(!pNode.get())
			{

				ClearChache();
				return TIterator(this,  NULL,-1);
				break;
			}
			//pNode->m_nParent = nParent;
			//pNode->m_nFoundIndex = nIndex;
			pNode->setParent(pParent.get(), nIndex);
			nType = 0;
			if(pNode->isLeaf())
			{
				//ClearChache();
				return TIterator(this, pNode.get(), pNode->binary_search(comp, key));
				break;
			}
			nNextAddr = pNode->inner_lower_bound(comp, key, nType, nIndex);
			pParent = pNode;
		}
		ClearChache();
		return TIterator(this, NULL,-1);
	}




	template<class TIterator, class TComp>
	TIterator find(TIterator& itFrom, TComp& comp, const TKey& key, bool bFoundNext = true)
	{
		TIterator it(this, itFrom.m_pCurNode.get(), itFrom.m_pCurNode->binary_search(comp, key));
		if(!it.isNull() || !bFoundNext)
			return it;
		return find<TIterator, TComp>(comp, key);
		
	}

	template<class TIterator>
	TIterator begin()
	{
		TBTreeNodePtr pFindBTNode(NULL);
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, -1);

		if(!m_pRoot.get())
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot.get())
			return TIterator(this, NULL, NULL);
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot.get(), m_pRoot->count() ? 0 : -1);
		}
		int64 nNextAddr = m_pRoot->less();
		//int64 nParent = m_pRoot->addr();
		TBTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNodePtr pNode = getNode(nNextAddr);
			//pNode->m_nParent = nParent;
			//pNode->m_nFoundIndex = -1;
			pNode->setParent(pParent.get());
			if(pNode->isLeaf())
			{
				pFindBTNode = pNode;
				break;
			}
			nNextAddr = pNode->less();
			pParent = pNode;
		}
		//ClearChache();
		return TIterator(this, pFindBTNode.get(), 0);
	}
	template<class TIterator>
	TIterator last()
	{
		TBTreeNodePtr pFindBTNode(NULL);
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, 0);

		if(!m_pRoot.get())
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot.get())
			return TIterator(this, NULL, 0);
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot.get(), m_pRoot->count()  - 1);
		}
		int64 nNextAddr = m_pRoot->backLink();
		//int64 nParent = m_pRoot->addr();
		int32 nIndex = m_pRoot->count() - 1;
		TBTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNodePtr pNode = getNode(nNextAddr);
			//pNode->m_nParent = nParent;
			//pNode->m_nFoundIndex = nIndex;

			pNode->setParent(pParent.get(), nIndex);

			if(pNode->isLeaf())
			{
				pFindBTNode = pNode;
				break;
			}
			nNextAddr = pNode->backLink();
			//nParent = pNode->addr();
			pParent = pNode;
			nIndex = pNode->count() - 1;
		}
	//	ClearChache();
		return TIterator(this, pFindBTNode.get(), pFindBTNode.get() ? pFindBTNode->count() - 1 : -1);
	}
	template<class TIterator, class TComp>
	TIterator upper_bound(const TComp& comp, const TKey& key)
	{
		 
		int32 nIndex = 0;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, 0);

		if(!m_pRoot.get())
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot.get())
			return TIterator(this, NULL, 0);

	 
		if(m_pRoot->isLeaf())
		{
			return TIterator(this, m_pRoot.get(), m_pRoot->leaf_upper_bound(comp, key));
		}
		nIndex = -1;
		int64 nNextAddr = m_pRoot->inner_upper_bound(comp, key, nIndex);
		//int64 nParent = m_pRoot->addr();
		 TBTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNodePtr pNode = getNode(nNextAddr);
			//pNode->m_nParent = nParent;
			//pNode->m_nFoundIndex = nIndex;

			pNode->setParent(pParent.get(), nIndex);
			if(pNode->isLeaf())
			{
				ClearChache();
				return TIterator(this, pNode.get(), pNode->leaf_upper_bound(comp, key));
				break;
			}
			nNextAddr = pNode->inner_upper_bound(comp, key, nIndex);
			//nParent = pNode->addr();
			pParent = pNode;
		}
		ClearChache();
		return TIterator(this, NULL,-1);
	}
	template<class TIterator, class TComp>
	TIterator lower_bound(const TComp& comp, const TKey& key)
	{
		int32 nIndex = 0;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return TIterator(this, NULL, 0);

		if(!m_pRoot.get())
			m_pRoot = getNode(m_nRootAddr, true); 
		if(!m_pRoot.get())
			return TIterator(this, NULL, 0);

		short nType = 0;
		if(m_pRoot->isLeaf())
		{
			
			return TIterator(this, m_pRoot.get(), m_pRoot->leaf_lower_bound(comp, key, nType));
		}
		nIndex = -1;
		int64 nNextAddr = m_pRoot->inner_lower_bound(comp, key, nType, nIndex);
		//int64 nParent = m_pRoot->addr();
		 TBTreeNodePtr pParent = m_pRoot;
		for (;;)
		{
			if( nNextAddr == -1)
			{
				break;
			}
			TBTreeNodePtr pNode = getNode(nNextAddr);
			//pNode->m_nParent = nParent;
			//pNode->m_nFoundIndex = nIndex;
			pNode->setParent(pParent.get(), nIndex);
			if(pNode->isLeaf())
			{
				//ClearChache();
				return TIterator(this, pNode.get(), pNode->leaf_lower_bound(comp, key, nType));
				break;
			}
			nNextAddr = pNode->inner_lower_bound(comp, key, nType, nIndex);
			//nParent = pNode->addr();
			pParent = pNode;
		}
		//ClearChache();
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



	void SetParentNext(TBTreeNode *pNode, TBTreeNode* pNodeNext)
	{
		int nFoundIndex = pNode->foundIndex();
		TBTreeNodePtr pParent = getNode(pNode->parentAddr());
		if((nFoundIndex + 1) < (int)pParent->count() || nFoundIndex == -1)
		{
			pNodeNext->setParent(pParent.get(), nFoundIndex + 1);
			return;
		}


		nFoundIndex = pParent->foundIndex();
		pParent = getNode(pParent->parentAddr());

		while(pParent.get())
		{
			
			if((nFoundIndex + 1) < (int)pParent->count() || nFoundIndex == -1)
			{
				break;
			}
			nFoundIndex = pParent->foundIndex();
			pParent = getNode(pParent->parentAddr());

		}
		if(!pParent.get())
			pParent = m_pRoot;

		TBTreeNodePtr pNextParentNode = getNode(pParent->link( nFoundIndex + 1));
		pNextParentNode->setParent(pParent.get(),  nFoundIndex + 1);

		TBTreeNodePtr pLessNode =  getNode(pNextParentNode->less());

		while(!pLessNode->isLeaf())
		{
			pLessNode->setParent(pNextParentNode.get(), -1);
			pNextParentNode = pLessNode;
			pLessNode =  getNode(pNextParentNode->less());
		}

		assert(pNodeNext->addr() == pNextParentNode->less());
		pNodeNext->setParent(pNextParentNode.get(), -1);
	}


	void SetParentBack(TBTreeNode *pNode, TBTreeNode* pNodePrev)
	{
		int nFoundIndex = pNode->foundIndex();
		TBTreeNodePtr pParent = getNode(pNode->parentAddr());
		if( nFoundIndex != -1)
		{
			pNodeNext->setParent(pParent.get(),  nFoundIndex - 1);
			return;
		}


		nFoundIndex = pParent->foundIndex();
		pParent = getNode(pParent->parentAddr());

		while(pParent.get())
		{

			if( nFoundIndex != -1)
			{
				break;
			}
			nFoundIndex = pParent->foundIndex();
			pParent = getNode(pParent->parentAddr());

		}
		if(!pParent.get())
			pParent = m_pRoot;

		TBTreeNodePtr pNextParentNode = getNode(pParent->link(nFoundIndex - 1));
		pNextParentNode->setParent(pParent.get(),  nFoundIndex - 1);

		TBTreeNodePtr pPrevNode =  getNode(pNextParentNode->link(pNextParentNode->count()));

		while(!pLessNode->isLeaf())
		{
			pLessNode->setParent(pNextParentNode.get(), -1);
			pNextParentNode = pLessNode;
			pLessNode =  getNode(pNextParentNode->less());
		}

		assert(pNodeNext->addr() == pNextParentNode->less());
		pNodeNext->setParent(pNextParentNode.get(), -1);
	}


	template<class TIterator>
	bool remove(TIterator& it)
	{
		if(it.isNull())
			return false;

		TBTreeNodePtr pNode = it.m_pCurNode;
		TLeafNode *pLeafNode = it.m_pCurLeafNode;
		TKey key = it.key();

		pLeafNode->removeByIndex(it.m_nIndex);
		pNode->setFlags(CHANGE_NODE, true);
		//m_ChangeNode.insert(pNode);
		assert(pLeafNode->count());

		int nFoundIndex = pNode->foundIndex();

		TBTreeNodePtr pParentNode = getNode(pNode->parentAddr());
		if(!pParentNode.get())
			return true;
		if(pLeafNode->size() >  m_pTransaction->getPageSize()/2)
		{

			if(pNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pNode->foundIndex()))
			{
				//TBTreeNodePtr pIndexNode = getNode(pParentNode->link(pNode->m_nFoundIndex ));
				pParentNode->updateKey(pNode->foundIndex() , pNode->key(0));
				pParentNode->setFlags(CHANGE_NODE, true);
				//m_ChangeNode.insert(pParentNode);
			}

			return removeUP(key, pParentNode.get());
		}
		if(pLeafNode->getFlags() & ROOT_NODE)
		{
			return true;
		}


		assert(pParentNode.get());
		assert(!pParentNode->isLeaf());


		TBTreeNodePtr pDonorNode(NULL);
		bool bLeft = false;
		
		if(pParentNode->less() == pNode->addr())
		{
			pDonorNode = getNode(pParentNode->link(0));
			//pDonorNode->m_nFoundIndex = 0;
			pDonorNode->setParent(pParentNode.get(), 0);
			bLeft = false;
			assert(pDonorNode->isLeaf());
		}
		else
		{

			TBTreeNodePtr pLeafNodeRight(NULL);
			TBTreeNodePtr pLeafNodeLeft(NULL);

			if(pNode->foundIndex() == 0)
			{
				pLeafNodeLeft = getNode(pParentNode->less());
				//pLeafNodeLeft->m_nFoundIndex = -1;
				//pLeafNodeLeft->m_nParent = pParentNode->addr();
				pLeafNodeLeft->setParent(pParentNode.get());
				if(pParentNode->count() > 1)
				{
					pLeafNodeRight = getNode(pParentNode->link(1));
					//pLeafNodeRight->m_nFoundIndex = 1;
					//pLeafNodeRight->m_nParent = pParentNode->addr();
					pLeafNodeRight->setParent(pParentNode.get(), 1);
				}
			}
			else
			{
				pLeafNodeLeft = getNode(pParentNode->link(pNode->foundIndex() - 1));
				//pLeafNodeLeft->m_nFoundIndex = pNode->m_nFoundIndex - 1;
				//pLeafNodeLeft->m_nParent = pParentNode->addr();
				pLeafNodeLeft->setParent(pParentNode.get(), pNode->foundIndex() - 1);
				if((int32)pParentNode->count() > pNode->foundIndex() + 1)
				{
					pLeafNodeRight = getNode(pParentNode->link(pNode->foundIndex()  + 1));
					//pLeafNodeRight->m_nFoundIndex = pNode->m_nFoundIndex + 1;
					//pLeafNodeRight->m_nParent = pParentNode->addr();
					pLeafNodeRight->setParent(pParentNode.get(), pNode->foundIndex() + 1);
				}
			}
 
			assert(pLeafNodeLeft.get() != NULL || pLeafNodeRight.get() != NULL);
			assert(pLeafNodeLeft != pNode && (pLeafNodeRight != pNode) && (pLeafNodeLeft != pLeafNodeRight));

			int nLeftsize =  pLeafNodeLeft.get() ? pLeafNodeLeft->rowSize() : -1;
			int nRightSize = pLeafNodeRight.get() ? pLeafNodeRight->rowSize() : -1;
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
		assert(pDonorNode.get());
		 
		size_t nSumSize = pDonorNode->rowSize() + pLeafNode->rowSize() + pNode->headSize();
		int nCnt = ((pLeafNode->count() + pDonorNode->count()))/2 - pLeafNode->count();

		bool bUnion = false;
		bool bAlignment = false;

		if(nSumSize <  m_pTransaction->getPageSize())	
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
			if(pDonorNode->foundIndex() == -1)
			{
				bRet = UnionLeafNode(pParentNode.get(), pDonorNode.get(), pNode.get(), false);
				nFoundIndex = -1;
			}
			else
			{
				bRet = UnionLeafNode(pParentNode.get(), pNode.get(), pDonorNode.get(), bLeft);
				nFoundIndex = pNode->foundIndex();
			}
			if(!bRet)
			{
				return false;
			}
		}
		else if(bAlignment)
		{
			if(!AlignmentLeafNode(pParentNode.get(), pNode.get(),  pDonorNode.get(),  bLeft))
				return false;

			assert(pNode->count());
			assert(pDonorNode->count());
			nFoundIndex = pNode->foundIndex();
		}
		if(nFoundIndex != -1 && pParentNode->isKey(m_comp, key, nFoundIndex))
		{
			TBTreeNodePtr pIndexNode = getNode(pParentNode->link(nFoundIndex));
			pParentNode->updateKey(nFoundIndex, pIndexNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
			//m_ChangeNode.insert(pParentNode);
		}
 
		return removeUP(key, pParentNode.get());
	 }



	 template<class TIterator>
	 TIterator remove(TIterator& it, int &nRet)
	 {
		 nRet = 1;
		 if(it.isNull())
		 {
			 nRet = -1;
			 return it;
		 }

		 TBTreeNodePtr pNode = it.m_pCurNode;
		 TLeafNode *pLeafNode = it.m_pCurLeafNode;
		 TKey key = it.key();

		 int nIndexNext = -1;
		 TBTreeNodePtr pNodeNext = pNode;
		
		if((int)pLeafNode->count() > it.m_nIndex + 1)
			 nIndexNext = it.m_nIndex;
		 else
		 {
			 pNodeNext = getNode(pNode->next());
			 if(pNode->next() != -1)
			 {
				 nIndexNext = 0;
				 SetParentNext(pNode.get(), pNodeNext.get());
			 }
		 }

		 pLeafNode->removeByIndex(it.m_nIndex);
		 pNode->setFlags(CHANGE_NODE, true);
		 //m_ChangeNode.insert(pNode);
		 assert(pLeafNode->count());

		 int nFoundIndex = pNode->foundIndex();

		 TBTreeNodePtr pParentNode = getNode(pNode->parentAddr());
		 if(!pParentNode.get())
		 {
			  return TIterator(this, pNodeNext.get(), nIndexNext);
			 
		 }
		 if(pLeafNode->size() >  m_pTransaction->getPageSize()/2)
		 {

			 if(pNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pNode->foundIndex()))
			 {
				 //TBTreeNodePtr pIndexNode = getNode(pParentNode->link(pNode->m_nFoundIndex ));
				 pParentNode->updateKey(pNode->foundIndex() , pNode->key(0));
				 pParentNode->setFlags(CHANGE_NODE, true);
				 //m_ChangeNode.insert(pParentNode);
			 }

			 nRet = removeUP(key, pParentNode.get()) ? 1 : -1;
			 return TIterator(this, pNodeNext.get(), nIndexNext);
		 }
		 if(pLeafNode->getFlags() & ROOT_NODE)
		 {
			 return TIterator(this, pNodeNext.get(), nIndexNext);
		 }


		 assert(pParentNode.get());
		 assert(!pParentNode->isLeaf());


		 TBTreeNodePtr pDonorNode(NULL);
		 bool bLeft = false;

		 if(pParentNode->less() == pNode->addr())
		 {
			 pDonorNode = getNode(pParentNode->link(0));
			 //pDonorNode->m_nFoundIndex = 0;
			 pDonorNode->setParent(pParentNode.get(), 0);
			 bLeft = false;
			 assert(pDonorNode->isLeaf());
		 }
		 else
		 {

			 TBTreeNodePtr pLeafNodeRight(NULL);
			 TBTreeNodePtr pLeafNodeLeft(NULL);

			 if(pNode->foundIndex() == 0)
			 {
				 pLeafNodeLeft = getNode(pParentNode->less());
				 //pLeafNodeLeft->m_nFoundIndex = -1;
				 //pLeafNodeLeft->m_nParent = pParentNode->addr();
				 pLeafNodeLeft->setParent(pParentNode.get());
				 if(pParentNode->count() > 1)
				 {
					 pLeafNodeRight = getNode(pParentNode->link(1));
					 //pLeafNodeRight->m_nFoundIndex = 1;
					 //pLeafNodeRight->m_nParent = pParentNode->addr();
					 pLeafNodeRight->setParent(pParentNode.get(), 1);
				 }
			 }
			 else
			 {
				 pLeafNodeLeft = getNode(pParentNode->link(pNode->foundIndex() - 1));
				 //pLeafNodeLeft->m_nFoundIndex = pNode->m_nFoundIndex - 1;
				 //pLeafNodeLeft->m_nParent = pParentNode->addr();
				 pLeafNodeLeft->setParent(pParentNode.get(), pNode->foundIndex() - 1);
				 if((int32)pParentNode->count() > pNode->foundIndex() + 1)
				 {
					 pLeafNodeRight = getNode(pParentNode->link(pNode->foundIndex()  + 1));
					 //pLeafNodeRight->m_nFoundIndex = pNode->m_nFoundIndex + 1;
					 //pLeafNodeRight->m_nParent = pParentNode->addr();
					 pLeafNodeRight->setParent(pParentNode.get(), pNode->foundIndex() + 1);
				 }
			 }

			 assert(pLeafNodeLeft.get() != NULL || pLeafNodeRight.get() != NULL);
			 assert(pLeafNodeLeft != pNode && (pLeafNodeRight != pNode) && (pLeafNodeLeft != pLeafNodeRight));

			 int nLeftsize =  pLeafNodeLeft.get() ? pLeafNodeLeft->rowSize() : -1;
			 int nRightSize = pLeafNodeRight.get() ? pLeafNodeRight->rowSize() : -1;
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
		 assert(pDonorNode.get());

		 size_t nSumSize = pDonorNode->rowSize() + pLeafNode->rowSize() + pNode->headSize();
		 int nCnt = ((pLeafNode->count() + pDonorNode->count()))/2 - pLeafNode->count();

		 bool bUnion = false;
		 bool bAlignment = false;

		 if(nSumSize <  m_pTransaction->getPageSize())	
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
			// bool bRet = true;
			 if(pDonorNode->foundIndex() == -1)
			 {
				 if(pNode->addr() == pNodeNext->addr())
				 {
					 nIndexNext += pNode->count();
					 pNodeNext = pDonorNode;
				 }
				 nRet = UnionLeafNode(pParentNode.get(), pDonorNode.get(), pNode.get(), false) ? 1 : -1;
				 nFoundIndex = -1;
				
			 }
			 else
			 {

				 if(pNode->addr() == pNodeNext->addr())
				 {
					 if(bLeft)
						 nIndexNext += pDonorNode->count();
				 }
				 else if(pNodeNext->addr() == pDonorNode->addr())
				 {
					 pNodeNext = pNode;
					 assert(!bLeft);
					nIndexNext += pNode->count();
				 }

				 nRet = UnionLeafNode(pParentNode.get(), pNode.get(), pDonorNode.get(), bLeft) ? 1 : -1;
				 nFoundIndex = pNode->foundIndex();
				 
			 }
			 if(nRet != -1)
			 {				 
				 return TIterator(this, pNodeNext.get(), nIndexNext);
			 }
		 }
		 else if(bAlignment)
		 {
			 int nNodeCnt = pNode->count();
			 int nDonorCnt = pDonorNode->count();
			 if(!AlignmentLeafNode(pParentNode.get(), pNode.get(),  pDonorNode.get(),  bLeft))
			 {
				// return false;
				 nRet = -1;
				 return TIterator(this, pNodeNext.get(), nIndexNext);
			 }

			 if(pNode->addr() == pNodeNext->addr())
			 {
				 if(bLeft)
					 nIndexNext += (pNode->count() - nNodeCnt);
			 }
			 else if(pNodeNext->addr() == pDonorNode->addr())
			 {
				
				 assert(!bLeft);
				  pNodeNext = pNode;
				  nIndexNext = nNodeCnt;
					 
			 }


			 assert(pNode->count());
			 assert(pDonorNode->count());
			 nFoundIndex = pNode->foundIndex();
		 }
		 if(nFoundIndex != -1 && pParentNode->isKey(m_comp, key, nFoundIndex))
		 {
			 TBTreeNodePtr pIndexNode = getNode(pParentNode->link(nFoundIndex));
			 pParentNode->updateKey(nFoundIndex, pIndexNode->key(0));
			 pParentNode->setFlags(CHANGE_NODE, true);
			 //m_ChangeNode.insert(pParentNode);
		 }

		 nRet = removeUP(key, pParentNode.get());
		 return TIterator(this, pNodeNext.get(), nIndexNext);
	 }


	bool UnionLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode*pDonorNode, bool bLeft, 
		int *nCheckIndex = 0)
	{
		pLeafNode->UnionWith(pDonorNode, bLeft, nCheckIndex);
		pLeafNode->setFlags(CHANGE_NODE, true);
		/*if(nNodeSize > m_pTransaction->getPageSize())
		{
			//pLeafNode->splitIn()
			return false;
		}*/
	
		if(bLeft)
		{

			TBTreeNodePtr pPrevNode = getNode( pDonorNode->prev());
			if(pPrevNode.get())
			{
				assert(pPrevNode->isLeaf());
				pLeafNode->setPrev(pPrevNode->m_nPageAddr);
				pPrevNode->setNext(pLeafNode->m_nPageAddr);
				pPrevNode->setFlags(CHANGE_NODE, true);
				//m_ChangeNode.insert(pPrevNode);
			}
			else
				pLeafNode->setPrev(-1);
		}
		else
		{

			TBTreeNodePtr pNextNode = getNode( pDonorNode->m_LeafNode.m_nNext);
			if(pNextNode.get())
			{
				assert(pNextNode->isLeaf());
				pLeafNode->setNext(pNextNode->m_nPageAddr);
				pNextNode->setPrev(pLeafNode->m_nPageAddr);
				pNextNode->setFlags(CHANGE_NODE, true);
				//m_ChangeNode.insert(pNextNode);
			}
			else
				pLeafNode->m_LeafNode.m_nNext = -1;
		}

		pParentNode->removeByIndex(pDonorNode->foundIndex());
		if(bLeft && pLeafNode->foundIndex() != -1)
		{
			//pLeafNode->m_nFoundIndex--;
			pLeafNode->setFoundIndex(pLeafNode->foundIndex() - 1);
			assert(pLeafNode->foundIndex() >= 0);
			pParentNode->updateKey(pLeafNode->foundIndex(), pLeafNode->key(0));
 		}
 
			
		deleteNode(pDonorNode);
		pParentNode->setFlags(CHANGE_NODE|CHECK_REM_NODE, true);
		//m_ChangeNode.insert(pParentNode);
		return true;

	}

	bool AlignmentLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode* pDonorNode , bool bLeft)
	{
		assert(pDonorNode->count());
		if(!pLeafNode->AlignmentOf(pDonorNode, bLeft))
			return false;
		if(bLeft)
		{
			pParentNode->updateKey(pLeafNode->foundIndex(), pLeafNode->key(0));
			pLeafNode->setFoundIndex(-1);
		}
		else
			pParentNode->updateKey(pDonorNode->foundIndex(), pDonorNode->key(0));
		pLeafNode->setFlags(CHANGE_NODE, true);
		//m_ChangeNode.insert(pLeafNode);
		pDonorNode->setFlags(CHANGE_NODE, true);
		//m_ChangeNode.insert(pDonorNode);
		pParentNode->setFlags(CHANGE_NODE, true);
		//m_ChangeNode.insert(pParentNode);
		return true;
	}


	bool removeUP(const TKey& key, TBTreeNode* pInNode)
	{
		TBTreeNodePtr pCheckNode(pInNode);	 
		while(pCheckNode.get())
		{
			
			TBTreeNodePtr  pParentNode = getNode(pCheckNode->parentAddr());
			if(!pParentNode.get())
			{
				if(!pCheckNode->count() && pCheckNode == m_pRoot)
				{
					TBTreeNodePtr pNode = getNode(pCheckNode->less());
					assert(pNode.get());//Должна быть
					m_Cache.remove(pNode->addr());
					//m_ChangeNode.remove(m_pRoot);
					//m_pRoot.release();
					deleteNode(pCheckNode.get());
				
					m_pRoot = pNode;
					m_pRoot->setParent(NULL);
					m_nRootAddr = m_pRoot->addr();
					pNode->setFlags(CHANGE_NODE|ROOT_NODE, true);
					//m_ChangeNode.insert(pNode);
					saveBTreeInfo();
					m_nStateTree |= (eBPTDeleteInnerNode | eBPTNewRootNode);
					
				}
				//ClearChache();
				return true;
			}
			

			if(pCheckNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pCheckNode->foundIndex() ))
			{
				TBTreeNodePtr pIndexNode = getNode(pParentNode->link(pCheckNode->foundIndex() ));
				pParentNode->updateKey(pCheckNode->foundIndex(), pIndexNode->key(0));
				pParentNode->setFlags(CHANGE_NODE, true);
				//m_ChangeNode.insert(pParentNode);
			}
			if(!(pCheckNode->getFlags() & CHECK_REM_NODE))
			{
				pCheckNode = getNode(pCheckNode->parentAddr());//		break;
				continue;
			}
			pCheckNode->setFlags(CHECK_REM_NODE, false);
			if(pCheckNode->size() >  m_pTransaction->getPageSize()/2)
			{
				pCheckNode = getNode(pCheckNode->parentAddr());
				continue;
			}

			
			if(pCheckNode->count() == 0)
			{
				int n = 0;
				n++;
				/*if(pCheckNode->foundIndex() == -1)
				{
					pParentNode->setLess(pCheckNode->less());
					pParentNode->setFlags(CHANGE_NODE, true);
				
				}
				else
				{
					TBTreeNodePtr pMinPtr = getMinimumNode(getNode(pCheckNode->less()));
					assert(pMinPtr.get());
					pParentNode->updateKey(pCheckNode->foundIndex(), pMinPtr->key(0));
					pParentNode->setFlags(CHANGE_NODE, true);
				}
				deleteNode(pCheckNode.get());
				pCheckNode = pParentNode;
				continue;*/

			}
			TBTreeNodePtr pDonorNode(NULL);
			bool bLeft = false;

			if(pParentNode->less() == pCheckNode->addr())
			{
				pDonorNode = getNode(pParentNode->link(0));
				//pDonorNode->m_nFoundIndex = 0;
				pDonorNode->setParent(pParentNode.get(), 0);
				bLeft = false;
			}
			else
			{

				TBTreeNodePtr pLeafNodeRight(NULL);
				TBTreeNodePtr pLeafNodeLeft (NULL);

				if(pCheckNode->foundIndex()  == 0)
				{
					pLeafNodeLeft = getNode(pParentNode->less());
					//pLeafNodeLeft->m_nFoundIndex = -1;
					//pLeafNodeLeft->m_nParent = pParentNode->addr();
					pLeafNodeLeft->setParent(pParentNode.get());
					if(pParentNode->count() > 1)
					{
						pLeafNodeRight = getNode(pParentNode->link(1));
						//pLeafNodeRight->m_nFoundIndex = 1;
						//pLeafNodeRight->m_nParent = pParentNode->addr();
						pLeafNodeRight->setParent(pParentNode.get(), 1);
					}
				}
				else
				{
					pLeafNodeLeft = getNode(pParentNode->link(pCheckNode->foundIndex() - 1));
					//pLeafNodeLeft->m_nFoundIndex = pCheckNode->m_nFoundIndex - 1;
					//pLeafNodeLeft->m_nParent = pParentNode->addr();
					pLeafNodeLeft->setParent(pParentNode.get(), pCheckNode->foundIndex() - 1);
					if((int32)pParentNode->count() > pCheckNode->foundIndex() + 1)
					{
						pLeafNodeRight = getNode(pParentNode->link(pCheckNode->foundIndex() + 1));
						//pLeafNodeRight->m_nFoundIndex = pCheckNode->m_nFoundIndex + 1;
						//pLeafNodeRight->m_nParent = pParentNode->addr();

						pLeafNodeRight->setParent(pParentNode.get(), pCheckNode->foundIndex() + 1);
					}
				}

				assert(pLeafNodeLeft.get() != NULL || pLeafNodeRight.get()!= NULL);
				assert(pLeafNodeLeft != pCheckNode && (pLeafNodeRight != pCheckNode) && (pLeafNodeLeft != pLeafNodeRight));


				int nLeftsize =  pLeafNodeLeft.get() ? pLeafNodeLeft->rowSize() : -1;
				int nRightSize = pLeafNodeRight.get() ? pLeafNodeRight->rowSize() : -1;
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
			assert(pDonorNode.get());
		
			size_t nSumSize = pCheckNode->rowSize() + pDonorNode->rowSize() + pDonorNode->headSize();
			nSumSize += pCheckNode->tupleSize(); //insert less	
			
			
			bool bUnion = false;
			bool bAlignment = false;
			int nCnt = ((pCheckNode->count() + pDonorNode->count()))/2 - pCheckNode->count();

			if(nSumSize <   m_pTransaction->getPageSize())	
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
					UnionInnerNode(pParentNode.get(), pDonorNode.get(), pCheckNode.get(), false);
					//pCheckNode = pDonorNode;
				}
				else
					UnionInnerNode(pParentNode.get(), pCheckNode.get(), pDonorNode.get(), bLeft);

				
				/*if(bLess)
				{
					pCheckNode = pParentNode;
					continue;
				}*/
				//assert(pParentNode->count());
			}
			else if(bAlignment)
			{
				if(AlignmentInnerNode(pParentNode.get(), pCheckNode.get(), pDonorNode.get(), bLeft))
				{
					pParentNode->setFlags(CHANGE_NODE, true);
					//m_ChangeNode.insert(pParentNode);
					assert(pCheckNode->count());
					assert(pDonorNode->count());
					assert(pParentNode->count());
					
				}

			}
			pCheckNode = pParentNode;//getNode(pCheckNode->m_nParent);
		}
		//ClearChache();
		return true;
	}


	bool UnionInnerNode(TBTreeNode* pParentNode, TBTreeNode* pNode, TBTreeNode* pDonorNode, bool bLeft)
	{

		TBTreeNodePtr pMinNode = bLeft ? getMinimumNode(getNode(pNode->less())) :   getMinimumNode(getNode(pDonorNode->less()));
		pNode->UnioInnerWith(pDonorNode, pMinNode->key(0)/*bLeft ? getMinimumKey(getNode(pNode->less())) :   getMinimumKey(getNode(pDonorNode->less()))*/,  bLeft);
		pNode->setFlags(CHANGE_NODE, true);
		pParentNode->removeByIndex(pDonorNode->foundIndex());
		if(bLeft && pNode->foundIndex() != -1)
		{
			//pNode->m_nFoundIndex--;
			pNode->setFoundIndex(pNode->foundIndex() - 1);
			assert(pNode->foundIndex() >= 0);
			 pMinNode = getMinimumNode(getNode(pNode->less()));
			pParentNode->updateKey(pNode->foundIndex() , pMinNode->key(0));
		}
		deleteNode(pDonorNode);
		SetParentInChildCacheOnly(pNode);

		assert(pParentNode->count() != 0 || pParentNode->less() != -1);
		pParentNode->setFlags(CHANGE_NODE|CHECK_REM_NODE, true);
		//m_ChangeNode.insert(pParentNode);
		m_nStateTree |= (eBPTDeleteInnerNode);
		return true;
	}


	TBTreeNodePtr  getMinimumNode(TBTreeNodePtr pNode)
	{
 
		TBTreeNodePtr pMinNode = pNode;
		while(!pMinNode->isLeaf())
		{
			pMinNode = getNode(pMinNode->less());
			assert(pMinNode.get());
		}
		assert(pMinNode.get());
		return pMinNode;
	}

	bool AlignmentInnerNode(TBTreeNode* pParentNode, TBTreeNode *pNode, TBTreeNode* pDonorNode, bool bLeft)
	{	


		
		TBTreeNodePtr pMinNode = bLeft ? getMinimumNode(getNode(pNode->less())) :   getMinimumNode(getNode(pDonorNode->less()));
		if(!pNode->AlignmentInnerNodeOf(pDonorNode, pMinNode->key(0)/*bLeft ? getMinimumKey(getNode(pNode->less())) :   getMinimumKey(getNode(pDonorNode->less()))*/,  bLeft))
			return false;

		if(!bLeft) //Нода донор справа
		{
			//const TKey& minKey = getMinimumKey(getNode(pDonorNode->less()));
			pMinNode = getMinimumNode(getNode(pDonorNode->less()));
			pParentNode->updateKey(pDonorNode->foundIndex(), pMinNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
			//m_ChangeNode.insert(pParentNode);
		}
		else
		{
			//const TKey& minKey = getMinimumKey(getNode(pNode->less()));
			pMinNode = getMinimumNode(getNode(pNode->less()));
			pParentNode->updateKey(pNode->foundIndex(),  pMinNode->key(0));
			pParentNode->setFlags(CHANGE_NODE, true);
			//m_ChangeNode.insert(pParentNode);
		}
	 
		SetParentInChildCacheOnly(pNode);
		SetParentInChildCacheOnly(pDonorNode);

		pNode->setFlags(CHANGE_NODE, true);
		//m_ChangeNode.insert(pNode);
		pDonorNode->setFlags(CHANGE_NODE, true);
		//m_ChangeNode.insert(pDonorNode);
		return true;
	}

		BPTreeStatisticsInfo m_BTreeInfo;
		TComp& getComp(){ return m_comp;}
	protected:

		TComp		 m_comp;
		TBTreeNodePtr m_pRoot; 
		TLink m_nRootAddr;
		TLink m_nRTreeStaticAddr;
		TLink m_nPageBTreeInfo;

		TLink m_nPageInnerCompInfo;
		TLink m_nPageLeafPageCompInfo;

		CommonLib::alloc_t* m_pAlloc;
		Transaction* m_pTransaction;
		//typedef RBSet<TBTreeNodePtr> TChangeNode;
		//TChangeNode m_ChangeNode;
		size_t m_nChacheSize;
		//typedef TSimpleCache<TLink, TBTreeNode> TNodesCache;
		typedef TCacheMRU<TLink, TBTreeNode> TNodesCache;
		TNodesCache m_Cache;
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
	class _TInnerNode = BPTreeInnerNodeSetv2<_TKey, /*_TComp,*/ _Transaction, _TInnerCompess>,
	class _TLeafNode =  BPTreeLeafNodeSetv2<_TKey,/*  _TComp, */_Transaction, _TLeafCompess>, 
	class _TBTreeNode = BPTreeNodeSetv2<_TKey,/* _TComp,*/ _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
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
				return TBase::find<iterator, TComp>(m_comp, key);
			}
			template<class _TComp>
			iterator find(_TComp& comp, const TKey& key)  
			{
				return TBase::find<iterator, _TComp>(comp, key);
			}
		 
			iterator find(iterator& itFrom, const TKey& key, bool bFoundNext = true)
			{
				return TBase::find<iterator, TComp>(itFrom, m_comp, key, bFoundNext);
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
				return TBase::upper_bound<iterator, TComp>(m_comp, key);
			}
			iterator lower_bound(const TKey& key)
			{
				return TBase::lower_bound<iterator, TComp>(m_comp, key);
			}

			template<class _Comp>
			iterator upper_bound(const _Comp& comp, const TKey& key)
			{
				return TBase::upper_bound<iterator, _Comp>(comp, key);
			}
			template<class _Comp>
			iterator lower_bound(const _Comp& comp, const TKey& key)
			{
				return TBase::lower_bound<iterator, _Comp>(comp, key);
			}


			

			bool  insert(const TKey& key, iterator *pFromIterator = NULL, iterator *pRetIterator = NULL)
			{
				return TBase::insert<iterator>(key, pFromIterator, pRetIterator);
			}

			bool remove(const TKey& key)
			{
				iterator it = find(key);
				if(it.isNull())
					return false;
				return TBase::remove<iterator>(it);
			}
			



			iterator remove(const TKey& key, int& nRet)
			{
				iterator it = find(key);
				if(it.isNull())
					return false;
				return TBase::remove<iterator>(it, nRet);
			}

			iterator remove(iterator it, int& nRet)
			{
				return TBase::remove<iterator>(it, nRet);
			}



		/* 
			iterator insertRetIT(const TKey& key, int& nRet)
			{
				return TBase::insertRetIT<iterator>(key, nRet);
			}

			iterator insertRetIT(const TKey& key, iterator& it, bool bFindNode, bool bFindIndex, int& nRet)
			{
				return TBase::insertRetIT<iterator>(key, it, bFindNode, bFindIndex, nRet);
			}*/
	};
	 
}
#endif