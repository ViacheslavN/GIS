class save_iterator
{
public:

	save_iterator(TBaseBTree* pBTree, __int64 _nBCurrNode, TNodeElem& _key) :
	m_pBTree(pBTree)
		,m_nBCurrNode(_nBCurrNode)
		,m_key(_key)

	{

	}
	~save_iterator(){}
	bool isNull() const
	{
		return m_nBCurrNode == -1;
	}
	TNodeElem& getKey()
	{
		return m_key;
	}
	bool back()
	{
		TBTreeNode* pBNode = m_pBTree->getNode(m_nBCurrNode);
		if(!pBNode)
		{
			m_nBCurrNode = -1;
			return false;
		}

		TTreeNode* pNode = pBNode->m_memset.findNode(m_key);
		if(!pNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		TTreeNode* pBackNode = pNode->m_pPrev;
		if(pBackNode)
		{
			TNodeElem& key = pBackNode->key_;
			if(key.m_nLink != -1)
			{
				return LoadBackNode(key.m_nLink );
			}
		}
		//TNodeElem& key = pNode->key_;
	}
	bool next()
	{
		TBTreeNode* pBNode = m_pBTree->getNode(m_nBCurrNode);
		if(!pBNode)
		{
			m_nBCurrNode = -1;
			return false;
		}

		TTreeNode* pNode = pBNode->m_memset.findNode(m_key);
		if(!pNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		TNodeElem& key = pNode->key_;
		if(key.m_nLink != -1)
		{
			return LoadNextNode(key.m_nLink );
		}
		else 
		{
			if(pNode->m_pNext) //дальше идти некуда
			{
				m_key = pNode->m_pNext->key_;
				return true;
			}
			else if(pBNode->m_nParent != -1)
			{

				__int64 nParentAddr = pBNode->m_nParent;
				while (nParentAddr != -1)
				{
					TBTreeNode* pParentNode = m_pBTree->getNode(nParentAddr);
					if(!pParentNode)
					{
						m_nBCurrNode = -1;
						return false;
					}
					if(pParentNode->m_nLess == pBNode->m_nPageAddr)
					{
						TBTreeNode::TMemSet& nodeTree = pParentNode->m_memset;
						TTreeNode* pNode = nodeTree.tree_minimum(nodeTree.root());
						if(!pNode)
						{
							m_nBCurrNode = -1;
							return false;
						}
						m_nBCurrNode = pParentNode->m_nPageAddr;
						m_key = pNode->key_;
						return true;
					}
					else
					{
						TBTreeNode::TMemSet& nodeTree = pParentNode->m_memset;


						short nType;
						TTreeNode* pNode = nodeTree.findNodeForBTree(m_key, nType);
						if(!pNode)
						{
							m_nBCurrNode = -1;
							return false;
						}
						TTreeNode* pNextNode  = pNode->m_pNext;
						if(pNextNode)
						{
							m_nBCurrNode = pParentNode->m_nPageAddr;
							m_key = pNextNode->key_;
							return true;
						}

						nParentAddr = pParentNode->m_nParent;
						pBNode = pParentNode;
					}
				}
				m_nBCurrNode= -1;
				return false;
			}
		}

		m_nBCurrNode= -1;
		return false;
	}
	
	bool LoadNextNode(__int64 nLink)
	{
		TBTreeNode* pBNode = m_pBTree->getNode(nLink);
		if(!pBNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		while(pBNode)
		{
			if(pBNode->m_nLess == -1) //уходим в лево
				break;
			pBNode = m_pBTree->getNode(pBNode->m_nLess);
		}

		if(!pBNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		m_nBCurrNode = pBNode->m_nPageAddr;
		TBTreeNode::TMemSet& nodeTree = pBNode->m_memset;
		TTreeNode* pNode = nodeTree.tree_minimum(nodeTree.root());
		if(!pNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		m_key = pNode->key_;
		return true;
	}
	bool LoadBackNode(__int64 nLink)
	{
		TBTreeNode* pBNode = m_pBTree->getNode(nLink);
		if(!pBNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		/*while(pBNode)
		{
			if(pBNode->m_nLess == -1) //уходим в лево
				break;
			pBNode = m_pBTree->getNode(pBNode->m_nLess);
		}*/

		if(!pBNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		m_nBCurrNode = pBNode->m_nPageAddr;
		TBTreeNode::TMemSet& nodeTree = pBNode->m_memset;
		TTreeNode* pNode = nodeTree.tree_maximim(nodeTree.m_pRoot);
		if(!pNode)
		{
			m_nBCurrNode = -1;
			return false;
		}
		while(pNode)
		{
			TBTreeNode::TMemSet& nodeRBTree = pNode->m_memset;
			pNode = nodeTree.tree_maximim(nodeTree.m_pRoot);
		}
		
		m_key = pNode->key_;
		return true;
	}
private:
	TBaseBTree* m_pBTree;
	TNodeElem m_key;
	__int64 m_nBCurrNode;


};

