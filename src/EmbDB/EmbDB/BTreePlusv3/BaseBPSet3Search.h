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