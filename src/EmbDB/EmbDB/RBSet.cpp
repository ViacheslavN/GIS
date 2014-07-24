template <class TypeKey, class TLess>
RBSet<TypeKey, TLess>::RBSet(CommonLib::alloc_t *pAlloc) : _Mybase(pAlloc), m_nSize(0)
{
	assert(m_pAlloc);
}

template <class TypeKey, class TLess>
RBSet<TypeKey, TLess>::RBSet(CommonLib::alloc_t *pAlloc, const TComporator& comp) : 
	_Mybase(pAlloc)
	,m_nSize(0)
	m_Cmp(comp)
{
	assert(m_pAlloc);
}


template <class TypeKey, class TLess>
void RBSet<TypeKey, TLess>::insert(const TypeKey& key)
{
	RBSetNode *x = m_pRoot;
	RBSetNode *y = NULL; 
	while(x)
	{
		if (m_Cmp.EQ(x->key_ , key)/*x->key_ == key*/) 
		{
			return;
		}
		y = x;
		if( m_Cmp.LE(key , x->key_)/*key < x->key_*/)
		{
			x = x->left_;
		}
		else
		{
			x = x->right_;
		}
	}

	RBSetNode* pNewNode =  new (m_pAlloc->alloc(sizeof(RBSetNode))) RBSetNode(key);
	if(!y)
	{
		m_pRoot = pNewNode;
	}
	else
	{

		if(m_Cmp.LE(pNewNode->key_, y->key_)/*z->key_ < y->key_*/)
		{
			y->left_ = pNewNode;
			pNewNode->next_ = y;
			if(y->prev_)
				y->prev_->next_ = pNewNode;
			y->prev_ = pNewNode;

		}
		else
		{
			y->right_ = pNewNode;
			if(y->next_)
				y->next_->prev_ = pNewNode;
			pNewNode->next_ = y->next_;
			y->next_ = pNewNode;
			pNewNode->prev_ = y;
		}
		pNewNode->parent_ = y;
	}
	m_nSize += 1;
	insertFixup(pNewNode);
}
template <class TypeKey, class TCmp>
void RBSet<TypeKey, TCmp>::remove(const TypeKey& key)
{
	RBSetNode* z = findNode(key);
	if(!z)
		return;
	deleteNode(z);
	m_nSize -= 1;
	m_pAlloc->free(z);
}


/*template <class TypeKey, class TCmp>
RBSetNode*  RBSet<TypeKey, TCmp>::findNode(TypeKey key)
{
	RBSetNode *x = m_pRoot;
	while(x && !m_Cmp.EQ(x->key_, key))
	{
		if( m_Cmp.LE(key, x->key_))
		{
			x = x->left_;
		}
		else
		{
			x = x->right_;
		}
	}

	return x;
}*/