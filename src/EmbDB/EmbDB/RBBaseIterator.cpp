

/*template <class TKey, class TNode>
RBTreeBaseIterator<TKey, TNode>::RBTreeBaseIterator(TNode* pNode ) : m_pNode(pNode)
{

}
template <class TKey, class TNode>
RBTreeBaseIterator<TKey, TNode>::~RBTreeBaseIterator()
{


}

template <class TKey, class TNode>
RBTreeBaseIterator<TKey, TNode>::RBTreeBaseIterator(const RBTreeBaseIterator& it) :
	m_pNode(it.m_pNode)

{

}
template <class TKey, class TNode>
RBTreeBaseIterator<TKey, TNode>& RBTreeBaseIterator<TKey, TNode>::operator = (const RBTreeBaseIterator& it)
{

	m_pNode = it.m_pNode;
	return *this;
}

template <class TKey, class TNode>
bool RBTreeBaseIterator<TKey, TNode>::operator == (const RBTreeBaseIterator& it){
	return m_pNode == it.m_pNode;
}
template <class TKey, class TNode>
bool RBTreeBaseIterator<TKey, TNode>::operator != (const RBTreeBaseIterator& it){
	return m_pNode != it.m_pNode;
}

template <class TKey, class TNode>
bool RBTreeBaseIterator<TKey, TNode>::next()
{
  if(!m_pNode)
    return false;

  m_pNode = m_pNode->m_pNext;
  return m_pNode != NULL;
}
template <class TKey, class TNode>
RBTreeBaseIterator<TKey, TNode>& RBTreeBaseIterator<TKey, TNode>::operator++ ()
{
    next();
 	return *this;
}
template <class TKey, class TNode>
RBTreeBaseIterator<TKey, TNode>& RBTreeBaseIterator<TKey, TNode>::operator-- ()
{
    back();
 	return *this;
}

template <class TKey, class TNode>
bool RBTreeBaseIterator<TKey, TNode>::back()
{

  if(!m_pNode)
    return false;

  m_pNode = m_pNode->m_pPrev;
  return m_pNode != NULL;
}
template <class TKey, class TNode>
bool RBTreeBaseIterator<TKey, TNode>::IsEnd()
{
	return m_pNode == NULL;
}
*/