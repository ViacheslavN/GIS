



template <class _TNodeElem, class _TComp>
BTreeNode<_TNodeElem,  _TComp>::BTreeNode(CommonLib::alloc_t *pAlloc, int64 nPageAddr, uint16 nCompID) : 
	m_memset(pAlloc)
	 ,m_nPageAddr(nPageAddr)
	 ,m_nLess(-1)
     ,m_nParent(-1)
	 ,m_pCompressor(0)
	 ,m_pAlloc(pAlloc)
	 ,m_nFlag(0)

{
	switch(nCompID)
	{
	case SIMPLE_COMPRESSOR:
		m_pCompressor = new SimpleNodeCompressor<TNodeElem>(pAlloc);
		break;
	}
}
template <class _TNodeElem, class _TComp>
BTreeNode<_TNodeElem,  _TComp>::~BTreeNode()
{
	if(m_pCompressor)
		delete m_pCompressor;
}
template < class _TNodeElem, class _TComp>
size_t BTreeNode<_TNodeElem,  _TComp>::size()
{
	return m_memset.size();// m_pCompressor->size();
}
template < class _TNodeElem, class _TComp>
bool BTreeNode<_TNodeElem,  _TComp>::isNoFull()
{
	return m_pCompressor->cnt_size(m_memset.size()) < DEFAULT_PAGE_SIZE;
}

template <class _TNodeElem, class _TComp>
bool  BTreeNode<_TNodeElem,  _TComp>::insert(const TNodeElem& key)
{
	m_pCompressor->insert(key);
	return m_memset.insert(key);
}
template <class _TNodeElem, class _TComp>
_TNodeElem* BTreeNode<_TNodeElem,  _TComp>::find(const TNodeElem& key)
{
	TMemSet::TTreeNode *pNode = m_memset.findNode(key);
	return &pNode->key_;
}

template <class _TNodeElem, class _TComp>
void  BTreeNode<_TNodeElem,  _TComp>::remove(const TNodeElem& key)
{
	m_memset.remove(key);
}

template <class _TNodeElem, class _TComp>
void  BTreeNode<_TNodeElem,  _TComp>::clear()
{

}

template < class _TNodeElem, class _TComp>
bool BTreeNode<_TNodeElem,  _TComp>::Load(IDBTransactions* pTransactions)
{
	if(m_nPageAddr == -1)
	{

		CFilePage* pFilePage = pTransactions->getNewPage();
		if(!pFilePage)
			return false;

		m_nPageAddr = pFilePage->getAddr();
		return true;
	}
	CFilePage* pFilePage =  pTransactions->getFilePage(m_nPageAddr);
	if(!pFilePage)
		return false; 
	return LoadFromPage(pFilePage);

}
template < class _TNodeElem, class _TComp>
bool BTreeNode<_TNodeElem,  _TComp>::LoadFromPage(CFilePage* pFilePage)
{
	CommonLib::FxMemoryReadStream stream;
	stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
	m_nLess  = stream.readInt64();
	m_nParent  = stream.readInt64();

	return m_pCompressor->LoadNode(m_memset, stream);
}
template < class _TNodeElem, class _TComp>
bool BTreeNode<_TNodeElem,  _TComp>::Save(IDBTransactions* pTransactions)
{
	CFilePage *pFilePage = NULL;
	if(m_nPageAddr != -1)
		pFilePage = pTransactions->getFilePage(m_nPageAddr);
	else
		pFilePage = pTransactions->getNewPage();

	if(!pFilePage)
		return false;
	CommonLib::FxMemoryWriteStream stream;
	stream.attach(pFilePage->getRowData(), pFilePage->getPageSize());
	stream.write(m_nLess);
	stream.write(m_nParent);
	m_pCompressor->WriteNode(m_memset, stream);
	pTransactions->saveFilePage(pFilePage);
	return true;
}
template <class _TNodeElem, class _TComp>
void BTreeNode<_TNodeElem, _TComp>::findKey(const TNodeElem& key, int64& nNextNode, typename TMemSet::TTreeNode** pRBTreeNode)
{
	short nType = 0;
	TMemSet::TTreeNode *pRBNode = m_memset.findNodeForBTree(key, nType);

	/*if(nType == LQ_KEY)
	{
		if(pRBNode)
		{
			TAggrKey& elem = pRBNode->key_;
			nNextNode = elem.m_nLink;
		}
	}
	else if(nType == LE_KEY)
	{
		if(pRBNode)
		{
			TAggrKey& elem = pRBNode->key_;
			nNextNode = elem.m_nLink;
		}
		else
			nNextNode = m_nLess;
	}*/
	if(nType == FIND_KEY)
	{
		*pRBTreeNode = pRBNode;
	}
	else
	{
		if(!m_memset.isNull(pRBNode))
		{
			TNodeElem& elem = pRBNode->key_;
			nNextNode = elem.m_nLink;
		}
		else
			nNextNode = m_nLess;

	}
}
template <class _TNodeElem, class _TComp>
bool BTreeNode<_TNodeElem, _TComp>::findNodeInsert(const TNodeElem& key, int64& nNextNode, BTreeNode** pNode)
{
	short nType;
	nNextNode = -1;
	*pNode = NULL;
	if(m_memset.size() == 0)
	{
		*pNode = this;
		return true;
	}

	TMemSet::TTreeNode *pFindNode = m_memset.findNodeForBTree(key, nType);
	if(nType == FIND_KEY)
		return false;
	if(!m_memset.isNull(pFindNode))
	{
		TNodeElem& elem = pFindNode->key_;
		nNextNode = elem.m_nLink;
		if(nNextNode == -1)
			*pNode = this;
	}
	else
	{
		nNextNode = m_nLess;
		if(nNextNode == -1)
			*pNode = this;
	}
	return true;
}
template <class _TNodeElem, class _TComp>
void BTreeNode<_TNodeElem, _TComp>::setFlags(int nFlag, bool bSet)
{
	if(bSet)
		m_nFlag |= nFlag;
	else
		m_nFlag &= ~nFlag;
}