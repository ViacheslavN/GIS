#pragma once

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator, class TComparator>
TIterator BPSETBASE_DECLARATION::find(const TComparator& comp, const TKey& key, TIterator *pFromIterator, bool bFindNext)
{
	if (pFromIterator)
	{
		TIterator it(this, pFromIterator->m_pCurNode, pFromIterator->m_pCurNode->binary_search(comp, key));
		if (!it.isNull() || !bFindNext)
			return it;
	}
	
	if (m_nRootAddr == -1)
		return TIterator(this, TBTreeNodePtr(), -1);

	if (!m_pRoot.get())
	{
		m_pRoot = getNode(m_nRootAddr, true);
	}
	if (!m_pRoot.get())
		return TIterator(this, TBTreeNodePtr(), -1);


	if (m_pRoot->isLeaf())
	{
		return TIterator(this, m_pRoot, m_pRoot->binary_search(comp, key));
	}
	int32 nIndex = -1;
	int64 nNextAddr = m_pRoot->inner_lower_bound(comp, key, nIndex);
	TBTreeNodePtr pParent = m_pRoot;
	for (;;)
	{
		if (nNextAddr == -1)
		{
			break;
		}
		TBTreeNodePtr pNode = getNode(nNextAddr);
		if (!pNode.get())
		{
			return TIterator(this, TBTreeNodePtr(), -1);
			break;
		}
		pNode->setParent(pParent, nIndex);

		if (pNode->isLeaf())
		{
			return TIterator(this, pNode, pNode->binary_search(comp, key));
		}
		nNextAddr = pNode->inner_lower_bound(comp, key, nIndex);
		pParent = pNode;
	} 
	return TIterator(this, TBTreeNodePtr(), -1);
}


BPSETBASE_TEMPLATE_PARAMS
template<class TIterator>
TIterator BPSETBASE_DECLARATION::begin()
{
	TBTreeNodePtr pFindBTNode(nullptr);
	if (m_nRootAddr == -1)
		loadBTreeInfo();
	if (m_nRootAddr == -1)
		return TIterator(this, pFindBTNode, -1);

	if (!m_pRoot.get())
		m_pRoot = getNode(m_nRootAddr, true);
	if (!m_pRoot.get())
		return TIterator(this, pFindBTNode, -1);
	if (m_pRoot->isLeaf())
	{
		return TIterator(this, m_pRoot, m_pRoot->count() ? 0 : -1);
	}
	int64 nNextAddr = m_pRoot->less();
	TBTreeNodePtr pParent = m_pRoot;
	for (;;)
	{
		if (nNextAddr == -1)
		{
			break;
		}
		TBTreeNodePtr pNode = getNode(nNextAddr);
 		pNode->setParent(pParent);
		if (pNode->isLeaf())
		{
			pFindBTNode = pNode;
			break;
		}
		nNextAddr = pNode->less();
		pParent = pNode;
	}
	return TIterator(this, pFindBTNode, 0);
}

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator>
TIterator BPSETBASE_DECLARATION::last()
{
	TBTreeNodePtr pFindBTNode(nullptr);
	if (m_nRootAddr == -1)
		loadBTreeInfo();
	if (m_nRootAddr == -1)
		return TIterator(this, pFindBTNode, 0);

	if (!m_pRoot.get())
		m_pRoot = getNode(m_nRootAddr, true);
	if (!m_pRoot.get())
		return TIterator(this, pFindBTNode, 0);
	if (m_pRoot->isLeaf())
	{
		return TIterator(this, m_pRoot, m_pRoot->count() - 1);
	}
	int64 nNextAddr = m_pRoot->backLink();
	int32 nIndex = m_pRoot->count() - 1;
	TBTreeNodePtr pParent = m_pRoot;
	for (;;)
	{
		if (nNextAddr == -1)
		{
			break;
		}
		TBTreeNodePtr pNode = getNode(nNextAddr);
		pNode->setParent(pParent, nIndex);

		if (pNode->isLeaf())
		{
			pFindBTNode = pNode;
			break;
		}
		nNextAddr = pNode->backLink();
		pParent = pNode;
		nIndex = pNode->count() - 1;
	}
	return TIterator(this, pFindBTNode, pFindBTNode.get() ? pFindBTNode->count() - 1 : -1);
}

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator, class TComparator>
TIterator BPSETBASE_DECLARATION::upper_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator, bool bFindNext)
{
	int32 nIndex = 0;
	if (m_nRootAddr == -1)
		loadBTreeInfo();
	if (m_nRootAddr == -1)
		return TIterator(this, TBTreeNodePtr(), -1);

	if (!m_pRoot.get())
		m_pRoot = getNode(m_nRootAddr, true);
	if (!m_pRoot.get())
		return TIterator(this, TBTreeNodePtr(), -1);


	if (m_pRoot->isLeaf())
	{
		return TIterator(this, m_pRoot, m_pRoot->leaf_upper_bound(comp, key));
	}
	nIndex = -1;
	int64 nNextAddr = m_pRoot->inner_upper_bound(comp, key, nIndex);
	TBTreeNodePtr pParent = m_pRoot;
	for (;;)
	{
		if (nNextAddr == -1)
		{
			break;
		}
		TBTreeNodePtr pNode = getNode(nNextAddr);
		pNode->setParent(pParent, nIndex);
		if (pNode->isLeaf())
		{
			return TIterator(this, pNode, pNode->leaf_upper_bound(comp, key));
		}
		nNextAddr = pNode->inner_upper_bound(comp, key, nIndex);
		pParent = pNode;
	}
	return TIterator(this, TBTreeNodePtr(), -1);
}

BPSETBASE_TEMPLATE_PARAMS
template<class TIterator, class TComparator>
TIterator BPSETBASE_DECLARATION::lower_bound(const TComparator& comp, const TKey& key, TIterator *pFromIterator , bool bFindNext)
{
	if (pFromIterator)
	{
		if (pFromIterator->isNull() && !bFindNext)
			return TIterator(this, TBTreeNodePtr(), -1);
	}

	int32 nIndex = 0;
	if (m_nRootAddr == -1)
		loadBTreeInfo();
	if (m_nRootAddr == -1)
		return TIterator(this, TBTreeNodePtr(), -1);

	if (!m_pRoot.get())
		m_pRoot = getNode(m_nRootAddr, true);
	if (!m_pRoot.get())
		return TIterator(this, TBTreeNodePtr(), -1);

	nIndex = -1;
	if (m_pRoot->isLeaf())
	{
		return TIterator(this, m_pRoot, m_pRoot->leaf_lower_bound(comp, key));
	}

	int64 nNextAddr = m_pRoot->inner_lower_bound(comp, key, nIndex);
	TBTreeNodePtr pParent = m_pRoot;
	for (;;)
	{
		if (nNextAddr == -1)
		{
			break;
		}
		TBTreeNodePtr pNode = getNode(nNextAddr);
		pNode->setParent(pParent, nIndex);
		if (pNode->isLeaf())
		{
			int nLeafIndex = pNode->leaf_lower_bound(comp, key);
			if (nLeafIndex != -1)
				return TIterator(this, pNode, nLeafIndex);
			else if (nIndex < (pParent->count() - 1))
			{
				TBTreeNodePtr pNode = getNode(pParent->link(nIndex + 1));
				pNode->setParent(pParent, nIndex + 1);
				return TIterator(this, pNode, 0);
			}

			if (pNode->next() == -1)
				return TIterator(this, TBTreeNodePtr(), -1);

			TBTreeNodePtr pNodeNext = getNode(pNode->next());
			SetParentNext(pNode.get(), pNodeNext.get());
			return TIterator(this, pNodeNext, 0);
		}
		nNextAddr = pNode->inner_lower_bound(comp, key, nIndex);
		pParent = pNode;
	}
	return TIterator(this, TBTreeNodePtr(), -1);
}