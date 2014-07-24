template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>::RBTreeIterator(TRBTree<TypeKey, TypeVal>* tree, RBTreeNode* node, CommonLib::alloc_t *pAlloc, bool bBegin):pAlloc_(pAlloc), rbNode_(node), tree_(tree)
{
	assert(pAlloc_);
	if(bBegin)
		rbCurrNode_ = tree_->tree_minimum(rbNode_);
	else rbCurrNode_ = tree->tree_maximim(rbNode_);
}
template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>::~RBTreeIterator()
{

}
template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>::RBTreeIterator(const RBTreeIterator& it) :
	pAlloc_(it.pAlloc_)
	,rbNode_(it.rbNode_)
	,tree_(it.tree_)
	,rbCurrNode_(it.rbCurrNode_)
{

}
template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>& RBTreeIterator<TypeKey, TypeVal>::operator = (const RBTreeIterator& it)
{
	pAlloc_ = it.pAlloc_;
	rbNode_ = it.rbNode_;
	tree_ = it.tree_;
	rbCurrNode_ = it.rbCurrNode_;
	return *this;
}

template <class TypeKey, class TypeVal>
bool RBTreeIterator<TypeKey, TypeVal>::Reset(bool bBegin)
{
   if(bBegin)
		rbCurrNode_ = tree_->tree_minimum(rbNode_);
	else rbCurrNode_ = tree_->tree_maximim(rbNode_);
   return rbCurrNode_ != NULL;
}
/*
	static _Nodeptr _Min(_Nodeptr _Pnode)
		{	// return leftmost node in subtree at _Pnode
		while (!_Isnil(_Left(_Pnode)))
			_Pnode = _Left(_Pnode);
		return (_Pnode);
		}
*/
template <class TypeKey, class TypeVal>
bool RBTreeIterator<TypeKey, TypeVal>::next()
{
  if(!rbCurrNode_)
    return false;

  rbCurrNode_ = rbCurrNode_->next_;
  return rbCurrNode_ != 0;

  /*if(rbCurrNode_->right_)
				rbCurrNode_ = tree_->tree_minimum(rbCurrNode_->right_);
	else
	{	// climb looking for right subtree
		RBTreeNode* _Pnode = 0;
		while ((_Pnode = rbCurrNode_->parent_)&& (rbCurrNode_ == _Pnode->right_))
    {
		  rbCurrNode_ = _Pnode;	// ==> parent while right subtree
    }
		rbCurrNode_ = _Pnode;	// ==> parent (head if end())
  }
  return rbCurrNode_ != 0;*/
}
template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>& RBTreeIterator<TypeKey, TypeVal>::operator ++ ()
{
    next();
 	return *this;
}
template <class TypeKey, class TypeVal>
RBTreeIterator<TypeKey, TypeVal>& RBTreeIterator<TypeKey, TypeVal>::operator -- ()
{
    back();
 	return *this;
}
template <class TypeKey, class TypeVal>
bool RBTreeIterator<TypeKey, TypeVal>::operator == (const RBTreeIterator<TypeKey, TypeVal>& it)
{
	return rbCurrNode_->key_ == it.rbCurrNode_->key_;
}
template <class TypeKey, class TypeVal>
bool RBTreeIterator<TypeKey, TypeVal>::back()
{

  if(!rbCurrNode_)
    return false;

  rbCurrNode_ = rbCurrNode_->prev_;
  return rbCurrNode_ != 0;
}

template <class TypeKey, class TypeVal>
TypeVal& RBTreeIterator<TypeKey, TypeVal>::GetValue()
{
  assert(rbCurrNode_);
  return rbCurrNode_->value_;
}
template <class TypeKey, class TypeVal>
TypeKey& RBTreeIterator<TypeKey, TypeVal>::GetKey()
{
   assert(rbCurrNode_);
  return rbCurrNode_->key_;
}




template <class TypeKey, class TypeVal>
bool RBTreeIterator<TypeKey, TypeVal>::IsEnd()
{
  return rbCurrNode_ != 0;
}

