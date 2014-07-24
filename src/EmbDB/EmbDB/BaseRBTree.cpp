
template <class TypeKey, class TreeNode, class TComp>
_RBTree<TypeKey, TreeNode,  TComp>::_RBTree(CommonLib::alloc_t *pAlloc, bool bMulti) : 
	//m_pRoot(0)
	m_pAlloc(pAlloc)
	,m_nSize(0)
	,m_bMulti(bMulti)
{
	//assert(m_pAlloc);
	if(!m_pAlloc)
		m_pAlloc = &m_simple_alloc;

	m_NullHeadNode.color_ = BLACK;
	m_NullHeadNode.m_pLeft = &m_NullHeadNode;
	m_NullHeadNode.m_pRight = &m_NullHeadNode;
	m_NullHeadNode.m_pParent = &m_NullHeadNode;
	m_NullHeadNode.m_pNext = &m_NullHeadNode;
	m_NullHeadNode.m_pPrev = &m_NullHeadNode;
}


template <class TypeKey, class TreeNode, class TComp>
_RBTree<TypeKey, TreeNode, TComp>::_RBTree( const TComporator& comp, CommonLib::alloc_t *pAlloc, bool bMulti) : 
	//m_pRoot(0)
	m_pAlloc(pAlloc)
	,m_Cmp(comp)
	,m_nSize(0)
	,m_bMulti(bMulti)
{
	//assert(m_pAlloc);
	if(!m_pAlloc)
		m_pAlloc = &m_simple_alloc;

	m_NullHeadNode.color_ = BLACK;
	m_NullHeadNode.m_pLeft = &m_NullHeadNode;
	m_NullHeadNode.m_pRight = &m_NullHeadNode;
	m_NullHeadNode.m_pParent = &m_NullHeadNode;
	m_NullHeadNode.m_pNext = &m_NullHeadNode;
	m_NullHeadNode.m_pPrev = &m_NullHeadNode;
}

template <class TypeKey, class TreeNode, class TComp>
_RBTree<TypeKey, TreeNode,  TComp>::~_RBTree()
{
	destroyTree();
}

template <class TypeKey, class TNode, class TComp>
void _RBTree<TypeKey, TNode, TComp>::clear()
{
	destroyTree();
}

template <class TypeKey, class TNode, class TComp>
void _RBTree<TypeKey, TNode, TComp>::destroyTree()
{
  if(isNull(root()))
	 return;
  	m_nSize = 0;
   TTreeNode *pNode = tree_minimum(root());
 

   TTreeNode *pNextNode = pNode->m_pNext;
   if(isNull(pNextNode))
   {
	  m_pAlloc->free(pNode);

	  m_NullHeadNode.color_ = BLACK;
	  m_NullHeadNode.m_pLeft = &m_NullHeadNode;
	  m_NullHeadNode.m_pRight = &m_NullHeadNode;
	  m_NullHeadNode.m_pParent = &m_NullHeadNode;
	  m_NullHeadNode.m_pNext = &m_NullHeadNode;
	  m_NullHeadNode.m_pPrev = &m_NullHeadNode;

	  return;
   }
   while(!isNull(pNextNode))
   {
	 m_pAlloc->free(pNode);
	 pNode = pNextNode;
	 pNextNode = pNextNode->m_pNext;
   }
    m_pAlloc->free(pNode);



	m_NullHeadNode.color_ = BLACK;
	m_NullHeadNode.m_pLeft = &m_NullHeadNode;
	m_NullHeadNode.m_pRight = &m_NullHeadNode;
	m_NullHeadNode.m_pParent = &m_NullHeadNode;
	m_NullHeadNode.m_pNext = &m_NullHeadNode;
	m_NullHeadNode.m_pPrev = &m_NullHeadNode;
	//m_pRoot = NULL;
}

/*template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::deleteNode(TTreeNode *pdelNode, bool bDecSize)
{
	TTreeNode* y, *x = 0;
	if(!pdelNode)
		return;

	if(bDecSize)
		m_nSize--;

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
	{
		if (y == y->m_pParent->m_pLeft)
			y->m_pParent->m_pLeft = x;
		else
			y->m_pParent->m_pRight = x;
	}
	else
		m_pRoot = x;
	COLOR  color_y = y->color_;
	if (y != pdelNode)
	{
		// replace z with y 
		y->color_ = pdelNode->color_ ;
		y->m_pLeft = pdelNode->m_pLeft;
		y->m_pRight = pdelNode->m_pRight;
		y->m_pParent = pdelNode->m_pParent;

		if (y->m_pParent)
		{
			if (pdelNode == y->m_pParent->m_pLeft)
				y->m_pParent->m_pLeft = y;
			else
				y->m_pParent->m_pRight = y;
		}
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
}*/

template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::deleteNode(TTreeNode *pWhere, bool bDecSize, bool bRemoveNode)
{	// erase element at _Where
/*#if _ITERATOR_DEBUG_LEVEL == 2
	if (_Where._Getcont() != this || this->_Isnil(_Where._Mynode()))
		_DEBUG_ERROR("map/set erase iterator outside range");
	_Nodeptr _Erasednode = _Where._Mynode();	// node to erase
	++_Where;	// save successor iterator for return
	_Orphan_ptr(*this, _Erasednode);

#else // _ITERATOR_DEBUG_LEVEL == 2
	if (this->_Isnil(_Where._Mynode()))
		_Xout_of_range("invalid map/set<T> iterator");
	_Nodeptr _Erasednode = _Where._Mynode();	// node to erase
	++_Where;	// save successor iterator for return
#endif //_ITERATOR_DEBUG_LEVEL == 2 */
	if(pWhere == &m_NullHeadNode)
		return;

	TTreeNode *pErasedNode = pWhere;
	//pWhere = nextDel(pWhere);
	pWhere = pWhere->m_pNext;
	// save successor iterator for return


	if(bDecSize)
		m_nSize--;
	if(pErasedNode->m_pPrev)
	{
		pErasedNode->m_pPrev->m_pNext = pErasedNode->m_pNext;
	}
	if(pErasedNode->m_pNext)
	{
		pErasedNode->m_pNext->m_pPrev = pErasedNode->m_pPrev;
	}

	//_Nodeptr _Fixnode;	// the node to recolor as needed
	TTreeNode *pFixNode;
	//_Nodeptr _Fixnodeparent;	// parent of _Fixnode (which may be nil)
	TTreeNode *pFixNodeParent;
	//_Nodeptr _Pnode = _Erasednode;
	TTreeNode *pNode = pErasedNode;

	//if (this->_Isnil(this->_Left(_Pnode)))
	if (pNode->m_pLeft == &m_NullHeadNode)
		//_Fixnode = this->_Right(_Pnode);	// stitch up right subtree
		pFixNode = pNode->m_pRight;
	//else if (this->_Isnil(this->_Right(_Pnode)))
	else if (pNode->m_pRight == &m_NullHeadNode)
		//	_Fixnode = this->_Left(_Pnode);	// stitch up left subtree
		pFixNode = pNode->m_pLeft;	// stitch up left subtree
	else
	{	// two subtrees, must lift successor node to replace erased
		//_Pnode = _Where._Mynode();	// _Pnode is successor node
		pNode = pWhere;//tree_successor(pErasedNode);
		//_Fixnode = this->_Right(_Pnode);	// _Fixnode is only subtree
		pFixNode = pNode->m_pRight;
	}
	//if (_Pnode == _Erasednode)
	if (pNode == pErasedNode)
	{	// at most one subtree, relink it
		//_Fixnodeparent = this->_Parent(_Erasednode);
		pFixNodeParent = pErasedNode->m_pParent;
		//if (!this->_Isnil(_Fixnode))
		if (pFixNode != &m_NullHeadNode)
			//this->_Parent(_Fixnode) = _Fixnodeparent;	// link up
			pFixNode->m_pParent = pFixNodeParent;
		//if (_Root() == _Erasednode)
		if (root() == pErasedNode)
			//_Root() = _Fixnode;	// link down from root
			m_NullHeadNode.m_pParent = pFixNode;
		//else if (this->_Left(_Fixnodeparent) == _Erasednode)
		else if (pFixNodeParent->m_pLeft == pErasedNode)
			//this->_Left(_Fixnodeparent) = _Fixnode;	// link down to left
			pFixNodeParent->m_pLeft = pFixNode;
		else
			//this->_Right(_Fixnodeparent) =	_Fixnode;	// link down to right
			pFixNodeParent->m_pRight = pFixNode;


		//if (_Lmost() == _Erasednode)
		if (lmost() == pErasedNode)
			//_Lmost() = this->_Isnil(_Fixnode) ? _Fixnodeparent	// smallest is parent of erased node
			//: this->_Min(_Fixnode);	// smallest in relinked subtree
			m_NullHeadNode.m_pLeft = (pFixNode == &m_NullHeadNode ? pFixNodeParent : tree_minimum(pFixNode));

		//if (_Rmost() == _Erasednode)
		if (rmost() == pErasedNode)
			//_Rmost() = this->_Isnil(_Fixnode)	? _Fixnodeparent	// largest is parent of erased node
			//: this->_Max(_Fixnode);	// largest in relinked subtree
			 m_NullHeadNode.m_pRight = (pFixNode == &m_NullHeadNode ? pFixNodeParent : tree_maximim(pFixNode));
	}
	else
	{	// erased has two subtrees, _Pnode is successor to erased
		//this->_Parent(this->_Left(_Erasednode)) =	_Pnode;	// link left up
		pErasedNode->m_pLeft->m_pParent = pNode;
		//this->_Left(_Pnode) =	this->_Left(_Erasednode);	// link successor down
		pNode->m_pLeft = pErasedNode->m_pLeft;
		//if (_Pnode == this->_Right(_Erasednode))
		if (pNode == pErasedNode->m_pRight)
			//_Fixnodeparent = _Pnode;	// successor is next to erased
			pFixNodeParent = pNode;
		else
		{	// successor further down, link in place of erased
			//_Fixnodeparent = this->_Parent(_Pnode);	// parent is successor's
			pFixNodeParent = pNode->m_pParent;
			//if (!this->_Isnil(_Fixnode))
			if(pFixNode != &m_NullHeadNode)
				//this->_Parent(_Fixnode) = _Fixnodeparent;	// link fix up
				pFixNode->m_pParent = pFixNodeParent;
			//this->_Left(_Fixnodeparent) = _Fixnode;	// link fix down
			pFixNodeParent->m_pLeft = pFixNode;
			//this->_Right(_Pnode) =	this->_Right(_Erasednode);	// link next down
			pNode->m_pRight = pErasedNode->m_pRight;
			//this->_Parent(this->_Right(_Erasednode)) =	_Pnode;	// right up
			pErasedNode->m_pRight->m_pParent = pNode;
		}

		//if (_Root() == _Erasednode)
		if (root() == pErasedNode)
			//_Root() = _Pnode;	// link down from root
			m_NullHeadNode.m_pParent = pNode;
		//else if (this->_Left(this->_Parent(_Erasednode)) == _Erasednode)
		else if (pErasedNode->m_pParent->m_pLeft == pErasedNode)
			//this->_Left(this->_Parent(_Erasednode)) =_Pnode;	// link down to left
			pErasedNode->m_pParent->m_pLeft = pNode;
		else
			//this->_Right(this->_Parent(_Erasednode)) =	_Pnode;	// link down to right
			pErasedNode->m_pParent->m_pRight = pNode;

		//this->_Parent(_Pnode) =	this->_Parent(_Erasednode);	// link successor up
		//_STD swap(this->_Color(_Pnode), 	this->_Color(_Erasednode));	// recolor it
		pNode->m_pParent = pErasedNode->m_pParent;
		bool bColor = pNode->color_;
		pNode->color_ = pErasedNode->color_;
		pErasedNode->color_ = bColor;
	}

	//if (this->_Color(_Erasednode) == this->_Black)
	if (pErasedNode->color_ == BLACK)
	{	// erasing black link, must recolor/rebalance tree
		//for (; _Fixnode != _Root()&& this->_Color(_Fixnode) == this->_Black;_Fixnodeparent = this->_Parent(_Fixnode))
		for (; pFixNode != root() && pFixNode->color_ == BLACK; pFixNodeParent = pFixNode->m_pParent)
			//if (_Fixnode == this->_Left(_Fixnodeparent))
			if (pFixNode == pFixNodeParent->m_pLeft)
			{	// fixup left subtree
				//_Pnode = this->_Right(_Fixnodeparent);
				pNode = pFixNodeParent->m_pRight;
				//if (this->_Color(_Pnode) == this->_Red)
				if (pNode->color_ == RED)
				{	// rotate red up from right subtree
					//this->_Color(_Pnode) = this->_Black;
					pNode->color_ = BLACK;
					//this->_Color(_Fixnodeparent) = this->_Red;
					pFixNodeParent->color_ = RED;
					//_Lrotate(_Fixnodeparent);
					rotateLeft(pFixNodeParent);
					//_Pnode = this->_Right(_Fixnodeparent);
					pNode = pFixNodeParent->m_pRight;
				}

				//if (this->_Isnil(_Pnode))
				if (pNode == &m_NullHeadNode)
					//_Fixnode = _Fixnodeparent;	// shouldn't happen
					pFixNode = pFixNodeParent;	// shouldn't happen
				//else if (this->_Color(this->_Left(_Pnode)) == this->_Black && this->_Color(this->_Right(_Pnode)) == this->_Black)
				else if (pNode->m_pLeft->color_ == BLACK && pNode->m_pRight->color_== BLACK)
				{	// redden right subtree with black children
					//this->_Color(_Pnode) = this->_Red;
					pNode->color_ = RED;
					//_Fixnode = _Fixnodeparent;
					pFixNode = pFixNodeParent;	
				}
				else
				{	// must rearrange right subtree
					//if (this->_Color(this->_Right(_Pnode))	== this->_Black)
					if (pNode->m_pRight->color_	== BLACK)
					{	// rotate red up from left sub-subtree
						//this->_Color(this->_Left(_Pnode)) = this->_Black;
						pNode->m_pLeft->color_ = BLACK;
						//this->_Color(_Pnode) = this->_Red;
						pNode->color_ = RED;
						//_Rrotate(_Pnode);
						rotateRight(pNode);
						//_Pnode = this->_Right(_Fixnodeparent);
						pNode = pFixNodeParent->m_pRight;
					}

					//this->_Color(_Pnode) = this->_Color(_Fixnodeparent);
					pNode->color_ = pFixNodeParent->color_;
					//this->_Color(_Fixnodeparent) = this->_Black;
					pFixNodeParent->color_ = BLACK;
					//this->_Color(this->_Right(_Pnode)) = this->_Black;
					pNode->m_pRight->color_ = BLACK;
					//_Lrotate(_Fixnodeparent);
					rotateLeft(pFixNodeParent);
					break;	// tree now recolored/rebalanced
				}
			}
			else
			{	// fixup right subtree
				//_Pnode = this->_Left(_Fixnodeparent);
				pNode = pFixNodeParent->m_pLeft;
				//if (this->_Color(_Pnode) == this->_Red)
				if(pNode->color_ == RED)
				{	// rotate red up from left subtree
					//this->_Color(_Pnode) = this->_Black;
					pNode->color_ = BLACK;
					//this->_Color(_Fixnodeparent) = this->_Red;
					pFixNodeParent->color_ = RED;
					//_Rrotate(_Fixnodeparent);
					rotateRight(pFixNodeParent);
					//_Pnode = this->_Left(_Fixnodeparent);
					pNode = pFixNodeParent->m_pLeft;
				}
				//if (this->_Isnil(_Pnode))
				if (pNode == &m_NullHeadNode)
					//_Fixnode = _Fixnodeparent;	// shouldn't happen
					pFixNode = pFixNodeParent;
				//else if (this->_Color(this->_Right(_Pnode)) == this->_Black	&& this->_Color(this->_Left(_Pnode)) == this->_Black)
				else if (pNode->m_pRight->color_ == BLACK	&& pNode->m_pLeft->color_ == BLACK)
				{	// redden left subtree with black children
					//this->_Color(_Pnode) = this->_Red;
					pNode->color_ = RED;
					//_Fixnode = _Fixnodeparent;
					pFixNode = pFixNodeParent;
				}
				else
				{	// must rearrange left subtree
					//if (this->_Color(this->_Left(_Pnode)) == this->_Black)
					if (pNode->m_pLeft->color_== BLACK)
					{	// rotate red up from right sub-subtree
						//this->_Color(this->_Right(_Pnode)) = this->_Black;
						pNode->m_pRight->color_ = BLACK;
						//this->_Color(_Pnode) = this->_Red;
						pNode->color_ = RED;
						//_Lrotate(_Pnode);
						rotateLeft(pNode);
						//_Pnode = this->_Left(_Fixnodeparent);
						pNode = pFixNodeParent->m_pLeft;
					}

					//this->_Color(_Pnode) = this->_Color(_Fixnodeparent);
					pNode->color_ = pFixNodeParent->color_;
					//this->_Color(_Fixnodeparent) = this->_Black;
					pFixNodeParent->color_ = BLACK;
					//this->_Color(this->_Left(_Pnode)) = this->_Black;
					pNode->m_pLeft->color_ = BLACK;
					//_Rrotate(_Fixnodeparent);
					rotateRight(pFixNodeParent);
					break;	// tree now recolored/rebalanced
				}
			}

			//this->_Color(_Fixnode) = this->_Black;	// stopping node is black
			pFixNode->color_ = BLACK;
	}
	if(bRemoveNode)
		m_pAlloc->free(pErasedNode);

	/*_Dest_val(this->_Alval,
		_STD addressof(this->_Myval(_Erasednode)));	// delete erased node

	this->_Alnod.deallocate(_Erasednode, 1);

	if (0 < this->_Mysize)
		--this->_Mysize;

	return (iterator(_Where._Ptr, this));	// return successor iterator*/
}


template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::rotateLeft(TTreeNode *pWherenode)
{
	/*TTreeNode *pNodeRight = pNodeLeft->m_pRight;

	// establish pNodeLeft->right link 
	pNodeLeft->m_pRight = pNodeRight->m_pLeft;
	if (pNodeRight->m_pLeft != 0) 
		pNodeRight->m_pLeft->m_pParent = pNodeLeft;

	// establish pNodeRight->parent link 
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

	///link pNodeLeft and pNodeRight
	pNodeRight->m_pLeft = pNodeLeft;
	if (pNodeLeft != 0) 
		pNodeLeft->m_pParent = pNodeRight;*/

	//_Nodeptr _Pnode = this->_Right(_Wherenode);
	TTreeNode *pNode = pWherenode->m_pRight;
	//this->_Right(_Wherenode) = this->_Left(_Pnode);
	 pWherenode->m_pRight = pNode->m_pLeft;

	//if (!this->_Isnil(this->_Left(_Pnode)))
	 if (pNode->m_pLeft != &m_NullHeadNode)
		//this->_Parent(this->_Left(_Pnode)) = _Wherenode;
		pNode->m_pLeft->m_pParent = pWherenode;
	//this->_Parent(_Pnode) = this->_Parent(_Wherenode);
	 pNode->m_pParent = pWherenode->m_pParent;

	//if (_Wherenode == _Root())
	 if(pWherenode == m_NullHeadNode.m_pParent)
		 //_Root() = _Pnode;
		m_NullHeadNode.m_pParent = pNode;
	//else if (_Wherenode == this->_Left(this->_Parent(_Wherenode)))
	 else if (pWherenode == pWherenode->m_pParent->m_pLeft)
		 //this->_Left(this->_Parent(_Wherenode)) = _Pnode;
		pWherenode->m_pParent->m_pLeft = pNode;
	else
		//this->_Right(this->_Parent(_Wherenode)) = _Pnode;
		pWherenode->m_pParent->m_pRight = pNode;

	//this->_Left(_Pnode) = _Wherenode;
	 pNode->m_pLeft = pWherenode;
	//this->_Parent(_Wherenode) = _Pnode;
	 pWherenode->m_pParent = pNode;
}

template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::rotateRight(TTreeNode *pWhereNode)
{
	/*TTreeNode *pNodeLeft = pNodeRight->m_pLeft;

	// establish pNodeRight->left link 
	pNodeRight->m_pLeft = pNodeLeft->m_pRight;
	if (pNodeLeft->m_pRight != 0) 
		pNodeLeft->m_pRight->m_pParent = pNodeRight;

	// establish pNodeLeft->parent link 
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

	// link pNodeRight and y
	pNodeLeft->m_pRight = pNodeRight;
	if (pNodeRight != 0) 
		pNodeRight->m_pParent = pNodeLeft;*/

	//_Nodeptr _Pnode = this->_Left(_Wherenode);
	TTreeNode *pNode = pWhereNode->m_pLeft;
	//this->_Left(_Wherenode) = this->_Right(_Pnode);
	 pWhereNode->m_pLeft = pNode->m_pRight;
	//if (!this->_Isnil(this->_Right(_Pnode)))
	 if (pNode->m_pRight != &m_NullHeadNode)
		//this->_Parent(this->_Right(_Pnode)) = _Wherenode;
		pNode->m_pRight->m_pParent = pWhereNode;
	//this->_Parent(_Pnode) = this->_Parent(_Wherenode);
	 pNode->m_pParent = pWhereNode->m_pParent;

	//if (_Wherenode == _Root())
	 if(pWhereNode == m_NullHeadNode.m_pParent)
		//_Root() = _Pnode;
		m_NullHeadNode.m_pParent = pNode;
	//else if (_Wherenode == this->_Right(this->_Parent(_Wherenode)))
	 else if (pWhereNode == pWhereNode->m_pParent->m_pRight)
		//this->_Right(this->_Parent(_Wherenode)) = _Pnode;
		pWhereNode->m_pParent->m_pRight = pNode;
	else
		//this->_Left(this->_Parent(_Wherenode)) = _Pnode;
		pWhereNode->m_pParent->m_pLeft = pNode;

	//this->_Right(_Pnode) = _Wherenode;
	 pNode->m_pRight = pWhereNode;
	//this->_Parent(_Wherenode) = _Pnode;
	 pWhereNode->m_pParent = pNode;
}

template <class TypeKey, class TreeNode, class TComp>
TreeNode * _RBTree<TypeKey, TreeNode, TComp>::insertFixup(TTreeNode *pNewNode)
{
	//for (_Nodeptr _Pnode = _Newnode;	this->_Color(this->_Parent(_Pnode)) == this->_Red; )
	for (TTreeNode* pNode = pNewNode; pNode->m_pParent->color_ == RED; )
		//if (this->_Parent(_Pnode) == this->_Left(this->_Parent(this->_Parent(_Pnode))))
		if (pNode->m_pParent == pNode->m_pParent->m_pParent->m_pLeft)
		{	// fixup red-red in left subtree
			//_Wherenode =	this->_Right(this->_Parent(this->_Parent(_Pnode)));
			TTreeNode* pWhereNode = pNode->m_pParent->m_pParent->m_pRight;
		 
			//if (this->_Color(_Wherenode) == this->_Red)
			if (pWhereNode->color_ == RED)
			{	// parent has two red children, blacken both
				//this->_Color(this->_Parent(_Pnode)) = this->_Black;
				pNode->m_pParent->color_ = BLACK;
				//this->_Color(_Wherenode) = this->_Black;
				pWhereNode->color_  = BLACK;
				//this->_Color(this->_Parent(this->_Parent(_Pnode)))	= this->_Red;
				pNode->m_pParent->m_pParent->color_ = RED;
				//_Pnode = this->_Parent(this->_Parent(_Pnode));
				pNode = pNode->m_pParent->m_pParent;
			}
			else
			{	// parent has red and black children
				//if (_Pnode == this->_Right(this->_Parent(_Pnode)))
				if (pNode == pNode->m_pParent->m_pRight)
				{	// rotate right child to left
					//_Pnode = this->_Parent(_Pnode);
					pNode = pNode->m_pParent;
					//_Lrotate(_Pnode);
					rotateLeft(pNode);

				}
				//this->_Color(this->_Parent(_Pnode)) =	this->_Black;	// propagate red up
				pNode->m_pParent->color_ = BLACK;
				//this->_Color(this->_Parent(this->_Parent(_Pnode))) = this->_Red;
				pNode->m_pParent->m_pParent->color_ = RED;
				//_Rrotate(this->_Parent(this->_Parent(_Pnode)));
				rotateRight(pNode->m_pParent->m_pParent);
			}
		}
		else
		{	// fixup red-red in right subtree
			//_Wherenode = this->_Left(this->_Parent(this->_Parent(_Pnode)));
			TTreeNode* pWhereNode = pNode->m_pParent->m_pParent->m_pLeft;
			//if (this->_Color(_Wherenode) == this->_Red)
			if ( pWhereNode->color_ == RED)
			{	// parent has two red children, blacken both
				//this->_Color(this->_Parent(_Pnode)) = this->_Black;
				pNode->m_pParent->color_ = BLACK;
				//this->_Color(_Wherenode) = this->_Black;
				pWhereNode->color_= BLACK;
				//this->_Color(this->_Parent(this->_Parent(_Pnode))) = this->_Red;
				pNode->m_pParent->m_pParent->color_ = RED;
				//_Pnode = this->_Parent(this->_Parent(_Pnode));
				pNode = pNode->m_pParent->m_pParent;
			}
			else
			{	// parent has red and black children
				//if (_Pnode == this->_Left(this->_Parent(_Pnode)))
				if (pNode == pNode->m_pParent->m_pLeft)
				{	// rotate left child to right
					//_Pnode = this->_Parent(_Pnode);
					pNode = pNode->m_pParent;
					//_Rrotate(_Pnode);
					rotateRight(pNode);
				}
				//this->_Color(this->_Parent(_Pnode)) =	this->_Black;	// propagate red up
				pNode->m_pParent->color_= BLACK;

				//this->_Color(this->_Parent(this->_Parent(_Pnode))) = this->_Red;
				pNode->m_pParent->m_pParent->color_= RED;
				//_Lrotate(this->_Parent(this->_Parent(_Pnode)));
				rotateLeft(pNode->m_pParent->m_pParent);
			}
		}

	//this->_Color(_Root()) = this->_Black;	// root is always black
	m_NullHeadNode.m_pParent->color_ = BLACK;
	return pNewNode;
}
/*template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::insertFixup(TTreeNode *x)
{
	// check Red-Black properties 
	while (x != m_pRoot && x->m_pParent->color_ == RED)
	{
		// we have a violation 
		if (x->m_pParent == x->m_pParent->m_pParent->m_pLeft)
		{
			TTreeNode *y = x->m_pParent->m_pParent->m_pRight;
			if (y && y->color_ == RED)
			{

				//uncle is RED 
				x->m_pParent->color_ = BLACK;
				y->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				x = x->m_pParent->m_pParent;
			}
			else
			{

				// uncle is BLACK 
				if (x == x->m_pParent->m_pRight)
				{
					// make x a left child 
					x = x->m_pParent;
					rotateLeft(x);
				}

				// recolor and rotate 
				x->m_pParent->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				rotateRight(x->m_pParent->m_pParent);
			}
		}
		else
		{

			// mirror image of above code 
			TTreeNode *y = x->m_pParent->m_pParent->m_pLeft;
			if (y && y->color_ == RED)
			{

				// uncle is RED 
				x->m_pParent->color_ = BLACK;
				y->color_ = BLACK;
				x->m_pParent->m_pParent->color_ = RED;
				x = x->m_pParent->m_pParent;
			}
			else
			{
				// uncle is BLACK 
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
}*/
/*
template <class TypeKey, class TypeNode, class TComp>
void _RBTree<TypeKey, TypeNode, TComp>::deleteFixup(TTreeNode *x, TTreeNode *x_par)
{
	while (x != m_pRoot && x->color_ == BLACK)
	{
		if (x == x_par->m_pLeft)
		{
			TTreeNode *w = x_par->m_pRight;
			if(!w)
				break;
			if (w->color_ == RED)
			{
				w->color_ = BLACK;
				x_par->color_ = RED;
				rotateLeft(x_par);
				w = x_par->m_pRight;
			}
			if(w && w->m_pRight && w->m_pLeft)
			{
				if (w->m_pLeft->color_ == BLACK && w->m_pRight->color_ == BLACK) 
				{
					w->color_ = RED;
					x = x_par;
					x_par = x->m_pParent;
				} 
				else
				{
					if (w->m_pRight->color_ == BLACK)
					{
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
			}
			else
				break;
		} 
		else 
		{
			TTreeNode *w = x_par->m_pLeft;
			if(!w)
				break;
			if (w->color_ == RED)
			{
				w->color_ = BLACK;
				x_par->color_= RED;
				rotateRight (x_par);
				w = x_par->m_pLeft;
			}
			if(w && w->m_pRight && w->m_pLeft)
			{
				if ((w->m_pRight->color_ == BLACK && w->m_pLeft->color_ == BLACK))
				{
					w->color_ = RED;
					x = x_par;
					x_par = x->m_pParent;

				} 
				else
				{
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
			else
				break;
			
		}
	}
	x->color_ = BLACK;
}*/

template <class TypeKey, class TreeNode, class TComp>
TreeNode* _RBTree<TypeKey, TreeNode, TComp>::insert(const TypeKey& key)
{


	//const value_type& _Val = this->_Myval(_Node);

	TTreeNode*  pTryNode = root();
	TTreeNode*  pWhereNode = &m_NullHeadNode;
	bool bAddleft = true;	// add to left of head if tree empty
	while (pTryNode != &m_NullHeadNode)
	{	// look for leaf to insert before (_Addleft) or after
			pWhereNode = pTryNode;

			bAddleft = m_Cmp.LE(key, pTryNode->m_key);
			//this->_Kfn(_Val),
			//this->_Key(_Trynode));	// favor right end
			pTryNode = bAddleft ? pTryNode->m_pLeft : pTryNode->m_pRight;
	}
	if (m_bMulti)
	{
		TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key);
		pNewNode->m_pLeft = &m_NullHeadNode;
		pNewNode->m_pRight = &m_NullHeadNode;
		pNewNode->m_pParent = &m_NullHeadNode;
		pNewNode->m_pNext = &m_NullHeadNode;
		pNewNode->m_pPrev = &m_NullHeadNode;
		return addNewNode(bAddleft, pWhereNode, pNewNode);
	}
		//return (_Pairib(_Insert(_Addleft, _Wherenode, _Node), true));
	else
	{	// insert only if unique
		//iterator _Where = iterator(_Wherenode, this);

		TTreeNode*  pWhereNodePrevNode = pWhereNode;
		if (!bAddleft)
			;	// need to test if insert after is okay
		else if (pWhereNode == tree_minimum(root()))
			//return (_Pairib(_Insert(true, _Wherenode, _Node), true));
		{
			TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key);
			pNewNode->m_pLeft = &m_NullHeadNode;
			pNewNode->m_pRight = &m_NullHeadNode;
			pNewNode->m_pParent = &m_NullHeadNode;
			pNewNode->m_pNext = &m_NullHeadNode;
			pNewNode->m_pPrev = &m_NullHeadNode;
			return addNewNode(true, pWhereNode, pNewNode);
			
		}
		else
			pWhereNodePrevNode = pWhereNode->m_pPrev;	// need to test if insert before is okay

		/*if (_DEBUG_LT_PRED(this->comp,
			this->_Key(_Where._Mynode()),
			this->_Kfn(_Val)))
			return (_Pairib(_Insert(_Addleft, _Wherenode, _Node), true));*/
		if( m_Cmp.LE(pWhereNodePrevNode->m_key, key))
		{
			TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key);
			pNewNode->m_pLeft = &m_NullHeadNode;
			pNewNode->m_pRight = &m_NullHeadNode;
			pNewNode->m_pParent = &m_NullHeadNode;
			pNewNode->m_pNext = &m_NullHeadNode;
			pNewNode->m_pPrev = &m_NullHeadNode;
			return addNewNode(bAddleft, pWhereNode, pNewNode);
		}
		else
		{	
			return &m_NullHeadNode;
		}
	}

	//if(!getNodeForInsert(&pNode, key))
	//	return false;
	/*	TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key);
		pNewNode->m_pLeft = &m_NullHeadNode;
		pNewNode->m_pRight = &m_NullHeadNode;
		pNewNode->m_pParent = &m_NullHeadNode;
	addNewNode(pNode, pNewNode);*/
	return &m_NullHeadNode;
}
template <class TypeKey, class TreeNode, class TComp>
TreeNode* _RBTree<TypeKey, TreeNode, TComp>::insertNode(TTreeNode* pNewNode)
{
	/*TTreeNode *pNode = NULL; 
	if(!getNodeForInsert(&pNode, pNewNode->key_))
		return false;
	pNewNode->color_ = RED;
	pNewNode->m_pLeft = &m_NullHeadNode;
	pNewNode->m_pRight = &m_NullHeadNode;
	pNewNode->m_pParent = &m_NullHeadNode;
	pNewNode->m_pPrev = &m_NullHeadNode;
	pNewNode->m_pNext = &m_NullHeadNode;
	addNewNode(pNode, pNewNode);*/

	
	TTreeNode*  pTryNode = root();
	TTreeNode*  pWhereNode = &m_NullHeadNode;
	bool bAddleft = true;	// add to left of head if tree empty
	while (pTryNode != &m_NullHeadNode)
	{	// look for leaf to insert before (_Addleft) or after
			pWhereNode = pTryNode;

			bAddleft = m_Cmp.LE(pNewNode->m_key, pTryNode->m_key);
			//this->_Kfn(_Val),
			//this->_Key(_Trynode));	// favor right end
			pTryNode = bAddleft ? pTryNode->m_pLeft : pTryNode->m_pRight;
	}
	if (m_bMulti)
	{
		pNewNode->color_ = RED;
		pNewNode->m_pLeft = &m_NullHeadNode;
		pNewNode->m_pRight = &m_NullHeadNode;
		pNewNode->m_pParent = &m_NullHeadNode;
		pNewNode->m_pNext = &m_NullHeadNode;
		pNewNode->m_pPrev = &m_NullHeadNode;
		return addNewNode(bAddleft, pWhereNode, pNewNode);
	}
		//return (_Pairib(_Insert(_Addleft, _Wherenode, _Node), true));
	else
	{	// insert only if unique
		//iterator _Where = iterator(_Wherenode, this);
		TTreeNode*  pWhereNodePrevNode = pWhereNode;
		if (!bAddleft)
			;	// need to test if insert after is okay
		else if (pWhereNode == tree_minimum(root()))
			//return (_Pairib(_Insert(true, _Wherenode, _Node), true));
		{
			pNewNode->color_ = RED;
			pNewNode->m_pLeft = &m_NullHeadNode;
			pNewNode->m_pRight = &m_NullHeadNode;
			pNewNode->m_pParent = &m_NullHeadNode;
			pNewNode->m_pNext = &m_NullHeadNode;
			pNewNode->m_pPrev = &m_NullHeadNode;
			return addNewNode(true, pWhereNode, pNewNode);
			
		}
		else
			pWhereNodePrevNode = pWhereNode->m_pPrev;	// need to test if insert before is okay

		/*if (_DEBUG_LT_PRED(this->comp,
			this->_Key(_Where._Mynode()),
			this->_Kfn(_Val)))
			return (_Pairib(_Insert(_Addleft, _Wherenode, _Node), true));*/
		if( m_Cmp.LE(pWhereNodePrevNode->m_key, pNewNode->m_key))
		{
			pNewNode->color_ = RED;
			pNewNode->m_pLeft = &m_NullHeadNode;
			pNewNode->m_pRight = &m_NullHeadNode;
			pNewNode->m_pParent = &m_NullHeadNode;
			pNewNode->m_pNext = &m_NullHeadNode;
			pNewNode->m_pPrev = &m_NullHeadNode;
			return addNewNode(bAddleft, pWhereNode, pNewNode);
		}
		else
		{	
			return &m_NullHeadNode;
		}
	}
	return &m_NullHeadNode;
}
template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::insert_aw(const TypeKey& key)
{

	TTreeNode *pNode = NULL; 
	if(!getNodeForInsertAw(&pNode, key))
		return;
	TTreeNode* pNewNode =  new (m_pAlloc->alloc(sizeof(TTreeNode))) TTreeNode(key);
	addNewNode(pNode, pNewNode);
}
template <class TypeKey, class TreeNode, class TComp>
 TreeNode * _RBTree<TypeKey, TreeNode, TComp>::addNewNode(bool bAddleft, TTreeNode *pWhereNode, TTreeNode* pNewNode)
 {


	 bool bWhereHead = pWhereNode == &m_NullHeadNode;
	 pNewNode->m_pParent = pWhereNode;
	 if (bWhereHead)
	 {	// first node in tree, just set head values
		m_NullHeadNode.m_pParent = pNewNode;
		m_NullHeadNode.m_pLeft = pNewNode;
		m_NullHeadNode.m_pRight = pNewNode;
		m_NullHeadNode.m_pNext = pNewNode;
		m_NullHeadNode.m_pPrev = pNewNode;
		/*m_pRoot = pNewNode;
		m_pRoot->color_ = BLACK;
		m_pRoot->m_pParent = &m_NullHeadNode;*/

		//pNewNode->color_ = BLACK;
		//pNewNode->m_pParent = &m_NullHeadNode;
	 }


	/* if(!pWhereNode)
	 {
		 m_pRoot = pNewNode;
		 m_pRoot->color_ = BLACK;
		 m_pRoot->m_pParent = &m_NullHeadNode;
	 }*/
	 else
	 {
		/* if(m_bMulti && m_Cmp.EQ(pNewNode->key_,  pWhereNode->key_))
		 {
		
			 if(!pWhereNode->m_pRight)
			 {
				 pWhereNode->m_pRight = pNewNode;
					
			 }
			 else
			 {
				 pWhereNode->m_pRight->m_pParent = pNewNode;
				 pNewNode->m_pRight = pWhereNode->m_pRight;
				 pWhereNode->m_pRight = pNewNode;

			 }
			 if(!bWhereHead)
			 {
				 if(pWhereNode->m_pNext)
					 pWhereNode->m_pNext->m_pPrev = pNewNode;
				 pNewNode->m_pNext = pWhereNode->m_pNext;
				 pNewNode->m_pPrev = pWhereNode;
				 pWhereNode->m_pNext = pNewNode;
			 }
			

		 }
		 else */if(bAddleft)
		 {
			 pWhereNode->m_pLeft = pNewNode;
			 if (pWhereNode == m_NullHeadNode.m_pLeft)
			 {
				 m_NullHeadNode.m_pLeft = pNewNode;
			 }
			

				 if(pWhereNode->m_pPrev)
					 pWhereNode->m_pPrev->m_pNext = pNewNode;
				 pNewNode->m_pNext = pWhereNode;
				 pNewNode->m_pPrev = pWhereNode->m_pPrev;
				 pWhereNode->m_pPrev = pNewNode;
			

		 }
		 else
		 {
			 pWhereNode->m_pRight = pNewNode;
			 if (pWhereNode == m_NullHeadNode.m_pRight)
			 {
				 m_NullHeadNode.m_pRight  = pNewNode;
			 }

	
				 if(pWhereNode->m_pNext)
					 pWhereNode->m_pNext->m_pPrev = pNewNode;
				 pNewNode->m_pNext = pWhereNode->m_pNext;
				 pNewNode->m_pPrev = pWhereNode;
				 pWhereNode->m_pNext = pNewNode;
			 
		 }
	
	 }
	 m_nSize += 1;
	 return insertFixup(pNewNode);
 }
template <class TypeKey, class TreeNode, class TComp>
bool _RBTree<TypeKey, TreeNode, TComp>::getNodeForInsert(TTreeNode **pNode, const TypeKey& key){
	TTreeNode *pTryNode = m_NullHeadNode.m_pParent; //m_pRoot;
	*pNode  = &m_NullHeadNode;
	while(pTryNode != &m_NullHeadNode)
	{
		if (m_Cmp.EQ(pTryNode->key_ , key)) 
		{
			if(!m_bMulti)
				return false;

			*pNode = pTryNode;
			return true;
		}
		*pNode = pTryNode;
		if( m_Cmp.LE(key , pTryNode->key_))
		{
			pTryNode = pTryNode->m_pLeft;
		}
		else
		{
			pTryNode = pTryNode->m_pRight;
		}
	}
	return true;
 }

template <class TypeKey, class TreeNode, class TComp>
bool _RBTree<TypeKey, TreeNode, TComp>::getNodeForInsertAw(TTreeNode **pNode, const TypeKey& key){
	TTreeNode *x = m_pRoot;
	while(x)
	{
		if (m_Cmp.EQ(x->key_ , key)) 
		{
			x->key_ = key;
			return false;
		}
		*pNode = x;
		if( m_Cmp.LE(key , x->key_))
		{
			x = x->m_pLeft;
		}
		else
		{
			x = x->m_pRight;
		}
	}
	return true;
}
template <class TypeKey, class TreeNode, class TComp>
void _RBTree<TypeKey, TreeNode, TComp>::remove(const TypeKey& key)
{
	TTreeNode* pDelNode = findNode(key);
	if(isNull(pDelNode))
		return;
	deleteNode(pDelNode);
	m_nSize -= 1;
	//m_pAlloc->free(pDelNode);
}


template <class TypeKey, class TreeNode, class TComp>
TreeNode*  _RBTree<TypeKey, TreeNode, TComp>::findNode(const TypeKey& key, TreeNode* pFromNode)
{
	TTreeNode *pNode  = pFromNode ? pFromNode : root();
	TTreeNode *pWherenode = &m_NullHeadNode;	// end() if search fails

	while (pNode != &m_NullHeadNode)
	{
		if (m_Cmp.LE(pNode->m_key, key))
			pNode = pNode->m_pRight;	// descend right subtree
		else
		{	// _Pnode not less than _Keyval, remember it
			pWherenode = pNode;
			pNode = pNode->m_pLeft;	// descend left subtree
		}
	}
	if (m_Cmp.EQ(pWherenode->m_key, key))
		return pWherenode;	
	return &m_NullHeadNode;
}
template <class TypeKey, class TreeNode, class TComp>
TreeNode* _RBTree<TypeKey, TreeNode, TComp>::findNodeForBTree(const TypeKey& key, short& nTypeRet)
{
	TTreeNode *pFindNode = root();
	TTreeNode *pLessNode = &m_NullHeadNode; //последняя меньшая нода
	nTypeRet = NOT_FOUND_KEY;
	while(pFindNode != &m_NullHeadNode)
	{
		if( m_Cmp.LE(key, pFindNode->m_key))
		{
			nTypeRet = LE_KEY;
			if(pFindNode->m_pLeft != &m_NullHeadNode)
				pFindNode = pFindNode->m_pLeft;
			else
				return pLessNode;
		}
		else if(m_Cmp.EQ(pFindNode->m_key, key))
		{			
			nTypeRet = FIND_KEY;
			return pFindNode;
		}
		else
		{
			nTypeRet = LQ_KEY;
			if(pFindNode->m_pRight != &m_NullHeadNode)
			{
				assert(pFindNode->m_pRight != pFindNode);
				pLessNode = pFindNode;
				pFindNode = pFindNode->m_pRight;
				
			}
			else
				return pFindNode; //больше всех вернем последний
			
		}
	}
	
	return pLessNode;
}
template <class TypeKey, class TreeNode, class TComp>
TreeNode* _RBTree<TypeKey, TreeNode, TComp>::findLessOrEQNode(const TypeKey& key, short& nTypeRet, TTreeNode* pFromNode)
{
	TTreeNode *pFindNode = pFromNode ? pFromNode : root();
	TTreeNode *pLessNode = &m_NullHeadNode; //последняя меньшая нода
	nTypeRet = NOT_FOUND_KEY;
	while(pFindNode != &m_NullHeadNode)
	{
		if( m_Cmp.LE(key, pFindNode->m_key))
		{
			nTypeRet = LE_KEY;
			pLessNode = pFindNode;
			 if(pFindNode->m_pLeft != &m_NullHeadNode)
			 {
				pFindNode = pFindNode->m_pLeft;
			 }
			 else
				 return pLessNode;
		}
		else if(m_Cmp.EQ(pFindNode->m_key, key))
		{			
			nTypeRet = FIND_KEY;
			return pFindNode;
		}
		else
		{
			//nTypeRet = LQ_KEY;
			if(pFindNode->m_pRight != &m_NullHeadNode)
			{
				assert(pFindNode->m_pRight != pFindNode);
				pFindNode = pFindNode->m_pRight;

			}
			else
				return pLessNode/*&m_NullHeadNode*/; 

		}
	}

	return pLessNode;
}

template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBTree<TypeKey, TTreeNode, TComp>::tree_successor(TTreeNode *pNode)
{
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
TTreeNode* _RBTree<TypeKey, TTreeNode, TComp>::tree_minimum(TTreeNode *pNode)
{
	while (pNode->m_pLeft != &m_NullHeadNode)
	{
		assert(pNode->m_pLeft);
		assert(pNode != pNode->m_pLeft);
		pNode = pNode->m_pLeft;
	}
	return pNode;
}
template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBTree<TypeKey, TTreeNode, TComp>::tree_maximim(TTreeNode *pNode)
{
	while (pNode->m_pRight != &m_NullHeadNode)
	{
		assert(pNode->m_pRight);
		pNode = pNode->m_pRight;
	}
	return pNode;
}




template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBTree<TypeKey, TTreeNode, TComp>::minimumNode()
{
	if(isEmpty())
		return &m_NullHeadNode;
	return tree_minimum(root());
}
template <class TypeKey, class TTreeNode, class TComp>
TTreeNode* _RBTree<TypeKey, TTreeNode, TComp>:: maximumNode()
{
	if(isEmpty())
		return &m_NullHeadNode;
	return tree_maximim(root());
}