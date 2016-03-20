#ifndef _EMBEDDED_DATABASE_B_TREE_READ_ONLY_H_
#define _EMBEDDED_DATABASE_B_TREE_READ_ONLY_H_

namespace embDB
{
	template <class _TNodeElem, class _TComp,
	class _TBreeNode = BTreeNodeRO<_TNodeElem, _TComp> >
	class TBaseBTreeRO
	{
	public:
		typedef _TNodeElem       TNodeElem ;
		typedef _TComp   TComp;
		typedef RBSet<TNodeElem , TComp>   TMemSet;
		typedef typename TMemSet::TTreeNode TTreeNode;
		typedef _TBreeNode TBTreeNode;

		TBaseBTreeRO(int64 nPageBTreeInfo, IDBTransaction* pTransaction, CommonLib::alloc_t* pAlloc, uint32 nChacheSize) :
			m_nPageBTreeInfo(nPageBTreeInfo)
			,m_pTransaction(pTransaction)
			,m_pAlloc(pAlloc)
			,m_nRootAddr(-1)
			,m_Chache(pAlloc)
			,m_nChacheSize(nChacheSize)
			,m_pRoot(NULL)

		{

		}
		~TBaseBTreeRO()
		{
			TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
			while(!it.isNull())
			{
				TBTreeNode* pBNode = it.getVal().pObj;
				delete pBNode;
				it.next();
			}
		}
		TNodeElem * search(const TNodeElem& key) //for tests
		{
			if(m_nRootAddr == -1)
			{
				if(!loadBTreeInfo())
					return NULL;
			}

			if(!m_pRoot)
			{
				m_pRoot = getNode(m_nRootAddr, true); 
				m_pRoot->setFlags(ROOT_NODE, true);
			}
			if(!m_pRoot)
				return NULL;

			TMemSet::TTreeNode* pRBTreeNode = NULL;
			int64 nNextAddr = -1;
			int32 nIndex = -1;
			m_pRoot->findKey(key, nNextAddr, nIndex);
			if(nIndex != -1)
			{
				return m_pRoot->getElement(nIndex);
				//return &pRBTreeNode->key_;
			}
			for (;;)
			{
				if( nNextAddr == -1)
					return NULL;
				TBTreeNode* pNode = getNode(nNextAddr, false);
				nNextAddr = -1;
				pNode->findKey(key, nNextAddr, nIndex);
				if(nIndex != -1)
				{
					return pNode->getElement(nIndex);
				}
			}

			return NULL;
		}
	private:
		TBTreeNode* findNode( TBTreeNode *node, const TNodeElem  &key);
		TBTreeNode *getNode(int64 nAddr, bool bNotMove)
		{
			if(nAddr == -1)
				return NULL;
			_TBreeNode *pBNode = m_Chache.GetElem(nAddr, bNotMove);
			if(!pBNode)
			{
				CFilePage* pFilePage = m_pTransaction->getFilePage(nAddr);
				if(!pFilePage)
					return NULL;
				pBNode = new TBTreeNode(m_pAlloc, nAddr, SIMPLE_COMPRESSOR);
				pBNode->LoadFromPage(pFilePage);
				if(m_Chache.size() > m_nChacheSize)
				{
					_TBreeNode *pDelNode = m_Chache.remove_back();
					if(pDelNode)
					{
					  delete pDelNode;
					}
				}
				m_Chache.AddElem(pBNode->m_nPageAddr, pBNode, bNotMove);
			}
			return pBNode;
		}
		bool loadBTreeInfo()
		{
			CFilePage * pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo);
			if(!pPage)
				return false;
			CommonLib::FxMemoryReadStream stream;
			stream.attachBuffer(pPage->getRowData(), pPage->getPageSize());
			sFilePageHeader header;
			header.read(stream, !pPage->isCheck());
			if(!pPage->isCheck() && header.m_nCRC32 != header.m_nCalcCRC32)
			{
				m_pTransaction->error(_T("BTREE: Page %I64d Error CRC for info page"), (int64)m_nPageBTreeInfo);
				return false;
			}
			pPage->setCheck(true);
			if(header.m_nObjectPageType != BTREE_PAGE || header.m_nSubObjectPageType != BTREE_INFO_PAGE)
			{
				m_pTransaction->error(_T("BTREE: Page %I64d is not BTreeInfoPage"), (int64)m_nPageBTreeInfo);
				return false;
			}
			m_nRootAddr = stream.readInt64();
			return m_nRootAddr != -1;

		}
	private:
		TBTreeNode *m_pRoot; 
		uint32 m_nChacheSize;
		int64 m_nRootAddr;
		int64 m_nPageBTreeInfo;

		CommonLib::alloc_t* m_pAlloc;
		IDBTransaction* m_pTransaction;
		typedef TSimpleCache<int64, TBTreeNode> TNodesCache;
		TNodesCache m_Chache;
	};

}
#endif