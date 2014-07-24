
template <class _TNodeElem , class _TComp, class _TBreeNode>
TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::TBaseBTree(size_t nNodeSize, int64 nPageBTreeInfo, IDBTransactions* pTransaction, CommonLib::alloc_t* pAlloc, size_t nChacheSize) :
	m_nNodeSize(nNodeSize)
	,m_nRootAddr(-1)
	,m_pTransaction(pTransaction)
	,m_pAlloc(pAlloc)
	,m_Chache(pAlloc)
	,m_ChangeNode(pAlloc)
	,m_pRoot(0)
	,m_nChacheSize(nChacheSize)
	,m_nPageBTreeInfo(nPageBTreeInfo)
	,m_bChangeRoot(false)
{

}

template < class _TNodeElem , class _TComp, class _TBreeNode>
TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::~TBaseBTree()
{
	TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
	while(!it.isNull())
	{
		TBTreeNode* pBNode = it.getVal().pObj;
		delete pBNode;
		it.next();
	}
}

template <class _TNodeElem , class _TComp, class _TBreeNode>
bool TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::setRootPage(int64 nPageBTreeInfo, bool bSave)
{
	m_nPageBTreeInfo = nPageBTreeInfo;
	if(bSave)
		saveBTreeInfo();
	else
		loadBTreeInfo();
	return true;
}
template <class _TNodeElem , class _TComp, class _TBreeNode>
bool  TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::loadBTreeInfo()
{
	if(m_nPageBTreeInfo == -1)
		return false;
	CFilePage * pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo);
	if(!pPage)
		return false;

	CommonLib::FxMemoryReadStream stream;
	stream.attach(pPage->getRowData(), pPage->getPageSize());
	if(stream.readInt64() != (int64)DB_BTREE_INFO_PAGE)
		return false;
	m_nRootAddr = stream.readInt64();
	
	return true;
}
template <class _TNodeElem , class _TComp, class _TBreeNode>
bool TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::saveBTreeInfo()
{
	if(m_nPageBTreeInfo == -1)
		return false;
	CFilePage * pPage = m_pTransaction->getFilePage(m_nPageBTreeInfo);
	if(!pPage)
		return false;
	CommonLib::FxMemoryWriteStream stream;
	stream.attach(pPage->getRowData(), pPage->getPageSize());
	stream.write((int64)DB_BTREE_INFO_PAGE);
	stream.write(m_nRootAddr);
	m_pTransaction->saveFilePage(pPage);
	return true;
}
template <class _TNodeElem , class _TComp, class _TBreeNode>
int64 TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::getRootAddr()
{
	return m_nRootAddr;
}
template <class _TNodeElem , class _TComp, class _TBreeNode>
_TNodeElem * TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::search(const TNodeElem& key)
{
	if(m_nRootAddr == -1)
		return NULL;

	if(!m_pRoot)
	{
		m_pRoot= getNode(m_nRootAddr, true); 
	}
	if(!m_pRoot)
		return NULL;

	TMemSet::TTreeNode* pRBTreeNode = NULL;
	int64 nNextAddr = -1;
	m_pRoot->findKey(key, nNextAddr, &pRBTreeNode);
	if(pRBTreeNode)
		return &pRBTreeNode->key_;
	for (;;)
	{
		if( nNextAddr == -1)
			return NULL;
		TBTreeNode* pNode = getNode(nNextAddr);
		pNode->findKey(key, nNextAddr, &pRBTreeNode);
		if(pRBTreeNode)
			return &pRBTreeNode->key_;
	}

}


template <class _TNodeElem , class _TComp, class _TBreeNode>
bool TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::insert(const TNodeElem & key)
{
	if(!m_pRoot)
	{
		//m_pRoot = m_nRootAddr == -1 ? newNode(true) : getNode(m_nRootAddr, true); 
		if(m_nRootAddr == -1)
		{
			m_pRoot = newNode(true);
			m_nRootAddr = m_pRoot->m_nPageAddr; 
			saveBTreeInfo();
		}
		else
		{
			m_pRoot = getNode(m_nRootAddr, true); 
			m_nRootAddr = m_pRoot->m_nPageAddr; 
		}
		
	}
	if(!m_pRoot)
		return false;
	int64 nNextAddr = -1;
	TBTreeNode* pNode = NULL;
	if(!m_pRoot->findNodeInsert(key, nNextAddr, &pNode))
		return false;
	if(pNode && nNextAddr == -1)
	{
		return InsertInNode(pNode, key);
	}
	for (;;)
	{
		if( nNextAddr == -1)
			break;

		TBTreeNode* pNode = getNode(nNextAddr);
		TBTreeNode* pFindNode = NULL;
		if(!pNode->findNodeInsert(key, nNextAddr, &pFindNode))
			return false;
		if(pFindNode && nNextAddr == -1)
		{
			return InsertInNode(pFindNode, key);
		}
	}
	return false;
}

template <class _TNodeElem , class _TComp, class _TBreeNode>
bool TBaseBTree<_TNodeElem , _TComp, _TBreeNode>::update(const TNodeElem& key)
{
	if(m_nRootAddr == -1)
		return NULL;

	if(!m_pRoot)
	{
		m_pRoot= getNode(m_nRootAddr, true); 
	}
	if(!m_pRoot)
		return NULL;

	TMemSet::TTreeNode* pRBTreeNode = NULL;
	TBTreeNode* pNode = NULL;
	int64 nNextAddr = -1;
	m_pRoot->findKey(key, nNextAddr, &pRBTreeNode);
	if(pRBTreeNode)
	{
		pNode = m_pRoot;
	}
	else 
	{
		for (;;)
		{
			if( nNextAddr == -1)
				return NULL;
			pNode = getNode(nNextAddr);
			pNode->findKey(key, nNextAddr, &pRBTreeNode);
			if(pRBTreeNode)
				break;
		}
	}
	if(!pNode || !pRBTreeNode)
		return false;

	pRBTreeNode->key_.m_val = key.m_val;
	pNode->Save(m_pTransaction);
	return true;
}
 
template <class _TNodeElem, class _TComp, class _TBreeNode>
_TBreeNode *TBaseBTree< _TNodeElem, _TComp, _TBreeNode>::newNode(bool bIsRoot)
{
	_TBreeNode *pNode = new TBTreeNode(m_pAlloc, -1, SIMPLE_COMPRESSOR);
	pNode->Load(m_pTransaction);
	if(m_Chache.size() > m_nChacheSize)
	{
		_TBreeNode *pDelNode = m_Chache.remove_back();
		/*if(pDelNode->getFlags() & ROOT_NODE)
		{
			m_Chache.AddElem(pDelNode->m_nPageAddr, pDelNode); //вставим в начало списка
			pDelNode = m_Chache.remove_back();
		}*/
		if(pDelNode)
		{
			if(pDelNode->getFlags() & CHANGE_NODE)
			{
				pDelNode->Save(m_pTransaction);
			}
			//std::cout << " pDelNode " << pDelNode->m_nPageAddr;
			//std::cout << std::endl;
			delete pDelNode;
		}

	}
	if(bIsRoot)
		pNode->setFlags(ROOT_NODE, true);
	m_Chache.AddElem(pNode->m_nPageAddr, pNode);
	return pNode;
}

template <class _TNodeElem , class _TComp, class _TBreeNode>
_TBreeNode *TBaseBTree< _TNodeElem, _TComp, _TBreeNode>::getNode(int64 nAddr, bool bIsRoot, bool bNotMove)
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
				if(pDelNode->getFlags() & CHANGE_NODE)
					pDelNode->Save(m_pTransaction);
				delete pDelNode;
			}
			
			

		}
		m_Chache.AddElem(pBNode->m_nPageAddr, pBNode, bNotMove);
	}
	if(bIsRoot)
		pBNode->setFlags(ROOT_NODE, true);
	/*TNodesCache::* pNode = m_Chache.GetElem(nAddr);
	if(pNode)
		return pNode->value_;*/
	return pBNode;

}


template <class _TNodeElem, class _TComp, class _TBreeNode>
_TBreeNode* TBaseBTree< _TNodeElem, _TComp, _TBreeNode>::findNode( TBTreeNode *pNode, const TNodeElem &key)
{
	TAggrKey* pKey = pNode->find(key);
}
template <class _TNodeElem, class _TComp, class _TBreeNode>
bool TBaseBTree< _TNodeElem, _TComp, _TBreeNode>::splitNode(const TNodeElem &key, TBTreeNode *pNode, TBTreeNode *pNewNode, TTreeNode** pMedian, bool bNotInsert)
{
	TBTreeNode::TMemSet& nodeTree = pNode->m_memset;
	TBTreeNode::TMemSet& nodeNewTree = pNewNode->m_memset;
	if(!bNotInsert)
		nodeTree.insert(key);

	TMemSet::TTreeNode* pCurr = nodeTree.tree_maximim(nodeTree.root());
	size_t nSize = nodeTree.size()/2;

	while(nSize)
	{
		TNodeElem& Key = pCurr->key_;
		if(Key.m_nLink != -1)
		{
			TBTreeNode* pLNode = getNode(Key.m_nLink, false, true);
			pLNode->m_nParent = pNewNode->m_nPageAddr;
			//pLNode->Save(m_pStorage);
			pNode->setFlags(CHANGE_NODE, true);
		}
		nodeTree.deleteNode(pCurr, true, false);
		TMemSet::TTreeNode* pPrev = pCurr->m_pPrev;
		nodeNewTree.insertNode(pCurr);
		pCurr = pPrev;
		--nSize;
	}
	*pMedian = pCurr;  // Медианный элемент
	TNodeElem& MKey = pCurr->key_;
	if(MKey.m_nLink != -1)
	{
		TBTreeNode* pLNode = getNode( MKey.m_nLink, false, true);
		pLNode->m_nParent = pNewNode->m_nPageAddr;
		//pLNode->Save(m_pStorage);
		pNode->setFlags(CHANGE_NODE, true);
	}
	pNewNode->m_nLess =  MKey.m_nLink;
	MKey.m_nLink = pNewNode->m_nPageAddr;

	nodeTree.deleteNode(pCurr, true, false);
	pNewNode->m_nParent = pNode->m_nParent;

	/*TMemSet::TTreeNode* pRoot = nodeTree.m_pRoot; // Медианный элемент
	TMemSet::TTreeNode* pCurr = pRoot->m_pNext;
	*pMedian = pRoot;

	TNodeElem& MKey = pRoot->key_;
	if(MKey.m_nLink != -1)
	{
		TBTreeNode* pLNode = getNode( MKey.m_nLink, false, true);
		pLNode->m_nParent = pNewNode->m_nPageAddr;
		pLNode->Save(m_pStorage);
	}
	pNewNode->m_nLess =  MKey.m_nLink;
	MKey.m_nLink = pNewNode->m_nPageAddr;
	while(pCurr)
	{
		TNodeElem& Key = pCurr->key_;
		if(Key.m_nLink != -1)
		{
			TBTreeNode* pLNode = getNode(Key.m_nLink, false, true);
			pLNode->m_nParent = pNewNode->m_nPageAddr;
			pLNode->Save(m_pStorage);
		}
		nodeTree.deleteNode(pCurr, true);
		TMemSet::TTreeNode* pNext = pCurr->m_pNext;
		nodeNewTree.insertNode(pCurr);
		pCurr = pNext;
	}
	nodeTree.deleteNode(pRoot, true);
	pNewNode->m_nParent = pNode->m_nParent;*/
	return true;
}
template < class _TNodeElem, class _TComp, class _TBreeNode>
bool TBaseBTree<_TNodeElem, _TComp, _TBreeNode>::InsertInNode(TBTreeNode*pNode, const TNodeElem& key)
{
	pNode->insert(key); 
	/*if ( pNode->size() < m_nNodeSize )
	{
		pNode->insert(key); 
		pNode->Save(m_pStorage);
		return true;
	}*/
	if (pNode->isNoFull())
	{
		//pNode->insert(key); 

		//pNode->Save(m_pStorage);
		pNode->setFlags(CHANGE_NODE, true);
		return true;
	}
	pNode->setFlags(CHANGE_NODE, true);

	TBTreeNode* pNodeNewRight = newNode();
	pNodeNewRight->setFlags(CHANGE_NODE, true);
	
	TBTreeNode* pNodeParent = getNode(pNode->m_nParent);
		// Split node and get median
	TMemSet::TTreeNode* pMedian = NULL;
	if (!splitNode(key, pNode, pNodeNewRight, &pMedian, true ) )
	{
		return false;
	}
	m_ChangeNode.insert(pNode);
	m_ChangeNode.insert(pNodeNewRight);

	TMemSet::TTreeNode* pOldMedian = pMedian;
	while ( pNodeParent != 0 )
	{
		// Add median to the parent
		if ( !pNodeParent->isNoFull()/*pNodeParent->size() >= m_nNodeSize*/ )
		{
			pNodeParent->setFlags(CHANGE_NODE, true);

			pNodeNewRight = newNode();

			pNodeNewRight->setFlags(CHANGE_NODE, true);
	
			if (!splitNode(pOldMedian->key_,  pNodeParent, pNodeNewRight, &pMedian))
			{
				return false;
			}
			m_pAlloc->free(pOldMedian);
			pOldMedian = pMedian;
			m_ChangeNode.insert(pNodeNewRight);
			m_ChangeNode.insert(pNodeParent);

			pNode = pNodeParent;

			// Move up
			pNodeParent = getNode(pNodeParent->m_nParent);
			
		}
		else
		{
			pNodeParent->setFlags(CHANGE_NODE, true);
			m_ChangeNode.insert(pNodeParent);
			pNodeParent->m_memset.insertNode(pMedian);
			pNodeNewRight->m_nParent = pNodeParent->m_nPageAddr;
			break;
		}
	}
	if(!pNodeParent)
	{
		TBTreeNode* pNodeNewRoot = newNode();

		pNodeNewRoot->m_memset.insertNode(pMedian);
		pNode->m_nParent = pNodeNewRoot->m_nPageAddr;
		pNodeNewRoot->m_nLess = pNode->m_nPageAddr;
		pNodeNewRight->m_nParent = pNodeNewRoot->m_nPageAddr;
		m_nRootAddr = pNodeNewRoot->m_nPageAddr;
		m_pRoot->setFlags(ROOT_NODE, false);
		m_pRoot = pNodeNewRoot;
		m_pRoot->setFlags(ROOT_NODE, true);
		m_pRoot->setFlags(CHANGE_NODE, true);
		m_ChangeNode.insert(pNodeNewRoot);
		saveBTreeInfo();
	}

	if(m_Chache.size() > m_nChacheSize)
	{
		TChangeNode::iterator it = m_ChangeNode.begin();
		for(; !it.isNull(); ++it)
		{
			TBTreeNode* pChNode = *it;
			//pChNode->Save(m_pStorage);
			//pChNode->setFlags(CHANGE_NODE, false);
			if(m_pRoot != pChNode)
			{
				TBTreeNode* pDelNode = m_Chache.remove(pChNode->m_nPageAddr);
				assert(pDelNode == pChNode);
				if(pDelNode)
				{
					pDelNode->Save(m_pTransaction);
					//std::cout << " pDelNode " << pDelNode->m_nPageAddr;
					//std::cout << std::endl;
					delete pDelNode;

				}
			}
			if(m_Chache.size() < m_nChacheSize)
				break;
		}
	}
	
	m_ChangeNode.destroyTree();
	return true;
	//split
}


template < class _TNodeElem, class _TComp, class _TBreeNode>
bool TBaseBTree<_TNodeElem, _TComp, _TBreeNode>::commit()
{
	TNodesCache::TCacheSet::iterator it =	m_Chache.m_set.begin();
	while(!it.isNull())
	{
		TBTreeNode* pBNode = it.getVal().pObj;
		if(pBNode->getFlags() & CHANGE_NODE)
		{
			pBNode->Save(m_pTransaction);
			pBNode->setFlags(CHANGE_NODE, false);
		}
		it.next();
	}
	return true;
}