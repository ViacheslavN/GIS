
template <class TypeKey, class TreeNode, class TComp>
_RBMultiTree<TypeKey, TreeNode,  TComp>::_RBMultiTree(CommonLib::alloc_t *pAlloc) : 
m_pRoot(0)
	,m_pAlloc(pAlloc)
	,m_nSize(0)
{
	assert(m_pAlloc);
}


template <class TypeKey, class TreeNode, class TComp>
_RBMultiTree<TypeKey, TreeNode, TComp>::_RBMultiTree(CommonLib::alloc_t *pAlloc, const TComporator& comp) : 
m_pRoot(0)
	,m_pAlloc(pAlloc)
	,m_Cmp(comp)
	,m_nSize(0)
{
	assert(m_pAlloc);
}

template <class TypeKey, class TreeNode, class TComp>
_RBMultiTree<TypeKey, TreeNode,  TComp>::~_RBMultiTree()
{
	destroyTree();
}


template <class TypeKey, class TNode, class TComp>
void _RBMultiTree<TypeKey, TNode, TComp>::destroyTree()
{
	if(!m_pRoot)
		return;

	TTreeNode *pNode = tree_minimum(m_pRoot);


	TTreeNode *pNextNode = pNode->m_pNext;
	if(!pNextNode)
	{
		destroyList(pNode->m_pFirst);
		m_pAlloc->free(pNode);
	}
	while(pNextNode)
	{
		destroyList(pNode->m_pFirst);
		m_pAlloc->free(pNode);
		pNode = pNextNode;
		pNextNode = pNextNode->m_pNext;
	}
	m_pAlloc->free(pNode);
}
template <class TypeKey, class TNode, class TComp>
void _RBMultiTree<TypeKey, TNode, TComp>::destroyList(TListElem* pEl){
	if(!pEl)
		return;

	TListElem* pNextEl = pEl->m_pNext;
	if(!pNextEl)
		m_pAlloc->free(pEl);
	while(pNextEl){
		m_pAlloc->free(pEl);
		pEl = pNextEl;
		pNextEl = pEl->m_pNext;
	}
	m_pAlloc->free(pEl);
}

template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::deleteNode(TTreeNode *pdelNode)
{
	TTreeNode* y, *x = 0;
	if(!pdelNode)
		return;

	if(pdelNode->m_pPrev)
	{
		pdelNode->m_pPrev->m_pNext = pdelNode->m_pNext;
	}
	if(pdelNode->m_pNext)
	{
		pdelNode->m_pNext->m_pPrev = pdelNode->m_pPrev;
	}
	if(pdelNode->m_pLeft == 0 || pdelNode->m_pRight == 0)
	{
		y = pdelNode;
	}
	else
	{
		y = tree_successor(pdelNode);
	}

	if(y->m_pLeft != 0)
	{
		x = y->m_pLeft;
	}
	else
	{
		x = y->m_pRight;
	}
	TTreeNode*  x_par = y->m_pParent;	
	if(x)
		x->m_pParent = x_par;

	if (y->m_pParent)
		if (y == y->m_pParent->m_pLeft)
			y->m_pParent->m_pLeft = x;
		else
			y->m_pParent->m_pRight = x;
	else
		m_pRoot = x;
	COLOR  color_y = y->color_;
	if (y != pdelNode)
	{
		/* replace z with y */ 
		y->color_ = pdelNode->color_ ;
		y->m_pLeft = pdelNode->m_pLeft;
		y->m_pRight = pdelNode->m_pRight;
		y->m_pParent = pdelNode->m_pParent;

		if (y->m_pParent)
			if (pdelNode == y->m_pParent->m_pLeft)
				y->m_pParent->m_pLeft = y;
			else
				y->m_pParent->m_pRight = y;
		else
			m_pRoot = y;

		if(y->m_pLeft != 0)
			y->m_pLeft->m_pParent = y;

		if(y->m_pRight != 0)
			y->m_pRight->m_pParent = y;

		if(x_par == pdelNode) 
			x_par = y;
	}
	if(color_y == BLACK && x)
		deleteFixup (x, x_par);
}

template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::rotateLeft(TTreeNode *pNodeLeft)
{
	TTreeNode *pNodeRight = pNodeLeft->m_pRight;

	/* establish pNodeLeft->right link */
	pNodeLeft->m_pRight = pNodeRight->m_pLeft;
	if (pNodeRight->m_pLeft != 0) 
		pNodeRight->m_pLeft->m_pParent = pNodeLeft;

	/* establish pNodeRight->parent link */
	if (pNodeRight != 0) 
		pNodeRight->m_pParent = pNodeLeft->m_pParent;
	if (pNodeLeft->m_pParent)
	{
		if (pNodeLeft == pNodeLeft->m_pParent->m_pLeft)
			pNodeLeft->m_pParent->m_pLeft = pNodeRight;
		else
			pNodeLeft->m_pParent->m_pRight = pNodeRight;
	}
	else
	{
		m_pRoot = pNodeRight;
	}

	/* link pNodeLeft and pNodeRight */
	pNodeRight->m_pLeft = pNodeLeft;
	if (pNodeLeft != 0) 
		pNodeLeft->m_pParent = pNodeRight;
}

template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::rotateRight(TTreeNode *pNodeRight)
{
	TTreeNode *pNodeLeft = pNodeRight->m_pLeft;

	/* establish pNodeRight->left link */
	pNodeRight->m_pLeft = pNodeLeft->m_pRight;
	if (pNodeLeft->m_pRight != 0) 
		pNodeLeft->m_pRight->m_pParent = pNodeRight;

	/* establish pNodeLeft->parent link */
	if (pNodeLeft != 0) 
		pNodeLeft->m_pParent = pNodeRight->m_pParent;
	if (pNodeRight->m_pParent)
	{
		if (pNodeRight == pNodeRight->m_pParent->m_pRight)
			pNodeRight->m_pParent->m_pRight = pNodeLeft;
		else
			pNodeRight->m_pParent->m_pLeft = pNodeLeft;
	}
	else
	{
		m_pRoot = pNodeLeft;
	}

	/* link pNodeRight and y */
	pNodeLeft->m_pRight = pNodeRight;
	if (pNodeRight != 0) 
		pNodeRight->m_pParent = pNodeLeft;
}

template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::insertFixup(TTreeNode *x)
{
	/* check Red-Black properties */
	while (x != m_pRoot && x->m_pParent->color_ == RED)
	{
		/* we have a violation */
		if (x->m_pParent == x->m_pParent->m_pParent->m_pLeft)
		{
			TTreeNode *y = x->m_pParent->m_pParent->m_pRight;
			if (y && y->color_ == RED)
			{

				/* uncle is RED */
				x->m_pParent->color_ = BLACK;
				y->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				x = x->m_pParent->m_pParent;
			}
			else
			{

				/* uncle is BLACK */
				if (x == x->m_pParent->m_pRight)
				{
					/* make x a left child */
					x = x->m_pParent;
					rotateLeft(x);
				}

				/* recolor and rotate */
				x->m_pParent->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				rotateRight(x->m_pParent->m_pParent);
			}
		}
		else
		{

			/* mirror image of above code */
			TTreeNode *y = x->m_pParent->m_pParent->m_pLeft;
			if (y && y->color_ == RED)
			{

				/* uncle is RED */
				x->m_pParent->color_ = BLACK;
				y->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				x = x->m_pParent->m_pParent;
			}
			else
			{
				/* uncle is BLACK */
				if (x == x->m_pParent->m_pLeft)
				{
					x = x->m_pParent;
					rotateRight(x);
				}
				x->m_pParent->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				rotateLeft(x->m_pParent->m_pParent);
			}
		}
	}
	m_pRoot->color_ = BLACK;
}

template <class TypeKey, class TypeNode, class TComp>
void _RBMultiTree<TypeKey, TypeNode, TComp>::deleteFixup(TTreeNode *x, TTreeNode *x_par)
{
	while (x != m_pRoot && x->color_ == BLACK) {
		if (x == x_par->m_pLeft) {
			TTreeNode *w = x_par->m_pRight;
			if (w->color_ == RED) {
				w->color_ = BLACK;
				x_par->color_ = RED;
				rotateLeft(x_par);
				w = x_par->m_pRight;
			}
			if (w->m_pLeft->color_ == BLACK && w->m_pRight->color_ == BLACK) {
				w->color_ = RED;
				x = x_par;
				x_par = x->m_pParent;
			} else {
				if (w->m_pRight->color_ == BLACK) {
					w->m_pLeft->color_ = BLACK;
					w->color_ = RED;
					rotateRight(w);
					w = x_par->m_pRight;
				}
				w->color_ = x_par->color_;
				x_par->color_ = BLACK;
				w->m_pRight->color_ = BLACK;
				rotateLeft (x_par);
				x = m_pRoot;
				x_par = NULL;
			}
		} else {
			TTreeNode *w = x_par->m_pLeft;
			if (w->color_ == RED) {
				w->color_ = BLACK;
				x_par->color_= RED;
				rotateRight (x_par);
				w = x_par->m_pLeft;
			}
			if (w->m_pRight->color_ == BLACK && w->m_pLeft->color_ == BLACK) {
				w->color_ = RED;
				x = x_par;
				x_par = x->m_pParent;

			} else {
				if (w->m_pLeft->color_ == BLACK) 
				{
					w->m_pRight->color_ = BLACK;
					w->color_ = RED;
					rotateLeft (w);
					w = x_par->m_pLeft;
				}
				w->color_ = x_par->color_;
				x_par->color_ = BLACK;
				w->m_pLeft->color_ = BLACK;
				rotateRight (x_par);
				x = m_pRoot;
				x_par = NULL;
			}
		}
	}
	x->color_ = BLACK;
}

template <class TypeKey, class TreeNode, class TComp>
bool _RBMultiTree<TypeKey, TreeNode, TComp>::insert(const TypeKey& key)
{
	TTreeNode *pNode = NULL; 
	TTreeNode *pFindNode = NULL; 
	getNodeForInsert(&pNode, &pFindNode, key);
	if(pFindNode){
		pFindNode->push_back(key, m_pAlloc);
		m_nSize++;
		return true;
	}
	TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode();
	pNewNode->push_back(key, m_pAlloc);
	addNewNode(pNode, pNewNode);
	return true;
}
template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::addNewNode(TTreeNode *pNode, TTreeNode* pNewNode){
	if(!pNode)
	{
		m_pRoot = pNewNode;
	}
	else
	{
		if(m_Cmp.LE(pNewNode->m_pFirst->key_, pNode->m_pFirst->key_))
		{
			pNode->m_pLeft = pNewNode;
			pNewNode->m_pNext = pNode;
			if(pNode->m_pPrev)
				pNode->m_pPrev->m_pNext = pNewNode;
			pNode->m_pPrev = pNewNode;

		}
		else
		{
			pNode->m_pRight = pNewNode;
			if(pNode->m_pNext)
				pNode->m_pNext->m_pPrev = pNewNode;
			pNewNode->m_pNext = pNode->m_pNext;
			pNode->m_pNext = pNewNode;
			pNewNode->m_pPrev = pNode;
		}
		pNewNode->m_pParent = pNode;
	}
	m_nSize += 1;
	insertFixup(pNewNode);
}
template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::getNodeForInsert(TTreeNode **pNode, TTreeNode **pFindNode, const TypeKey& key){
	TTreeNode *x = m_pRoot;
	while(x)
	{
		if (m_Cmp.EQ(x->m_pFirst->key_ , key)) 
		{
			*pFindNode = x;
			return;
		}
		*pNode = x;
		if( m_Cmp.LE(key , x->m_pFirst->key_))
		{
			x = x->m_pLeft;
		}
		else
		{
			x = x->m_pRight;
		}
	}
}


template <class TypeKey, class TreeNode, class TComp>
void _RBMultiTree<TypeKey, TreeNode, TComp>::remove(const TypeKey& key)
{
	TTreeNode* pDelNode = findNode(key);
	if(!pDelNode)
		return;
	pDelNode->remove(pDelNode->m_pFirst, m_pAlloc);
	m_nSize -= 1;
	if(!pDelNode->m_pFirst){
		deleteNode(pDelNode);
		m_pAlloc->free(pDelNode);
	}
}


template <class TypeKey, class TreeNode, class TComp>
TreeNode*  _RBMultiTree<TypeKey, TreeNode, TComp>::findNode(const TypeKey& key)
{
	TTreeNode *pFindNode = m_pRoot;
	while(pFindNode && !m_Cmp.EQ(pFindNode->m_pFirst->key_, key))
	{
		if( m_Cmp.LE(key, pFindNode->m_pFirst->key_))
		{
			pFindNode = pFindNode->m_pLeft;
		}
		else
		{
			pFindNode = pFindNode->m_pRight;
		}
	}

	return pFindNode;
}
template <class TypeKey, class TreeNode, class TComp>
TreeNode* _RBMultiTree<TypeKey, TreeNode, TComp>::findNodeForBTreeInsert(const TypeKey& key)
{
	TreeNode *pFindNode = m_pRoot;
	TreeNode *pLessNode = m_pRoot;

	while(pFindNode)
	{
		if( m_Cmp.LE(key, pFindNode->m_pFirst->key_))
		{
			pLessNode = pFindNode;
			pFindNode = pFindNode->m_pLeft;

		}
		else if(m_Cmp.EQ(pFindNode->m_pFirst->key_, key))
		{
			return pFindNode;
		}
		else
		{
			pFindNode = pFindNode->m_pRight;
		}
	}

	return pLessNode;
}

template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBMultiTree<TypeKey, TTreeNode, TComp>::tree_successor(TTreeNode *pNode){
	if(pNode->m_pRight)
	{
		return tree_minimum(pNode->m_pRight);
	}
	TTreeNode* y = pNode->m_pParent;
	while(y != 0 && pNode == y->m_pRight)
	{
		pNode = y;
		y = y->m_pParent;
	}
	return y;
}
template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBMultiTree<TypeKey, TTreeNode, TComp>::tree_minimum(TTreeNode *pNode){
	while (pNode->m_pLeft)
	{
		pNode = pNode->m_pLeft;
	}
	return pNode;
}
template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBMultiTree<TypeKey, TTreeNode, TComp>::tree_maximim(TTreeNode *pNode){
	while (pNode->m_pRight)
	{
		pNode = pNode->m_pRight;
	}
	return pNode;
}
