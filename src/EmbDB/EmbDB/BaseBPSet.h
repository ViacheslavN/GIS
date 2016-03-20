#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "BPSetTraits.h"
#include "embDBInternal.h"
#include "simple_stack.h"
#include "CacheLRU.h"
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPIteratorSet.h"
#include "BPTree.h"
#include "CompressorParams.h"
namespace embDB
{


	/*template <class _TKey>
	class CKeyFunctor
	{
		_TKey inc(const _TKey& key);
		_TKey dec(const _TKey& key);
	};
	*/

	

	template <class _TKey,	class _TLink, class _TComp, class _Transaction,
	class _TInnerMemSet = RBMap<_TKey, _TLink, _TComp>,	
	class _TLeafMemSet = RBSet<_TKey, _TComp>,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<_TInnerMemSet> ,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressor<_TLeafMemSet>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeSet<_TLink, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeSet< _Transaction, _TInnerNode, _TLeafNode> >
	class TBPlusTreeSet /*: public IBPTree<_TKey, _TValue>*/
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


		TBPlusTreeSet(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, bool bMulti = false, bool bCheckCRC32 = true) :
		  m_nPageBTreeInfo(nPageBTreeInfo), m_pTransaction(pTransaction), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
		 ,m_bChangeRoot(false), m_nRootAddr(-1), m_bMulti(bMulti)
		 ,m_Chache(pAlloc)
		 ,m_ChangeNode(pAlloc)
		 ,m_pRoot(NULL)
		// ,m_nRTreeStaticAddr(-1)
		 ,m_nPageInnerCompInfo(-1)
		 ,m_nPageLeafPageCompInfo(-1)
		 ,m_bCheckCRC32(bCheckCRC32)
		 ,m_BTreeInfo(bCheckCRC32)
		 ,m_nStateTree(eBPTNoChange)
		{}
		~TBPlusTreeSet()
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
		typedef _TLink     TLink;
		typedef _Transaction  Transaction;
		typedef _TBTreeNode  TBTreeNode;
		typedef _TInnerMemSet TInnerMemSet;  
		typedef _TLeafMemSet TLeafMemSet; 
		typedef _TInnerCompess  TInnerCompess;
		typedef _TLeafCompess  TLeafCompess;
		typedef typename TInnerMemSet::TTreeNode TInnerMemSetNode;
		typedef typename TLeafMemSet::TTreeNode TLeftMemSetNode;
		typedef _TInnerNode TInnerNode;
		typedef _TLeafNode TLeafNode;
		typedef CompressorParamsBase<Transaction> TCompressorParamsBase;
		
		//typedef BPTreeNodeSet<TKey, TLink, TComp, Transaction, TInnerMemSet, TLeafMemSet, TInnerNode, TLeafNode> TBTreeNode;
		typedef BPTreeStatistics<_TLink, _Transaction, _TKey> BPTreeStatisticsInfo;
		typedef TBPSetIterator<TKey,  TLink, TComp, Transaction,  TInnerMemSet, 
			TLeafMemSet, TInnerCompess,	TLeafCompess ,TInnerNode , TLeafNode,  TBTreeNode> iterator;
		//TBTreeNode::TInnerMemSet::TTreeNode


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
				//m_BTreeInfo.clear();
				//m_BTreeInfo.Save(m_pTransaction);
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
			FilePagePtr pPage(m_pTransaction->getFilePage(m_nPageBTreeInfo));
			if(!pPage.get())
			{
				CommonLib::CString sMsg;
				sMsg.format(_T("BTREE: Error load BTreeInfoPage: %I64d"), (int64)m_nPageBTreeInfo);
				m_pTransaction->error(sMsg);
				return false;
			}

			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());

			sFilePageHeader header(stream, m_bCheckCRC32 && !pPage->isCheck());
			if(m_bCheckCRC32 && !pPage->isCheck() && !header.isValid())
			{
				CommonLib::CString sMsg;
				sMsg.format(_T("BTREE: Page %I64d Error CRC for info page"), (int64)m_nPageBTreeInfo);
				m_pTransaction->error(sMsg);
				return false;
			}
			pPage->setCheck(true);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_INFO_PAGE)
			{
				CommonLib::CString sMsg;
				sMsg.format(_T("BTREE: Page %I64d is not BTreeInfoPage"), (int64)m_nPageBTreeInfo);
				m_pTransaction->error(sMsg);
				return false;
			}
			m_nRootAddr = stream.readInt64();
		//	m_nRTreeStaticAddr = stream.readInt64();
			m_bMulti = stream.readBool();
			m_nPageInnerCompInfo = stream.readInt64();
			m_nPageLeafPageCompInfo = stream.readInt64();
		/*	if(m_nRTreeStaticAddr != -1)
			{
				m_BTreeInfo.setPage(m_nRTreeStaticAddr);
				m_BTreeInfo.Load(m_pTransaction);
			}*/


			m_InnerCompParams.reset(TInnerCompess::LoadCompressor(m_nPageInnerCompInfo, m_pTransaction));
			m_LeafCompParams.reset(TLeafCompess::LoadCompressor(m_nPageLeafPageCompInfo, m_pTransaction));
			
			return !m_pTransaction->isError();
		}


		void save(CommonLib::IWriteStream *pStream)
		{
			pStream->write(m_nRootAddr);
			pStream->write(m_bMulti);
			if(m_InnerCompParams.get())
				m_InnerCompParams->save(pStream);
			if(m_LeafCompParams.get())
				m_LeafCompParams->save(pStream);

		}


		void load(CommonLib::IReadStream *pStream)
		{
			m_nRootAddr = pStream->readInt64();
			m_bMulti = pStream->readInt64();

			if(!m_InnerCompParams.get())
				m_InnerCompParams.reset(TInnerCompess::LoadCompressor(-1, m_pTransaction));
			m_InnerCompParams->load(pStream);
			if(!m_LeafCompParams.get())
				m_LeafCompParams.reset(TLeafCompess::LoadCompressor(-1, m_pTransaction));

			m_LeafCompParams->load(pStream);

		}

		bool saveBTreeInfo()
		{
			FilePagePtr pPage(NULL);
			if(m_nPageBTreeInfo == -1)
			{
				pPage = m_pTransaction->getNewPage();
				if(pPage.get())
					m_nPageBTreeInfo = pPage->getAddr();
				if(m_nPageBTreeInfo == -1)
				{
					CommonLib::CString sMsg;
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
					CommonLib::CString sMsg;
					sMsg.format(_T("BTREE: Error save BTreeInfoPage: %I64d is not load"), (int64)m_nPageBTreeInfo);
					m_pTransaction->error(sMsg);
					return false;
				}
			}
					
			CommonLib::FxMemoryWriteStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header(stream, BTREE_PAGE, BTREE_INFO_PAGE);
			stream.write(m_nRootAddr);
			//stream.write(m_nRTreeStaticAddr);
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
			//m_BTreeInfo.Save(m_pTransaction);
			m_nStateTree = eBPTNoChange;
			return true;
		}
		
		TBTreeNode *newNode(bool bIsRoot, bool bIsLeaf)
		{
				TBTreeNode *pNode = new TBTreeNode(-1, m_pAlloc, -1, m_bMulti, bIsLeaf, m_bCheckCRC32, (ICompressorParams *)m_InnerCompParams.get(),
					(ICompressorParams *)m_LeafCompParams.get() );
				pNode->Load(m_pTransaction);
				//m_BTreeInfo.AddNode(1, bIsLeaf);
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
				FilePagePtr pFilePage(m_pTransaction->getFilePage(nAddr));
				assert(pFilePage.get());
				if(!pFilePage.get())
					return NULL;
				pBNode = new TBTreeNode(-1, m_pAlloc, nAddr, m_bMulti, false, m_bCheckCRC32, (ICompressorParams *)m_InnerCompParams.get(), (ICompressorParams *)m_LeafCompParams.get());
				if(!pBNode->LoadFromPage(pFilePage.get(), m_pTransaction))
					return NULL;
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
		//m_BTreeInfo.AddNode(-1, pNode->isLeaf());
		m_Chache.remove(pNode->m_nPageAddr);
		m_pTransaction->dropFilePage(pNode->m_nPageAddr);
		m_ChangeNode.remove(pNode);
		delete pNode;
		return true;
	}
	void ClearChache()
	{
		if(m_Chache.size() <= m_nChacheSize)
			return;
		for (uint32 i = 0, sz = m_Chache.size(); i < sz - m_nChacheSize; i++)
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
		/*FilePagePtr pFilePage = m_pTransaction->getNewPage();
		if(!pFilePage.get())
		{
			CommonLib::CString sMsg;
			sMsg.format(_T("BTREE: Error create new static page"));
			m_pTransaction->error(sMsg);
			return false;
		}
		//m_nRTreeStaticAddr = pFilePage->getAddr();*/
		//m_BTreeInfo.setPage(m_nRTreeStaticAddr);
		if(!saveBTreeInfo())
			return false;
		m_pRoot->Save(m_pTransaction);
		//m_BTreeInfo.Save(m_pTransaction);
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
			CommonLib::CString sMsg;
			sMsg.format(_T("BTREE: Error load root page: %I64d"), (int64)m_nRootAddr);
			m_pTransaction->error(sMsg);
			return false;
		}
		return true;
	}

	bool insert(const TKey& key)
	{
		if(!m_pRoot)
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
			}
			TChangeNode::iterator it = m_ChangeNode.begin();
			for(; !it.isNull(); ++it)
			{
				TBTreeNode* pChNode = *it;
				pChNode->setFlags(BUSY_NODE, false);
			}
			m_ChangeNode.clear();
 			ClearChache();
			//if(bRet)
			//	m_BTreeInfo.AddKey(1);
			return bRet;	
		}
		template<class TKeyFunctor>
		bool insertLast(TKeyFunctor& keyFunctor, TKey* pKey = NULL)
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
			TKey key = keyFunctor.inc(pMemNode->m_key);
			if(pKey)
				*pKey = key;
			bool bRet = InsertInLeafNode(pBNode, key);
			TChangeNode::iterator it = m_ChangeNode.begin();
			for(; !it.isNull(); ++it)
			{
				TBTreeNode* pChNode = *it;
				pChNode->setFlags(BUSY_NODE, false);
			}
			m_ChangeNode.clear();
			ClearChache();
			//if(bRet)
			//	m_BTreeInfo.AddKey(1);
			return bRet;	
		}
		TBTreeNode *getLastLeafNode()
		{
			if(m_pRoot->isLeaf())
				return m_pRoot;
			TLink nParent = m_pRoot->addr();
			TBTreeNode *pNode = getNode(m_pRoot->last());
			while(!pNode->isLeaf())
			{
				pNode->m_nParent = nParent;
				nParent = pNode->addr();
				pNode = getNode(pNode->last());
			}
			pNode->m_nParent = nParent;
			return pNode;
		}
		bool InsertInLeafNode(TBTreeNode *pNode, const TKey& key)
		{
			assert(pNode->isLeaf());
			if(!pNode->insertInLeaf(key))
			{
				CommonLib::CString sMsg;
				sMsg.format(_T("BTREE: Error insert"));
				m_pTransaction->error(sMsg);
				return false;
			}
			pNode->setFlags(CHANGE_NODE |BUSY_NODE, true);
			
			m_ChangeNode.insert(pNode);
			m_nStateTree |= eBPTChangeLeafNode;
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
						CommonLib::CString sMsg;
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
				pParentNode->m_InnerNode.m_nLess = pNode->m_nPageAddr;
			}
			if(pNode->m_LeafNode.m_nNext != -1)
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
					CommonLib::CString sMsg;
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
				CommonLib::CString sMsg;
				sMsg.format(_T("BTREE: Error create new root node"));
				m_pTransaction->error(sMsg);
				return false;
			}
			m_nStateTree |= eBPTNewRootNode;
			//pNodeNewRoot->m_innerMemSet.insert(nMedianKey, pNodeNewRight->m_nPageAddr);
			pNodeNewRoot->insertInInnerNode(nMedianKey, pNodeNewRight->m_nPageAddr);

			pNode->m_nParent = pNodeNewRoot->m_nPageAddr;
			pNodeNewRoot->m_InnerNode.m_nLess = pNode->m_nPageAddr;
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
	TKey* search(const TKey& key) //for debug
	{
		TKey* pValue = 0;
		if(m_nRootAddr == -1)
			return pValue;
		
		if(!m_pRoot)
		{
			m_pRoot= getNode(m_nRootAddr, true); 
		}
		if(!m_pRoot)
			return pValue;

		if(m_pRoot-> isLeaf())
		{
			TLeftMemSetNode *pRBNode = m_pRoot->findNode(key);//m_leafMemSet.findNode(key);
			if(pRBNode)
			{
				pValue =  &pRBNode->m_key;
			}
			 
		}
		else
		{
			int64 nNextAddr = m_pRoot->findNext(key);
			for (;;)
			{
				if( nNextAddr == -1)
					break;
				TBTreeNode* pNode = getNode(nNextAddr);
				if(pNode->isLeaf())
				{
					TLeftMemSetNode *pRBNode = pNode->findNode(key);//m_leafMemSet.findNode(key);
					if(pRBNode)
						pValue =  &pRBNode->m_key;
					break;
				}
				nNextAddr = pNode->findNext(key);
			}
		}
	

		ClearChache();
		return pValue;

	}
	iterator begin()
	{
		TBTreeNode* pFindBTNode = NULL;
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

	iterator last()
	{
		TBTreeNode* pFindBTNode = NULL;
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
			
			return iterator(this, m_pRoot, m_pRoot->lastLeftMemSetNode());
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
			nNextAddr = pNode->lastPage();
		}
		ClearChache();
		return iterator(this, pFindBTNode, pFindBTNode ? pFindBTNode->lastLeftMemSetNode() : NULL);
	}

	bool findNode(const TKey& key, bool bFirst, TBTreeNode** pFindBTNode,	TLeftMemSetNode **pFindMemNode,bool bNextKey = false )
	{

		*pFindBTNode = NULL;
		*pFindMemNode= NULL;
		if(m_nRootAddr == -1)
			loadBTreeInfo();
		if(m_nRootAddr == -1)
			return false;

		if(!m_pRoot)
		{
			m_pRoot= getNode(m_nRootAddr, true); 
		}
		if(!m_pRoot)
			return false;

		if(m_pRoot-> isLeaf())
		{
			TLeftMemSetNode *pRBNode = m_pRoot->findNode(key);//m_leafMemSet.findNode(key);
			if(pRBNode)
			{
				*pFindBTNode = m_pRoot;
				*pFindMemNode = pRBNode;
			}

		}
		else
		{

			TInnerMemSetNode* pParentMemsetNode = 0;
			TBTreeNode* pInnerParentNode  = 0;
			pInnerParentNode = m_pRoot;
			int64 nNextAddr = m_pRoot->findNext(key, &pParentMemsetNode);
			for (;;)
			{
				if( nNextAddr == -1)
					break;
				TBTreeNode* pNode = getNode(nNextAddr);
				//pNode->m_nParent = pInnerParentNode->addr();
				//pNode->m_pParrentMemsetNode = pParentMemsetNode;
				if(pNode->isLeaf())
				{
					TLeftMemSetNode *pRBNode = pNode->findNode(key);//m_leafMemSet.findNode(key);
					if(pRBNode)
					{						
						*pFindBTNode = pNode;
						*pFindMemNode = pRBNode;
						if(bFirst && m_bMulti)
						{
							TLeftMemSetNode *pPrev = pRBNode->m_pPrev;
							if(pNode->isLeamMemsetNodeNull(pPrev) && pNode->prev() != -1)
							{
								//TBTreeNode *pNode1 = pNode;
								TBTreeNode* pNodePrev = getNode(pNode->prev());
								TLeftMemSetNode *pRBNode = pNodePrev->findNode(key);
								if(pRBNode)
								{

									if(bNextKey)
									{
										if(pNodePrev->addr() == pInnerParentNode->less())
										{
											pNodePrev->m_bValidNextKey = true;
											pNodePrev->m_NextLeafKey = pInnerParentNode->firstInnerMemSetNode()->m_key;
										}
										else if(pParentMemsetNode)
										{
											if(!pInnerParentNode->isInnerMemsetNodeNull(pParentMemsetNode->m_pPrev)
												&& pParentMemsetNode->m_pPrev->m_val == pNodePrev->addr() )
											{
												pNodePrev->m_bValidNextKey = true;
												pNodePrev->m_NextLeafKey = pParentMemsetNode->m_key;
											}
											else
												pNodePrev->m_bValidNextKey = false;
										}
										else
											pNodePrev->m_bValidNextKey = false;
									}
									*pFindBTNode = pNodePrev;
									*pFindMemNode = pRBNode;
									break;
								}
							}
						}
						if(bNextKey)
						{
							if(pNode->addr() == pInnerParentNode->less())
							{
								pNode->m_bValidNextKey = true;
								pNode->m_NextLeafKey = pInnerParentNode->firstInnerMemSetNode()->m_key;
							}
							else if(pParentMemsetNode)
							{
								if(!pInnerParentNode->isInnerMemsetNodeNull(pParentMemsetNode->m_pNext))
								{
									pNode->m_bValidNextKey = true;
									pNode->m_NextLeafKey = pParentMemsetNode->m_pNext->m_key;
								}
								else
									pNode->m_bValidNextKey = false;
							}
							else
								pNode->m_bValidNextKey = false;
						}
					}
					break; //дальше искать не куда
				}
				pInnerParentNode = pNode;
				nNextAddr = pNode->findNext(key, &pParentMemsetNode);
			}
		}
		if(*pFindBTNode)
			(*pFindBTNode)->setFlags(BUSY_NODE, true);
		ClearChache();

		return true;
	}
	iterator find(const TKey& key, bool bFirst = false)
	{

		TBTreeNode* pFindBTNode = 0;
		TLeftMemSetNode *pFindRBNode= 0;

		findNode(key, bFirst, &pFindBTNode, &pFindRBNode);
		return iterator(this, pFindBTNode, pFindRBNode);
	}
	
	/*bool search(const Tkey& key, IFieldVariant* value)
	{
		iterator it = find(key);
		if(it.isNull())
			return false;
		value->setVal(it.value());
		return false;
	}*/
	bool remove(const TKey& key, bool bFirst = false)
	{

		//TBTreeNode* pFindBTNode = 0;
		//TLeftMemSetNode *pFindRBNode= 0;
		if(m_nRootAddr == -1)
			return false;

		if(!m_pRoot)
		{
			m_pRoot= getNode(m_nRootAddr, true); 
		}
		if(!m_pRoot)
			return false;

		bool bRet = false;
		if(m_pRoot-> isLeaf())
		{
			return m_pRoot->remove(key);
		}
		else
		{
			TInnerMemSetNode* pRBNode = 0;
			TBTreeNode* pInnerParentNode  = 0;
			pInnerParentNode = m_pRoot;
			short nType = 0;
			int64 nNextAddr = m_pRoot->findNextForDelete(key, &pRBNode, nType);
			for (;;)
			{
				if( nNextAddr == -1)
					break;
				
				TBTreeNode* pNode = getNode(nNextAddr);
				assert(pNode);
				pNode->m_nParent = pInnerParentNode->m_nPageAddr;
				pNode->m_pParrentMemsetNode = pRBNode;
				pNode->m_nType = nType;
				if(pNode->isLeaf())
				{
					TLeftMemSetNode *pRBNode = pNode->findNode(key);
					if(pRBNode)
					{
						//pFindBTNode = pNode;
						//pFindRBNode = pRBNode;
						if(removeFromLeafNode(key, pNode, pRBNode, pInnerParentNode))
						{
							//m_BTreeInfo.AddKey(-1);
							bRet = true;
						}
					}
					break;
				}
				else
				{

				}
				pInnerParentNode = pNode;
				nNextAddr = pNode->findNextForDelete(key, &pRBNode, nType);
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
		return bRet;
	}
	bool removeUP(const TKey& key, TBTreeNode* pCheckNode, TInnerMemSetNode *pRBInnerKeyNode, short nType)
	{
		    		
		
			TInnerMemSetNode * pRBInnerNode = pRBInnerKeyNode;
			TBTreeNode* pParentNode = getNode(pCheckNode->m_nParent);
			TInnerMemSet*  innerTree = &pCheckNode->m_InnerNode.m_innerMemSet;

			if(nType == FIND_KEY)
			{
				assert(pRBInnerNode);
				TBTreeNode* pNode = getNode(pRBInnerNode->m_val);
				assert(pNode && pNode->isLeaf() && pNode->count() > 0);
				pRBInnerNode->m_key = pNode->firstKey();
				pCheckNode->setFlags(CHANGE_NODE, true);
				m_ChangeNode.insert(pCheckNode);
			}

			while(pCheckNode)
			{
				nType = pCheckNode->m_nType;
				pRBInnerNode = pCheckNode->m_pParrentMemsetNode;
				pParentNode =  getNode(pCheckNode->m_nParent);
				innerTree = &pCheckNode->m_InnerNode.m_innerMemSet;
				if(nType == FIND_KEY)
				{ //TO DO fix
					assert(pRBInnerNode);
					assert(pParentNode->addr() == pRBInnerNode->m_val);
					if(pCheckNode->count() > 0 || pCheckNode->less() != -1)
					{
						TKey *pKey = NULL;
						getMinimumKeyValue(pCheckNode, &pKey);
						pRBInnerNode->m_key = *pKey;
					}
					else
					{
						pParentNode->removeRBInnerNode(pRBInnerNode);
						pParentNode->setFlags(CHANGE_NODE, true);
						m_ChangeNode.insert(pParentNode);
						pCheckNode = getNode(pCheckNode->m_nParent);
						
						deleteNode(pCheckNode);
						m_nStateTree |= eBPTDeleteInnerNode;
						continue;
					}
					pParentNode->setFlags(CHANGE_NODE, true);
					m_ChangeNode.insert(pParentNode);
				}
				if(!(pCheckNode->getFlags() & CHANGE_NODE))
				{
					pCheckNode = getNode(pCheckNode->m_nParent);
					continue;
				}

				if(pCheckNode->size() < m_pTransaction->getPageSize()/2)
				{
					if(!pParentNode)
					{
						//root
						if(innerTree->size() == 0)
						{
							//делаем рутом листовую ноду
							TBTreeNode* pNode = getNode(pCheckNode->m_InnerNode.m_nLess);
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
							return true;
						 
						}
						return true;
					}
					TInnerMemSet& parentInnerTree = pParentNode->m_InnerNode.m_innerMemSet;
					if(pCheckNode->count() == 0)
					{
						bool bRem = false;
						if(pCheckNode->less() == -1)
						{
							bRem = true;
							if(pCheckNode->addr() == pParentNode->less())
							{
								pParentNode->setLess(-1);
								CreateLessNode(pParentNode, NULL);
							
							}
							else
							{
								//TTreeInnerNode *pRBInnerNode = innerTree->findNode(key);
								if(!innerTree->isNull(pRBInnerNode))
								{
									pParentNode->removeRBInnerNode(pRBInnerNode);
									pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
									m_ChangeNode.insert(pParentNode);
								}
							}
							TBTreeNode* pNode = pCheckNode;		
							pCheckNode = getNode(pCheckNode->m_nParent);
							deleteNode(pNode);
							m_nStateTree |= (eBPTDeleteInnerNode);
							continue;
						}
		 
					}
					TBTreeNode* pLeftNode = NULL;
					TBTreeNode* pRightNode = NULL;
					TBTreeNode* pDonorNode = NULL;
					bool bLeft = false;
					bool bLess = false;
					//TTreeInnerNode *pRBParentInnerNode = pCheckNode->m_pParrentRBNode;
					TInnerMemSetNode *pDonorParentKeyNode = pRBInnerNode;
					if(pParentNode->less() == pCheckNode->addr())
					{
						//крайнея левая нода, у нее только есть правый брат в контексте родителя
						pDonorParentKeyNode = parentInnerTree.minimumNode();
						if(!parentInnerTree.isNull(pDonorParentKeyNode))
						{
							pDonorNode = getNode(pDonorParentKeyNode->m_val);
							bLeft = false;
						}
						else
						{
							assert(false); //такого быть не должно 
						}
					}
					else
					{
		
						assert(pRBInnerNode);
						assert(pRBInnerNode->m_val == pCheckNode->m_nPageAddr);

						if(!parentInnerTree.isNull(pRBInnerNode->m_pPrev))
						{
							pLeftNode = getNode(pRBInnerNode->m_pPrev->m_val);
							assert(pLeftNode->addr() != pParentNode->less());
						}
						if(!parentInnerTree.isNull(pRBInnerNode->m_pNext))
						{
							pRightNode = getNode(pRBInnerNode->m_pNext->m_val);
							assert(pLeftNode->addr() != pParentNode->less());
						}
					

						if(!pLeftNode)// либо всего две ноды, либо удаляется из самой первой
						{
							bLess = true;
							pLeftNode = getNode(pParentNode->m_InnerNode.m_nLess);
						}

						if(!pLeftNode && !pRightNode)
						{
							pCheckNode = getNode(pCheckNode->m_nParent);
							continue;
						}
						uint32 nLeftsize =  pLeftNode ? pLeftNode->rowSize() : 0;
						uint32 nRightSize = pRightNode ? pRightNode->rowSize() : 0;
						if(nLeftsize > nRightSize)
						{
							pDonorNode = pLeftNode;
							if(bLess)
								pDonorParentKeyNode = pRBInnerNode;
							else
								pDonorParentKeyNode = pRBInnerNode->m_pPrev;
							bLeft = true;
						}
						else
						{
							pDonorNode = pRightNode;
							pDonorParentKeyNode = pRBInnerNode->m_pNext;
							bLeft = false;
						}
					}
					if(!pDonorNode)
					{
						pCheckNode = getNode(pCheckNode->m_nParent);
						continue;
					}
					bool bUnion = false;
					bool bAlignment = false;
					uint32 nSumSize = pCheckNode->rowSize() + pDonorNode->rowSize() + pDonorNode->headSize();


					if(bLeft)
					{
						if(pCheckNode->less() != -1)
						{
							//считаем самый худший вариант для компрессии
							nSumSize += pCheckNode->tupleSize();
						}
						else
						{
							assert(pDonorNode->less() != -1);//должна быть
							nSumSize += pDonorNode->tupleSize();
						}
						
					}

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
						UnionInnerNode(pParentNode, pCheckNode, pDonorNode, bLeft, pDonorParentKeyNode, pRBInnerNode);
						if(bLess)
						{
							pCheckNode = pParentNode;
							continue;
						}
					}
					else if(bAlignment)
					{
						AlignmentInnerNode(pCheckNode, pRBInnerNode, pDonorNode, pDonorParentKeyNode, bLeft);
						pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
						m_ChangeNode.insert(pParentNode);
					
					}

					

				}
		
				pCheckNode = getNode(pCheckNode->m_nParent);
				
			}
		return true;
	}
	bool CreateLessNode(TBTreeNode* pInnerNode, TInnerMemSetNode *pRBInnerNode)
	{
		assert(!pInnerNode->isLeaf());
		assert(pInnerNode->less() == -1);
		if(pInnerNode->count() == 0 )
		{

			pInnerNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
			m_ChangeNode.insert(pInnerNode);
			return false;
		}

		TInnerMemSet&  innerTree = pInnerNode->m_InnerNode.m_innerMemSet;
		TInnerMemSetNode *pMinNode = innerTree.minimumNode();
		assert(!innerTree.isNull(pMinNode));

		bool bNode = (pRBInnerNode == pMinNode);
		/*TKey *pKey = NULL;
		TValue *pValue = NULL;
		getMinimumKeyValue(getNode(pMinNode->m_val), &pKey, &pValue);*/
			
		pInnerNode->setLess(pMinNode->m_val);
		pInnerNode->removeRBInnerNode(pMinNode);
	

		pInnerNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pInnerNode);
		return bNode;
 
	}
	bool UnionLeafNode(TBTreeNode* pParentNode, TBTreeNode* pLeafNode, TBTreeNode* pDonorNode, bool bLeft, TInnerMemSetNode *pDonorParentKeyNode, TInnerMemSetNode *pParentKeyNode)
	{
		if(pDonorNode->m_nPageAddr == pParentNode->m_InnerNode.m_nLess) //тогда будет объяденять с less нодой
		{
			pDonorNode->UnionWith(pLeafNode);

			if(pDonorNode->size() > m_pTransaction->getPageSize()) 
			{

				AlignmentLeafNode(pLeafNode, pParentKeyNode, pDonorNode, pDonorParentKeyNode, false);
				return false;
			}
			else
			{
				TBTreeNode* pNextNode = getNode( pLeafNode->m_LeafNode.m_nNext);
				if(pNextNode)
				{
					pDonorNode->m_LeafNode.m_nNext = pNextNode->m_nPageAddr;
					pNextNode->m_LeafNode.m_nPrev = pDonorNode->m_nPageAddr;
					pNextNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
					m_ChangeNode.insert(pNextNode);
				}
				else
					pDonorNode->m_LeafNode.m_nNext = -1;
				pParentNode->removeRBInnerNode(pParentKeyNode); //удаляем и считаем что это не критичный размер изменения родительской ноды, хотя возможно придетсья  это учитывать
				assert(pParentNode->count() != 0 || pParentNode->less() != -1);
				deleteNode(pLeafNode);
				pDonorNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pDonorNode);
				m_nStateTree |= (eBPTDeleteLeafNode);


			}
			return true;
		}
		pLeafNode->UnionWith(pDonorNode);
		uint32 nNodeSize = pLeafNode->size() ;
		if(nNodeSize > m_pTransaction->getPageSize())
		{
			AlignmentLeafNode(pDonorNode, pDonorParentKeyNode, pLeafNode, pParentKeyNode, bLeft);
			return false;
		}
		else
		{
			if(bLeft)
			{
		
				TBTreeNode* pPrevNode = getNode( pDonorNode->m_LeafNode.m_nPrev);
				if(pPrevNode)
				{
					assert(pPrevNode->isLeaf());
					pLeafNode->m_LeafNode.m_nPrev = pPrevNode->m_nPageAddr;
					pPrevNode->m_LeafNode.m_nNext = pLeafNode->m_nPageAddr;
					pPrevNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
					m_ChangeNode.insert(pPrevNode);
				}
				else
					pLeafNode->m_LeafNode.m_nPrev = -1;
			}
			else
			{
		
				TBTreeNode* pNextNode = getNode( pDonorNode->m_LeafNode.m_nNext);
				if(pNextNode)
				{
					assert(pNextNode->isLeaf());
					pLeafNode->m_LeafNode.m_nNext = pNextNode->m_nPageAddr;
					pNextNode->m_LeafNode.m_nPrev = pLeafNode->m_nPageAddr;
					pNextNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
					m_ChangeNode.insert(pNextNode);
				}
				else
					pLeafNode->m_LeafNode.m_nNext = -1;
			}
			
			if(bLeft)
			{
				//TTreeLeftNode* pMinNode = pLeafNode->m_LeafNode.m_leafMemSet.minimumNode();
				pDonorParentKeyNode->m_val = pLeafNode->m_nPageAddr;
				pParentNode->removeRBInnerNode(pParentKeyNode);
			}
			else
				pParentNode->removeRBInnerNode(pDonorParentKeyNode); 
			deleteNode(pDonorNode);
			pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
			m_ChangeNode.insert(pParentNode);
		}
	
		return true;
		
	}
	bool AlignmentLeafNode(TBTreeNode* pNode, TInnerMemSetNode *pParentKeyNode, TBTreeNode* pDonorNode, TInnerMemSetNode *pParentDonorKeyNode, bool bLeft)
	{
		if(!pNode->AlignmentOf(pDonorNode, bLeft))
			return false;
		if(bLeft)
		{
			TLeafMemSet&  leafTree = pNode->m_LeafNode.m_leafMemSet;
			TLeftMemSetNode *pNode = leafTree.minimumNode();
			assert(!leafTree.isNull(pNode));
			pParentKeyNode->m_key = pNode->m_key;
		}
		else
		{
			TLeafMemSet&  leafTree = pDonorNode->m_LeafNode.m_leafMemSet;
			TLeftMemSetNode *pNode = leafTree.minimumNode();
			assert(!leafTree.isNull(pNode));
			pParentDonorKeyNode->m_key = pNode->m_key;
		}
		pNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pNode);
		pDonorNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pDonorNode);
		return true;
	}

	bool AlignmentInnerNode(TBTreeNode* pNode, TInnerMemSetNode *pParentKeyNode, TBTreeNode* pDonorNode, TInnerMemSetNode *pParentDonorKeyNode, bool bLeft)
	{	
		if(bLeft) //Нода донор слева
		{

			TBTreeNode* pLessNode = getNode(pNode->less());
			if(pLessNode)
			{
				assert(pLessNode->count());
				TKey *pKey = NULL;
				if(getMinimumKeyValue(pLessNode, &pKey))
				{

					pDonorNode->m_InnerNode.insert(*pKey, pLessNode->addr()); 
				}
				
			}
			TKey key;
			TLink addr;
			if(!pNode->AlignmentInnerNodeOf(pDonorNode, bLeft, key, addr))
			{
				pNode->m_InnerNode.remove(pLessNode->firstKey());
				pNode->m_InnerNode.m_nLess = pLessNode->addr();
				return false;
			}
			//assert(pLastNode);


			TKey *pKey = NULL;
			if(getMinimumKeyValue(getNode(addr), &pKey))
			{

				pNode->m_InnerNode.m_nLess = addr;
				pParentKeyNode->m_key =*pKey; 
			}
		}
		else
		{
			TBTreeNode* pLessNode = getNode(pDonorNode->less());
			if(pLessNode)
			{
				assert(pLessNode->count());
				TKey *pKey = NULL;
				getMinimumKeyValue(pLessNode, &pKey);
				pDonorNode->m_InnerNode.insert(/*pLessNode->firstKey()*/*pKey, pLessNode->m_nPageAddr);
			}
			TKey key;
			TLink addr;
			if(!pNode->AlignmentInnerNodeOf(pDonorNode, bLeft, key, addr)) //
			{
				pNode->m_InnerNode.remove(pLessNode->firstKey());
				pDonorNode->m_InnerNode.m_nLess = pLessNode->m_nPageAddr;
				//pDonorNode->m_InnerNode.insert(key, addr);
				return false;
			}
			//assert(pLastNode);
			pDonorNode->m_InnerNode.m_nLess = addr; 
			TKey *pKey = NULL;
			if(getMinimumKeyValue(getNode(addr), &pKey))
			{
				pParentDonorKeyNode->m_key = *pKey; 
			}
		}
		assert(pNode->size() < m_pTransaction->getPageSize());
		assert(pDonorNode->size() < m_pTransaction->getPageSize());
		pNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pNode);
		pDonorNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pDonorNode);
		return true;
	}

	bool removeFromLeafNode(const TKey& key, TBTreeNode* pLeafNode, TLeftMemSetNode *pRBNode, TBTreeNode* pParentNode)
	{
		assert(pLeafNode->isLeaf());

		pLeafNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
		m_ChangeNode.insert(pLeafNode);
		pLeafNode->removeRBLeafNode(pRBNode);

		m_nStateTree |= eBPTChangeLeafNode;
		if(pLeafNode->size() >  m_pTransaction->getPageSize()/2)
		{
			return removeUP(key, pParentNode, pLeafNode->m_pParrentMemsetNode, pLeafNode->m_nType);
		}
		if(pLeafNode->getFlags() & ROOT_NODE)
		{
			return true;
		}
		assert(pParentNode);
		assert(!pParentNode->isLeaf());
		

		TBTreeNode* pLeafNodeRight = NULL;
		TBTreeNode* pLeafNodeLeft = NULL;
		TBTreeNode* pDonorNode = NULL;
		TInnerMemSetNode *pDonorParentKeyNode = NULL;
		TInnerMemSet& parentInnerTree = pParentNode->m_InnerNode.m_innerMemSet;
		bool bLeft = false;
		
		short nType = pLeafNode->m_nType;
		TInnerMemSetNode *pRBInnerNode = pLeafNode->m_pParrentMemsetNode;
		assert(pRBInnerNode || pLeafNode->addr() ==  pParentNode->less());

	
		if(pLeafNode->count() == 0)
		{

			//вырожденный  случай для больших полей 1-4 на странице
			if(pLeafNode->addr() == pParentNode->less())
			{
				if(pParentNode->count() > 1)
				{
					//TO DO modify parent node
					pParentNode->setLess(-1);
					if(CreateLessNode(pParentNode, pRBInnerNode))
					{
						pRBInnerNode = NULL;
						nType = 0;
					}
				}
				else
				{
					pParentNode->setLess(-1);
					pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
					m_ChangeNode.insert(pParentNode);
					//TO DO remove parent node
				}
			}
			else
			{
				pParentNode->m_InnerNode.deleteNode(pRBInnerNode, true, true);
				pRBInnerNode = NULL;
				nType = 0;
				//pParentNode->m_pParrentRBNode->m_val = pLeafNode->m_nPageAddr;
				pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pParentNode);
			}
		

			TBTreeNode* pNextNode = getNode( pLeafNode->next());
			TBTreeNode* pPrevNode = getNode( pLeafNode->prev());
			if(pNextNode)
			{
				pNextNode->setPrev(pPrevNode ? pPrevNode->m_nPageAddr : -1);
				pNextNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pNextNode);

			}
			if(pPrevNode) 
			{
				pPrevNode->setNext(pNextNode ? pNextNode->m_nPageAddr : -1);
				pPrevNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pPrevNode);
			}
			
		
			deleteNode(pLeafNode);
			m_nStateTree |= (eBPTDeleteLeafNode);
			return removeUP(key, pParentNode, pRBInnerNode, nType);
		}
		if(pParentNode->less() == pLeafNode->addr())
		{
			//крайнея левая нода, у нее только есть правый брат в контексте родителя
			pDonorParentKeyNode = parentInnerTree.minimumNode();
			if(!parentInnerTree.isNull(pDonorParentKeyNode))
			{
				pDonorNode = getNode(pDonorParentKeyNode->m_val);
				bLeft = false;
			}
			else
			{
				assert(false); //такого быть не должно 
			}
		}
		else
		{
			assert(!parentInnerTree.isNull(pRBInnerNode)); //должна быть
			assert(pRBInnerNode->m_val == pLeafNode->addr());
		 
			if(!parentInnerTree.isNull(pRBInnerNode->m_pPrev))
				pLeafNodeLeft = getNode(pRBInnerNode->m_pPrev->m_val);
			if(!parentInnerTree.isNull(pRBInnerNode->m_pNext))
				pLeafNodeRight = getNode(pRBInnerNode->m_pNext->m_val);

			bool bLess = false;

			if(!pLeafNodeLeft)// либо всего две ноды, либо удаляется из самой первой
			{
				pLeafNodeLeft = getNode(pParentNode->m_InnerNode.m_nLess);
				bLess = true;
			}

			assert(pLeafNodeLeft != NULL || pLeafNodeRight != NULL);
			/*if(!pLeafNodeLeft && !pLeafNodeRight)
			{
				return removeUP(key, pParentNode);
			}*/
		
			int nLeftsize =  pLeafNodeLeft ? pLeafNodeLeft->rowSize() : -1;
			int nRightSize = pLeafNodeRight ? pLeafNodeRight->rowSize() : -1;
			if(nLeftsize > nRightSize)
			{
				pDonorNode = pLeafNodeLeft;
				if(bLess)
					pDonorParentKeyNode = pRBInnerNode;
				else
					pDonorParentKeyNode = pRBInnerNode->m_pPrev;
				bLeft = true;
			}
			else
			{
				pDonorNode = pLeafNodeRight;
				pDonorParentKeyNode = pRBInnerNode->m_pNext;
				bLeft = false;
			}
		}
			assert(pDonorNode);
			uint32 nSumSize = pDonorNode->rowSize() + pLeafNode->rowSize() + pLeafNode->headSize();
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
				if(UnionLeafNode(pParentNode, pLeafNode, pDonorNode, bLeft, pDonorParentKeyNode, pRBInnerNode))
				{
					if(bLeft)
					{
						pRBInnerNode = NULL;
						nType = 0;
					}
				}
			}
			else if(bAlignment)
			{
				AlignmentLeafNode(pLeafNode, pLeafNode->m_pParrentMemsetNode, pDonorNode, pDonorParentKeyNode, bLeft);
					
		 	}

			return removeUP(key, pParentNode, pRBInnerNode, nType);
		
	
	}

	bool getMinimumKeyValue(TBTreeNode* pNode, TKey** pKey)
	{
		while(!pNode->isLeaf())
		{
			pNode = getNode(pNode->less());
			assert(pNode);
		}
		assert(pNode);
		if(!pNode)
			return false;

		TLeafMemSet& LeafTree = pNode->m_LeafNode.m_leafMemSet;
		assert(LeafTree.size());
		TLeafMemSet::iterator it = LeafTree.begin();

		if(it.isNull())
		{
			return false;
		}
		*pKey = &it.key();
		return true;
	}

	bool InsertLessNodeInInnerNode(TBTreeNode* pNode, TBTreeNode* pDonorNode, bool bLeft)
	{
		assert(!pNode->isLeaf());
		assert(!pDonorNode->isLeaf());
		TBTreeNode* pLessNode = NULL;
		if(bLeft)
		{
			pLessNode = getNode(pNode->less());
		}
		else
			pLessNode = getNode(pDonorNode->less());

		if(pLessNode)
		{
			if(pLessNode->isLeaf())
			{
				TLeafMemSet& LeafTree = pLessNode->m_LeafNode.m_leafMemSet;
				TLeafMemSet::iterator it = LeafTree.begin();
				if(!it.isNull())
				{
					pNode->m_InnerNode.insert(it.key(),  pLessNode->m_nPageAddr);
				}
			}
			else
			{
				/*TInnerMemSet& innerTree = pLessNode->m_InnerNode.m_innerMemSet; //баг добираться до самого первого ключа из less ноды
				TInnerMemSet::iterator it = innerTree.begin();
				if(!it.isNull())
				{
					pNode->m_InnerNode.insert(it.key(),  pLessNode->m_nPageAddr);
				}*/

				TKey *pKey = NULL;
				if(getMinimumKeyValue(pLessNode, &pKey))
					pNode->m_InnerNode.insert(*pKey,  pLessNode->m_nPageAddr);

			}
		}

		if(bLeft)
		{
			pNode->m_InnerNode.m_nLess = pDonorNode->less(); //TO DO нужно less ноду сохранить.

		}
		return true;
	
	}

	bool UnionInnerNode(TBTreeNode* pParentNode, TBTreeNode* pNode, TBTreeNode* pDonorNode, bool bLeft, TInnerMemSetNode *pDonorParentKeyNode, TInnerMemSetNode *pParentKeyNode)
	{
		
		if(pDonorNode->addr() == pParentNode->less()) //тогда будет объяденять с less нодой
		{
			
			pDonorNode->UnionWith(pNode);
			InsertLessNodeInInnerNode(pDonorNode, pNode, false);

			if(pDonorNode->size() > m_pTransaction->getPageSize()) //плохо, придеться выравнивть рамер
			{
				AlignmentInnerNode(pDonorNode, pDonorParentKeyNode, pNode, pParentKeyNode, false);
			}
			else
			{
				//если объеденяли с  Less нодой, то нужно поменять ключь в родительсокм элементе
				pParentNode->removeRBInnerNode(pParentKeyNode); 
				deleteNode(pNode);
				pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pParentNode);
				pDonorNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
				m_ChangeNode.insert(pDonorNode);

				m_nStateTree |= (eBPTDeleteInnerNode);
			}
		
			return true;
		}
		pNode->UnionWith(pDonorNode);
		InsertLessNodeInInnerNode(pNode, pDonorNode, bLeft);
		if(pNode->size() > m_pTransaction->getPageSize()) //плохо, придеться выравнивть рамер
		{
			AlignmentInnerNode(pNode, pParentKeyNode, pDonorNode, pDonorParentKeyNode,  bLeft);
			
		}
		else
		{
			if(bLeft)
			{
				//TTreeInnerNode* pMinNode = pNode->m_InnerNode.m_innerMemSet.minimumNode();
				pDonorParentKeyNode->m_val = pNode->m_nPageAddr;
				pParentNode->removeRBInnerNode(pParentKeyNode);
			}
			else
				pParentNode->removeRBInnerNode(pDonorParentKeyNode); //удаляем и считаем что это не критичный размер изменения родительской ноды, хотя возможно придетсья  это учитывать
			deleteNode(pDonorNode);
			assert(pParentNode->count() != 0 || pParentNode->less() != -1);
			pParentNode->setFlags(CHANGE_NODE|BUSY_NODE, true);
			m_ChangeNode.insert(pParentNode);
			m_nStateTree |= (eBPTDeleteInnerNode);
		}
		return true;
	}

		//BPTreeStatisticsInfo m_BTreeInfo;
	protected:
		TBTreeNode *m_pRoot; 
		TLink m_nRootAddr;
		//TLink m_nRTreeStaticAddr;
		TLink m_nPageBTreeInfo;

		TLink m_nPageInnerCompInfo;
		TLink m_nPageLeafPageCompInfo;

		CommonLib::alloc_t* m_pAlloc;
		Transaction* m_pTransaction;
		typedef RBSet<TBTreeNode*> TChangeNode;
		TChangeNode m_ChangeNode;
		uint32 m_nChacheSize;
		typedef TCacheLRU<TLink, TBTreeNode> TNodesCache;
		TNodesCache m_Chache;
		bool m_bChangeRoot;
		bool m_bMulti;
		bool m_bCheckCRC32;

		std::auto_ptr<TCompressorParamsBase*> m_LeafCompParams;
		std::auto_ptr<TCompressorParamsBase*> m_InnerCompParams;
		uint32 m_nStateTree;

		
	};
	template<class _TKey, class _TComp ,
	class _TInnerCompess = BPInnerNodeSetSimpleCompressor<RBMap<_TKey, int64, _TComp> > ,
	class _TLeafCompess = BPLeafNodeSetSimpleCompressor<RBSet<_TKey, _TComp> > ,
	class _Transaction = IDBTransaction, 
	class _TInnerMemSet = RBMap<_TKey, int64, _TComp>,	
	class _TLeafMemSet = RBSet<_TKey, _TComp>,
	class _TInnerNode = BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>,
	class _TLeafNode = BPTreeLeafNodeSet<int64, _TLeafCompess, _TLeafMemSet>,	
	class _TBTreeNode = BPTreeNodeSet< _Transaction, _TInnerNode, _TLeafNode> 
	>
	class TBPSet : public TBPlusTreeSet<_TKey, int64, _TComp, _Transaction, 
		_TInnerMemSet, _TLeafMemSet, _TInnerCompess, _TLeafCompess,
		_TInnerNode, 	_TLeafNode,	_TBTreeNode	>
	{
	public:

		typedef TBPlusTreeSet<_TKey, int64, _TComp, _Transaction,
			RBMap<_TKey, int64, _TComp>, RBSet<_TKey, _TComp>,	 _TInnerCompess  ,	 _TLeafCompess,
			BPTreeInnerNodeSet<_TInnerCompess, _TInnerMemSet>, 	BPTreeLeafNodeSet<int64, _TLeafCompess, _TLeafMemSet>,	
			BPTreeNodeSet< _Transaction, _TInnerNode, _TLeafNode> > TBase;
		
		
		TBPSet(int64 nPageBTreeInfo, _Transaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize, bool bMulti = false) :
		  TBase(nPageBTreeInfo, pTransaction, pAlloc, nChacheSize, bMulti)
		  {

		  }
	};
}
#endif