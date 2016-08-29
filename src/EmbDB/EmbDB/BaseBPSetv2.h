#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_V2_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_V2_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPTreeNodeSetV2.h"
#include "embDBInternal.h"
#include "simple_stack.h"
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPInnerNodeSimpleCompressorV2.h"
#include "BPLeafNodeSetSimpleCompressorV2.h"
//#include "BPTree.h"
#include "CompressorParams.h"
#include "BPIteratorSetV2.h"
#include "CommonLibrary/delegate.h"
#include "CacheLRU.h"
#include "DBConfig.h"
#include "WriteStreamPage.h"
#include "ReadStreamPage.h"
#include <memory>
#include "BPTreeStatistics.h"
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

	//static int nLeafCount = 0;
	//static int nInnerCount = 0;

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


		TBPlusTreeSetV2(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodesPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
		  m_nPageBTreeInfo(nPageBTreeInfo), m_pTransaction(pTransaction), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
		 ,m_bChangeRoot(false), m_nRootAddr(-1), m_bMulti(bMulti)
		 ,m_Cache(pAlloc)
		 ,m_pRoot(NULL)
		// ,m_nRTreeStaticAddr(-1)
		 ,m_nPageInnerCompInfo(-1)
		 ,m_nPageLeafPageCompInfo(-1)
		 ,m_bCheckCRC32(bCheckCRC32)
	//	 ,m_BTreeInfo(bCheckCRC32)
		 ,m_nStateTree(eBPTNoChange)
		 ,m_bMinSplit(false)
		 ,m_nNodesPageSize(nNodesPageSize)
		 ,m_pBPTreeStatistics(NULL)
		{

			m_NodeRemove.reset(new TRemoveNodeDelegate(this, &TBPlusTreeSetV2::deleteNodeRef));

		}
		~TBPlusTreeSetV2()
		{
			DeleteNodes();
		}

		void DeleteNodes()
		{
			typename TNodesCache::iterator it = m_Cache.begin();
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
			m_Cache.clear();
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
		typedef CommonLib::IRefCntPtr<TBTreeNode> TBTreeNodePtr;
 
 
		typedef CommonLib::delegateimpl1_t<TBPlusTreeSetV2, CommonLib::RefCounter*> TRemoveNodeDelegate;
		std::auto_ptr<TRemoveNodeDelegate> m_NodeRemove;

		typedef typename TInnerNode::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TLeafNode::TLeafCompressorParams TLeafCompressorParams;
	
/*
		typedef TBPSetIteratorV2<TKey, TComp,Transaction, TInnerCompess, TLeafCompess,
		TInnerNode, TLeafNode, TBTreeNode> iterator;*/
		
	

		void setTransactions(Transaction *pTransaction)
		{
			m_pTransaction  = pTransaction;
		}


		Transaction * getTransactions() const
		{
			return m_pTransaction;
		}

		void SetMinSplit(bool bMinSplit)
		{
			m_bMinSplit = bMinSplit;
			
		}

		void SetBPTreeStatistics(CBPTreeStatistics* pBPTreeStatistics)
		{
			m_pBPTreeStatistics = pBPTreeStatistics;
		}

		void setRootPage(int64 nPageBTreeInfo)
		{
			m_nPageBTreeInfo = nPageBTreeInfo;
		}
		bool  loadBTreeInfo()
		{
			if(m_nPageBTreeInfo == -1)
			{
				m_pTransaction->error(L"BTREE: Error Load  BTreeInfoPage: -1");
				return false;
			}
			FilePagePtr pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo, MIN_PAGE_SIZE);
			if(!pPage.get())
			{
				m_pTransaction->error(L"BTREE: Error load BTreeInfoPage: %I64d", (int64)m_nPageBTreeInfo);
				return false;
			}
			ReadStreamPage stream(m_pTransaction, MIN_PAGE_SIZE, m_bCheckCRC32, BTREE_PAGE, BTREE_INFO_PAGE);
		 	if(!stream.open(pPage))
			{
				return false;
			}

			m_nRootAddr = stream.readInt64();
			m_bMulti = stream.readBool();


			if(m_InnerCompParams.get() == NULL)
			{
				m_InnerCompParams.reset(TInnerCompess::LoadCompressorParams(m_pTransaction));
				if(m_InnerCompParams.get())
					m_InnerCompParams->load(&stream, m_pTransaction);
			}
		

			if(m_LeafCompParams.get() == NULL)
			{
				m_LeafCompParams.reset(TLeafCompess::LoadCompressorParams(m_pTransaction));
				if(m_LeafCompParams.get())
					m_LeafCompParams->load(&stream, m_pTransaction);
			}
			 
	 
		
			
		

			return !m_pTransaction->isError();
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
		//	saveBTreeInfo();
		}
		bool commit()
		{
			if(!m_pRoot.get())
				return true;
			std::vector<TBTreeNode*> vecNodes;
			{			
				typename TNodesCache::iterator it =	m_Cache.begin();
				while(!it.isNull())
				{
					TBTreeNode* pBNode = it.object();
					if(pBNode->getFlags() & CHANGE_NODE)
					{
						vecNodes.push_back(pBNode);
						
					}
					it.next();
				}
				if(m_pRoot->getFlags() & CHANGE_NODE)
					CheckNodeBeforeSave(m_pRoot.get());
			}

			for (size_t i = 0, sz = vecNodes.size(); i < sz; ++i)
			{
				TBTreeNode* pBNode = vecNodes[i];
				CheckNodeBeforeSave(pBNode);
			}
			

			typename TNodesCache::iterator it =	m_Cache.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.object();
				if(pBNode->getFlags() & CHANGE_NODE)
				{
					SaveNode(pBNode, false);
					//pBNode->Save(m_pTransaction);
					//pBNode->setFlags(CHANGE_NODE, false);

					if(m_pBPTreeStatistics)
						m_pBPTreeStatistics->SaveNode(pBNode->isLeaf());
				}
				it.next();
			}
			if(m_pRoot->getFlags() & CHANGE_NODE)
			{

				SaveNode(m_pRoot.get(), false);

				if(m_pBPTreeStatistics)
					m_pBPTreeStatistics->SaveNode(m_pRoot->isLeaf());
			}

			m_nStateTree = eBPTNoChange;

			if(m_LeafCompParams.get())
				m_LeafCompParams->SaveState(m_pTransaction);
			if(m_InnerCompParams.get())
				m_InnerCompParams->SaveState(m_pTransaction);
			return true;
		}
		
	virtual TBTreeNode* CreateNode(int64 nAddr, bool bIsLeaf)
	{
							 
			TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, nAddr, m_bMulti, bIsLeaf, m_bCheckCRC32,  m_nNodesPageSize, m_InnerCompParams.get(),
				m_LeafCompParams.get());
		return pNode;
	}
	virtual TBTreeNodePtr newNode(bool bIsRoot, bool bIsLeaf)
	{
		if(m_pBPTreeStatistics)
			m_pBPTreeStatistics->CreateNode(bIsLeaf);

		TBTreeNode *pNode = CreateNode(-1, bIsLeaf);/*new TBTreeNode(-1, m_pAlloc, -1, m_bMulti, bIsLeaf, m_bCheckCRC32,  m_InnerCompParams.get(),
				 m_LeafCompParams.get() );*/
			pNode->Load(m_pTransaction);
			pNode->SetMinSplit(m_bMinSplit);
			//m_BTreeInfo.AddNode(1, bIsLeaf);
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
				FilePagePtr pFilePage = m_pTransaction->getFilePage(nAddr, m_nNodesPageSize);
				assert(pFilePage.get());
				if(!pFilePage.get())
				{
					return TBTreeNodePtr(NULL);
				}
				pBNode =  CreateNode(nAddr, false);//new TBTreeNode(-1, m_pAlloc, nAddr, m_bMulti, false, m_bCheckCRC32,  m_InnerCompParams.get(),  m_LeafCompParams.get());
				if(!pBNode->LoadFromPage(pFilePage.get(), m_pTransaction))
				{
					delete pBNode;
					return TBTreeNodePtr(NULL);
				}
				pBNode->SetMinSplit(m_bMinSplit);
				if(m_pBPTreeStatistics)
					m_pBPTreeStatistics->LoadNode(pBNode->isLeaf());


				pBNode->m_pRemFunk = m_NodeRemove.get();
				if(bCheckCache)
				{
					if(m_Cache.size() > m_nChacheSize)
					{
						TBTreeNode *pDelNode = m_Cache.remove_back();
						if(pDelNode)
						{
							if(pDelNode->getFlags() & CHANGE_NODE)
							{
								//pDelNode->Save(m_pTransaction);
								SaveNode(pDelNode);
								if(m_pBPTreeStatistics)
									m_pBPTreeStatistics->SaveNode(pDelNode->isLeaf());
							}
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
	 
		if(m_pBPTreeStatistics)
			m_pBPTreeStatistics->DeleteNode(pNode->isLeaf());
		m_pTransaction->dropFilePage(pNode->m_nPageAddr, m_nNodesPageSize);
		pNode->setFlags(REMOVE_NODE, true);
		m_Cache.remove(pNode->m_nPageAddr);
		 
		return true;
	}
	void deleteNodeRef(CommonLib::RefCounter *pRefPtr)
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
		for (uint32 i = 0, sz = m_Cache.size(); i < sz - m_nChacheSize; i++)
		{
			TBTreeNode* pDelNode = m_Cache.remove_back();//remove(pChNode->m_nPageAddr);
			if(!pDelNode)
				break;
			if(pDelNode)
			{
				if( pDelNode->getFlags() & CHANGE_NODE)
				{

 
					SaveNode(pDelNode);
					if(m_pBPTreeStatistics)
						m_pBPTreeStatistics->SaveNode(pDelNode->isLeaf());
				}

				assert(pDelNode->IsFree()); //TO DO
				delete pDelNode;
			}
		}
	
	}


/*	void save(CommonLib::IWriteStream *pStream)
	{
		pStream->write(m_nRootAddr);
		pStream->write(m_bMulti);
		pStream->write(m_nNodesPageSize);

		if(m_InnerCompParams.get())
			m_InnerCompParams->save(pStream);
		if(m_LeafCompParams.get())
			m_LeafCompParams->save(pStream);

	}


	void load(CommonLib::IReadStream *pStream)
	{
		m_nRootAddr = pStream->readInt64();
		m_bMulti = pStream->readBool();
		m_nNodesPageSize = pStream->readIntu32();

		if(!m_InnerCompParams.get())
			m_InnerCompParams.reset(TInnerCompess::LoadCompressorParams(m_pTransaction));
		m_InnerCompParams->load(pStream);
		if(!m_LeafCompParams.get())
			m_LeafCompParams.reset(TLeafCompess::LoadCompressorParams(m_pTransaction));

		m_LeafCompParams->load(pStream);

	}*/



	bool init(int64 nBPTreePage, TInnerCompressorParams* pInnerCompParams = NULL, TLeafCompressorParams* pLeafCompParams = NULL)
	{
		if(m_pRoot.get() || m_nRootAddr != -1 || m_nPageBTreeInfo != -1)
			return false;


		if(pInnerCompParams)
		{
			m_InnerCompParams.reset(pInnerCompParams);
		}
		if(pLeafCompParams)
		{
			m_LeafCompParams.reset(pLeafCompParams);
		}



		m_nPageBTreeInfo = nBPTreePage;
		FilePagePtr pPage (NULL);
		if(m_nPageBTreeInfo == -1)
		{
			pPage = m_pTransaction->getNewPage(MIN_PAGE_SIZE);
			if(pPage.get())
				m_nPageBTreeInfo = pPage->getAddr();
			if(m_nPageBTreeInfo == -1)
			{
				m_pTransaction->error(L"BTREE: Error save BTreeInfoPage: -1", (int64)m_nPageBTreeInfo);
				return false;
			}
		}
		else
		{
			pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo, MIN_PAGE_SIZE, false);
			if(!pPage.get())
			{
				m_pTransaction->error(L"BTREE: Error save BTreeInfoPage: %I64d is not load", (int64)m_nPageBTreeInfo);
				return false;
			}
		}

		if(!createRootPage())
			return false;

		if(!pPage.get())
		{
			m_pTransaction->error(L"BTREE: Error save BTreeInfoPage: %I64d is not load", (int64)m_nPageBTreeInfo);
			return false;
		}
		
		WriteStreamPage stream(m_pTransaction, MIN_PAGE_SIZE, m_bCheckCRC32, BTREE_PAGE, BTREE_INFO_PAGE);
		stream.open(pPage);
		stream.write(m_nRootAddr);
		stream.write(m_bMulti);
		if(pInnerCompParams)
			pInnerCompParams->save(&stream, m_pTransaction);
		if(pLeafCompParams)
			pLeafCompParams->save(&stream, m_pTransaction);

		stream.Save();

		m_InnerCompParams.release();
		m_LeafCompParams.release();
		return true;

	}
	bool createRootPage()
	{
		m_pRoot = newNode(true, true);
		m_nRootAddr = m_pRoot->m_nPageAddr; 
		m_pRoot->Save(m_pTransaction);
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
			m_pTransaction->error(L"BTREE: Error load root page: %I64d", (int64)m_nRootAddr);
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
		if(pFromIterator && pFromIterator->m_pCurNode.get())
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
	//	if(pNode.get())
	//		m_BTreeInfo.AddKey(1);

		bRet = pNode.get() ? true : false;	
		if(pRetIterator)
		{
			*pRetIterator = TIterator(this, pNode.get(), nIndex);
		}

		return bRet;	
	}
	

	void SaveNode(TBTreeNode *pNode, bool bCheckNode = true)
	{
		if(bCheckNode)
			CheckNodeBeforeSave(pNode);
		pNode->Save(m_pTransaction);
		pNode->setFlags(CHANGE_NODE, false);
	}
	

	void CheckNodeBeforeSave(TBTreeNode *pNode)
	{
	//	pNode->PreSave(m_pTransaction);
		if(pNode->isLeaf())
		{
			CheckLeafNode(pNode, true, NULL);
		}
		else if(pNode->isNeedSplit())
		{
			splitInnerNode(pNode);
		}
	}


	template<class TIterator, class TKeyFunctor>
	bool insertLast(TKeyFunctor& keyFunctor, TKey* pKey = NULL,  TIterator* pRetIterator = NULL)
		{

			TIterator it = last<TIterator>();
			if(!it.m_pCurLeafNode)
				return false;
			
			TKey key;
			if(it.m_nIndex == -1)
				key = keyFunctor.begin();
			else
				key = keyFunctor.inc(it.key());

			if(pKey)
				*pKey = key;
			TBTreeNodePtr pNode = InsertInLeafNode(it.m_pCurLeafNode, key);
			ClearChache();
		 
			return pNode.get() ? true : false;	
		}
		TBTreeNodePtr InsertInLeafNode(TBTreeNode *pNode, const TKey& key, int *pInIndex = NULL, int nInsertLeafIndex = -1)
		{
			assert(pNode->isLeaf());
			int nIndex = pNode->insertInLeaf(m_comp, key, nInsertLeafIndex);
			if(nIndex == -1)
			{
				m_pTransaction->error(L"BTREE: Error insert");
				return TBTreeNodePtr(NULL);
			}
			pNode->setFlags(CHANGE_NODE, true);
			
			//m_ChangeNode.insert(pNode);
			m_nStateTree |= eBPTChangeLeafNode;
		
			if(pInIndex)
				*pInIndex = nIndex;
			return CheckLeafNode(pNode, false, pInIndex);
		}
		TBTreeNodePtr CheckLeafNode(TBTreeNode *pNode, bool bPreSave, int *pInIndex = NULL)
		{
			TBTreeNodePtr  pCheckNode(pNode);
			TBTreeNodePtr pRetNode(pNode);
			int nInsertIndex = pInIndex ? *pInIndex : -1;
			TBTreeNodePtr pParentNode = getNode(pCheckNode->parentAddr());
			if(bPreSave)
				pCheckNode->PreSave(m_pTransaction);

			while(pCheckNode->isNeedSplit())
			{ 
				
				bool bNewRoot = false;
				if(!pParentNode.get())
				{
				 
					pParentNode  = newNode(false, true);
					if(!pParentNode.get())
					{
						m_pTransaction->error(L"BTREE: Error create new root node");
						return TBTreeNodePtr(NULL);
					}
					bNewRoot = true;
 
				}
 

				TBTreeNodePtr pNewLeafNode = newNode(false, true);
				int nSplitIndex = splitLeafNode(pCheckNode.get(), pNewLeafNode.get(), pParentNode.get());
				
				if(bPreSave)
				{
					if(bNewRoot)
						pParentNode->PreSave(m_pTransaction);
					pCheckNode->PreSave(m_pTransaction);
				}


				pNewLeafNode->setFlags(CHANGE_NODE, true);
				pParentNode->setFlags(CHANGE_NODE, true);
				pCheckNode->setFlags(CHANGE_NODE, true);
				if(pInIndex)
				{
					if(*pInIndex >= nSplitIndex)
					{
						*pInIndex = *pInIndex - nSplitIndex;
						pRetNode = pNewLeafNode;
					}
					else
						pRetNode = bNewRoot ? pParentNode : pCheckNode;
				}
				if(bNewRoot)
					pParentNode = pCheckNode;

				pCheckNode = pNewLeafNode;
				if(bPreSave)
					pCheckNode->PreSave(m_pTransaction);

			}

			if(pParentNode.get() && pParentNode->isNeedSplit())
			{
				if(!splitInnerNode(pParentNode.get()))
				{
					return TBTreeNodePtr(NULL);
				}
			}
			return pRetNode;
		}
	/*	TBTreeNodePtr splitLeafNode(TBTreeNode *pNode, TBTreeNode *pNewNode, TBTreeNode *pParentNode, int *pInIndex = NULL)
		{
			assert(pNewNode->isLeaf());
			assert(pNode->isLeaf());
			TBTreeNodePtr pRetNode(pNode);

			if(pNode->parentAddr() == -1)
			{
				//transform root

				TKey splitKey;
				assert(pParentNode->isLeaf());

				pNewNode->setParent(pNode, -1);
				

				int nSplitIndex = pNode->splitIn(pNewNode, pParentNode, &splitKey);
				pNode->clear();
				pNode->TransformToInner(m_pTransaction);

				pNode->setLess(pNewNode->addr());
				int nInsertIndex =  pNode->insertInInnerNode(m_comp, splitKey, pParentNode->addr());
				pParentNode->setParent(pNode, nInsertIndex);
				pNewNode->setNext(pParentNode->addr());
				pParentNode->setPrev(pNewNode->addr());
				if(pInIndex)
				{
					if(*pInIndex >= nSplitIndex)
					{
						*pInIndex = *pInIndex - nSplitIndex;
						pRetNode = pParentNode;
					}
					else
						pRetNode = pNewNode;
				}

				return   pRetNode; 
			}
			TKey splitKey;
			int nSplitIndex = pNode->splitIn(pNewNode, &splitKey);
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
					SetParentNext(pNode, pNextNode.get());
				}
				pNextNode->setFlags(CHANGE_NODE, true);
			}
			pNode->m_LeafNode.m_nNext = pNewNode->m_nPageAddr;
			pNewNode->m_LeafNode.m_nPrev = pNode->m_nPageAddr;

			//pNewNode->m_nParent = pParentNode->m_nPageAddr;
			int nInsertIndex = pParentNode->insertInInnerNode(m_comp, splitKey, pNewNode->m_nPageAddr);
			pNewNode->setParent(pParentNode, nInsertIndex);
		

			if(pInIndex)
			{
				if(*pInIndex >= nSplitIndex)
				{
					*pInIndex = *pInIndex - nSplitIndex;
					pRetNode = pNewNode;
				}
			}


			return pRetNode;
	}*/


		int splitLeafNode(TBTreeNode *pNode, TBTreeNode *pNewNode, TBTreeNode *pParentNode)
		{
			assert(pNewNode->isLeaf());
			assert(pNode->isLeaf());
			TBTreeNodePtr pRetNode(pNode);

			if(pNode->parentAddr() == -1)
			{
				//transform root

				TKey splitKey;
				assert(pParentNode->isLeaf());

				pNewNode->setParent(pNode, -1);


				int nSplitIndex = pNode->splitIn(pParentNode, pNewNode, &splitKey);
				pNode->clear();
				pNode->TransformToInner(m_pTransaction);
				pNode->setFlags(CHANGE_NODE, true);
				pNode->setLess(pParentNode->addr());
				int nInsertIndex =  pNode->insertInInnerNode(m_comp, splitKey, pNewNode->addr());
				pParentNode->setParent(pNode, nInsertIndex);
				pParentNode->setNext(pNewNode->addr());
				pNewNode->setPrev(pParentNode->addr());

				return nSplitIndex;
				/*if(pInIndex)
				{
					if(*pInIndex >= nSplitIndex)
					{
						*pInIndex = *pInIndex - nSplitIndex;
						pRetNode = pParentNode;
					}
					else
						pRetNode = pNewNode;
				}

				return   pRetNode; */
			}
			TKey splitKey;
			int nSplitIndex = pNode->splitIn(pNewNode, &splitKey);
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
					SetParentNext(pNode, pNextNode.get());
				}
				pNextNode->setFlags(CHANGE_NODE, true);
			}
			pNode->m_LeafNode.m_nNext = pNewNode->m_nPageAddr;
			pNewNode->m_LeafNode.m_nPrev = pNode->m_nPageAddr;

			//pNewNode->m_nParent = pParentNode->m_nPageAddr;
			int nInsertIndex = pParentNode->insertInInnerNode(m_comp, splitKey, pNewNode->m_nPageAddr);
			pNewNode->setParent(pParentNode, nInsertIndex);


		/*	if(pInIndex)
			{
				if(*pInIndex >= nSplitIndex)
				{
					*pInIndex = *pInIndex - nSplitIndex;
					pRetNode = pNewNode;
				}
			
			return pRetNode;*/
			return nSplitIndex;
		}

	bool splitInnerNode(TBTreeNode *pInNode)
	{

		if(pInNode == m_pRoot.get())
		{
			splitInnerRootNode(pInNode);
			return true;
		}

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
		
		while ( pNodeParent.get() != 0 )
		{
			// Add median to the parent
			int nIndex = pNodeParent->insertInInnerNode(m_comp, nMedianKey, pNodeNewRight->m_nPageAddr);
			pNodeNewRight->setParent(pNodeParent.get(), nIndex);
			if (pNodeParent->isNeedSplit())
			{
				pNodeParent->setFlags(CHANGE_NODE, true);

				if(pNodeParent == m_pRoot.get())
				{
					return splitInnerRootNode(pNodeParent.get());
				}

				pNodeNewRight = newNode(false, false);
				if(!pNodeNewRight.get())
				{
					m_pTransaction->error(L"BTREE: Error create new right B Node");
					return false;
				}

				pNodeNewRight->setFlags(CHANGE_NODE, true);
				if (!pNodeParent->splitIn(pNodeNewRight.get(), &nMedianKey))
				{
					return false;
				}
				SetParentInChildCacheOnly(pNodeNewRight.get());
				pNode = pNodeParent;

				// Move up
				pNodeParent = getNode(pNodeParent->parentAddr());
				m_nStateTree |= eBPTNewInnerNode;
			}
			else
			{
				pNodeParent->setFlags(CHANGE_NODE, true);
				break;
			}
		}
		return true;
	}

	bool splitInnerRootNode(TBTreeNode *pNode)
	{
		assert(pNode == m_pRoot.get());

		TBTreeNodePtr pLeftNode = newNode(false, false);
		TBTreeNodePtr pRightNode = newNode(false, false);
		
		pLeftNode->setFlags(CHANGE_NODE, true);
		pRightNode->setFlags(CHANGE_NODE, true);
		pNode->setFlags(CHANGE_NODE, true);



		TKey nMedianKey;
		pNode->splitIn(pLeftNode.get(), pRightNode.get(), &nMedianKey);
		int nIndex = pNode->insertInInnerNode(m_comp, nMedianKey, pRightNode->addr());
		pNode->setLess(pLeftNode->addr());
		pLeftNode->setParent(pNode, -1);
		pRightNode->setParent(pNode, nIndex);

		SetParentInChildCacheOnly(pLeftNode.get());
		SetParentInChildCacheOnly(pRightNode.get());
		return true;
	}

	void SetParentInChildCacheOnly(TBTreeNode *pNode)
	{
		assert(!pNode->isLeaf());
		TBTreeNode *pLessNode = m_Cache.GetElem(pNode->less(), true);
		if(pLessNode)
			pLessNode->setParent(pNode, -1);
		for (uint32 i = 0,  sz = pNode->count(); i < sz; ++i)
		{
			TBTreeNode *pChildNode = m_Cache.GetElem(pNode->link(i), true);
			if(pChildNode)
				pChildNode->setParent(pNode, i);
		}
	}

	
	template<class TIterator, class TComp>
	TIterator find(const TComp& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true)
	{
		if(pFromIterator)
		{
			TIterator it(this, pFromIterator->m_pCurNode.get(), pFromIterator->m_pCurNode->binary_search(comp, key));
			if(!it.isNull() || !bFindNext)
				return it;
		}

	

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




	/*template<class TIterator, class TComp>
	TIterator find(TIterator& itFrom, TComp& comp, const TKey& key, bool bFoundNext = true)
	{
		TIterator it(this, itFrom.m_pCurNode.get(), itFrom.m_pCurNode->binary_search(comp, key));
		if(!it.isNull() || !bFoundNext)
			return it;
		return find<TIterator, TComp>(comp, key);
		
	}*/

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
			return TIterator(this, NULL, -1);
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
	TIterator upper_bound(const TComp& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true)
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
	TIterator lower_bound(const TComp& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true)
	{

		if(pFromIterator)
		{
			if(pFromIterator->isNull() && !bFindNext)
				return TIterator(this, NULL, 0);

			 
		}

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
				int nLeafIndex =   pNode->leaf_lower_bound(comp, key, nType);
				if(nLeafIndex != -1)
					return TIterator(this, pNode.get(), pNode->leaf_lower_bound(comp, key, nType));
				else if(nIndex < (pParent->count() - 1))
				{
					TBTreeNodePtr pNode = getNode(pParent->link(nIndex + 1));
					pNode->setParent(pParent.get(), nIndex + 1);
					return TIterator(this, pNode.get(), 0);
				}

				if(pNode->next() == -1)
					return TIterator(this, NULL, -1);

				TBTreeNodePtr pNodeNext = getNode(pNode->next());
				SetParentNext(pNode.get(), pNodeNext.get());
				return TIterator(this, pNodeNext.get(), 0);
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


/*	void SetParentBack(TBTreeNode *pNode, TBTreeNode* pNodePrev)
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
	*/

	template<class TIterator>
	bool remove(const TIterator& it)
	{
		if(it.isNull())
			return false;

		TBTreeNodePtr pNode = it.m_pCurNode;
		TLeafNode *pLeafNode = it.m_pCurLeafNode;
		TKey key = it.key();

		pLeafNode->removeByIndex(it.m_nIndex);
		pNode->setFlags(CHANGE_NODE, true);


		if(pLeafNode->getFlags() & ROOT_NODE)
		{
			return true;
		}

		//m_ChangeNode.insert(pNode);
		assert(pLeafNode->count());

		int nFoundIndex = pNode->foundIndex();

		TBTreeNodePtr pParentNode = getNode(pNode->parentAddr());
		if(!pParentNode.get())
			return true;
		if(!pLeafNode->isHalfEmpty())
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

			uint32 nLeftCount =  pLeafNodeLeft.get() ? pLeafNodeLeft->count() : 0xFFFFFFFF;
			uint32 nRightCount= pLeafNodeRight.get() ? pLeafNodeRight->count() : 0xFFFFFFFF;
			if(nLeftCount < nRightCount)
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



		bool bUnion = pNode->IsHaveUnion(pDonorNode.get());
		bool bAlignment = false;
		if(!bUnion)
		{
			bAlignment = pNode->IsHaveAlignment(pDonorNode.get());
		}


	
		/*size_t nSumSize = pDonorNode->rowSize() + pLeafNode->rowSize() + pNode->headSize();
		int nCnt = ((pLeafNode->count() + pDonorNode->count()))/2 - pLeafNode->count();

		bool bUnion = false;
		

		if(nSumSize <  m_nNodesPageSize)	
			bUnion = true;
		else if(nCnt > 0)
			bAlignment = true;
		else
		{
			if(nSumSize < m_nNodesPageSize)
			{
				bUnion = true;
			}
		}*/
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
		 if(!pLeafNode->isHalfEmpty())
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

		 uint32 nSumSize = pDonorNode->rowSize() + pLeafNode->rowSize() + pNode->headSize();
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

					if(pNode->isLeaf())
					{
						pCheckNode->clear();
						pCheckNode->TransformToLeaf(m_pTransaction);
						pCheckNode->UnionWith(pNode.get(), false);
						pCheckNode->setFlags(ROOT_NODE, true);
					}
					else
					{
						pCheckNode->setLess(-1);
						pCheckNode->UnionInnerWith(pNode.get(), NULL, true);
					}
				

					m_Cache.remove(pNode->addr());
					deleteNode(pNode.get());
					/*m_Cache.remove(pNode->addr());
					deleteNode(pCheckNode.get());
				
					m_pRoot = pNode;
					m_pRoot->setParent(NULL);
					m_nRootAddr = m_pRoot->addr();
					pNode->setFlags(CHANGE_NODE|ROOT_NODE, true);
			
					saveBTreeInfo();
					m_nStateTree |= (eBPTDeleteInnerNode | eBPTNewRootNode);*/
					
				}
				//ClearChache();
				return true;
			}
			

			if(pCheckNode->foundIndex() != -1 && pParentNode->isKey(m_comp, key, pCheckNode->foundIndex() ))
			{
				TBTreeNodePtr pIndexNode = getNode(pParentNode->link(pCheckNode->foundIndex() ));
				TBTreeNodePtr pMinNode =  getMinimumNode(pIndexNode); 
				pParentNode->updateKey(pCheckNode->foundIndex(), pMinNode->key(0));
				pParentNode->setFlags(CHANGE_NODE, true);
				//m_ChangeNode.insert(pParentNode);
			}
			if(!(pCheckNode->getFlags() & CHECK_REM_NODE))
			{
				pCheckNode = getNode(pCheckNode->parentAddr());//		break;
				continue;
			}
			pCheckNode->setFlags(CHECK_REM_NODE, false);
			if(!pCheckNode->isHalfEmpty())
			{
				pCheckNode = getNode(pCheckNode->parentAddr());
				continue;
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


				uint32 nLeftCount =  pLeafNodeLeft.get() ? pLeafNodeLeft->count() : 0xFFFFFFFF;
				uint32 nRightCount = pLeafNodeRight.get() ? pLeafNodeRight->count() : 0xFFFFFFFF;
				if(nLeftCount < nRightCount)
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


			bool bUnion = false;
			bool bAlignment = false;

			bUnion = pCheckNode->IsHaveUnion(pDonorNode.get());
			if(!bUnion)
			{
				bAlignment = pCheckNode->IsHaveAlignment(pDonorNode.get());
			}

		
			/*size_t nSumSize = pCheckNode->rowSize() + pDonorNode->rowSize() + pDonorNode->headSize();
			nSumSize += pCheckNode->tupleSize(); //insert less	
			
			
			bool bUnion = false;
			bool bAlignment = false;
			int nCnt = ((pCheckNode->count() + pDonorNode->count()))/2 - pCheckNode->count();

			if(nSumSize <   m_nNodesPageSize)	
				bUnion = true;
			else if(nCnt > 0)
				bAlignment = true;
			else
			{
				if(nSumSize < m_nNodesPageSize)
				{
					bUnion = true;
				}
			}*/
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
		pNode->UnionInnerWith(pDonorNode, &pMinNode->key(0)/*bLeft ? getMinimumKey(getNode(pNode->less())) :   getMinimumKey(getNode(pDonorNode->less()))*/,  bLeft);
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

			pMinNode = getNode(pNode->less());
			pMinNode->setParent(pNode.get(), -1);
			pNode = pMinNode;
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

		//BPTreeStatisticsInfo m_BTreeInfo;
		TComp& getComp(){ return m_comp;}
		
		TInnerCompressorParams* GetInnerCompressorParams()
		{
			return m_InnerCompParams.get();
		}
		TLeafCompressorParams* GetLeafCompressorParams()
		{
			return m_LeafCompParams.get();
		}
	protected:

		TComp		 m_comp;
		TBTreeNodePtr m_pRoot; 
		TLink m_nRootAddr;
		//TLink m_nRTreeStaticAddr;
		TLink m_nPageBTreeInfo;


		uint32 m_nNodesPageSize;

		TLink m_nPageInnerCompInfo;
		TLink m_nPageLeafPageCompInfo;

		CommonLib::alloc_t* m_pAlloc;
		Transaction* m_pTransaction;
		uint32 m_nChacheSize;
		typedef TCacheLRU<TLink, TBTreeNode> TNodesCache;
		TNodesCache m_Cache;
		bool m_bChangeRoot;
		bool m_bMulti;
		bool m_bCheckCRC32;

		std::auto_ptr<TLeafCompressorParams> m_LeafCompParams;
		std::auto_ptr<TInnerCompressorParams> m_InnerCompParams;
		uint32 m_nStateTree;
		bool m_bMinSplit;

		CBPTreeStatistics* m_pBPTreeStatistics;
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

		TBPSetV2(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodePageSize, bool bMulti = false, bool bCheckCRC32 = true) :
			TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, nNodePageSize, bMulti, bCheckCRC32 )
			{

			}

			iterator find(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)  
			{
				return TBase::template find<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
			}
			template<class _TCustComp>
			iterator find(_TCustComp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)  
			{
				return TBase::template find<iterator, _TCustComp>(comp, key, pFromIterator, bFindNext);
			}
		 
			/*iterator find(iterator& itFrom, const TKey& key, bool bFoundNext = true)
			{
				return TBase::find<iterator, TComp>(itFrom, m_comp, key, bFoundNext);
			}*/


			iterator begin()
			{
				return TBase::template begin<iterator>();
			}

			iterator last()
			{
				return TBase::template last<iterator>();
			}


			iterator upper_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
			{
				return TBase::template upper_bound<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
			}
			iterator lower_bound(const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
			{
				return TBase::template lower_bound<iterator, TComp>(this->m_comp, key, pFromIterator, bFindNext);
			}

			template<class _Comp>
			iterator upper_bound(const _Comp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
			{
				return TBase::template upper_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
			}
			template<class _Comp>
			iterator lower_bound(const _Comp& comp, const TKey& key, iterator *pFromIterator = NULL, bool bFindNext = true)
			{
				return TBase::template lower_bound<iterator, _Comp>(comp, key, pFromIterator, bFindNext);
			}


			

			bool  insert(const TKey& key, iterator *pFromIterator = NULL, iterator *pRetIterator = NULL)
			{
				return TBase::template insert<iterator>(key, pFromIterator, pRetIterator);
			}

			bool remove(const TKey& key)
			{
				iterator it = find(key);
				if(it.isNull())
					return false;
				return TBase::template remove<iterator>(it);
			}
			



			iterator remove(const TKey& key, int& nRet)
			{
				iterator it = find(key);
				if(it.isNull())
					return false;
				return TBase::template remove<iterator>(it, nRet);
			}

			iterator remove(iterator it, int& nRet)
			{
				return TBase::template remove<iterator>(it, nRet);
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