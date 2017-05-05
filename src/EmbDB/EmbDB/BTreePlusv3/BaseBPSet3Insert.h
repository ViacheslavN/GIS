#pragma once
 

BPSETBASE_TEMPLATE_PARAMS
bool BPSETBASE_DECLARATION::insert(const TKey& key)
{
	TBTreeNodePtr pNode = findLeafNodeForInsert(key);
	return true;
}

BPSETBASE_TEMPLATE_PARAMS
std::shared_ptr<_TBTreeNode> BPSETBASE_DECLARATION::findLeafNodeForInsert(const TKey& key)
{
	if (!m_pRoot.get())
	{
		if (!checkRoot())
			return TBTreeNodePtr(nullptr);
	}
	if (m_pRoot->isLeaf())
	{
		return m_pRoot;
	}
	else
	{
		int64 nNextAddr = m_pRoot->findNodeInsert(m_comp, key);
		TBTreeNodePtr pParent = m_pRoot;
		while (nNextAddr != -1)
		{
			TBTreeNodePtr pNode = getNode(nNextAddr);
			pNode->setParent(pParent, -1);
			if (pNode->isLeaf())
			{
				return TBTreeNodePtr(pNode);
				break;
			}
			pParent = pNode;
			nNextAddr = pNode->findNodeInsert(m_comp, key);
		}
	}

	return TBTreeNodePtr(NULL);
}