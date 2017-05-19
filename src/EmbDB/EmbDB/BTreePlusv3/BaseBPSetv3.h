#pragma once
#include <memory>
#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "CommonLibrary/delegate.h"

#include "../embDBInternal.h"
#include "../CompressorParams.h"
#include "../CacheLRU_2Q.h"

#include "../WriteStreamPage.h"
#include "../ReadStreamPage.h"
#include "../BPTreeStatistics.h"

#include "BPTreeNodeSetV3.h"
#include "BPInnerNodeSimpleCompressorV3.h"
#include "BPLeafNodeSetSimpleCompressorV3.h"


namespace embDB
{

	template <	class _TKey, class _TComp, class _Transaction,
		class _TInnerCompess = BPInnerNodeSimpleCompressorV3<_TKey>,
		class _TLeafCompess = BPLeafNodeSetSimpleCompressorV3<_TKey>,
		class _TInnerNode = BPTreeInnerNodeSetv3<_TKey, _Transaction, _TInnerCompess>,
		class _TLeafNode = BPTreeLeafNodeSetv3<_TKey,  _Transaction, _TLeafCompess>,
		class _TBTreeNode = BPTreeNodeSetv3<_TKey, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode>
	>
	class TBPlusTreeSetBaseV3
	{
	public: 


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
		typedef std::shared_ptr<TBTreeNode> TBTreeNodePtr;


		typedef typename TInnerNode::TInnerCompressorParams TInnerCompressorParams;
		typedef typename TLeafNode::TLeafCompressorParams TLeafCompressorParams;

		TBPlusTreeSetBaseV3(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, uint32 nNodesPageSize, bool bMulti = false, bool bCheckCRC32 = true) :
			m_nPageBTreeInfo(nPageBTreeInfo), m_pTransaction(pTransaction), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
			, m_bChangeRoot(false), m_nRootAddr(-1), m_bMulti(bMulti)
			, m_Cache(pAlloc)
			, m_pRoot(NULL)
			, m_nPageInnerCompInfo(-1)
			, m_nPageLeafPageCompInfo(-1)
			, m_bCheckCRC32(bCheckCRC32)
			, m_bMinSplit(false)
			, m_nNodesPageSize(nNodesPageSize)
		{

			 

		}
		~TBPlusTreeSetBaseV3()
		{
			 
		}


		void setTransactions(Transaction *pTransaction)
		{
			m_pTransaction = pTransaction;
		}


		Transaction * getTransactions() const
		{
			return m_pTransaction;
		}

		void SetMinSplit(bool bMinSplit)
		{
			m_bMinSplit = bMinSplit;

		}
		void setRootPage(int64 nPageBTreeInfo)
		{
			m_nPageBTreeInfo = nPageBTreeInfo;
		}
		bool  loadBTreeInfo()
		{
			if (m_nPageBTreeInfo == -1)
			{
				m_pTransaction->error(L"BTREE: Error Load  BTreeInfoPage: -1");
				return false;
			}
			FilePagePtr pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo, MIN_PAGE_SIZE);
			if (!pPage.get())
			{
				m_pTransaction->error(L"BTREE: Error load BTreeInfoPage: %I64d", (int64)m_nPageBTreeInfo);
				return false;
			}
			ReadStreamPage stream(m_pTransaction, MIN_PAGE_SIZE, m_bCheckCRC32, BTREE_PAGE, BTREE_INFO_PAGE);
			if (!stream.open(pPage))
			{
				return false;
			}

			m_nRootAddr = stream.readInt64();
			m_bMulti = stream.readBool();


			if (m_InnerCompParams.get() == NULL)
			{
				m_InnerCompParams.reset(TInnerCompess::LoadCompressorParams(m_pTransaction));
				if (m_InnerCompParams.get())
					m_InnerCompParams->load(&stream, m_pTransaction);
			}


			if (m_LeafCompParams.get() == NULL)
			{
				m_LeafCompParams.reset(TLeafCompess::LoadCompressorParams(m_pTransaction));
				if (m_LeafCompParams.get())
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
			if (!m_pRoot.get())
				return true;
			std::vector<TBTreeNodePtr> vecNodes;
			{
				typename TNodesCache::iterator it = m_Cache.begin();
				while (!it.isNull())
				{
					TBTreeNodePtr pBNode = it.object();
					if (pBNode->getFlags() & CHANGE_NODE)
					{
						vecNodes.push_back(pBNode);

					}
					it.next();
				}
				if (m_pRoot->getFlags() & CHANGE_NODE)
					CheckNodeBeforeSave(m_pRoot.get());
			}

			for (size_t i = 0, sz = vecNodes.size(); i < sz; ++i)
			{
				TBTreeNodePtr& pBNode = vecNodes[i];
				 CheckNodeBeforeSave(pBNode.get());
			}


			typename TNodesCache::iterator it = m_Cache.begin();
			while (!it.isNull())
			{
				TBTreeNodePtr pBNode = it.object();
				if (pBNode->getFlags() & CHANGE_NODE)
				{
					SaveNode(pBNode.get(), false);
					//pBNode->Save(m_pTransaction);
					//pBNode->setFlags(CHANGE_NODE, false);

 
				}
				it.next();
			}
			if (m_pRoot->getFlags() & CHANGE_NODE)
				SaveNode(m_pRoot.get(), false);

			if (m_LeafCompParams.get())
				m_LeafCompParams->SaveState(m_pTransaction);
			if (m_InnerCompParams.get())
				m_InnerCompParams->SaveState(m_pTransaction);
			return true;
		}

		bool init(int64 nBPTreePage, TInnerCompressorParams* pInnerCompParams = NULL, TLeafCompressorParams* pLeafCompParams = NULL)
		{
			if (m_pRoot.get() || m_nRootAddr != -1 || m_nPageBTreeInfo != -1)
				return false;


			if (pInnerCompParams)
			{
				m_InnerCompParams.reset(pInnerCompParams);
			}
			if (pLeafCompParams)
			{
				m_LeafCompParams.reset(pLeafCompParams);
			}



			m_nPageBTreeInfo = nBPTreePage;
			FilePagePtr pPage(NULL);
			if (m_nPageBTreeInfo == -1)
			{
				pPage = m_pTransaction->getNewPage(MIN_PAGE_SIZE);
				if (pPage.get())
					m_nPageBTreeInfo = pPage->getAddr();
				if (m_nPageBTreeInfo == -1)
				{
					m_pTransaction->error(L"BTREE: Error save BTreeInfoPage: -1", (int64)m_nPageBTreeInfo);
					return false;
				}
			}
			else
			{
				pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo, MIN_PAGE_SIZE, false);
				if (!pPage.get())
				{
					m_pTransaction->error(L"BTREE: Error save BTreeInfoPage: %I64d is not load", (int64)m_nPageBTreeInfo);
					return false;
				}
			}

			if (!createRootPage())
				return false;

			if (!pPage.get())
			{
				m_pTransaction->error(L"BTREE: Error save BTreeInfoPage: %I64d is not load", (int64)m_nPageBTreeInfo);
				return false;
			}

			WriteStreamPage stream(m_pTransaction, MIN_PAGE_SIZE, m_bCheckCRC32, BTREE_PAGE, BTREE_INFO_PAGE);
			stream.open(pPage);
			stream.write(m_nRootAddr);
			stream.write(m_bMulti);
			if (pInnerCompParams)
				pInnerCompParams->save(&stream, m_pTransaction);
			if (pLeafCompParams)
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
			if (m_pRoot.get())
				return true;

			if (m_nPageBTreeInfo != -1)
				loadBTreeInfo();
			if (m_nRootAddr == -1)
			{
				if (!createRootPage())
					return false;
			}
			else
			{
				m_pRoot = getNode(m_nRootAddr, true);
				m_nRootAddr = m_pRoot->m_nPageAddr;
			}
			if (!m_pRoot.get())
			{
				m_pTransaction->error(L"BTREE: Error load root page: %I64d", (int64)m_nRootAddr);
				return false;
			}
			return true;
		}

		TBTreeNodePtr CreateNode(int64 nAddr, bool bIsLeaf)
		{

			return TBTreeNodePtr(new TBTreeNode(-1, m_pAlloc, nAddr, m_bMulti, bIsLeaf, m_bCheckCRC32, m_nNodesPageSize, m_InnerCompParams.get(),
				m_LeafCompParams.get()), std::bind(&TBPlusTreeSetBaseV3::deleteNodePtr, this, std::placeholders::_1));
	 
		}
		void deleteNodePtr(TBTreeNode* pNode)
		{
			if (pNode->getFlags() & REMOVE_NODE)
			{
				delete pNode;
				return;
			}
			if (pNode->getFlags() & CHANGE_NODE)
				SaveNode(pNode, true);
			delete pNode;
		}
		virtual TBTreeNodePtr newNode(bool bIsRoot, bool bIsLeaf)
		{
			TBTreeNodePtr pNode = CreateNode(-1, bIsLeaf);

			pNode->Load(m_pTransaction);
			pNode->SetMinSplit(m_bMinSplit);
			//m_BTreeInfo.AddNode(1, bIsLeaf);
			if (bIsRoot)
				pNode->setFlags(ROOT_NODE, true);
			else
			{
				if (m_Cache.size() > m_nChacheSize)
				{
					m_Cache.remove_back();
				}
				m_Cache.AddElem(pNode->m_nPageAddr, pNode);
			}
			return TBTreeNodePtr(pNode);
		}

		void SaveNode(TBTreeNode *pNode, bool bCheckNode = true)
		{
			if (bCheckNode)
				CheckNodeBeforeSave(pNode);
			pNode->Save(m_pTransaction);
			pNode->setFlags(CHANGE_NODE, false);
		}


		void CheckNodeBeforeSave(TBTreeNode *pNode)
		{
			//	pNode->PreSave(m_pTransaction);
			if (pNode->isLeaf())
			{
				CheckLeafNode(pNode, true);
			}
			else if (pNode->isNeedSplit())
			{
				TBTreeNodePtr pParent = getParentNode(pNode);
				splitInnerNode(pNode, pParent);
			}
		}

		TBTreeNodePtr getParentNode(TBTreeNode *pNode)
		{
			TBTreeNodePtr pParent = pNode->parentNodePtr();
			if (!pParent.get())
			{
				pParent = getNode(pNode->parentAddr());
				pNode->setParent(pParent, pNode->foundIndex());
			}

			if (!pParent.get() && pNode->addr() != m_nRootAddr)
			{
				FindParent(pNode);
				pParent = pNode->parentNodePtr();
				if (!pParent.get())
				{
					pParent = getNode(pNode->parentAddr());
					pNode->setParent(pParent, pNode->foundIndex());
				}
			}

			return pParent;
		}

		void  FindParent(TBTreeNode *pCheckNode)
		{
			assert(pCheckNode->count() != 0);

			const TKey& key = pCheckNode->key(0);
			int32 nIndex = -1;
			int64 nNextAddr = m_pRoot->inner_lower_bound(m_comp, key, nIndex);
			TBTreeNodePtr pParent = m_pRoot;
			for (;;)
			{
				if (nNextAddr == -1)
				{
					break;
				}
				if (pCheckNode->addr() == nNextAddr)
				{
					pCheckNode->setParent(pParent, nIndex);
					break;
				}
				
				TBTreeNodePtr pNode = getNode(nNextAddr);
				if (!pNode.get())
				{
					break;
				}
				pNode->setParent(pParent, nIndex);				
				nNextAddr = pNode->inner_lower_bound(m_comp, key, nIndex);
				pParent = pNode;
			}
		}

		TBTreeNodePtr getNode(TLink nAddr, bool bNotMove = false, bool bCheckCache = true)
		{

			if (nAddr == -1)
				return TBTreeNodePtr(NULL);
			if (nAddr == m_nRootAddr && m_pRoot.get())
			{
				return m_pRoot;
			}
			TBTreeNodePtr pBNode = m_Cache.GetElem(nAddr, bNotMove);
			if (!pBNode.get())
			{
				FilePagePtr pFilePage = m_pTransaction->getFilePage(nAddr, m_nNodesPageSize);
				assert(pFilePage.get());
				if (!pFilePage.get())
				{
					return TBTreeNodePtr(NULL);
				}
				pBNode = CreateNode(nAddr, false);
				if (!pBNode->LoadFromPage(pFilePage.get(), m_pTransaction))
					return TBTreeNodePtr(nullptr);
				pBNode->SetMinSplit(m_bMinSplit);
				if (bCheckCache)
				{
					if (m_Cache.size() > m_nChacheSize)
						m_Cache.remove_back();
				}
				m_Cache.AddElem(pBNode->m_nPageAddr, pBNode);
			}
			return pBNode;
		}


		TBTreeNodePtr root()
		{
			if (!m_pRoot.get())
			{
				m_pRoot = getNode(m_nRootAddr);
			}
			return m_pRoot;
		}

		/*insert*/
		TBTreeNodePtr findLeafNodeForInsert(const TKey& key);
		bool insert(const TKey& key);
		void CheckLeafNode(TBTreeNode* pNode, bool bPreSave);
		void TransformRootToInner();
		void SplitRootInnerNode();
		void SetParentInChildCacheOnly(TBTreeNodePtr&  pNode);
		int splitLeafNode(TBTreeNode *pNode, TBTreeNode *pNewNode, TBTreeNodePtr& pParentNode);
		void SetParentNext(TBTreeNode *pNode, TBTreeNode* pNodeNext);
		void splitInnerNode(TBTreeNode *pInNode, TBTreeNodePtr& pParentNode);
	
		
		/*search*/
		template<class TIterator, class TComparator>
		TIterator find(const TComparator& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true);

		template<class TIterator>
		TIterator begin();

		template<class TIterator>
		TIterator last();

		template<class TIterator, class TComparator>
		TIterator upper_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true);

		template<class TIterator, class TComparator>
		TIterator lower_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator = NULL, bool bFindNext = true);


		/*remove*/
		bool remove(const TKey& key);
		TBTreeNodePtr findLeafNodeForRemove(const TKey& key);
		void RemoveFromLeafNode(TBTreeNodePtr& pLeafNode, int32 nIndex, const TKey& key);
		void RemoveFromInnerNode(TBTreeNodePtr pNode,  const TKey& key);
		void deleteNode(TBTreeNode* pNode);
		void UnionLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode*pDonorNode, bool bLeft);
		void AlignmentLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode* pDonorNode, bool bLeft);
		TBTreeNodePtr  getMinimumNode(TBTreeNodePtr pNode);
		void UnionInnerNode(TBTreeNodePtr& pParentNode, TBTreeNodePtr& pNode, TBTreeNodePtr& pDonorNode, bool bLeft);
		void AlignmentInnerNode(TBTreeNodePtr& pParentNode, TBTreeNodePtr& pNode, TBTreeNodePtr&pDonorNode, bool bLeft);
	protected:
		TComp		 m_comp;
		TBTreeNodePtr m_pRoot;
		TLink m_nRootAddr;
		TLink m_nPageBTreeInfo;
		uint32 m_nNodesPageSize;

		TLink m_nPageInnerCompInfo;
		TLink m_nPageLeafPageCompInfo;

		CommonLib::alloc_t* m_pAlloc;
		Transaction* m_pTransaction;
		uint32 m_nChacheSize;
		typedef TCacheLRU_2Q<TLink, TBTreeNodePtr> TNodesCache;
		TNodesCache m_Cache;
		bool m_bChangeRoot;
		bool m_bMulti;
		bool m_bCheckCRC32;

		std::auto_ptr<TLeafCompressorParams> m_LeafCompParams;
		std::auto_ptr<TInnerCompressorParams> m_InnerCompParams;
		uint32 m_nStateTree;
		bool m_bMinSplit;
	};
#define  BPSETBASE_TEMPLATE_PARAMS template <class _TKey, class _TComp, class _Transaction, class _TInnerCompess,class _TLeafCompess, class _TInnerNode, class _TLeafNode, class _TBTreeNode>
#define  BPSETBASE_DECLARATION  TBPlusTreeSetBaseV3<_TKey, _TComp, _Transaction, _TInnerCompess, _TLeafCompess, _TInnerNode, _TLeafNode, _TBTreeNode>

#include "BaseBPSet3Insert.h"
#include "BaseBPSet3Search.h"
#include "BaseBPSet3Delete.h"
}