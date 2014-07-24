
#include "RBTree.h"
#include <assert.h>
/*namespace emdb
{*/


template <class TypeKey, class TypeVal>
TRBTree<TypeKey, TypeVal>::TRBTree(CommonLib::alloc_t *alloc, bool bMuitiset):pAlloc_(alloc), pRoot_(0), tree_size_(0)
{
	assert(pAlloc_);
}
template <class TypeKey, class TypeVal>
TRBTree<TypeKey, TypeVal>::~TRBTree()
{
	 DestroyTree();
}

template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::insert(const TypeKey& key, const TypeVal& val)
{
	RBTreeNode *x = pRoot_;
	RBTreeNode *y = 0; 
	//bool bFound = false;
	while(x)
	{
    if (x->key_ == key) 
	{
		//bFound = true;
		return;
	}
		y = x;
		if( key < x->key_)
		{
			x = x->left_;
		}
		else
		{
			x = x->right_;
		}
	}
	/*if(bFound && !bMuitiset_)
		return;*/

	RBTreeNode* z =  new (pAlloc_->alloc(sizeof(RBTreeNode))) RBTreeNode(key, val);

	/*if(bFound && bMuitiset_)
	{
		tree_size_ += 1;


	}*/
	if(!y)
	{
		pRoot_ = z;
	}
	else
	{

		if(z->key_ < y->key_)
		{
			y->left_ = z;
			z->next_ = y;
			if(y->prev_)
				y->prev_->next_ = z;
			y->prev_ = z;

		}
		else
		{
			y->right_ = z;
			if(y->next_)
				y->next_->prev_ = z;
			z->next_ = y->next_;
			y->next_ = z;
			z->prev_ = y;
		}
		z->parent_ = y;
	}
	tree_size_ += 1;
	insertFixup(z);
}


template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>  TRBTree<TypeKey, TypeVal>::remove(const iterator& it)
{
	assert(this == it.tree_);

	RBTreeNode* z = it.rbCurrNode_;
	if(!z)
		return it;
	RBTreeIterator<TypeKey, TypeVal> ret_it = it;
	++ret_it;
	DeleteNode(z);
	return  ret_it;
}


template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::remove(const TypeKey& key)
{
	RBTreeNode* z = findNode(key);
	if(!z)
		return;
	DeleteNode(z);
	
}
template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::DeleteNode(RBTreeNode *z)
{
	RBTreeNode* y, *x = 0;
	if(!z)
		return;

	if(z->prev_)
	{
		z->prev_->next_ = z->next_;
	}
	if(z->next_)
	{
		z->next_->prev_ = z->prev_;
	}
	if(z->left_ == 0 || z->right_ == 0)
	{
		y = z;
	}
	else
	{
		y = tree_successor(z);
	}

	if(y->left_ != 0)
	{
		x = y->left_;
	}
	else
	{
		x = y->right_;
	}
	RBTreeNode*  x_par = y->parent_;	
	if(x)
		x->parent_ = x_par;

	if (y->parent_)
    if (y == y->parent_->left_)
        y->parent_->left_ = x;
    else
        y->parent_->right_ = x;
	 else
		pRoot_ = x;
	COLOR  color_y = y->color_;
	if (y != z)
	{
		/* replace z with y */ 
		y->color_ = z->color_ ;
		y->left_ = z->left_;
		y->right_ = z->right_;
		y->parent_ = z->parent_;

	  if (y->parent_)
        if (z == y->parent_->left_)
            y->parent_->left_ = y;
        else
            y->parent_->right_ = y;
		else
			pRoot_ = y;

		if(y->left_ != 0)
			y->left_->parent_ = y;

		if(y->right_ != 0)
			y->right_->parent_ = y;

		if(x_par == z) 
			x_par = y;
	}
tree_size_ -= 1;
pAlloc_->free(z);
 if(color_y == BLACK && x)
    deleteFixup (x, x_par);
}
template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::rotateLeft(RBTreeNode *pNodeLeft)
{
	RBTreeNode *pNodeRight = pNodeLeft->right_;

	/* establish pNodeLeft->right link */
	pNodeLeft->right_ = pNodeRight->left_;
	if (pNodeRight->left_ != 0) 
		pNodeRight->left_->parent_ = pNodeLeft;

	/* establish pNodeRight->parent link */
	if (pNodeRight != 0) 
		pNodeRight->parent_ = pNodeLeft->parent_;
	if (pNodeLeft->parent_)
	{
		if (pNodeLeft == pNodeLeft->parent_->left_)
			pNodeLeft->parent_->left_ = pNodeRight;
		else
			pNodeLeft->parent_->right_ = pNodeRight;
	}
	else
	{
		pRoot_ = pNodeRight;
	}

	/* link pNodeLeft and pNodeRight */
	pNodeRight->left_ = pNodeLeft;
	if (pNodeLeft != 0) 
		pNodeLeft->parent_ = pNodeRight;
}

template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::rotateRight(RBTreeNode *pNodeRight)
{
	RBTreeNode *pNodeLeft = pNodeRight->left_;

	/* establish pNodeRight->left link */
	pNodeRight->left_ = pNodeLeft->right_;
	if (pNodeLeft->right_ != 0) 
		pNodeLeft->right_->parent_ = pNodeRight;

	/* establish pNodeLeft->parent link */
	if (pNodeLeft != 0) 
		pNodeLeft->parent_ = pNodeRight->parent_;
	if (pNodeRight->parent_)
	{
		if (pNodeRight == pNodeRight->parent_->right_)
			pNodeRight->parent_->right_ = pNodeLeft;
		else
			pNodeRight->parent_->left_ = pNodeLeft;
	}
	else
	{
		pRoot_ = pNodeLeft;
	}

	/* link pNodeRight and y */
	pNodeLeft->right_ = pNodeRight;
	if (pNodeRight != 0) 
		pNodeRight->parent_ = pNodeLeft;
}

template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::insertFixup(RBTreeNode *x)
{
/* check Red-Black properties */
while (x != pRoot_ && x->parent_->color_ == RED)
{
    /* we have a violation */
    if (x->parent_ == x->parent_->parent_->left_)
    {
        RBTreeNode *y = x->parent_->parent_->right_;
        if (y && y->color_ == RED)
        {

            /* uncle is RED */
            x->parent_->color_ = BLACK;
            y->color_ = BLACK;
            x->parent_->parent_->color_ = RED;
            x = x->parent_->parent_;
        }
        else
        {

            /* uncle is BLACK */
            if (x == x->parent_->right_)
            {
                /* make x a left child */
                x = x->parent_;
                rotateLeft(x);
            }

            /* recolor and rotate */
            x->parent_->color_ = BLACK;
            x->parent_->parent_->color_ = RED;
            rotateRight(x->parent_->parent_);
        }
    }
    else
    {

        /* mirror image of above code */
        RBTreeNode *y = x->parent_->parent_->left_;
        if (y && y->color_ == RED)
        {

            /* uncle is RED */
            x->parent_->color_ = BLACK;
            y->color_ = BLACK;
            x->parent_->parent_->color_ = RED;
            x = x->parent_->parent_;
        }
        else
        {
            /* uncle is BLACK */
            if (x == x->parent_->left_)
            {
                x = x->parent_;
                rotateRight(x);
            }
            x->parent_->color_ = BLACK;
            x->parent_->parent_->color_ = RED;
            rotateLeft(x->parent_->parent_);
        }
    }
}
pRoot_->color_ = BLACK;
}
template <class TypeKey, class TypeVal>
RedBlackTreeNode<TypeKey, TypeVal>*  TRBTree<TypeKey, TypeVal>::tree_successor(RBTreeNode *x)
{
	if(x->right_)
	{
		return tree_minimum(x->right_);
	}
	RBTreeNode* y = x->parent_;
	while(y != 0 && x == y->right_)
	{
		x = y;
		y = y->parent_;
	}
	return y;
}

template <class TypeKey, class TypeVal>
RedBlackTreeNode<TypeKey, TypeVal>*  TRBTree<TypeKey, TypeVal>::findNode(TypeKey key)
{
  	RBTreeNode *x = pRoot_;
	while(x && x->key_ != key)
	{
		if( key < x->key_)
		{
			x = x->left_;
		}
		else
		{
			x = x->right_;
		}
	}

	return x;
}

template <class TypeKey, class TypeVal>
RedBlackTreeNode<TypeKey, TypeVal>*  TRBTree<TypeKey, TypeVal>::GetRoot()
{
	return pRoot_;
}


template <class TypeKey, class TypeVal>
RedBlackTreeNode<TypeKey, TypeVal>*  TRBTree<TypeKey, TypeVal>::tree_minimum(RBTreeNode *x)
{
	if(!x)
		return NULL;

	while (x->left_)
	{
		x = x->left_;
	}
	return x;
}
template <class TypeKey, class TypeVal>
RedBlackTreeNode<TypeKey, TypeVal>*  TRBTree<TypeKey, TypeVal>:: tree_maximim(RBTreeNode *x)
{
	if(!x)
		return NULL;
	while (x->right_)
	{
		x = x->right_;
	}
	return x;
}
template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::deleteFixup(RBTreeNode *x, RBTreeNode *x_par)
{
    while (x != pRoot_ && x->color_ == BLACK) {
        if (x == x_par->left_) {
            RBTreeNode *w = x_par->right_;
            if (w->color_ == RED) {
                w->color_ = BLACK;
                x_par->color_ = RED;
                rotateLeft(x_par);
                w = x_par->right_;
            }
            if (w->left_->color_ == BLACK && w->right_->color_ == BLACK) {
                w->color_ = RED;
                x = x_par;
                x_par = x->parent_;
            } else {
                if (w->right_->color_ == BLACK) {
                    w->left_->color_ = BLACK;
                    w->color_ = RED;
                    rotateRight(w);
                    w = x_par->right_;
                }
                w->color_ = x_par->color_;
                x_par->color_ = BLACK;
                w->right_->color_ = BLACK;
                rotateLeft (x_par);
                x = pRoot_;
                x_par = NULL;
            }
        } else {
            RBTreeNode *w = x_par->left_;
            if (w->color_ == RED) {
                w->color_ = BLACK;
                x_par->color_= RED;
                rotateRight (x_par);
                w = x_par->left_;
            }
            if (w->right_->color_ == BLACK && w->left_->color_ == BLACK) {
                w->color_ = RED;
                x = x_par;
                x_par = x->parent_;

            } else {
                if (w->left_->color_ == BLACK) 
					{
                    w->right_->color_ = BLACK;
                    w->color_ = RED;
                    rotateLeft (w);
                    w = x_par->left_;
					}
                w->color_ = x_par->color_;
                x_par->color_ = BLACK;
                w->left_->color_ = BLACK;
                rotateRight (x_par);
                x = pRoot_;
                x_par = NULL;
            }
        }
    }
    x->color_ = BLACK;
}


template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>  TRBTree<TypeKey, TypeVal>::find(TypeKey key)
{
	return iterator(this, findNode(key), pAlloc_);
}

template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>  TRBTree<TypeKey, TypeVal>::begin()
{
  return RBTreeIterator<TypeKey, TypeVal>(this, pRoot_ ,pAlloc_);
}

template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>  TRBTree<TypeKey, TypeVal>::last()
{
  return RBTreeIterator<TypeKey, TypeVal>(this, pRoot_ ,pAlloc_, false);
}


template <class TypeKey, class TypeVal>
size_t TRBTree<TypeKey, TypeVal>::size() const
{
  return tree_size_;
}


template <class TypeKey, class TypeVal>
void TRBTree<TypeKey, TypeVal>::DestroyTree()
{
  if(!pRoot_)
    return;

   RBTreeNode *pNode = tree_minimum(pRoot_);
 

   RBTreeNode *pNextNode = pNode->next_;
   if(!pNextNode)
   {
	  pAlloc_->free(pNode);
   }
   while(pNextNode)
   {
	 pAlloc_->free(pNode);
	 pNode = pNextNode;
	 pNextNode = pNextNode->next_;
   }
    pAlloc_->free(pNode);
   /*
   while(true)
   {
     if(!pNode->left_ && !pNode->right_)
     {
       RBTreeNode *pDelNode = pNode;
       if( pNode->parent_)
       {
        pNode->parent_->left_ == pNode ?  pNode->parent_->left_  = 0 : pNode->parent_->right_ = 0;
        pNode = pNode->parent_;
       }
       else
       {
          pAlloc_->free(pNode);
          break;
         //pNode = tree_minimum(pNode);
       }
   
       pAlloc_->free(pDelNode);
     }
     else if(pNode->left_)
     {
       pNode = tree_minimum(pNode->left_);
     }
     else
     {
        pNode = tree_minimum(pNode->right_);
     }
   }*/
}