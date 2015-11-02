#ifndef _EMBEDDED_DATABASE_B_PLUS_TREE_SET_READ_ONLY_H_
#define _EMBEDDED_DATABASE_B_PLUS_TREE_SET_READ_ONLY_H_

#include "CommonLibrary/general.h"
#include "Commonlibrary/alloc_t.h"
#include "embDBInternal.h"
#include "simple_stack.h"
#include "CacheNodes.h"
#include "DBMagicSymbol.h"
#include "BPTreeStatistics.h"
#include "BPSetNodeRO.h"
#include "BPSetIteratorRO.h"
#include "BPInnerNodeSetROSimpleCompressor.h"
#include "BPLeafNodeROSetSimpleCompressor.h"
namespace embDB
{



	template <class _TKey,  class _TLink, class _TComp, class _Transaction,
	class _TInnerComp  = BPInnerNodeSetROSimpleCompressor<_TKey, _TLink>,
	class _TLeafComp = BPLeafNodeSetROSimpleCompressor<_TKey>,
	class _TBreeNode = BPTreeSetNodeRO<_TKey, _TLink, _TComp, _Transaction, _TInnerComp, _TLeafComp> >
	class TBaseBPlusTreeSetRO
	{
	public:
		TBaseBPlusTreeSetRO(int64 nPageBTreeInfo, IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize, bool bMulti = false, bool bCheckCRC = true) :
		  m_nPageBTreeInfo(nPageBTreeInfo), m_pTransaction(pTransaction), m_pAlloc(pAlloc), m_nChacheSize(nChacheSize)
		 ,m_bChangeRoot(false), m_nRootAddr(-1), m_bMulti(bMulti)
		 ,m_Chache(pAlloc)
		 ,m_ChangeNode(pAlloc)
		 ,m_pRoot(NULL)
		 ,m_nRTreeStaticAddr(-1)
		 ,m_bCheckCRC32(bCheckCRC)
		 ,m_BTreeInfo(bCheckCRC)
		{}
		~TBaseBPlusTreeSetRO()
		{
			TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.value().pListEl->obj_;
				delete pBNode;
				it.next();
			}
			if(m_pRoot)
			{
				delete m_pRoot;
			}
		}

		typedef _TKey      TKey;
		//typedef _TValue    TValue;
		typedef _TComp	   TComp;
		typedef _TLink    TLink;
		typedef _TBreeNode  TBTreeNode;
		typedef	_Transaction  Transaction;
		typedef _TInnerComp TInnerCompressor;
		typedef _TLeafComp TLeafCompressor;

		//typedef typename TBTreeNode::TInnerMemSet TInnerMemSet;  
		//typedef typename TBTreeNode::TLeafMemSet TLeafMemSet; 
		typedef BPTreeStatistics<TLink, Transaction, TKey> BPTreeStatisticsInfo;
		typedef TBPSetIteratorRO<TKey,  TLink, TComp, Transaction, TInnerCompressor, TLeafCompressor, TBTreeNode> iterator;
		//TBTreeNode::TInnerMemSet::TTreeNode

		void setRootPage(int64 nPageBTreeInfo)
		{
			m_nPageBTreeInfo = nPageBTreeInfo;
		}
		bool  loadBTreeInfo()
		{
			if(m_nPageBTreeInfo == -1)
				return false;
			CFilePage * pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo);
			if(!pPage)
				return false;

			CommonLib::FxMemoryReadStream stream;
			stream.attach(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header;
			header.read(stream, m_bCheckCRC32 && !pPage->isCheck());
			if(m_bCheckCRC32 && !pPage->isCheck() && !header.isValid())
			{
				m_pTransaction->error(_T("BTREE RO: Page %I64d Error CRC for info page"), (int64)m_nPageBTreeInfo);
				return false;
			}
			pPage->setCheck(true);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_INFO_PAGE)
			{
				m_pTransaction->error(_T("BTREE RO: Page %I64d is not BTreeInfoPage"), (int64)m_nPageBTreeInfo);
				return false;
			}
			stream.read(m_nRootAddr);
			stream.read(m_nRTreeStaticAddr);
			m_bMulti = stream.readBool();
			m_BTreeInfo.setPage(m_nRTreeStaticAddr);
			m_BTreeInfo.Load(m_pTransaction);
			return true;
		}
	
		TBTreeNode* getNode(TLink nAddr, bool bIsRoot = false, bool bNotMove = false, bool bCheckCache = false)
		{
			if(nAddr == -1)
				return NULL;
			if(nAddr == m_nRootAddr)
			{
				return m_pRoot;
			}
			TBTreeNode *pBNode = m_Chache.GetElem(nAddr, bNotMove);
			if(!pBNode)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(nAddr);
				if(!pFilePage)
					return NULL;
				pBNode = new TBTreeNode(m_pAlloc, nAddr, m_bMulti, m_bCheckCRC32);
				pBNode->LoadFromPage(pFilePage, m_pTransaction);
				if(bCheckCache)
				{
					if(m_Chache.size() > m_nChacheSize)
					{
						_TBreeNode *pDelNode = m_Chache.remove_back();
						if(pDelNode)
						{
							delete pDelNode;
						}



					}
				}
				if(bIsRoot)
					pBNode->setFlags(ROOT_NODE, true);
				else
					m_Chache.AddElem(pBNode->m_nPageAddr, pBNode, bNotMove);
			}
				/*TNodesCache::* pNode = m_Chache.GetElem(nAddr);
				if(pNode)
					return pNode->value_;*/
				return pBNode;

			}
		void ClearChache()
		{
			if(m_Chache.size() <= m_nChacheSize)
				return;
			for (size_t i = 0, sz = m_Chache.size(); i < sz - m_nChacheSize; i++)
			{
				TBTreeNode* pDelNode = m_Chache.remove_back();//remove(pChNode->m_nPageAddr);
				if(pDelNode)
				{
					delete pDelNode;
				}
			}
		}

			
		/*TValue* search(const TKey& key) //for debug
		{
			TValue* pValue = 0;
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return pValue;
		
			if(!m_pRoot)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(m_nRootAddr);
				if(!pFilePage)
					return NULL;
				m_pRoot = new TBTreeNode(m_pAlloc, m_nRootAddr, m_bMulti);
				if(!m_pRoot->LoadFromPage(pFilePage))
				{
					delete m_pRoot;
					m_pRoot = NULL;
					return pValue;
				}
			}
			if(!m_pRoot)
				return pValue;

			if(m_pRoot-> isLeaf())
			{
				TLeafObj *pRBNode = m_pRoot->findNode(key);//m_leafMemSet.findNode(key);
				if(pRBNode)
				{
					pValue =  &pRBNode->m_val;
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
						TLeafObj *pRBNode = pNode->findNode(key);//m_leafMemSet.findNode(key);
						if(pRBNode)
							pValue =  &pRBNode->m_val;
						break;
					}
					nNextAddr = pNode->findNext(key);
				}
			}
	

			ClearChache();
			return pValue;

		}*/
		iterator find(const TKey& key, bool bFirst = false)
		{

			TBTreeNode* pFindBTNode = 0;
			TBTreeNode* pParentNode = 0;
			int32 nIndex = 0;
			 
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return iterator(this, 0, 0);

			if(!m_pRoot)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(m_nRootAddr);
				if(!pFilePage)
					return iterator(this, 0, 0);
				m_pRoot = new TBTreeNode(m_pAlloc, m_nRootAddr, m_bMulti, m_bCheckCRC32);
				if(!m_pRoot->LoadFromPage(pFilePage, m_pTransaction))
				{
					delete m_pRoot;
					m_pRoot = NULL;
					return iterator(this, 0, 0);
				}
			}
			if(!m_pRoot)
				return iterator(this, 0, 0);

			if(m_pRoot-> isLeaf())
			{
				nIndex = m_pRoot->findIndex(key);//m_leafMemSet.findNode(key);
				if(nIndex != -1)
					pFindBTNode = m_pRoot;

			}
			else
			{
				pParentNode = m_pRoot;
				int64 nNextAddr = m_pRoot->findNext(key);
				for (;;)
				{
					if( nNextAddr == -1)
						break;
					TBTreeNode* pNode = getNode(nNextAddr);
					if(pNode->isLeaf())
					{
						nIndex  = pNode->findIndex(key);
						if(nIndex != -1)
						{
							pFindBTNode = pNode;
							if(bFirst && m_bMulti)
							{
								if(nIndex == 0 && pNode->prev() != -1)
								{
									pNode = getNode(pNode->prev());
									int32 nSubIndex  = pNode->findIndex(key);
									if(nSubIndex != -1)
									{
										pFindBTNode = pNode;
										nIndex = nSubIndex;
									}
								}

							}
						}
						break;
					}
					pParentNode = pNode;
					nNextAddr = pNode->findNext(key);
				}
			}


			ClearChache();
			return  iterator(this, pFindBTNode, nIndex);

		}
		iterator begin()
		{
			if(m_nRootAddr == -1)
				loadBTreeInfo();
			if(m_nRootAddr == -1)
				return iterator(this, 0, 0);

			if(!m_pRoot)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(m_nRootAddr);
				if(!pFilePage)
					return  iterator(this, 0, 0);
				m_pRoot = new TBTreeNode(m_pAlloc, m_nRootAddr, m_bMulti, m_bCheckCRC32);
				if(!m_pRoot->LoadFromPage(pFilePage, m_pTransaction))
				{
					delete m_pRoot;
					m_pRoot = NULL;
					return  iterator(this, 0, 0);;
				}
			}
			if(!m_pRoot)
				 return iterator(this, 0, 0);
			if(m_pRoot->isLeaf())
			{
				 return iterator(this, m_pRoot, 0);
			}


			int64 nNextAddr = m_pRoot->m_nLess;
			for (;;)
			{
				if( nNextAddr == -1)
					break;
				TBTreeNode* pNode = getNode(nNextAddr);
				if(pNode->isLeaf())
				{
					return iterator(this, pNode, 0);
				}
				nNextAddr = pNode->m_nLess;
			}
			 return iterator(this, 0, 0);
		}
		bool insert (const TKey& key)
		{
			//m_pTransaction->
			return false;
		}
		bool update (const TKey& key)
		{
			//m_pTransaction->
			return false;
		}
		bool remove (const TKey& key)
		{
			//m_pTransaction->
			return false;
		}
		virtual bool commit()
		{
			return false;
		}
		BPTreeStatisticsInfo m_BTreeInfo;
	protected:
		TBTreeNode *m_pRoot; 
		TLink m_nRootAddr;
		TLink m_nPageBTreeInfo;
		TLink m_nRTreeStaticAddr;
		CommonLib::alloc_t* m_pAlloc;
		Transaction* m_pTransaction;
		typedef RBSet<TBTreeNode*> TChangeNode;
		TChangeNode m_ChangeNode;
		size_t m_nChacheSize;
		typedef TSimpleCache<int64, TBTreeNode> TNodesCache;
		TNodesCache m_Chache;
		bool m_bChangeRoot;
		bool m_bMulti;
		bool m_bCheckCRC32;
		
	};
}
#endif